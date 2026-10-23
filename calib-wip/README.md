# Near misses not yet matched

Whole calib-style files (compile with `tools/cc-psx.sh`, check with `tools/cmpfunc.py asm/water.s ProcessWaterEffect`).
Not under `calib/`, so `tools/calibrate.sh` doesn't build them.

## ProcessWaterEffect (asm/water.s, 336 instructions)

The last unmatched game function. Every diff is inside the 20 instructions of the angle update (lines 139-154).

- `water_ProcessWaterEffect_volatile.c`: 8 lines off; needs volatile casts on the WaterAngleX store, WaterAngleY
  load and WaterAngleX reload. A 960-combination search of volatile flags and statement forms found nothing better.
- `water_ProcessWaterEffect_plain.c`: 12 lines off, plain C. The reload goes straight into AngleX (`lh s4`) where
  the original has `lhu v0` + `andi s4,v0`, and SizeX is loaded early.

Why (traced with -dS/-ds/-dl dumps against gcc 2.8.1): CSE drops the cached X value at the Y store (any struct
store invalidates struct memory), which is why the original reloads WaterAngleX. sched1 schedules backward; in plain
C `AngleY & 4095` never becomes "birthing" (AngleY is set more than once), so the X store is picked first. The orders
that reproduce the original need either the Y sum to reuse the X sum's pseudo (which then changes registers) or
AngleY to read 774 back from memory, survive both CSE passes and be removed by reload_cse: a code-free memory
invalidation or CSE block break between the Y store and the Y read, which no plain C form tried gives.
