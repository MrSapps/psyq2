#!/usr/bin/env python3
"""Link PSY-Q objects into an overlay module: the raw image (.bin) and its relocation list (.rel)
for __SN_RELOCATE, laid out the way the game's modules are.

usage: ovllink.py OUT.bin OBJ...      (writes OUT.bin and OUT.rel)

Layout, from address 0: every object's .header (the interface struct the engine reads at
offset 0, QLEVELINTERFACE or QCREINTERFACE, see src/ovl/include/ovl.h), then .rdata, .text,
.data, .sdata, .sbss, .bss, each object's piece in command-line order and word-aligned, then the
commons in order of first definition.

.rel: one word per relocated place: offset | type, 0 = word, 1 = hi16 (followed by
the target), 2 = lo16, 3 = jal; ended by 0xffffffff, in the order of the objects' patches. In the image a hi16 field is 0, a lo16
field the target's low half, a jal field the target / 4, a word the target.
"""
import struct
import sys

ORDER = ['.header', '.rdata', '.text', '.data', '.sdata', '.sbss', '.bss']
BSS = ('.sbss', '.bss')


class Obj:
    def __init__(self, path):
        self.path = path
        d = open(path, 'rb').read()
        if d[:4] != b'LNK\x02':
            raise ValueError(f'{path}: not an SN object')
        self.secs = {}        # id -> name
        self.data = {}        # id -> bytearray
        self.patches = []     # (sec id, offset, type, expr)
        self.syms = {}        # symbol number -> name (xdef/xref/xbss)
        self.defs = {}        # name -> (sec id, offset)
        self.locals = []
        self.commons = []     # (number, sec id, size, name)
        pos = 4
        cur = None
        start = {}

        def rd(fmt):
            nonlocal pos
            v = struct.unpack_from(fmt, d, pos)
            pos += struct.calcsize(fmt)
            return v[0] if len(v) == 1 else v

        def s():
            nonlocal pos
            n = d[pos]
            pos += 1 + n
            return d[pos - n:pos].decode('latin-1')

        def expr():
            t = rd('<B')
            if t == 0:
                return ('const', rd('<I'))
            if t == 2:
                return ('sym', rd('<H'))
            if t == 4:
                return ('sect', rd('<H'))
            if t in (6, 8, 10, 12, 14, 16, 18, 20, 22, 24):
                raise ValueError(f'{path}: expression {t} not handled')
            a = expr()
            b = expr()
            return ({44: '+', 46: '-', 50: '*', 54: '/', 56: '&', 58: '|', 60: '^', 62: '<<', 64: '>>'}
                    .get(t, t), a, b)

        while True:
            c = rd('<B')
            if c == 0:
                break
            if c == 2:
                n = rd('<H')
                start[cur] = len(self.data.setdefault(cur, bytearray()))
                self.data[cur] += d[pos:pos + n]
                pos += n
            elif c in (4, 8):
                self.data.setdefault(cur, bytearray()).extend(bytes(rd('<I')))
            elif c == 6:
                cur = rd('<H')
            elif c == 10:
                t, off = rd('<BH')
                self.patches.append((cur, start.get(cur, 0) + off, t, expr()))
            elif c == 12:
                num, sec, off = rd('<HHI')
                n = s()
                self.syms[num] = n
                self.defs[n] = (sec, off)
            elif c == 14:
                num = rd('<H')
                self.syms[num] = s()
            elif c == 16:
                sid, _, _ = rd('<HHB')
                self.secs[sid] = s()
            elif c == 18:
                sec, off = rd('<HI')
                self.locals.append((sec, off, s()))
            elif c == 20:
                rd('<BH')
                s()
            elif c == 28:
                rd('<H')
                s()
            elif c == 46:
                rd('<B')
            elif c == 48:
                num, sec, size = rd('<HHI')
                n = s()
                self.syms[num] = n
                self.commons.append((num, sec, size, n))
            elif c in (50, 60):
                rd('<H')
            elif c == 52:
                rd('<HB')
            elif c == 54:
                rd('<HH')
            elif c == 56:
                rd('<HI')
            elif c == 58:
                rd('<HIH')
            elif c == 74:
                pos += 28
                s()
            elif c in (76, 78, 80):
                pos += 10
            elif c == 82:
                pos += 14
                s()
            elif c == 84:
                pos += 14
                n = rd('<H')
                pos += 4 * n
                s()
                s()
            else:
                raise ValueError(f'{path}: unknown command {c} at {pos - 1}')

    def sec(self, name):
        for sid, n in self.secs.items():
            if n == name:
                return sid


def align(v, a=4):
    return (v + a - 1) & -a


def link(objs):
    base = {}      # (obj index, sec id) -> address
    addr = 0
    for name in ORDER:
        for i, o in enumerate(objs):
            sid = o.sec(name)
            if sid is None:
                continue
            addr = align(addr)
            base[i, sid] = addr
            addr += len(o.data.get(sid, b''))
    image = bytearray(addr)
    for (i, sid), a in base.items():
        b = objs[i].data.get(sid, b'')
        if objs[i].secs[sid] not in BSS:
            image[a:a + len(b)] = b
    glob = {}
    for i, o in enumerate(objs):
        for n, (sid, off) in o.defs.items():
            if n in glob:
                raise ValueError(f'{n} defined twice')
            glob[n] = base[i, sid] + off
    # commons that nothing defines: after everything, in order of appearance
    for i, o in enumerate(objs):
        for num, sid, size, n in o.commons:
            if n not in glob:
                addr = align(addr)
                glob[n] = addr
                addr += size
    image += bytes(addr - len(image))

    def ev(i, e):
        """-> (value, relocatable)"""
        if e[0] == 'const':
            return e[1], False
        if e[0] == 'sect':
            return base[i, e[1]], True
        if e[0] == 'sym':
            n = objs[i].syms[e[1]]
            if n not in glob:
                raise ValueError(f'{objs[i].path}: undefined {n}')
            return glob[n], True
        (a, ra), (b, rb) = ev(i, e[1]), ev(i, e[2])
        if e[0] == '+':
            return a + b, ra or rb
        if e[0] == '-':
            return a - b, ra and not rb
        raise ValueError(f'operator {e[0]} not handled')

    rel = []
    for i, o in enumerate(objs):
        for sid, off, t, e in o.patches:
            at = base[i, sid] + off
            v, r = ev(i, e)
            w = struct.unpack_from('<I', image, at)[0]
            if t == 16:
                w, rt = v & 0xffffffff, 0
            elif t == 82:
                w, rt = w & 0xffff0000, 1
            elif t == 84:
                w, rt = (w & 0xffff0000) | (v & 0xffff), 2
            elif t == 74:
                w, rt = (w & 0xfc000000) | ((v >> 2) & 0x3ffffff), 3
            else:
                raise ValueError(f'{o.path}: patch type {t} not handled')
            struct.pack_into('<I', image, at, w)
            if r:
                rel.append((at, rt, v))
    words = []
    for at, rt, v in rel:
        words.append(at | rt)
        if rt == 1:
            words.append(v)
    words.append(0xffffffff)
    return bytes(image), struct.pack(f'<{len(words)}I', *words), glob


def main():
    out = sys.argv[1]
    image, rel, glob = link([Obj(p) for p in sys.argv[2:]])
    open(out, 'wb').write(image)
    open(out[:-4] + '.rel', 'wb').write(rel)
    with open(out[:-4] + '.map', 'w') as f:
        for n, a in sorted(glob.items(), key=lambda x: x[1]):
            f.write(f'{a:08x} {n}\n')


if __name__ == '__main__':
    main()
