#!/bin/bash
# Compile a C file with the PSY-Q compiler (under wine) into an ELF object.
#   tools/cc-psx.sh file.c out.o [cc1psx flags]      default flags: -O2 -G0
# Env: PSYQ (SDK dir, default ~/dev/psyq_sdk/psyq_4.4), MIPS_PREFIX (default mipsel-linux-gnu-)
set -e
R=$(cd "$(dirname "$0")/.." && pwd)
PSYQ=${PSYQ:-$HOME/dev/psyq_sdk/psyq_4.4}
CROSS=${MIPS_PREFIX:-mipsel-linux-gnu-}
src=$1; out=$2; shift 2
flags=${*:--O2 -G0}
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export WINEDEBUG=-all
gcc -E -P -I "$R/src/include" -I "$R/include" "$src" -o "$tmp/a.i"
wine "$PSYQ/bin/cc1psx.exe" -quiet $flags "$tmp/a.i" -o "$tmp/a.s" 2>&1 | grep -v MESA || true
# maspsx reads stdin when there is any, so give it the file explicitly
python3 "$R/tools/maspsx/maspsx.py" --aspsx-version=${ASPSX_VERSION:-2.79} "$tmp/a.s" < /dev/null > "$tmp/a.m.s"
${CROSS}as -EL -march=r3000 -mabi=32 -no-pad-sections -I "$R/include" -o "$out" "$tmp/a.m.s"
