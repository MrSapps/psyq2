#!/usr/bin/env python3
"""Make PSY-Q 4.0/4.1 CC1PSX.EXE usable under wine.

usage: patch-cc1psx-tmp.py SRC_CC1PSX.EXE DST_CC1PSX.EXE

These builds write a temp file to "<TMPDIR>/ctaXXXXX" and fall back to "\\" when TMPDIR is unset.
Wine reads the resulting "\\/cta00032" as a UNC path and the compile fails, and setting TMPDIR
breaks wine itself. This rewrites the "\\" fallback to "." so the temp file goes in the current
directory. Write the result to a copy of the SDK; the original stays untouched.
"""
import sys

src, dst = sys.argv[1:3]
d = bytearray(open(src, "rb").read())
i = d.find(b"TMPDIR\0\0\\\0\0\0\\\0")
if i < 0:
    sys.exit("fallback temp dir not found (not a 4.0/4.1 CC1PSX.EXE?)")
d[i + 8] = d[i + 12] = ord(".")
open(dst, "wb").write(d)
print(f"patched temp dir fallback at {i + 8:#x}")
