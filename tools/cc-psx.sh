#!/bin/bash
# Compile a C file with the PSY-Q compiler (under wine) into an ELF object.
#   tools/cc-psx.sh file.c out.o [cc1psx flags]      default flags: -O3 -G8
# Env: PSYQ (SDK dir, default ~/dev/psyq_sdk/psyq_4.4), MIPS_PREFIX (default mipsel-linux-gnu-),
#      CC1 (a native cc1 to run instead of cc1psx under wine)
set -e
R=$(cd "$(dirname "$0")/.." && pwd)
PSYQ=${PSYQ:-$HOME/dev/psyq_sdk/psyq_4.4}
CROSS=${MIPS_PREFIX:-mipsel-linux-gnu-}
src=$1; out=$2; shift 2
flags=${*:--O3 -G8}
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export WINEDEBUG=-all
gcc -E -P -I "$R/src/include" -I "$R/include" "$src" -o "$tmp/a.i"
if [ -n "$CC1" ]; then  # a native cc1 instead of cc1psx under wine, e.g. for the permuter
    "$CC1" -quiet $flags "$tmp/a.i" -o "$tmp/a.s" 2>&1 || true
else
    wine "$PSYQ/bin/cc1psx.exe" -quiet $flags "$tmp/a.i" -o "$tmp/a.s" 2>&1 | grep -v MESA || true
fi
[ -s "$tmp/a.s" ] || { echo "cc1psx produced no output" >&2; exit 1; }
# maspsx reads stdin when there is any, so give it the file explicitly
# small-data threshold: maspsx gets the compiler's -G and makes symbols defined in this file $gp-relative.
# GNU as runs with -G0: like real ASPSX (ccpsx runs `aspsx -q -G8`), externs from other files
# always get %hi/%lo, even when gcc emitted `.extern sym,size` with a small size.
G=$(echo "$flags" | sed -nE 's/.*-G ?([0-9]+).*/\1/p')
G=${G:-0}
python3 "$R/tools/maspsx/maspsx.py" --aspsx-version=${ASPSX_VERSION:-2.81} --dont-force-G0 --use-comm-for-lcomm --use-comm-section --expand-div -G"$G" "$tmp/a.s" < /dev/null > "$tmp/a.m.s"
${CROSS}as -EL -march=r3000 -mabi=32 -G0 -no-pad-sections -I "$R/include" -o "$out" "$tmp/a.m.s"
