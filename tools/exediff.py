#!/usr/bin/env python3
"""Compare a build with the original executable.

usage: tools/exediff.py [ORIGINAL] [BUILD]     (default baserom/SLUS_007.57 build/SLUS_007.57)

Prints, per section, how many words differ and where the first difference is,
then where symbol addresses (from build/SLUS_007.map against src/symbols.txt)
first move, which is usually the place to look.
"""
import os
import re
import sys

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT = 0x80018000
SECTIONS = [('.text', 0x80018000, 0x8009a70c), ('.data', 0x8009a70c, 0x800ab0b4),
            ('.rdata', 0x800ab0b4, 0x800ae870), ('.sdata', 0x800ae870, 0x800aefa0),
            ('CODE', 0x800aefa0, 0x800b2940), ('.sbss', 0x800b2940, 0x800b3000)]


def symbols_of(map_path):
    out = {}
    for m in re.finditer(r'^\s+([0-9A-F]{8})\s+(\S+)\s*$', open(map_path, errors='replace').read(), re.M):
        out.setdefault(m.group(2), int(m.group(1), 16))
    return out


def main():
    orig_path = sys.argv[1] if len(sys.argv) > 1 else f'{R}/baserom/SLUS_007.57'
    new_path = sys.argv[2] if len(sys.argv) > 2 else f'{R}/build/SLUS_007.57'
    o = open(orig_path, 'rb').read()
    n = open(new_path, 'rb').read()
    print(f'size: original {len(o):#x}, build {len(n):#x}')
    hd = [i for i in range(0x800) if i < len(n) and o[i] != n[i]]
    print(f'header: {len(hd)} bytes differ' + (f' (first at {hd[0]:#x})' if hd else ''))
    for name, a, b in SECTIONS:
        lo, hi = a - TEXT + 0x800, b - TEXT + 0x800
        d = [i for i in range(lo, hi, 4) if o[i:i + 4] != n[i:i + 4]]
        first = f', first at {d[0] - 0x800 + TEXT:#x}' if d else ''
        print(f'{name:7} {len(d):6} words differ{first}')
    mp = os.path.join(os.path.dirname(new_path), 'SLUS_007.map')
    if not os.path.exists(mp):
        return
    ours = symbols_of(mp)
    orig = {}
    for line in open(f'{R}/src/symbols.txt'):
        a, s = line.split()
        orig.setdefault(s, int(a, 16))
    moved = sorted((orig[s], s, ours[s]) for s in ours if s in orig and ours[s] != orig[s]
                   and not re.match(r'_\w*_(org|orgend|obj|objend|size)$', s))
    print(f'{len(moved)} symbols at other addresses' + (':' if moved else ''))
    prev = None
    shown = 0
    for a, s, b in moved:
        d = b - a
        if d != prev and shown < 25:
            print(f'   {s:32} {a:#x} -> {b:#x} ({d:+#x})')
            shown += 1
        prev = d


if __name__ == '__main__':
    main()
