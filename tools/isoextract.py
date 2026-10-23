#!/usr/bin/env python3
"""Extract one root-directory file from a raw 2352-byte-sector (MODE2) PSX .bin.

usage: isoextract.py DISC.bin FILENAME OUTFILE     e.g. ... "Quake II (USA) (Track 1).bin" MAIN.SYM MAIN.SYM
"""
import struct
import sys

SECTOR, HEADER = 2352, 24  # 12 sync + 4 header + 8 subheader


def main():
    disc, want, out = sys.argv[1:]
    f = open(disc, "rb")

    def sec(n):
        f.seek(n * SECTOR + HEADER)
        return f.read(2048)

    root = sec(16)[156:190]
    lba, ln = struct.unpack("<I", root[2:6])[0], struct.unpack("<I", root[10:14])[0]
    d = b"".join(sec(lba + i) for i in range((ln + 2047) // 2048))
    i = 0
    while i < len(d):
        n = d[i]
        if n == 0:
            i = (i // 2048 + 1) * 2048
            continue
        r = d[i:i + n]
        i += n
        name = r[33:33 + r[32]].decode("latin1").split(";")[0]
        if name.upper() == want.upper():
            el, es = struct.unpack("<I", r[2:6])[0], struct.unpack("<I", r[10:14])[0]
            open(out, "wb").write(b"".join(sec(el + k) for k in range((es + 2047) // 2048))[:es])
            print(f"{name}: {es} bytes -> {out}")
            return
    sys.exit(f"{want} not found in root directory")


if __name__ == "__main__":
    main()
