#!/usr/bin/env python3
"""Compare SN objects' code with the original executable, before linking.

usage: tools/objcmp.py EXE OBJ...

Each game object's .text is one contiguous run in the original. This compares
it word by word, ignoring the fields the linker patches (it checks those after
linking), and names the first function that differs. Unlike a per-function
diff of the GNU-as build, this sees exactly what ASPSX produced, including
which accesses are $gp-relative.
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import snobj  # noqa: E402

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MASK = {74: 0x03ffffff, 82: 0xffff, 84: 0xffff, 100: 0xffff, 16: 0xffffffff}


def main():
    exe = open(sys.argv[1], 'rb').read()
    fn = json.load(open(f'{R}/src/functions.json'))
    first = {}  # obj base name -> address of its first function
    for f in sorted(fn, key=lambda f: f['addr']):
        first.setdefault(f['obj'].rsplit('.', 1)[0].lower(), f['addr'])
    bad = 0
    for path in sys.argv[2:]:
        name = os.path.basename(path).rsplit('.', 1)[0].lower()
        if name not in first:
            continue
        secs, syms = snobj.parse(path)
        text = secs.get('.text', b'')
        masks = {}
        for sec, off, t in snobj.patches(path):
            if sec == '.text':
                masks[off] = masks.get(off, 0) | MASK.get(t, 0xffffffff)
        base = first[name] - 0x80018000 + 0x800
        funcs = sorted((off, n) for sec, off, n, _ in syms if sec == '.text' and off >= 0)
        diff = None
        for off in range(0, len(text), 4):
            a = struct.unpack_from('<I', text, off)[0]
            b = struct.unpack_from('<I', exe, base + off)[0]
            m = ~masks.get(off, 0) & 0xffffffff
            if (a ^ b) & m:
                diff = off
                break
        if diff is not None:
            bad += 1
            f = max((x for x in funcs if x[0] <= diff), default=(0, '?'))
            print(f'{name}: differs in {f[1]} at +{diff - f[0]:#x} (ours {a:08x}, original {b:08x})')
    print(f'{len(sys.argv) - 2 - bad} objects match' if bad == 0 else f'{bad} objects differ')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
