#!/usr/bin/env python3
"""Diff one compiled function against the original asm.

usage: cmpfunc.py ASMFILE FUNC COMPILED.o [--relax] [--cross mipsel-linux-gnu-]

--relax ignores symbol names on relocations (compare reloc kinds only), for functions touching statics
whose names differ between the original and the compiled object.

Extracts FUNC from the splat asm file, assembles it, and prints both listings
side by side (relocations shown symbolically); exit status 0 means identical.
"""
import re
import subprocess
import sys
import tempfile
import os

asmfile, func, obj = sys.argv[1:4]
relax = "--relax" in sys.argv
cross = sys.argv[sys.argv.index("--cross") + 1] if "--cross" in sys.argv else "mipsel-linux-gnu-"
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

text = open(asmfile).read()
m = re.search(rf"^glabel {re.escape(func)}\n.*?^endlabel {re.escape(func)}\n", text, re.S | re.M)
if not m:
    sys.exit(f"{func} not found in {asmfile}")
body = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text, "ax"\n' + m.group(0)


def listing(path):
    out = subprocess.run([cross + "objdump", "-dr", "-z", "--no-show-raw-insn", path], capture_output=True,
                         text=True, check=True).stdout
    lines = []
    for l in out.splitlines():
        l = l.strip()
        if "R_MIPS" in l:
            if lines:
                lines[-1] += "   " + l.split()[-2].replace("R_MIPS_", "") + ("" if relax else " " + l.split()[-1])
        elif re.match(r"[0-9a-f]+:\s", l):
            lines.append(re.sub(r"^[0-9a-f]+:\s+", "", l).replace("\t", " "))
    return lines


with tempfile.TemporaryDirectory() as d:
    s = os.path.join(d, "o.s")
    o = os.path.join(d, "o.o")
    open(s, "w").write(body)
    subprocess.run([cross + "as", "-EL", "-march=r3000", "-mabi=32", "-no-pad-sections", "-I",
                    os.path.join(root, "include"), "-o", o, s], check=True)
    want = listing(o)
have = listing(obj)
n = max(len(want), len(have))
same = want == have
print(f"{func}: original {len(want)} instructions, compiled {len(have)}  {'MATCH' if same else 'DIFFERS'}")
if not same:
    for i in range(n):
        a = want[i] if i < len(want) else ""
        b = have[i] if i < len(have) else ""
        print(f"{'  ' if a == b else '!!'} {a:<46} | {b}")
sys.exit(0 if same else 1)
