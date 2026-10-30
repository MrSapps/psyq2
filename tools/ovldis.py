#!/usr/bin/env python3
"""Disassemble an overlay module (baserom/ovl/NAME.bin + NAME.rel) with its relocations applied
as symbols, for reading and decompiling.

usage: ovldis.py baserom/ovl/NAME.bin [--start HEX] [--end HEX]

Relocations (the .rel is __SN_RELOCATE's list, offset | type):
  0  word:  the word is a module offset
  1  hi16:  next word is the full target (the lo16 half of the pair has its low part)
  2  lo16:  the low half is a module offset
  3  jal:   the target is a module offset
Function starts: jal targets, words relocated to text (the interface's pointers, tables), the
instruction after a jr ra's delay slot.
Text is found as the run of code from the first function start to the last jr ra.
"""
import os
import struct
import subprocess
import sys


def load(path):
    code = open(path, 'rb').read()
    rel = open(path[:-4] + '.rel', 'rb').read()
    words = struct.unpack(f'<{len(rel) // 4}I', rel)
    relocs = {}
    i = 0
    while words[i] != 0xffffffff:
        w = words[i]
        i += 1
        t, off = w & 3, w & ~3
        if t == 1:
            relocs[off] = (1, words[i])
            i += 1
        else:
            relocs[off] = (t, None)
    return code, relocs


def word(code, off):
    return struct.unpack_from('<I', code, off)[0]


def targets(code, relocs):
    """module offset each relocation refers to"""
    out = {}
    for off, (t, add) in relocs.items():
        w = word(code, off)
        if t == 0:
            out[off] = w
        elif t == 1:
            out[off] = add
        elif t == 2:
            out[off] = (w & 0xffff) - ((w & 0x8000) << 1)
        else:
            out[off] = (w & 0x3ffffff) << 2
    return out


def text_range(code, relocs, tg):

    # the first jr ra's function: walk back from it to the stack adjust, or take the lowest jal target
    jrs = [o for o in range(0, len(code) - 3, 4) if word(code, o) == 0x03e00008]
    if not jrs:
        return 0, 0
    end = jrs[-1] + 8
    # start: the lowest word-reloc'd / jal target that is not past the first jr ra
    cands = [v for o, v in tg.items() if v % 4 == 0 and v <= jrs[0] and v + 4 <= len(code)
             and (relocs[o][0] == 3 or word(code, v) >> 16 == 0x27bd)]  # jal target or addiu sp,sp,-N
    return (min(cands) if cands else jrs[0]), end


def main():
    path = sys.argv[1]
    code, relocs = load(path)
    tg = targets(code, relocs)
    tstart, tend = text_range(code, relocs, tg)
    if '--start' in sys.argv:
        tstart = int(sys.argv[sys.argv.index('--start') + 1], 16)
    if '--end' in sys.argv:
        tend = int(sys.argv[sys.argv.index('--end') + 1], 16)
    funcs = {v for o, v in tg.items() if tstart <= v < tend and v % 4 == 0 and
             (relocs[o][0] == 0 or relocs[o][0] == 3 and word(code, o) >> 26 == 3)}  # words and jal, not j
    # after each jr ra + delay slot a new function starts (unless padding)
    for o in range(tstart, tend - 8, 4):
        if word(code, o) == 0x03e00008:
            funcs.add(o + 8)
    funcs = sorted(f for f in funcs if f < tend)
    name = os.path.basename(path)[:-4]
    print(f'# {name}: {len(code):#x} bytes, text {tstart:#x}-{tend:#x}, {len(funcs)} functions')

    objdump = os.environ.get('OBJDUMP', 'mipsel-linux-gnu-objdump')
    out = subprocess.run([objdump, '-b', 'binary', '-m', 'mips:3000', '-EL', '-D', '-z',
                          f'--start-address={tstart}', f'--stop-address={tend}', '-M', 'reg-names=numeric', path],
                         capture_output=True, text=True).stdout
    fset = set(funcs)
    for line in out.splitlines():
        p = line.split('\t')
        if len(p) < 3 or not p[0].strip().endswith(':'):
            continue
        off = int(p[0].strip()[:-1], 16)
        ins = '\t'.join(p[2:]).strip()
        if off in fset:
            print(f'\nfunc_{off:x}:')
        note = ''
        if off in relocs:
            t = relocs[off][0]
            v = tg[off]
            lab = f'func_{v:x}' if v in fset else f'D_{v:x}'
            note = ['', f'%hi({lab})', f'%lo({lab})', f'{lab}'][t]
        # branches: show the label
        print(f'  /* {off:05x} {word(code, off):08x} */  {ins:40s}{"  # " + note if note else ""}')
    # data words relocated outside text
    print('\n# relocated words outside text')
    for o in sorted(relocs):
        if relocs[o][0] == 0 and not tstart <= o < tend:
            v = tg[o]
            print(f'  {o:05x}: {"func" if v in fset else "D"}_{v:x}')


if __name__ == '__main__':
    main()
