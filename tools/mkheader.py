#!/usr/bin/env python3
"""Write the 0x800-byte PS-X EXE header.

usage: mkheader.py OUT TEXT_ADDR TEXT_SIZE PC0 SP
"""
import struct
import sys

out = sys.argv[1]
t_addr, t_size, pc0, sp = (int(x, 0) for x in sys.argv[2:6])
h = bytearray(0x800)
h[0:8] = b"PS-X EXE"
# pc0, gp0, t_addr, t_size, d_addr, d_size, b_addr, b_size, s_addr, s_size
struct.pack_into("<10I", h, 0x10, pc0, 0, t_addr, t_size, 0, 0, 0, 0, sp, 0)
region = b"Sony Computer Entertainment Inc. for North America area"
h[0x4c:0x4c + len(region)] = region
open(out, "wb").write(h)
