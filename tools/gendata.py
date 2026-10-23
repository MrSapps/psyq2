#!/usr/bin/env python3
"""Generate C definitions of the game's data from the original executable.

usage: tools/gendata.py EXE OUTDIR

Writes OUTDIR/<obj>.h for every game object: the definitions of the variables
the object owns in .data/.sdata (with initialisers from EXE) and .sbss/.bss, in
address order. Ownership is by address: psylink lays each section out object by
object in link order, so a variable belongs to the object whose run it is in.
Pointers become references to the symbol they point at.
"""
import collections
import json
import os
import re
import struct
import sys

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT = 0x80018000
SECTIONS = [  # name, start, end (the game objects' part ends where the libraries' starts)
    ('.data', 0x8009a70c, 0x800ab0b4),
    ('.rdata', 0x800ab0b4, 0x800ae870),
    ('.sdata', 0x800ae870, 0x800aefa0),
    ('.sbss', 0x800b2940, 0x800b3254),
    ('.bss', 0x800b3254, 0x800ee988),
    ('endgroup.sbss', 0x800ee988, 0x800ee990),
]
BSS = ('.sbss', '.bss', 'endgroup.sbss')
# library variables SYM files under a game object that declared them
LIBRARY = {'_spu_isCalled'}

exe = open(sys.argv[1], 'rb').read()
out = sys.argv[2]
os.makedirs(out, exist_ok=True)


def rd(addr, n):
    off = addr - TEXT + 0x800
    return exe[off:off + n]


def section_of(addr):
    for s, a, b in SECTIONS:
        if a <= addr < b:
            return s
    return None


fn = json.load(open(f'{R}/src/functions.json'))
gl = json.load(open(f'{R}/src/globals.json'))
types = {(t['kind'], t['name']): t for t in json.load(open(f'{R}/src/types.json'))}

# ---- link order of the objects ----------------------------------------------
text_order = []
for f in sorted(fn, key=lambda f: f['addr']):
    if not text_order or text_order[-1] != f['obj']:
        text_order.append(f['obj'])
# objects with only data, placed by where their data is
DATA_ONLY_AFTER = {'QLevIfce.obj': 'snrelmod.obj', 'dbuffer.obj': 'main.obj', 'weapons.obj': 'quakegun.obj',
                   'Window.Obj': 'GenEfect.obj', 'skybox.obj': 'scene.obj'}
link_order = list(text_order) + ['endgroup.obj']
for o, after in DATA_ONLY_AFTER.items():
    link_order.insert(link_order.index(after) + 1, o)
rank = {o: i for i, o in enumerate(link_order)}

# ---- symbol table for pointers --------------------------------------------
syms = []  # (addr, size, name)
for f in fn:
    syms.append((f['addr'], 0, f['name']))
for line in open(f'{R}/src/symbols.txt'):
    a, n = line.split()
    if re.match(r'_\w*_(org|orgend|obj|objend|size)$', n):
        continue  # linker section/group symbols
    syms.append((int(a, 16), 0, n))

# ---- C declarations ---------------------------------------------------------
BASE = {'char': (1, True), 'signed char': (1, True), 'unsigned char': (1, False),
        'short': (2, True), 'unsigned short': (2, False), 'int': (4, True), 'unsigned int': (4, False),
        'long': (4, True), 'unsigned long': (4, False), 'unsigned': (4, False), 'void': (0, False)}


class T:
    """kind: int/ptr/array/struct/union/enum/func; for ptr .to, array .of/.n, int .size/.signed"""
    def __init__(self, kind, **kw):
        self.kind = kind
        self.__dict__.update(kw)

    def size(self):
        k = self.kind
        if k == 'int':
            return self.nbytes
        if k == 'ptr':
            return 4
        if k == 'array':
            return self.n * self.of.size()
        if k in ('struct', 'union', 'enum'):
            return types[(k, self.tag)]['size']
        raise ValueError(k)

    def align(self):
        k = self.kind
        if k == 'int':
            return self.nbytes
        if k in ('ptr', 'enum'):
            return 4
        if k == 'array':
            return self.of.align()
        if k in ('struct', 'union'):
            return max([member_type(m).align() for m in types[(k, self.tag)]['members']] or [1])
        return 4


def base_type(b):
    b = ' '.join(b.split())
    m = re.match(r'(struct|union|enum) (\w+)$', b)
    if m:
        return T(m.group(1), tag=m.group(2))
    if b in BASE:
        n, s = BASE[b]
        return T('int', nbytes=n, signed=s)
    raise ValueError('base type ' + b)


def parse_decl(decl):
    """'struct X *name[3]' -> (name, T)"""
    decl = ' '.join(decl.split())
    m = re.match(r'^((?:(?:unsigned|signed|struct|union|enum|const|volatile)\s+)*\w+)\s*(.*)$', decl)
    base, d = m.group(1), m.group(2)
    base = base.replace('const ', '').replace('volatile ', '')
    bt = base_type(base)
    # function pointer: (*name)() or (*name[n])()
    fm = re.match(r'^(\**)\s*\(\s*(\*+)\s*(\w+)\s*((?:\[\d+\])*)\s*\)\s*\(.*\)$', d)
    if fm:
        rt = bt
        for _ in fm.group(1):
            rt = T('ptr', to=rt)
        t = T('ptr', to=T('func', ret=rt))
        for _ in fm.group(2)[1:]:
            t = T('ptr', to=t)
        name = fm.group(3)
        dims = [int(x) for x in re.findall(r'\[(\d+)\]', fm.group(4))]
    else:
        dm = re.match(r'^(\**)\s*(\w+)\s*((?:\[\d*\])*)$', d)
        if not dm:
            raise ValueError('declarator ' + decl)
        t = bt
        for _ in dm.group(1):
            t = T('ptr', to=t)
        name = dm.group(2)
        dims = [int(x) if x else 0 for x in re.findall(r'\[(\d*)\]', dm.group(3))]
    for n in reversed(dims):
        t = T('array', of=t, n=n)
    return name, t


def member_type(m):
    return parse_decl(m['decl'])[1]


# ---- values -----------------------------------------------------------------
def sym_at(addr):
    """name of the symbol at addr, or (name, offset) of the one it is inside"""
    best = None
    for a, _, n in symbols_sorted:
        if a <= addr:
            best = (a, n)
        else:
            break
    return best


def literal(b):
    s = ''
    for c in b:
        ch = chr(c)
        if ch == '"':
            s += '\\"'
        elif ch == '\\':
            s += '\\\\'
        elif 32 <= c < 127:
            s += ch
        elif c == 10:
            s += '\\n'
        else:
            s += '\\%03o' % c
    return '"' + s + '"'


def string_at(addr):
    """the NUL-terminated string at addr if it looks like a string literal"""
    b = b''
    while True:
        c = rd(addr + len(b), 1)
        if not c or c == b'\0':
            break
        b += c
        if len(b) > 400:
            return None
    if all(32 <= c < 127 or c in (9, 10, 13) or c >= 0x80 for c in b):
        return b
    return None


Strings = collections.OrderedDict()
REFS = set()


def pointer(v, t):
    if v == 0:
        return '0'
    sec = section_of(v)
    if sec in ('.rdata',) or (sec == '.sdata' and named.get(v) is None and v not in data_owner):
        s = string_at(v)
        if s is not None:
            return literal(s)
    hit = sym_at(v)
    if hit is None:
        return '(void *)0x%x' % v
    a, n = hit
    REFS.add(n)
    if t.to.kind == 'func':
        if a == v:
            return n
        return '(void *)((char *)%s + %d)' % (n, v - a)
    amp = '' if n in arrays or n in funcs else '&'
    if a == v:
        return '(void *)%s%s' % (amp, n)
    return '(void *)((char *)%s%s + %d)' % (amp, n, v - a)


def value(addr, t, indent=''):
    k = t.kind
    if k == 'int':
        v = int.from_bytes(rd(addr, t.nbytes), 'little', signed=t.signed)
        return str(v)
    if k == 'enum':
        return str(struct.unpack('<i', rd(addr, 4))[0])
    if k == 'ptr':
        return pointer(struct.unpack('<I', rd(addr, 4))[0], t)
    if k == 'array':
        if t.of.kind == 'int' and t.of.nbytes == 1:
            b = rd(addr, t.n)
            z = b.rstrip(b'\0')
            if b'\0' not in z and len(z) < t.n and all(32 <= c < 127 for c in z):
                return literal(z)
            return '{ ' + ', '.join(str(int.from_bytes(bytes([c]), 'little', signed=t.of.signed)) for c in b) + ' }'
        es = t.of.size()
        items = [value(addr + i * es, t.of, indent + '    ') for i in range(t.n)]
        if t.of.kind in ('struct', 'union', 'array') and t.n > 1:
            return '{\n' + ''.join(f'{indent}    {x},\n' for x in items) + indent + '}'
        return '{ ' + ', '.join(items) + ' }'
    if k in ('struct', 'union'):
        ty = types[(k, t.tag)]
        parts = []
        for m in ty['members']:
            if m['cls'] == 18:  # bitfield: off/size in bits
                word = int.from_bytes(rd(addr + (m['off'] // 32) * 4, 4), 'little')
                parts.append(str((word >> (m['off'] % 32)) & ((1 << m['size']) - 1)))
            else:
                parts.append(value(addr + m['off'], member_type(m), indent))
            if k == 'union':
                break
        return '{ ' + ', '.join(parts) + ' }'
    raise ValueError(k)


# ---- which object owns what -------------------------------------------------
items = []  # (addr, global record)
for g in gl:
    if g['addr'] and section_of(g['addr']) and section_of(g['addr']) != '.rdata' and g['name'] not in LIBRARY:
        items.append(g)
items.sort(key=lambda g: g['addr'])
named = {g['addr']: g for g in items}

# smooth the obj tags so each section's objects come in link order
owner = {}
for s, a, b in SECTIONS:
    run = [g for g in items if a <= g['addr'] < b]
    cur = -1
    for i, g in enumerate(run):
        r = rank.get(g['obj'], -1)
        if r >= cur:
            # don't let a stray tag jump ahead if the next one goes back
            nxt = [rank.get(x['obj'], -1) for x in run[i + 1:i + 3]]
            if r > cur and nxt and all(n < r for n in nxt) and cur >= 0:
                owner[g['addr']] = link_order[cur]
                continue
            cur = r
            owner[g['addr']] = g['obj']
        else:
            owner[g['addr']] = link_order[cur]
data_owner = owner

symbols_sorted = sorted(set((a, n) for a, _, n in syms) | set((g['addr'], g['name']) for g in items))
symbols_sorted = [(a, 0, n) for a, n in symbols_sorted]
symbols_sorted = [(a, n) for a, _, n in symbols_sorted]
arrays = set()
funcs = set(f['name'] for f in fn)
for g in items:
    try:
        if parse_decl(g['decl'])[1].kind == 'array':
            arrays.add(g['name'])
    except ValueError:
        pass


def sym_at(addr):  # noqa: F811
    import bisect
    i = bisect.bisect_right(symbols_sorted, (addr, '\x7f')) - 1
    return symbols_sorted[i] if i >= 0 else None


per_obj = collections.OrderedDict((o, []) for o in link_order)
bss_end = {}
report = collections.defaultdict(list)
for g in items:
    o = owner[g['addr']]
    s = section_of(g['addr'])
    try:
        name, t = parse_decl(g['decl'])
    except ValueError as e:
        per_obj[o].append(f'/* {g["decl"]}: {e} */')
        continue
    st = 'static ' if g['storage'] == 'static' else ''
    REFS.clear()
    if s in BSS:
        # placed by hand in definition order, word aligned (see src/include/bss.h)
        prev = bss_end.get((o, s))
        mac = 'SBSS' if s in ('.sbss', 'endgroup.sbss') else 'BSS'
        if mac == 'BSS' and g['addr'] % 8 == 0 and prev is not None and (prev + 3) & ~3 != g['addr']:
            mac = 'BSS8'
        if prev is not None and (prev + (7 if mac == 'BSS8' else 3)) & ~(7 if mac == 'BSS8' else 3) != g['addr']:
            per_obj[o].append(f'/* gap before {g["name"]}: {prev:#x} .. {g["addr"]:#x} */')
        bss_end[(o, s)] = g['addr'] + t.size()
        per_obj[o].append(f'{st}{g["decl"]} {mac};')
    else:
        try:
            decl = g['decl']
            init = value(g['addr'], t)
            if s == '.data' and t.kind == 'array' and t.size() <= 8:
                # an array sized by its initialiser goes to .data even when it is small (gcc picks
                # the section before the initialiser completes the type); sized, it would be .sdata
                decl = re.sub(r'\[\d+\]', '[]', decl, count=1)
                if init.startswith('"') and len(eval('b' + init)) + 1 != t.n:
                    init = '{ ' + ', '.join(str(c) for c in rd(g['addr'], t.n)) + ' }'
            per_obj[o].append(f'{st}{decl} = {init};')
        except Exception as e:  # noqa: BLE001
            per_obj[o].append(f'/* {g["decl"]}: {e} */')
    report[o].append(dict(name=g['name'], addr=g['addr'], section=s, size=t.size() if t.kind != 'func' else 4,
                          refs=sorted(REFS)))

for o, lines in per_obj.items():
    base = o.rsplit('.', 1)[0].lower()
    open(f'{out}/{base}.h', 'w').write(
        f'/* data of {o}, generated by tools/gendata.py */\n' + '\n'.join(lines) + '\n')
json.dump(report, open(f'{out}/owners.json', 'w'), indent=1)
print(sum(len(v) for v in per_obj.values()), 'definitions for', len(per_obj), 'objects')
