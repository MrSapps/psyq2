#!/usr/bin/env python3
"""Write linker assignments for symbols that no object defines (bss, unlabelled data).

usage: gen_undefined.py NM OUT symbol_addrs.txt OBJ...

Addresses come from symbol_addrs.txt, or from the name itself for splat's D_<ADDR> / jtbl_<ADDR> names.
"""
import re
import subprocess
import sys

nm, out, symfile, *objs = sys.argv[1:]
addrs = {}
for line in open(symfile):
    m = re.match(r"(\S+) = (0x[0-9a-fA-F]+);", line)
    if m:
        addrs[m.group(1)] = int(m.group(2), 16)

defined, undef = set(), set()
for o in objs:
    for line in subprocess.run([nm, "-A", o], capture_output=True, text=True, check=True).stdout.splitlines():
        parts = line.split()
        name, kind = parts[-1], parts[-2]
        (undef if kind == "U" else defined).add(name)

with open(out, "w") as f:
    for n in sorted(undef - defined - {"_gp"}):  # _gp comes from the linker script
        if n in addrs:
            a = addrs[n]
        elif m := re.fullmatch(r"(?:D|jtbl|func|L)_([0-9A-F]{6,8})", n):
            a = int(m.group(1), 16)
        else:
            sys.exit(f"no address for undefined symbol {n}")
        f.write(f"{n} = {a:#x};\n")
