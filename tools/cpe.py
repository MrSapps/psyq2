#!/usr/bin/env python3
"""Read a PSY-Q .cpe file (psylink output).

    load(path) -> (chunks, regs)   chunks: [(addr, bytes)], regs: {reg number: value}
    image(path, start, end) -> bytes   memory start..end, zero where nothing was loaded
"""
import struct


def load(path):
    b = open(path, 'rb').read()
    if b[:4] != b'CPE\x01':
        raise ValueError(f'{path}: not a CPE file')
    p = 4
    chunks = []
    regs = {}
    while p < len(b):
        t = b[p]
        p += 1
        if t == 0:
            break
        if t == 1:          # load data
            addr, n = struct.unpack_from('<II', b, p)
            p += 8
            chunks.append((addr, b[p:p + n]))
            p += n
        elif t == 3:        # set register
            r, v = struct.unpack_from('<HI', b, p)
            p += 6
            regs[r] = v
        elif t == 8:        # select unit
            p += 1
        else:
            raise ValueError(f'{path}: unknown chunk {t} at {p - 1}')
    return chunks, regs


def image(path, start, end):
    out = bytearray(end - start)
    for addr, data in load(path)[0]:
        lo, hi = max(addr, start), min(addr + len(data), end)
        if lo < hi:
            out[lo - start:hi - start] = data[lo - addr:hi - addr]
    return bytes(out)
