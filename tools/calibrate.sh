#!/bin/bash
# Compile every calib/<name>.c and diff each function it defines against asm/<name>.s.
#   tools/calibrate.sh [cc1psx flags]        default: -O3 -G8 -g
# Env: PSYQ (SDK dir, as for cc-psx.sh). Prints one line per function (OK, ~~ = only a symbol name
# differs, -- = differs); exit 1 if any differ.
# A calib file whose original was built with other flags names them in a /* calib-flags: ... */
# comment, which overrides [flags] for that file. Loop over SDKs/flags to compare compilers, e.g.
#   for v in 4.3 4.4 4.5; do PSYQ=~/dev/psyq_sdk/psyq_$v tools/calibrate.sh -O3 -G8; done
R=$(cd "$(dirname "$0")/.." && pwd)
CROSS=${MIPS_PREFIX:-mipsel-linux-gnu-}
flags=${*:--O3 -G8 -g}
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
ok=0; names=0; bad=0
for c in "$R"/calib/*.c; do
    n=$(basename "$c" .c)
    # a file built with other flags than the game says so: /* calib-flags: -O0 -G8 */
    fflags=$(sed -nE 's|.*calib-flags: *([^*]*[^ *]) *\*/.*|\1|p' "$c" | head -1)
    if ! "$R/tools/cc-psx.sh" "$c" "$tmp/$n.o" ${fflags:-$flags} > "$tmp/log" 2>&1; then
        echo "$n.c: compile failed"; cat "$tmp/log"; bad=$((bad + 1)); continue
    fi
    for f in $(${CROSS}nm "$tmp/$n.o" | awk '$2 == "T" { print $3 }'); do
        if python3 "$R/tools/cmpfunc.py" "$R/asm/$n.s" "$f" "$tmp/$n.o" > /dev/null; then
            echo "  OK  $f"; ok=$((ok + 1))
        elif python3 "$R/tools/cmpfunc.py" "$R/asm/$n.s" "$f" "$tmp/$n.o" --relax > /dev/null; then
            # same code, only a relocation's symbol differs
            echo "  ~~  $f"; names=$((names + 1))
        else
            echo "  --  $f"; bad=$((bad + 1))
        fi
    done
done
echo "$(basename "${PSYQ:-psyq_4.4}") $flags: $ok match, $names match but for a symbol name, $bad differ"
[ "$bad" -eq 0 ]
