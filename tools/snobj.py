#!/usr/bin/env python3
"""Minimal reader for PSY-Q (SN Systems "LNK" version 2) object files.

    sections(path) -> {name: bytearray}    the bytes each section holds (bss sections: zero-filled)
    symbols(path)  -> [(section name, offset, name, exported)]

usage: snobj.py FILE.obj      prints section sizes and symbols
"""
import struct
import sys


def parse(path):
    data = open(path, 'rb').read()
    if data[:3] != b'LNK' or data[3] != 2:
        raise ValueError(f'{path}: not an SN object')
    pos = 4
    names = {}
    code = {}
    bss = {}
    syms = []
    patches = []
    parse.numbers = getattr(parse, 'numbers', []) + [[]]
    chunk_start = {}
    cur = None

    def u8():
        nonlocal pos
        pos += 1
        return data[pos - 1]

    def u16():
        nonlocal pos
        pos += 2
        return struct.unpack_from('<H', data, pos - 2)[0]

    def u32():
        nonlocal pos
        pos += 4
        return struct.unpack_from('<I', data, pos - 4)[0]

    def s():
        n = u8()
        nonlocal pos
        pos += n
        return data[pos - n:pos].decode('latin-1')

    def expr():
        # relocation expression tree
        t = u8()
        if t == 0:      # constant
            u32()
        elif t in (2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24):  # symbol/section references
            u16()
        else:           # binary operator
            expr()
            expr()

    while pos < len(data):
        cmd = u8()
        if cmd == 0:
            break
        elif cmd == 2:                     # code bytes
            n = u16()
            chunk_start[cur] = len(code.get(cur, b''))
            code.setdefault(cur, bytearray()).extend(data[pos:pos + n])
            pos += n
        elif cmd == 4:                     # run of zeroes? (count)
            n = u32()
            code.setdefault(cur, bytearray()).extend(bytes(n))
        elif cmd == 6:                     # switch section
            cur = u16()
        elif cmd == 8:                     # n zero bytes (.space, and all of a bss section)
            n = u32()
            code.setdefault(cur, bytearray()).extend(bytes(n))
        elif cmd == 10:                    # patch: type, offset in the current section, expression
            ptype = u8()
            off = u16()
            expr()
            patches.append((cur, chunk_start.get(cur, 0) + off, ptype, off))
        elif cmd == 12:                    # xdef
            u16()
            sec = u16()
            off = u32()
            syms.append((sec, off, s(), True))
        elif cmd == 14:                    # xref
            u16()
            s()
        elif cmd == 16:                    # section definition
            sid = u16()
            u16()
            u8()
            names[sid] = s()
        elif cmd == 18:                    # local symbol
            sec = u16()
            off = u32()
            syms.append((sec, off, s(), False))
        elif cmd == 20:                    # group
            u8()
            u16()
            s()
        elif cmd == 28:                    # source file name
            u16()
            s()
        elif cmd == 46:                    # processor type
            u8()
        elif cmd == 48:                    # xbss (common)
            parse.numbers[-1].append(u16())
            sec = u16()
            size = u32()
            syms.append((sec, -size, s(), True))
        elif cmd == 50:
            u16()
        elif cmd == 52:
            u16()
            u8()
        elif cmd == 54:
            u16()
            u16()
        elif cmd == 56:
            u16()
            u32()
        elif cmd == 58:
            u16()
            u32()
            u16()
        elif cmd == 60:
            u16()
        elif cmd == 74:                    # function start
            pos += 2 + 4 + 2 + 4 + 2 + 4 + 2 + 4 + 4
            s()
        elif cmd in (76, 78, 80):          # function end, block start/end
            pos += 2 + 4 + 4
        elif cmd == 82:                    # def
            pos += 2 + 4 + 2 + 2 + 4
            s()
        elif cmd == 84:                    # def2
            pos += 2 + 4 + 2 + 2 + 4
            n = u16()
            pos += 4 * n
            s()
            s()
        else:
            raise ValueError(f'{path}: unknown command {cmd} at {pos - 1}')
    secs = {}
    for sid, name in names.items():
        b = code.get(sid, bytearray())
        if sid in bss:
            b = b + bytes(bss[sid])
        secs[name] = b
    parse.patches = [(names.get(sec, sec), off, t) for sec, off, t, _ in patches]
    return secs, [(names.get(sec, sec), off, n, x) for sec, off, n, x in syms]


def sections(path):
    return parse(path)[0]


def symbols(path):
    return parse(path)[1]


def patches(path):
    """[(section name, offset, patch type)]: the places the linker fills in"""
    parse(path)
    return parse.patches


if __name__ == '__main__':
    secs, syms = parse(sys.argv[1])
    for n, b in secs.items():
        if b:
            print(f'{n:10} {len(b):#x}')
    for sec, off, n, x in syms:
        print(f'  {sec:10} {off:#8x} {"X" if x else "l"} {n}')
