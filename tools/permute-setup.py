#!/usr/bin/env python3
"""Set up a decomp-permuter directory for one function.

usage: permute-setup.py ASMFILE FUNC CFILE OUTDIR
       then: python3 ~/decomp-permuter/permuter.py OUTDIR [-j N] [--best-only]

CFILE is a C file (calib-style: whole file, so inlining comes out right) whose FUNC nearly matches.
OUTDIR gets base.c (CFILE preprocessed), target.o (the original FUNC assembled, as cmpfunc does),
compile.sh (cc-psx.sh with the default toolchain) and settings.toml, which restricts both
disassemblies to FUNC so the other functions in the file don't count. The permuter only edits FUNC.
Check a candidate it reports with tools/cmpfunc.py before taking it: its scoring ignores symbol names.
"""
import os
import re
import subprocess
import sys

asmfile, func, cfile, out = sys.argv[1:5]
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
cross = os.environ.get("MIPS_PREFIX", "mipsel-linux-gnu-")
os.makedirs(out, exist_ok=True)

# base.c: preprocessed, without GNU asm/attribute syntax pycparser can't parse
subprocess.run(["gcc", "-E", "-P", "-I", os.path.join(root, "src/include"), "-I", os.path.join(root, "include"),
                cfile, "-o", os.path.join(out, "base.c")], check=True)

text = open(asmfile).read()
m = re.search(rf"^glabel {re.escape(func)}\n.*?^endlabel {re.escape(func)}\n", text, re.S | re.M)
if not m:
    sys.exit(f"{func} not found in {asmfile}")
body = re.sub(r"^\s*jlabel (\S+)", r"\1:", m.group(0), flags=re.M)
open(os.path.join(out, "target.s"), "w").write(
    '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text, "ax"\n' + body)
subprocess.run([cross + "as", "-EL", "-march=r3000", "-mabi=32", "-no-pad-sections", "-I",
                os.path.join(root, "include"), "-o", os.path.join(out, "target.o"),
                os.path.join(out, "target.s")], check=True)

sh = os.path.join(out, "compile.sh")
open(sh, "w").write(f'#!/bin/bash\n# called as: compile.sh input.c -o output.o\nexec "{root}/tools/cc-psx.sh" "$1" "$3" >/dev/null\n')
os.chmod(sh, 0o755)
open(os.path.join(out, "settings.toml"), "w").write(
    f'func_name = "{func}"\ncompiler_type = "gcc"\n'
    f'objdump_command = "{cross}objdump -drz -m mips:3000 --disassemble={func}"\n')
print(f"{out}: ready for {func}")
