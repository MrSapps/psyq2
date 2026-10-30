#!/usr/bin/env python3
"""Build the overlay modules from src/ovl/ and check them against config/overlays.txt.

usage: tools/ovlbuild.py [-j N] [NAME...]        (default: every src/ovl/NAME.c)

  src/ovl/<module>.c   gcc -E -> CC1PSX 4.4 (-O3 -G0 -g) -> ASPSX 2.81 (-G0) -> build/ovl/<module>.obj
                       tools/ovllink.py                                 -> build/ovl/<module>.bin, .rel

A module is one C file. Prints, per module, whether .bin and .rel match the sha1s that
tools/ovlextract.py recorded; with baserom/ovl/ present a mismatch also names the first
differing offset. Exit status 1 if any module fails to build or match.
Env and flags as for tools/build.py (PSYQ, ASPSX, /* calib-flags: ... */).
"""
import concurrent.futures
import glob
import hashlib
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build      # noqa: E402  (shares the toolchain paths and helpers)
import ovllink    # noqa: E402

R = build.R
OUT = os.path.join(R, 'build', 'ovl')
FLAGS = ['-O3', '-G0', '-g']
HEADERS = glob.glob(f'{R}/src/include/*.h') + glob.glob(f'{R}/src/ovl/include/*.h')


def compile_ovl(name):
    src = f'{R}/src/ovl/{name}.c'
    obj = f'{OUT}/{name}.obj'
    if not build.newer(obj, [src] + HEADERS + [__file__]):
        return 0, ''
    text = open(src).read()
    import re
    m = re.search(r'calib-flags: *([^*]*[^ *]) *\*/', text)
    flags = m.group(1).split() if m else FLAGS
    i, s = f'{OUT}/{name}.i', f'{OUT}/{name}.s'
    rc, out = build.run(['gcc', '-E', '-P', '-I', f'{R}/src/include', '-I', f'{R}/src/ovl/include',
                         '-I', f'{R}/src/ovl', src, '-o', i])
    if rc:
        return rc, out
    rc, out = build.run(['wine', f'{build.PSYQ}/bin/cc1psx.exe', '-quiet'] + flags + [i, '-o', s])
    if rc:
        return rc, '\n'.join(l for l in out.splitlines() if 'warning' not in l and l.strip())
    with open(s, 'a', newline='') as f:
        f.write('\t.data\r\n\t.align\t2\r\n\t.section .bss,"aw",@progbits\r\n\t.align\t2\r\n')
    g = next((f[2:] for f in flags if f.startswith('-G')), '0')
    rc, out = build.run(['wine', build.ASPSX, '-q', f'-G{g}', s, '-o', obj])
    return rc, out if rc else ''


def sha1(path):
    return hashlib.sha1(open(path, 'rb').read()).hexdigest()


def first_diff(a, b):
    for i in range(min(len(a), len(b))):
        if a[i] != b[i]:
            return f'{i:#x}'
    return f'size {len(a):#x} vs {len(b):#x}' if len(a) != len(b) else None


def main():
    args = sys.argv[1:]
    jobs = os.cpu_count()
    if '-j' in args:
        k = args.index('-j')
        jobs = int(args[k + 1])
        del args[k:k + 2]
    expect = {}
    for line in open(f'{R}/config/overlays.txt'):
        if line.strip() and not line.startswith('#'):
            p = line.split()
            expect[p[0]] = (p[1], p[2])
    names = args or sorted(os.path.basename(p)[:-2] for p in glob.glob(f'{R}/src/ovl/*.c'))
    os.makedirs(OUT, exist_ok=True)
    build.subprocess.run(['wineserver', '-p'], stdout=build.subprocess.DEVNULL, stderr=build.subprocess.DEVNULL, env=build.ENV)
    bad = 0
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        results = list(ex.map(compile_ovl, names))
    for name, (rc, out) in zip(names, results):
        if rc:
            print(f'{name}: compile failed\n{out}')
            bad += 1
            continue
        binp = f'{OUT}/{name}.bin'
        try:
            image, rel, glob_ = ovllink.link([ovllink.Obj(f'{OUT}/{name}.obj')])
        except ValueError as e:
            print(f'{name}: link failed: {e}')
            bad += 1
            continue
        open(binp, 'wb').write(image)
        open(binp[:-4] + '.rel', 'wb').write(rel)
        with open(binp[:-4] + '.map', 'w') as f:
            for n, a in sorted(glob_.items(), key=lambda x: x[1]):
                f.write(f'{a:08x} {n}\n')
        want = expect.get(name)
        got = (hashlib.sha1(image).hexdigest(), hashlib.sha1(rel).hexdigest())
        if want == got:
            print(f'{name}: OK')
            continue
        bad += 1
        why = 'not in config/overlays.txt' if not want else ''
        orig = f'{R}/baserom/ovl/{name}.bin'
        if want and os.path.exists(orig):
            d = first_diff(image, open(orig, 'rb').read())
            r = first_diff(rel, open(orig[:-4] + '.rel', 'rb').read())
            why = (f'.bin differs at {d}' if d else '.bin ok') + ', ' + (f'.rel differs at {r}' if r else '.rel ok')
        elif want:
            why = '.bin ' + ('ok' if want[0] == got[0] else 'differs') + ', .rel ' + ('ok' if want[1] == got[1] else 'differs')
        print(f'{name}: MISMATCH ({why})')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
