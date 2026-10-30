#!/usr/bin/env python3
"""Extract the overlays (relocatable code modules) from the level files of the disc.

usage: ovlextract.py DISC.bin [OUTDIR]       (OUTDIR defaults to baserom/ovl)

The overlays are not files of their own: each level's COMMON.DAT holds a level module
(LevelBin + LevelRel), and COMMON.DAT and the ZONEn.DATs hold creature AI modules (CreAIBin +
CreAIRel), see ReadCommonFiles/ReadZoneFiles (loadlev.c) and LoadRelocateCreAi (qents.c).
A .DAT is a FILEHDR table ({IDENTITYNAME Name; long Offset}, ended by an entry whose offset
is the end of the data). CreAIBin and CreAIRel are lists of {IDENTITYNAME; int next} + data,
ended by a zero word. The .rel is __SN_RELOCATE's input.

Writes OUTDIR/<module>.bin and .rel, one per distinct module, and prints the list
config/overlays.txt keeps: module, sha1 of .bin and .rel, and the level files that have it.
Level modules are level_<level>; one module many levels share is named after the first of
them. Creature modules are cre_<name>; a second build of one is cre_<name>_<first level>.
"""
import collections
import hashlib
import os
import struct
import sys

SECTOR, HEADER = 2352, 24  # raw MODE2 sector: 12 sync + 4 header + 8 subheader


class Disc:
    def __init__(self, path):
        self.f = open(path, 'rb')

    def sec(self, n):
        self.f.seek(n * SECTOR + HEADER)
        return self.f.read(2048)

    def read(self, lba, size):
        return b''.join(self.sec(lba + i) for i in range((size + 2047) // 2048))[:size]

    def walk(self, lba=None, size=None, path=''):
        if lba is None:
            root = self.sec(16)[156:190]
            lba, size = struct.unpack_from('<I', root, 2)[0], struct.unpack_from('<I', root, 10)[0]
        d = self.read(lba, size)
        i = 0
        while i < len(d):
            n = d[i]
            if n == 0:
                i = (i // 2048 + 1) * 2048
                continue
            r = d[i:i + n]
            i += n
            name = r[33:33 + r[32]]
            if name in (b'\0', b'\1'):
                continue
            name = name.decode('latin-1').split(';')[0]
            el, es = struct.unpack_from('<I', r, 2)[0], struct.unpack_from('<I', r, 10)[0]
            if r[25] & 2:
                yield from self.walk(el, es, path + name + '/')
            else:
                yield path + name, el, es


def name12(b):
    return b[:12].split(b'\0')[0].decode('latin-1')


def entries(d):
    out = {}
    i = 0
    while True:
        name, off = name12(d[i:i + 12]), struct.unpack_from('<i', d, i + 12)[0]
        nxt = struct.unpack_from('<i', d, i + 28)[0]
        if not name:
            return out
        out[name] = d[off:nxt]
        i += 16


def main():
    disc = Disc(sys.argv[1])
    outdir = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(__file__), '..', 'baserom', 'ovl')
    os.makedirs(outdir, exist_ok=True)
    mods = collections.OrderedDict()   # (kind name, sha1) -> [bin, rel, where]

    def add(name, code, rel, where):
        key = (name, hashlib.sha1(code + rel).hexdigest())
        mods.setdefault(key, [code, rel, []])[2].append(where)

    for path, lba, size in sorted(disc.walk()):
        parts = path.split('/')
        if len(parts) != 4 or parts[:2] != ['Q2DATA', 'LEVELS'] or parts[3] == 'SNDVRAM.DAT' \
                or not parts[3].endswith('.DAT'):
            continue
        level, where = parts[2].lower(), f'{parts[2]}/{parts[3]}'
        e = entries(disc.read(lba, size))
        if len(e.get('LevelBin', b'')) > 4:
            add('level', e['LevelBin'], e['LevelRel'], where)
        cb, cr = e.get('CreAIBin', b''), e.get('CreAIRel', b'')
        c = r = 0
        while len(cb) > 4 and struct.unpack_from('<i', cb, c)[0]:
            nm = name12(cb[c:c + 12])
            assert name12(cr[r:r + 12]) == nm, where
            cn, rn = struct.unpack_from('<i', cb, c + 12)[0], struct.unpack_from('<i', cr, r + 12)[0]
            add('cre_' + nm.lower(), cb[c + 16:cn], cr[r + 16:rn], where)
            c, r = cn, rn

    # one name per distinct module
    byhash = collections.OrderedDict()
    for (kind, h), (code, rel, where) in mods.items():
        if kind == 'level':
            if h in byhash:
                byhash[h][3].extend(where)
                continue
            name = 'level_' + where[0].split('/')[0].lower()
        else:
            name = kind
        byhash[h] = [name, code, rel, list(where)]
    count = collections.Counter(v[0] for v in byhash.values())
    seen = set()
    for h, v in byhash.items():
        if count[v[0]] > 1:
            if v[0] in seen:
                v[0] += '_' + v[3][0].split('/')[0].lower()
            seen.add(v[0])
    for h, (name, code, rel, where) in byhash.items():
        open(f'{outdir}/{name}.bin', 'wb').write(code)
        open(f'{outdir}/{name}.rel', 'wb').write(rel)
        print(f'{name:24s} {hashlib.sha1(code).hexdigest()} {hashlib.sha1(rel).hexdigest()} {" ".join(where)}')


if __name__ == '__main__':
    main()
