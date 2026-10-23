#!/usr/bin/env python3
"""Check one object's data sections against the original, on their own.

usage: tools/tudata.py EXE OBJ...

For each data section of an SN object this finds where its contribution sits in
the original (from the address of a variable it defines, or, for .rdata, by
searching for its bytes) and compares it byte for byte, ignoring the fields the
linker patches. .sbss/.bss are checked by the offsets of the variables in them
(statics first, then commons, as psylink lays them out). Prints OK or the first
difference per section.
"""
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import snobj  # noqa: E402

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT = 0x80018000
RANGES = {'.data': (0x8009a70c, 0x800ab0b4), '.rdata': (0x800ab0b4, 0x800ae870),
          '.sdata': (0x800ae870, 0x800aefa0), '.sbss': (0x800b2940, 0x800b3254),
          '.bss': (0x800b3254, 0x800ee988)}
MASK = {74: 0x03ffffff, 82: 0xffff, 84: 0xffff, 100: 0xffff, 16: 0xffffffff}

ADDR = {}
for line in open(f'{R}/src/symbols.txt'):
    a, n = line.split()
    ADDR.setdefault(n, int(a, 16))
# function-scope statics are only in config/symbol_addrs.txt, as name.N
for line in open(f'{R}/config/symbol_addrs.txt'):
    m = re.match(r'(\w+)\.\d+ = 0x([0-9a-fA-F]+);', line)
    if m:
        ADDR.setdefault(m.group(1), int(m.group(2), 16))
for g in json.load(open(f'{R}/src/globals.json')):
    if g['addr']:
        ADDR.setdefault(g['name'], g['addr'])


def bytemask(b, masks):
    m = bytearray(b'\xff' * len(b))
    for off, mk in masks.items():
        for k in range(4):
            if off + k < len(m) and (mk >> (8 * k)) & 0xff:
                m[off + k] = 0
    return m


def first_diff(b, m, exe, o):
    for i in range(len(b)):
        if (b[i] ^ exe[o + i]) & m[i]:
            return i
    return None


def check(exe, path):
    secs, syms = snobj.parse(path)
    patches = snobj.patches(path)
    out = []
    for sec in ('.data', '.rdata', '.sdata'):
        b = bytes(secs.get(sec, b''))
        if not b:
            continue
        masks = {}
        for s, off, t in patches:
            if s == sec:
                masks[off] = masks.get(off, 0) | MASK.get(t, 0xffffffff)
        m = bytemask(b, masks)
        lo, hi = RANGES[sec]
        start = None
        for s, off, n, _ in sorted(syms, key=lambda x: x[1]):
            base = re.sub(r'\.\d+$', '', n)
            if s == sec and off >= 0 and base in ADDR and lo <= ADDR[base] < hi:
                start = ADDR[base] - off
                break
        if start is None:
            # search for the longest masked prefix match, on 4-byte boundaries
            best = (-1, None)
            for a in range(lo, hi - 3, 4):
                o = a - TEXT + 0x800
                d = first_diff(b, m, exe, o) if a + len(b) <= hi else None
                n = len(b) if d is None and a + len(b) <= hi else (d or 0)
                if n > best[0]:
                    best = (n, a)
                    if n == len(b):
                        break
            start = best[1]
        o = start - TEXT + 0x800
        d = first_diff(b, m, exe, o)
        if d is None:
            out.append(f'{sec:7} OK    {start:#x}..{start + len(b):#x}')
        else:
            lab = max(((off, n) for s, off, n, _ in syms if s == sec and 0 <= off <= d), default=(0, '(start)'))
            out.append(f'{sec:7} DIFF  at {start + d:#x} (+{d:#x}, {lab[1]}+{d - lab[0]:#x}): '
                       f'ours {b[d:d + 12].hex()} original {exe[o + d:o + d + 12].hex()}')
    # commons psylink allocates itself: in hash order, and one of 8 bytes or more makes it
    # align every later bss piece to 8 (the original link did neither)
    big = [n for s, off, n, _ in syms if s in ('.sbss', '.bss') and off < 0 and -off >= 8]
    if big:
        out.append(f'.bss    DIFF  commons of 8 bytes or more (define them, see src/include/bss.h): {" ".join(big)}')
    # a .lcomm static and variables placed with section attributes collide in ASPSX: both are
    # allocated from offset 0
    for sec in ('.sbss', '.bss'):
        offs = {}
        for s_, off, n, _ in syms:
            if s_ == sec and off >= 0:
                offs.setdefault(off, []).append(n)
        clash = [v for v in offs.values() if len(v) > 1]
        if clash:
            out.append(f'{sec:7} DIFF  variables at the same offset (a .lcomm static next to SBSS/BSS ones?): {clash[0]}')
    # bss: statics in section order, then commons in definition order
    for sec in ('.sbss', '.bss'):
        lo, hi = RANGES[sec]
        size = len(secs.get(sec, b''))
        lay = [(off, n) for s, off, n, _ in syms if s == sec and off >= 0]
        cur = size
        for s, off, n, _ in syms:
            if s == sec and off < 0:    # common: -size
                sz = -off
                al = 8 if sz >= 8 else 4 if sz >= 4 else 2 if sz >= 2 else 1
                cur = (cur + al - 1) & ~(al - 1)
                lay.append((cur, n))
                cur += sz
        named = [(off, n) for off, n in lay if re.sub(r'\.\d+$', '', n) in ADDR]
        if not named:
            if lay:
                out.append(f'{sec:7} ??    no variable with a known address')
            continue
        o0, n0 = min(named)
        start = ADDR[re.sub(r'\.\d+$', '', n0)] - o0
        bad = [(n, start + off, ADDR[re.sub(r'\.\d+$', '', n)]) for off, n in sorted(named)
               if start + off != ADDR[re.sub(r'\.\d+$', '', n)]]
        if bad:
            n, ours, orig = bad[0]
            out.append(f'{sec:7} DIFF  {n} would be at {ours:#x}, original {orig:#x} ({len(bad)} misplaced)')
        else:
            out.append(f'{sec:7} OK    {start:#x}..{start + cur:#x}')
    return out


def main():
    exe = open(sys.argv[1], 'rb').read()
    bad = 0
    for path in sys.argv[2:]:
        res = check(exe, path)
        name = os.path.basename(path)
        for r in res:
            print(f'{name:14} {r}')
        bad += any('DIFF' in r for r in res)
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
