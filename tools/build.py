#!/usr/bin/env python3
"""Build SLUS_007.57 from src/ with the original PSY-Q toolchain (under wine).

usage: tools/build.py [-j N] [--only NAME...] [--link-only]
  --only: just compile those src/NAME.c, no link; --link-only: link what is in build/obj

  src/<obj>.c   cpp -> CC1PSX (-O3 -G8 -g) -> ASPSX 2.81 -> build/obj/<obj>.obj
  src/hand_asm.s, src/libsn/*.s, src/libbss/*.s   ASPSX -> build/obj/...
  the PSY-Q libraries' modules              PSYLIB /x  -> build/lib/<lib>/<module>.OBJ
  config/SLUS_007.lnk + the PSY-Q libraries -> PSYLINK  -> build/SLUS_007.cpe (+ .map, .sym)
  build/SLUS_007.cpe                        -> CPE2EXE  -> build/SLUS_007.57

Env: PSYQ   SDK whose bin/ has the compiler and linker (default ../psyq_sdk/psyq_4.4)
     PSYQ_LIB  SDK whose lib/ has the libraries (default ../psyq_sdk/psyq_4.5)
     ASPSX  the assembler (default ../psyq_sdk/aspsx/2.81/ASPSX.EXE)
A file whose original was built with other flags names them in a
/* calib-flags: ... */ comment.
"""
import concurrent.futures
import glob
import os
import re
import shutil
import subprocess
import sys

R = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PSYQ = os.environ.get('PSYQ', os.path.join(os.path.dirname(R), 'psyq_sdk', 'psyq_4.4'))
PSYQ_LIB = os.environ.get('PSYQ_LIB', os.path.join(os.path.dirname(R), 'psyq_sdk', 'psyq_4.5'))
ASPSX = os.environ.get('ASPSX', os.path.join(os.path.dirname(R), 'psyq_sdk', 'aspsx', '2.81', 'ASPSX.EXE'))
B = os.path.join(R, 'build')
OBJ = os.path.join(B, 'obj')
FLAGS = ['-O3', '-G8', '-g']
ENV = dict(os.environ, WINEDEBUG='-all')


def winpath(p):
    return 'Z:' + os.path.abspath(p).replace('/', '\\')


def run(cmd, cwd=None):
    p = subprocess.run(cmd, capture_output=True, text=True, env=ENV, cwd=cwd)
    out = (p.stdout + p.stderr).replace('\r', '')
    return p.returncode, out


def crlf(src, dst):
    # the SN tools want DOS line endings
    with open(src) as f, open(dst, 'w', newline='\r\n') as g:
        g.write(f.read())


def newer(target, deps):
    if not os.path.exists(target):
        return True
    t = os.path.getmtime(target)
    return any(os.path.getmtime(d) > t for d in deps)


HEADERS = glob.glob(f'{R}/src/include/*.h') + glob.glob(f'{R}/include/*.h')


def compile_c(name):
    src = f'{R}/src/{name}.c'
    obj = f'{OBJ}/{name}.obj'
    if not newer(obj, [src] + HEADERS + [__file__]):
        return name, 0, ''
    text = open(src).read()
    m = re.search(r'calib-flags: *([^*]*[^ *]) *\*/', text)
    flags = m.group(1).split() if m else FLAGS
    i, s = f'{OBJ}/{name}.i', f'{OBJ}/{name}.s'
    rc, out = run(['gcc', '-E', '-P', '-I', f'{R}/src/include', '-I', f'{R}/include', src, '-o', i])
    if rc:
        return name, rc, out
    rc, out = run(['wine', f'{PSYQ}/bin/cc1psx.exe', '-quiet'] + flags + [i, '-o', s])
    errors = '\n'.join(l for l in out.splitlines() if 'warning' not in l and l.strip())
    if rc:
        return name, rc, errors
    rc, out2 = run(['wine', ASPSX, '-q', '-G8', s, '-o', obj])
    return name, rc, out2 if rc else ''


def assemble(src, obj, g='0'):
    if not newer(obj, [src]):
        return 0, ''
    s = obj[:-4] + '.s'
    crlf(src, s)
    return run(['wine', ASPSX, '-q', f'-G{g}', s, '-o', obj])


def main():
    jobs = int(sys.argv[sys.argv.index('-j') + 1]) if '-j' in sys.argv else os.cpu_count()
    os.makedirs(OBJ, exist_ok=True)
    run(['wineserver', '-p'])  # keep wine up between runs: much faster
    names = [l.split()[0] for l in open(f'{R}/config/link_order.txt') if l.strip()]
    only = sys.argv[sys.argv.index('--only') + 1:] if '--only' in sys.argv else None
    if only:
        names = [n for n in names if n in only]
    failed = False
    if '--link-only' in sys.argv:
        names = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        for name, rc, out in ex.map(compile_c, names):
            if rc:
                print(f'{name}.c: failed\n{out}')
                failed = True
    if only:
        sys.exit(1 if failed else 0)
    for d in ('libsn', 'libbss'):
        os.makedirs(f'{OBJ}/{d}', exist_ok=True)
    for src in [f'{R}/src/hand_asm.s'] + sorted(glob.glob(f'{R}/src/libsn/*.s') + glob.glob(f'{R}/src/libbss/*.s')):
        obj = os.path.join(OBJ, os.path.relpath(src, f'{R}/src'))[:-2] + '.obj'
        rc, out = assemble(src, obj)
        if rc:
            print(f'{src}: failed\n{out}')
            failed = True
    # the library modules the link file names
    for lib in sorted(set(re.findall(r'"lib\\(\w+)\\', open(f'{R}/config/SLUS_007.lnk').read()))):
        d = f'{B}/lib/{lib}'
        if not os.path.isdir(d):
            os.makedirs(d)
            shutil.copy(f'{PSYQ_LIB}/lib/{lib}.lib', d)
            rc, out = run(['wine', f'{PSYQ}/bin/psylib.exe', '/x', f'{lib}.lib'], cwd=d)
            os.remove(f'{d}/{lib}.lib')
        if not os.path.exists(f'{B}/lib/{lib}.lib'):
            shutil.copy(f'{PSYQ_LIB}/lib/{lib}.lib', f'{B}/lib')
            if rc:
                print(f'{lib}.lib: extract failed\n{out}')
                failed = True
    if failed:
        sys.exit(1)

    lnk = f'{B}/SLUS_007.lnk'
    crlf(f'{R}/config/SLUS_007.lnk', lnk)
    rc, out = run(['wine', f'{PSYQ}/bin/psylink.exe', '/c', '/n', '4000', '/q', '/gp', '.sdata',
                   '/m',
                   '@SLUS_007.lnk,SLUS_007.cpe,SLUS_007.sym,SLUS_007.map'], cwd=B)
    if rc or 'Error' in out:
        print(out)
        sys.exit(1)
    if os.path.exists(f'{B}/SLUS_007.57'):
        os.remove(f'{B}/SLUS_007.57')
    rc, out = run(['wine', f'{PSYQ}/bin/cpe2exe.exe', '-CA', 'SLUS_007.cpe'], cwd=B)
    exe = f'{B}/SLUS_007.exe'
    if rc or not os.path.exists(exe):
        print(out)
        sys.exit(1)
    shutil.move(exe, f'{B}/SLUS_007.57')
    rc, out = run(['sha1sum', '-c', f'{R}/config/SLUS_007.57.sha1'], cwd=B)
    print(out.strip())
    sys.exit(rc)


if __name__ == '__main__':
    main()
