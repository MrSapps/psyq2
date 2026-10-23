#!/usr/bin/env python3
"""Generate config/SLUS_007.yaml and config/symbol_addrs.txt from the decoded symbols.

usage: gen_splat.py SYMDIR(src) OUTDIR(config)

Text is split per source file: objects are linked contiguously, so the first
function of each source file starts a new segment, and anything after the last
function of a file (library code without debug info) becomes lib_<addr>.
Data sections are not split further yet.
"""
import json
import os
import re
import sys

BASE_VRAM, BASE_OFF = 0x80018000, 0x800
# from the linker labels in MAIN.SYM
TEXT_END, DATA_END, RDATA_END, SDATA_END, CODE_END = 0x8009a70c, 0x800ab0b4, 0x800ae870, 0x800aefa0, 0x800b2940
IMAGE_END, BSS_END = 0x800b3000, 0x800ee990


def off(vram):
    return vram - BASE_VRAM + BASE_OFF


def main():
    src, out = sys.argv[1:]
    funcs = sorted(json.load(open(os.path.join(src, "functions.json"))), key=lambda f: f["addr"])
    text_funcs = [f for f in funcs if f["addr"] < TEXT_END]

    # (vram, name) segment starts for the text section
    starts, used = [], {}
    cur_file, last_end = None, None
    for f in text_funcs:
        if f["file"] != cur_file:
            if last_end is not None and f["addr"] > last_end:
                starts.append((last_end, f"lib_{last_end:08x}"))
            base = re.sub(r"\W", "_", f["file"].replace("/", "\\").split("\\")[-1].rsplit(".", 1)[0]).lower()
            n = used.get(base, 0)
            used[base] = n + 1
            starts.append((f["addr"], base if n == 0 else f"{base}_{n + 1}"))
            cur_file = f["file"]
        last_end = f["end"]
    if last_end < TEXT_END:
        starts.append((last_end, f"lib_{last_end:08x}"))
    if starts[0][0] > BASE_VRAM:
        starts.insert(0, (BASE_VRAM, "text_start"))

    L = f"""options:
  basename: SLUS_007
  target_path: baserom/SLUS_007.57
  base_path: ..
  platform: psx
  compiler: GCC
  asm_path: asm
  asset_path: assets
  build_path: build
  src_path: src
  symbol_addrs_path: config/symbol_addrs.txt
  ld_script_path: config/SLUS_007.ld
  asm_inc_header: ""
  find_file_boundaries: False
  section_order: [".text", ".data", ".rodata", ".sdata", ".bss"]
  gp_value: 0x800ae870
  use_legacy_include_asm: False
  disassemble_all: True
  ld_align_section_vram_end: False
  ld_align_segment_vram_end: False

segments:
  - name: header
    type: bin
    start: 0x0
  - name: main
    type: code
    start: {BASE_OFF:#x}
    vram: {BASE_VRAM:#x}
    subalign: 4
    subsegments:
"""
    for v, n in starts:
        L += f"      - [{off(v):#x}, asm, {n}]\n"
    L += f"""      - [{off(TEXT_END):#x}, data, data]
      - [{off(DATA_END):#x}, rodata, rodata]
  - name: sdata
    type: code
    start: {off(RDATA_END):#x}
    vram: {RDATA_END:#x}
    subalign: 4
    subsegments:
      - [{off(RDATA_END):#x}, data, sdata]
  - name: hand
    type: code
    start: {off(SDATA_END):#x}
    vram: {SDATA_END:#x}
    subalign: 4
    bss_size: {BSS_END - IMAGE_END:#x}
    subsegments:
      - [{off(SDATA_END):#x}, asm, hand_asm]
      - [{off(CODE_END):#x}, data, sbss_in_image]
      - {{ start: {off(IMAGE_END):#x}, type: bss, vram: {IMAGE_END:#x}, name: bss }}
  - [{off(IMAGE_END):#x}]
"""
    os.makedirs(out, exist_ok=True)
    open(os.path.join(out, "SLUS_007.yaml"), "w").write(L)

    skip = re.compile(r"^_+(text|data|rdata|sdata|sbss|bss|ctors|dtors|CODE)_(obj|org|objend|orgend|size)$|^_endgroup|_size$")
    # splat wants one name per address: prefer functions, keep aliases as comments
    byaddr = {}
    for line in open(os.path.join(src, "symbol_addrs.txt")):
        m = re.match(r"(\S+) = (0x[0-9a-f]+);(.*)", line)
        name, addr, rest = m.group(1), int(m.group(2), 16), m.group(3).replace("type:data", "").rstrip()
        if BASE_VRAM <= addr < BSS_END and not skip.search(name):
            byaddr.setdefault(addr, []).append((name, rest))
    names, count = set(), 0
    with open(os.path.join(out, "symbol_addrs.txt"), "w") as o:
        for addr in sorted(byaddr):
            ents = sorted(byaddr[addr], key=lambda e: "type:func" not in e[1])
            name, rest = ents[0]
            # statics reuse names across objects; splat needs unique names
            if name in names:
                name = f"{name}_{addr:08X}"
            names.add(name)
            o.write(f"{name} = {addr:#x};{rest}\n")
            for alias, _ in ents[1:]:
                o.write(f"// alias at {addr:#x}: {alias}\n")
            count += 1
    seen = range(count)
    print(len(starts), "text segments,", len(seen), "symbols")


if __name__ == "__main__":
    main()
