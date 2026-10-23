#!/usr/bin/env python3
"""Diff one compiled function against the original asm.

usage: cmpfunc.py ASMFILE FUNC COMPILED.o [--relax] [--cross mipsel-linux-gnu-]

Relocations against data C can't name the same way compare equal: unnamed data (string literals, switch
tables: `D_`/`jtbl_` labels in the original, section symbols such as `.rodata`, `$LC` labels) all become
`<anon>` with the addend in the instruction ignored, as do gcc's numbered function-local statics
(`Temp.100`), which splat names inconsistently. References to globals listed in config/symbol_addrs.txt compare by
the address they reach, since splat names an address after the symbol it falls in (`LastObjective`
for `&FlyPicks[4]`). Use --strict to compare every symbol and addend exactly.

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
# jump-table labels are global symbols in splat's output; as plain labels the assembler resolves
# branches to them itself, as it does in the compiled object
body = re.sub(r"^\s*jlabel (\S+)", r"\1:", m.group(0), flags=re.M)
body = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text, "ax"\n' + body


ANON = re.compile(r"^((D|jtbl)_[0-9A-Fa-f]{8}|\.(rodata|rdata|sdata|data|sbss|bss)|\$LC?\d+)([+-]0x[0-9a-f]+)?$")


# addresses of named globals: splat names an address after whichever symbol it falls in, so the
# original may say `LastObjective` where C says `FlyPicks + 0x160`; compare such references by address
SYMADDR = {}
for _l in open(os.path.join(root, "config", "symbol_addrs.txt")):
    if (_m := re.match(r"(\w+) = (0x[0-9A-Fa-f]+);", _l)) and "type:func" not in _l:
        SYMADDR[_m.group(1)] = int(_m.group(2), 16)
IMM = re.compile(r"(-?(?:0x)?[0-9a-f]+)(\(\w+\))?$")


def imm_of(ins):
    v = IMM.search(ins).group(1)
    return int(v, 16) if v.lstrip("-").startswith("0x") else int(v)


def normalise_symbol(sym):
    """Return (symbol, is_anonymous_data) with names C can't reproduce made comparable."""
    # function-local statics (gcc's `Temp.100`) too: splat names some after SYM (`Temp.7`) and leaves
    # others unnamed (`D_800AE890`)
    if ANON.match(sym) or re.match(r"^\w+\.\d+([+-]0x[0-9a-f]+)?$", sym):
        return "<anon>", True
    return sym, False


def listing(path, only=None):
    out = subprocess.run([cross + "objdump", "-dr", "-z", "--no-show-raw-insn"]
                         + ([f"--disassemble={only}"] if only else []) + [path], capture_output=True,
                         text=True, check=True).stdout
    lines = []  # [address, instruction, branch target address or None, reloc text]
    for l in out.splitlines():
        l = l.strip()
        if "R_MIPS" in l:
            if lines:
                kind, sym = l.split()[-2].replace("R_MIPS_", ""), l.split()[-1]
                if not strict:
                    sym, anon = normalise_symbol(sym)
                    if anon:  # the addend lives in the instruction's immediate
                        lines[-1][1] = re.sub(r"(-?(0x)?[0-9a-f]+)(\(\w+\))?$", r"ANON\3", lines[-1][1])
                if kind == "26" and not re.match(r"^(\.|\$L)", sym):
                    lines[-1][2] = None  # call/jump to another symbol: the reloc says where
                    lines[-1][1] = lines[-1][1].replace("@T", "0x0")
                lines[-1][3] += "   " + kind + ("" if relax else " " + sym)
                lines[-1][4].append((kind, l.split()[-1]))
        elif m := re.match(r"([0-9a-f]+):\s+(.*)", l):
            ins = m.group(2).replace("\t", " ")
            # branch targets become offsets into the function: labels in the original (jlabel, .L) are
            # symbols there but not in the compiled object, so objdump's <sym+off> can't be compared
            t = re.search(r"\b([0-9a-f]+) <[^>]*>", ins)
            target = int(t.group(1), 16) if t else None
            if t:
                ins = ins[:t.start()] + "@T" + ins[t.end():]
            lines.append([int(m.group(1), 16), ins, target, "", []])
    if not strict:
        # a file-local symbol can share its name with another file's global: never look those up
        local = {f[-1] for f in (l.split() for l in subprocess.run(
            [cross + "objdump", "-t", path], capture_output=True, text=True).stdout.splitlines())
            if len(f) >= 5 and f[1] == "l"}
        resolve_addresses(lines, local)
    start = lines[0][0] if lines else 0
    return [(ins.replace("@T", hex(target - start)) if target is not None else ins) + rel
            for _, ins, target, rel, _ in lines]


def resolve_addresses(lines, local):
    """Rewrite %hi/%lo/%gp_rel references to known globals as the address they reach."""
    for i, line in enumerate(lines):
        for kind, sym in line[4]:
            if sym not in SYMADDR or sym in local or kind not in ("HI16", "LO16", "GPREL16") or normalise_symbol(sym)[1]:
                continue
            imm = imm_of(line[1])
            if kind == "HI16":
                lo = next((imm_of(l[1]) for l in lines[i + 1:] for k, s in l[4] if k == "LO16" and s == sym), 0)
                addr = SYMADDR[sym] + ((imm << 16) & 0xffffffff) + lo
                text = f"HI16 %hi({(addr + 0x8000) >> 16 << 16:#x})"
            elif kind == "LO16":  # the low half only: any carry is in the paired %hi
                text = f"LO16 %lo({(SYMADDR[sym] + imm) & 0xffff:#x})"
            else:
                imm = imm - 0x10000 if imm >= 0x8000 else imm
                text = f"{kind} {SYMADDR[sym] + imm:#x}"
            line[1] = IMM.sub(r"ADDR\2", line[1])
            line[3] = re.sub(rf"{kind} {re.escape(sym)}\b", text, line[3]) if not relax else line[3]

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
