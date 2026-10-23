#!/usr/bin/env python3
"""Compare the game objects' data sections with the original, before linking.

usage: tools/datacmp.py EXE LINKORDER OBJDIR

psylink lays out each section object by object in link order (LINKORDER: one
object name per line). Starting from the original section start, this walks
the objects, compares each one's contribution with the bytes at that place
(ignoring fields the linker patches) and reports the first object per section
whose bytes or size are off. bss sections are compared by size only.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import snobj  # noqa: E402

TEXT = 0x80018000
SECTIONS = [('.data', 0x8009a70c), ('.rdata', 0x800ab0b4), ('.sdata', 0x800ae870),
            ('.sbss', 0x800b2940), ('.bss', 0x800b3254)]
MASK = {74: 0x03ffffff, 82: 0xffff, 84: 0xffff, 100: 0xffff, 16: 0xffffffff}


def main():
    exe = open(sys.argv[1], 'rb').read()
    order = [l.split()[0] for l in open(sys.argv[2]) if l.strip()]
    objdir = sys.argv[3]
    parsed = {}
    for name in order:
        p = f'{objdir}/{name}.obj'
        if os.path.exists(p):
            parsed[name] = (snobj.parse(p), snobj.patches(p))
    for sec, start in SECTIONS:
        cur = start
        for name in order:
            if name not in parsed:
                continue
            (secs, syms), patches = parsed[name]
            b = secs.get(sec, b'')
            if not b:
                continue
            cur = (cur + 3) & ~3
            if sec in ('.sbss', '.bss'):
                cur += len(b)
                continue
            masks = {}
            for s, off, t in patches:
                if s == sec:
                    masks[off] = MASK.get(t, 0xffffffff)
            o = cur - TEXT + 0x800
            bad = None
            for i in range(len(b)):
                m = masks.get(i & ~3, 0)
                mb = (~m >> (8 * (i & 3))) & 0xff
                if (b[i] ^ exe[o + i]) & mb:
                    bad = i
                    break
            if bad is not None:
                lab = max(((off, n) for s, off, n, _ in syms if s == sec and 0 <= off <= bad), default=(0, '-'))
                print(f'{sec}: {name} differs at {cur + bad:#x} (+{bad:#x}, {lab[1]}+{bad - lab[0]:#x}); '
                      f'ours {b[bad:bad + 8].hex()} original {exe[o + bad:o + bad + 8].hex()}')
                break
            cur += len(b)
        else:
            print(f'{sec}: all objects match, game part ends at {cur:#x}')


if __name__ == '__main__':
    main()
