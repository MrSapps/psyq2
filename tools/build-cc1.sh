#!/bin/sh
# Build a native Linux cc1 from the gcc 2.8.1 sources that compiles exactly like PSY-Q 4.4's
# CC1PSX.EXE (gcc 2.8.1, SN32 build), for fast iteration, RTL dumps, gdb and decomp-permuter.
#
# usage: tools/build-cc1.sh GCC_2.8.1_SOURCE_DIR [OUT_DIR]     (OUT_DIR defaults to build/cc1)
# then:  OUT_DIR/cc1 -quiet -O3 -G0 -g -mgas -msplit-addresses -msoft-float -funsigned-char X.i -o X.s
#        (tools/cc1n wraps this)
#
# CC1PSX is stock gcc 2.8.1 for mips with these defaults: -mgas -msplit-addresses (explicit
# %hi/%lo), -msoft-float (the FP registers are fixed, which changes loop.c's invariant-motion
# threshold and so register allocation), unsigned char. Built as is on a 64-bit host it
# differs, so the host build here:
#   - declares the gcc-internal functions some files call undeclared (an implicit int return
#     truncates their pointers: crashes in reorg on some inputs),
#   - uses a 32-bit HOST_WIDE_INT and -fwrapv, so constants fold as on the 32-bit original
#     (x + 0x80000000 vs x - 0x80000000).
# Checked: every function of every src/ovl module and of 78 of the 79 src/*.c objects compiles
# to the same assembly as CC1PSX (draw.c's DrawTimeBar is scheduled differently).
set -e
SRC=$(cd "$1" && pwd)
OUT=${2:-$(dirname "$0")/../build/cc1}
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)
cat > "$OUT/prelude.h" <<'EOF'
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
extern struct rtx_def *alter_subreg (), *emit (), *emit_line_note (), *expand_expr (),
    *label_rtx (), *negate_rtx (), *remove_death ();
extern union tree_node *reorder_blocks (), *truthvalue_conversion ();
#define HOST_BITS_PER_WIDE_INT 32
#define HOST_WIDE_INT int
EOF
cd "$OUT"
if [ ! -f config.status ]; then
    "$SRC/configure" --target=mipsel-unknown-ecoff --host=x86_64-linux-gnu --build=x86_64-linux-gnu >configure.log
    # gcc 2.8.1 doesn't know an x86_64 host: give config.h the i386 Linux host file and the
    # target, and guard tm.h, which then gets included twice
    printf '#include "i386/xm-linux.h"\n#include "tm.h"\n' >> config.h
    { echo '#ifndef MY_TM_H'; echo '#define MY_TM_H'; cat tm.h; echo '#endif'; } > tm.h.new && mv tm.h.new tm.h
fi
make CC="gcc -std=gnu89 -fgnu89-inline -fcommon -O1 -fwrapv -fno-strict-aliasing -w -include $OUT/prelude.h" cc1 >make.log 2>&1 \
    || { tail -20 make.log; exit 1; }
echo "$OUT/cc1"
