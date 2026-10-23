#!/usr/bin/env python3
"""Compile src/<obj>.c files and check them against the disassembly.

usage: tools/checktu.py [-v] [name...]      (default: every src/*.c with code)

For each file: the functions must come out in the original order (psylink keeps
it), and each one must match its asm/ counterpart (tools/cmpfunc.py).
Env: PSYQ as for tools/cc-psx.sh.
"""
import collections
import json
import os
import re
import subprocess
import sys
import tempfile

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CROSS = os.environ.get('MIPS_PREFIX', 'mipsel-linux-gnu-')


def pieces_by_obj():
    """obj base name -> [asm piece], from the splat config and SYM function info."""
    fn = json.load(open(f'{R}/src/functions.json'))
    byaddr = {f['addr']: f for f in fn}
    out = collections.OrderedDict()
    for line in open(f'{R}/config/SLUS_007.yaml'):
        m = re.match(r'\s*- \[0x([0-9a-f]+), asm, (\w+)\]', line)
        if not m or m.group(2) in ('hand_asm', 'lib_80082b84'):
            continue
        vram = int(m.group(1), 16) - 0x800 + 0x80018000
        obj = byaddr[vram]['obj'].rsplit('.', 1)[0].lower()
        out.setdefault(obj, []).append(m.group(2))
    return out


def labels(piece):
    return [m.group(1) for m in re.finditer(r'^glabel (\w+)', open(f'{R}/asm/{piece}.s').read(), re.M)]


def compile_tu(src, out):
    text = open(src).read()
    m = re.search(r'calib-flags: *([^*]*[^ *]) *\*/', text)
    flags = m.group(1).split() if m else []
    p = subprocess.run([f'{R}/tools/cc-psx.sh', src, out] + flags, capture_output=True, text=True)
    return p.returncode == 0, p.stderr


def text_symbols(obj):
    out = subprocess.run([CROSS + 'objdump', '-t', obj], capture_output=True, text=True).stdout
    syms = [(int(l.split()[0], 16), l.split()[-1]) for l in out.splitlines() if ' F .text' in l]
    return [n for _, n in sorted(syms)]


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    verbose = '-v' in sys.argv
    objs = pieces_by_obj()
    names = args or list(objs)
    tmp = tempfile.mkdtemp()
    total = good = 0
    bad_files = 0
    for name in names:
        pcs = objs[name]
        want = [l for p in pcs for l in labels(p)]
        piece_of = {l: p for p in pcs for l in labels(p)}
        o = f'{tmp}/{name}.o'
        ok, err = compile_tu(f'{R}/src/{name}.c', o)
        if not ok:
            print(f'{name}: compile failed')
            print('\n'.join(l for l in err.splitlines() if 'warning' not in l)[-2000:])
            total += len(want)
            bad_files += 1
            continue
        got = text_symbols(o)
        fails = []
        for f in want:
            total += 1
            r = subprocess.run(['python3', f'{R}/tools/cmpfunc.py', f'{R}/asm/{piece_of[f]}.s', f, o],
                               capture_output=True, text=True)
            if r.returncode:
                fails.append(f)
            else:
                good += 1
        if got != want or fails:
            bad_files += 1
            msg = f'{name}:'
            if got != want:
                i = next((i for i, (a, b) in enumerate(zip(got, want)) if a != b), min(len(got), len(want)))
                msg += f' order differs at #{i} (got {got[i] if i < len(got) else "-"}, want {want[i] if i < len(want) else "-"})'
            if fails:
                msg += f' {len(fails)} differ: ' + ' '.join(fails)
            print(msg)
            if verbose and got != want:
                for i in range(max(len(got), len(want))):
                    a = got[i] if i < len(got) else '-'
                    b = want[i] if i < len(want) else '-'
                    print(f'   {i:3} {"  " if a == b else "!!"} {b:40} {a}')
    print(f'{good}/{total} functions match, {len(names) - bad_files}/{len(names)} files in order and matching')
    sys.exit(0 if good == total and bad_files == 0 else 1)


if __name__ == '__main__':
    main()
