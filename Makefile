# Build SLUS_007.57 from the disassembly and check it against the original hash.
#
#   make          assemble, link, and verify the SHA-1
#   make split    regenerate asm/ from baserom/ (needs .venv with splat64)
#
# Needs mipsel-linux-gnu binutils (apt: binutils-mipsel-linux-gnu, or tools/build-binutils.sh).

CROSS   ?= mipsel-linux-gnu-
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

TARGET  := SLUS_007.57
BUILD   := build
ASM_SRC := $(wildcard asm/*.s asm/data/*.s)
OBJS    := $(ASM_SRC:%=$(BUILD)/%.o)

TEXT_ADDR := 0x80018000
TEXT_SIZE := 0x9b000
PC0       := 0x800861a4
SP        := 0x801ffff0

.PHONY: all check split clean
all: check

$(BUILD)/asm/%.o: asm/%
	@mkdir -p $(dir $@)
	$(AS) -EL -march=r3000 -mabi=32 -no-pad-sections -L -I include -o $@ $<

assets/header.bin: tools/mkheader.py
	@mkdir -p assets
	python3 tools/mkheader.py $@ $(TEXT_ADDR) $(TEXT_SIZE) $(PC0) $(SP)

$(BUILD)/assets/header.bin.o: assets/header.bin
	@mkdir -p $(dir $@)
	$(LD) -r -b binary -o $@ $<
	$(OBJCOPY) --rename-section .data=.data $@

# symbols nothing defines (bss, unlabelled data) become linker assignments
$(BUILD)/undefined_syms.txt: $(OBJS) config/symbol_addrs.txt tools/gen_undefined.py
	python3 tools/gen_undefined.py $(CROSS)nm $@ config/symbol_addrs.txt $(OBJS)

$(BUILD)/$(TARGET).elf: $(OBJS) $(BUILD)/assets/header.bin.o $(BUILD)/undefined_syms.txt config/SLUS_007.ld
	$(LD) -EL -T $(BUILD)/undefined_syms.txt -T config/SLUS_007.ld -o $@ -Map $(BUILD)/$(TARGET).map

$(BUILD)/$(TARGET): $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

check: $(BUILD)/$(TARGET)
	@cd $(BUILD) && sha1sum -c ../config/$(TARGET).sha1 && echo "$(TARGET): matches"

split:
	python3 tools/gen_splat.py src config
	.venv/bin/python -m splat split config/SLUS_007.yaml

clean:
	rm -rf $(BUILD)
