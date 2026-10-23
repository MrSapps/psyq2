#!/usr/bin/env python3
"""Diff one compiled function against the original asm.

usage: cmpfunc.py ASMFILE FUNC COMPILED.o [--relax] [--cross mipsel-linux-gnu-]

Relocations against data C can't name the same way compare equal: unnamed data (string literals and
other `D_xxxxxxxx` labels in the original, section symbols such as `.rodata`, `$LC` labels) all become
`<anon>` with the addend in the instruction ignored, and gcc's numbered function-local statics
(`Temp.100`) compare by base name. Use --strict to compare every symbol and addend exactly.

--relax ignores symbol names on relocations altogether (compare reloc kinds only), for diagnosis.

Extracts FUNC from the splat asm file and from COMPILED.o (which may hold other functions),
assembles it, and prints both listings
side by side (relocations shown symbolically); exit status 0 means identical.
"""
import re
import subprocess
import sys
import tempfile
import os
import signal

signal.signal(signal.SIGPIPE, signal.SIG_DFL)  # quiet when piped into head

asmfile, func, obj = sys.argv[1:4]
relax = "--relax" in sys.argv
strict = "--strict" in sys.argv
cross = sys.argv[sys.argv.index("--cross") + 1] if "--cross" in sys.argv else "mipsel-linux-gnu-"
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

text = open(asmfile).read()
m = re.search(rf"^glabel {re.escape(func)}\n.*?^endlabel {re.escape(func)}\n", text, re.S | re.M)
if not m:
    sys.exit(f"{func} not found in {asmfile}")
body = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text, "ax"\n' + m.group(0)


ANON = re.compile(r"^(D_[0-9A-Fa-f]{8}|\.(rodata|rdata|sdata|data|sbss|bss)|\$LC?\d+)([+-]0x[0-9a-f]+)?$")


def normalise_symbol(sym):
    """Return (symbol, is_anonymous_data) with names C can't reproduce made comparable."""
    if ANON.match(sym):
        return "<anon>", True
    return re.sub(r"^(\w+)\.\d+", r"\1.N", sym), False


def listing(path, only=None):
    out = subprocess.run([cross + "objdump", "-dr", "-z", "--no-show-raw-insn"]
                         + ([f"--disassemble={only}"] if only else []) + [path], capture_output=True,
                         text=True, check=True).stdout
    lines = []
    for l in out.splitlines():
        l = l.strip()
        if "R_MIPS" in l:
            if lines:
                kind, sym = l.split()[-2].replace("R_MIPS_", ""), l.split()[-1]
                if not strict:
                    sym, anon = normalise_symbol(sym)
                    if anon:  # the addend lives in the instruction's immediate
                        lines[-1] = re.sub(r"(-?(0x)?[0-9a-f]+)(\(\w+\))?$", r"ANON\3", lines[-1])
                lines[-1] += "   " + kind + ("" if relax else " " + sym)
        elif re.match(r"[0-9a-f]+:\s", l):
            ins = re.sub(r"^[0-9a-f]+:\s+", "", l).replace("\t", " ")
            # branch targets as offsets into the function, not the section; relocated calls by reloc alone
            ins = re.sub(r"[0-9a-f]+ <[^>+]+\+(0x[0-9a-f]+)>", r"\1", ins)
            ins = re.sub(r"[0-9a-f]+ <[^>+]+>", "0x0", ins)
            lines.append(ins)
    return lines


with tempfile.TemporaryDirectory() as d:
    s = os.path.join(d, "o.s")
    o = os.path.join(d, "o.o")
    open(s, "w").write(body)
    subprocess.run([cross + "as", "-EL", "-march=r3000", "-mabi=32", "-no-pad-sections", "-I",
                    os.path.join(root, "include"), "-o", o, s], check=True)
    want = listing(o)
have = listing(obj, func)
n = max(len(want), len(have))
same = want == have
print(f"{func}: original {len(want)} instructions, compiled {len(have)}  {'MATCH' if same else 'DIFFERS'}")
if not same:
    for i in range(n):
        a = want[i] if i < len(want) else ""
        b = have[i] if i < len(have) else ""
        print(f"{'  ' if a == b else '!!'} {a:<46} | {b}")
sys.exit(0 if same else 1)
