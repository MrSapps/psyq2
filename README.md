# Quake II (PS1, USA) decompilation

Target: `SLUS_007.57` (PS-X EXE, loaded at `0x80018000`, entry `0x800861a4`).
`make` builds it from the C in `src/` with the original PSY-Q toolchain and the PSY-Q libraries, and
checks that its SHA-1 matches `config/SLUS_007.57.sha1`. **It does: the build is byte-identical.**
All 1037 game functions are C; the only assembly is what the original had as assembly (`src/hand_asm.s`)
and four libsn modules from a release we don't have (`src/libsn/`).

## Build

Needs wine, a host `gcc` (only its preprocessor) and Python 3. The SDKs are those of
[FoxdieTeam/psyq_sdk](https://github.com/FoxdieTeam/psyq_sdk), as for mgs_reversing, cloned next to this repo:

    git clone https://github.com/FoxdieTeam/psyq_sdk ../psyq_sdk
    make                    # tools/build.py: compile, link, cpe2exe, SHA-1
    make check              # per-object code and data comparison with baserom/SLUS_007.57

`tools/build.py --only NAME...` compiles just those `src/NAME.c`; `--link-only` relinks. `PSYQ`, `PSYQ_LIB`
and `ASPSX` override the SDK paths. `make check` and the tools below need the original executable in
`baserom/SLUS_007.57` (ignored by git; `tools/isoextract.py` gets it and `MAIN.SYM` from the disc).

The pipeline, per object of the original link (`config/link_order.txt`):

    src/<obj>.c        CPPPSX -> CC1PSX 4.4 (-O3 -G8 -g) -> ASPSX 2.81    -> build/obj/<obj>.obj
    src/hand_asm.s, src/libsn/*.s, src/libbss/*.s       ASPSX 2.81       -> build/obj/...
    PSY-Q 4.5 libraries                                 PSYLIB /x        -> build/lib/<lib>/*.OBJ
    config/SLUS_007.lnk                                 PSYLINK          -> build/SLUS_007.cpe
                                                        CPE2EXE          -> build/SLUS_007.57

## Layout

- `src/<obj>.c`     one file per object of the original link: `event.c` is EVENT.C and QEVENT.C (QEVENT is
                    included into it), `models.c` is QUATANIM.C and MODELS.C. Data-only objects (`qlevifce`,
                    `dbuffer`, `weapons`, `window`, `skybox`, `endgroup`) are C files with just their data.
- `src/include/`    `types.h` and `globals.h` (decoded from `MAIN.SYM`, see Headers), `bss.h`
- `src/hand_asm.s`  the original's hand-written assembly (section `CODE`)
- `src/libsn/`      SNMAIN, `__sn_cpp_structors`, PCread and PCwrite, rebuilt from the original
- `src/libbss/`     the library variables (commons) the original link placed between library modules
- `src/*.json`, `src/symbols.txt`, `src/symbol_addrs.txt`   debug info decoded by `tools/symdump.py`
- `config/`         `SLUS_007.lnk` (psylink command file), `link_order.txt`, `symbol_addrs.txt`, the hash
- `tools/`          `build.py`, and the comparison tools below

## Comparing with the original

- `tools/objcmp.py EXE OBJ...` compares each object's code with its place in the executable, ignoring
  the fields the linker patches. It is the authority for code: all 80 objects match.
- `tools/tudata.py EXE OBJ...` checks one object's .data/.rdata/.sdata bytes and the layout of its
  .sbss/.bss variables, so a data or bss problem shows in the object that has it, not as a shift of
  everything after it. Known false reports: `event` (two `Sound_Explosion`s), `models`/`loadlev` (commons
  of QLevIfce's variables), `cdaudio` (`Trax`, see below), `qmpower` (anchor).
- `tools/datacmp.py EXE config/link_order.txt build/obj` compares the data sections cumulatively in link order.
- `tools/exediff.py` diffs a build with the original by section and lists where symbols first move.

## Overlays

The game's other code is in 51 relocatable modules inside the level files: each level's
`COMMON.DAT` has a level module (`LevelBin`/`LevelRel`), and `COMMON.DAT` and the `ZONEn.DAT`s have
creature AI modules (`CreAIBin`/`CreAIRel`), loaded by `ReadCommonFiles`/`ReadZoneFiles` and relocated
by `__SN_RELOCATE`. They have no debug info. The engine reaches a module through the interface struct
at its start (`QLEVELINTERFACE`, `QCREINTERFACE`); the module reaches the engine only through the
function pointers the engine fills in there (`LevelInterface.Engine->...`, `Iface.rand(...)`).

    python3 tools/ovlextract.py "Quake II (USA) (Track 1).bin"   # -> baserom/ovl/<module>.bin, .rel
    make ovl                                                     # build src/ovl/*.c, compare with config/overlays.txt
    python3 tools/ovldis.py baserom/ovl/<module>.bin             # disassembly with the relocations as labels

- `config/overlays.txt` lists every module, the sha1s of its image and relocations, and the level files that hold it.
- `src/ovl/<module>.c` is one module. Same compiler as the executable but **`-O3 -G0 -g`** (a module has no `$gp`).
- `tools/ovllink.py` links a module: `.header` (the interface, `OVL_HEADER` in `src/ovl/include/ovl.h`), then
  `.rdata`, `.text`, `.data`, `.bss`, from address 0. The image holds 0 in every hi16 and the target's low half
  in every lo16; the `.rel` lists the relocations in the order of the object's patches, a hi16 followed by its target.
- Some modules also link PSY-Q library modules (libpress's MDEC code, `memset`, ...) after their own object;
  the source names them in an `/* ovl-libs: libpress/LIBPRESS ... */` comment and `tools/ovlbuild.py` links
  the real objects from the PSY-Q 4.5 libraries, whose patch order the `.rel` keeps.
- The interface is a plain writable variable: `const` lets gcc keep its fields in registers across calls,
  which the originals don't.
- `lib/MODULE@SIZE` in `ovl-libs` cuts the module's `.text` to SIZE bytes (zero padding only, checked).
  level_biggun's RAND is 0x3c bytes in the original and 0x40 (the same code and a trailing `nop`) in
  PSY-Q 4.3 to 4.5.
- The movie-playing modules (level_qfront, level_qendmis1, level_qdummy, level_qmagextr) link libpress,
  whose LIBPRESS.OBJ calls `printf`, `ResetCallback` and `DMACallback`. A module can't link libetc or
  libc, so it defines them itself (`printf` empty, the others through the engine's pointers).

### Overlay workarounds

Each is commented where it is:

- **Stray bytes.** Some originals have non-zero bytes in the `.rdata` alignment padding after the last
  string (garbage left by their assembler), reproduced with a `__asm__(".rdata\n\t.byte ...")`.
- **Unused space.** `.data`, `.bss` and stack frames have room no surviving code uses: `Unused*`
  variables and locals hold it.
- **`.bss` alignment.** cre_gunner, cre_infantry, cre_strider and cre_soldier_magdemo start `.bss` 4
  bytes later than their data ends (8-aligned, which other modules aren't), filled with a `.data` word,
  `BssAlign`.
- **bss order.** Uninitialised statics use the `BSS` section attribute (`bss.h`): ASPSX's `.lcomm`
  aligns 8-byte statics to 8 and bigger ones to 16, which the originals don't.
- **Functions still in assembly** (the C is under `#ifdef NONMATCHING`, with what was tried):
  - cre_boss1's `PreInit_Supertank`: sched1 puts a load of an uninitialised variable, passed as a
    stray extra argument, into `$a3` last, because `$a3` is also set by three 4-argument calls, so the
    load isn't boosted.

## A native CC1PSX

CC1PSX is stock gcc 2.8.1 for MIPS with `-mgas -msplit-addresses -msoft-float -funsigned-char` as
defaults, built for a 32-bit host. `tools/build-cc1.sh GCC_2.8.1_SOURCE` builds the same compiler for
Linux into `build/cc1/` (with the host fixes that takes, see the script), and `tools/cc1n` runs it with
those flags: every function of every overlay module and of 78 of the 79 objects of the executable
compiles to the same assembly as under wine (draw.c's `DrawTimeBar` is scheduled differently). It is
much faster than wine and has the gcc 2.8.1 source behind it: RTL dumps (`-da`), gdb, printf in a
pass, decomp-permuter. Matches are still checked with CC1PSX. It takes a preprocessed file (the build
leaves one as `build/obj/<obj>.i`, `build/ovl/<module>.i`); the ECOFF target has no named sections, so
strip the `section` attributes first.

## Toolchain

The game was built with the **`cc1psx` of PSY-Q 4.4 (gcc 2.8.1 SN32 build 4.0.0010), `-O3 -G8 -g`,
assembled by an ASPSX of version 2.80 or later** (2.81, from PSY-Q 4.5), and linked with the **PSY-Q 4.5
libraries**, `libc` (not `libc2`). SNRELMOD.OBJ alone was built at `-O0`; a file built with other flags
names them in a `/* calib-flags: ... */` comment.

| cc1psx                  | -O1 | -O2     | -O3     | -O3 -G0 | -O3 -G4 |
|-------------------------|-----|---------|---------|---------|---------|
| 4.0, 4.1 (cygnus 2.7.2) | 2/8 | 2/8     | 2/8     | 1/8     | 2/8     |
| 4.3 (gcc 2.8.0)         | 4/8 | (worse) | 885/959 | 5/8     | 8/8     |
| 4.4 (gcc 2.8.1)         | 4/8 | 837/959 | 959/959 | 5/8     | 8/8     |
| 4.5 (egcs 1.1.2)        | 4/8 | 4/8     | 4/8     | 3/8     | 4/8     |
| 4.6 (gcc 2.95.2)        | 2/8 | 2/8     | 2/8     | 1/8     | 2/8     |

- **4.4, not 4.3.** 74 functions match only with 4.4. All of them pass a byte-aligned struct by value (a
  12-byte `IDENTITYNAME`, a 4-byte `COLOUR`), whose temporaries 4.3 allocates and schedules differently.
- **`-O3`.** 122 functions need a function defined earlier in the file inlined into them, without `inline`.
  `-O3` also sets the function order: gcc 2.8 emits every inline candidate (insn limit `8 * (8 + nparams)`)
  at the end of the file, in the order of first declaration. So each file has its functions in line
  order, then the small ones in prototype order, which is what the prototype block at the top of each
  file reproduces (implicit declarations don't count).
- **`-g`** moves where CSE flushes its table (`InitialisePlayerSounds` only matches with it).
- **`-G8`**, and `$gp` only for variables the file defines: no ASPSX makes an extern `$gp`-relative. A
  variable is defined in the file that SYM says owns it (`obj` in `src/globals.json`) and `extern`
  elsewhere, except where the original reached another object's variable through `$gp`: then the file
  has a tentative definition, a small common.
- **ASPSX >= 2.80** gives `addiu $a0,$gp,%gp_rel(X)` for the address of a small variable (2.77/2.79
  give `lui`/`addiu`). It also adds the divide-by-zero and overflow checks to every divide.

`char` is unsigned. Some SYM types are wrong where the code says otherwise (fields loaded with `lb` are
signed char, some `int`s are `short`); the source casts or redeclares around them.

## Headers

`types.h` and `globals.h` are generated from `MAIN.SYM` by `tools/symdump.py`, except for the types the
PSY-Q headers define (`SDK_HEADERS` there): `types.h` includes those headers (`libgte.h`, `libgpu.h`,
`libcd.h`, `libspu.h`, `libsnd.h`, `libpress.h`, `kernel.h`, `libmcrd.h`, ...) from the PSY-Q 4.4 SDK
instead of redefining them, and the code names their types the SDK's way, `RECT` and `POLY_FT4` (the
SDK's are typedefs of untagged structs, which SYM names after the typedef). Library functions are
declared by the SDK headers, never by hand: a file includes `<memory.h>`, `<rand.h>`, `<libetc.h>`, ...
for those `types.h` doesn't bring in. Every object matches with the SDK's prototypes.

The preprocessor is the SDK's CPPPSX, run with the predefines CCPSX gives it for C (`tools/build.py`'s
`CPP`). `_LANGUAGE_C` matters: without it `asm.h`, which `kernel.h` includes, defines the register
names (`sp`, `v0`, ...) as macros.

The prototypes left in the sources are the files' own functions: the block at the top of a file sets
the order in which gcc emits the small ones (see Toolchain), and others declare a function or table used
before its definition. A prototype that changed neither was removed.

The executable's files declare the game's own variables and other objects' functions themselves
(`extern ...` near their use), not through `globals.h`. That is how the original was written: what
matches is per-file declarations that disagree with SYM and with each other. `tppalx` is
`unsigned short` in draw.c (SYM: `short`), a file calls `SeekL` and `HideObjWithSolid` with no
prototype in scope (SYM's `short` parameters would change the calls), some `int` functions are
`void` where they're called, and `DoDamage` is called with four arguments and with five. One
generated header can't reproduce that. (A definition after an `extern` declaration also turns into
a common in gcc 2.8, losing the `.sbss`/`.bss` section attribute that `bss.h` relies on.)
`globals.h` stays a reference.

## Data

- `x[] = {...}` goes to `.data`, `x[N] = {...}` of 8 bytes or less to `.sdata`, as do strings of up to 8
  bytes. Constant `IDENTITYNAME`s are compound literals, `(struct IDENTITYNAME){"Default"}`: 12 bytes of
  `.rdata`, merged when equal.
- The original has data that no surviving code uses: literals and constants of functions compiled out
  or lost. A never-called `static __inline__` function emits its literals, constants and statics but no
  code, so such stand-ins, each commented, supply them where the original has them.
- **bss** (`src/include/bss.h`). The original link laid out each object's uninitialised variables in
  definition order, word-aligned. psylink 2.73 allocates commons in hash order, and after a common of 8
  bytes or more it 8-aligns every later bss piece. So the variables each object owns are defined into
  `.sbss`/`.bss` with section attributes (`SBSS`, `BSS`), and there are no commons of 8 bytes or more:
  the library's ones are defined in `src/libbss/`. gcc drops the attribute from a definition that follows
  a declaration, and an `.lcomm` static next to attributed variables lands at offset 0 in ASPSX. Tightly
  packed variables (`qfr_sing`'s `Time*`, `cdaudio`'s `actcdvol`) use `section` without `aligned`.
- The original objects' `.data` and `.bss` were whole words long (SNMAIN's `__datalen`/`__bsslen` are
  the sums of the pieces), so `tools/build.py` pads them.

## Libraries

psylink resolves one library at a time, so `config/SLUS_007.lnk` includes every library module
explicitly, in the original order, with the data-only modules where the original had them (CSTBL before
RATAN, SQRTBL before SPU, S_M before S_N2P, S_RMP0/S_RMP2/ZEROBUF after PADIF); `inclib` lines stay as a
fallback. The original's libsn was a release that is in none of the SDKs (its `__main` calls
`__sn_cpp_structors` instead of running the constructors itself), so SNMAIN, `__sn_cpp_structors`,
PCread and PCwrite are rebuilt in `src/libsn/`. `/gp .sdata` makes psylink put `__SN_GP_BASE` at the
start of `.sdata`.

ASPSX wants CRLF, numeric registers, GTE instructions as `.word`s (it pads `cop2` with nops), `b` as
`beq $0,$0` and two-operand `divu`. psylink 2.82 reads the `PSYLINK` environment variable as options.

## Matching techniques

- **Inline helpers.** SYM shows an inlined callee only as a nested `{ }` block of its locals. Recreate it
  as a small `static` function defined before its callers; larger ones need `static __inline__`.
- **Loops.** `while (1) { if (!x) break; ... }` keeps an exit test at the top (`for (;;)` and `while (x)`
  get rotated). A count-down loop is often gcc's reversal of `for (i = 0; i < n; i++)`.
- **Constant structs.** `*&(struct COLOUR){..}` gives the original's anonymous constants; `(struct X){0}`
  as a return value gives its `memset`.
- **Register allocation.** Declaration order sets the operand order of commutative operations. Try
  statement order, a temporary, nested `if`s instead of `&&`, a narrower or signed type, or `volatile` where
  the original reloads.
- **GTE.** PSY-Q `inline_c.h`-style macros with one `asm` statement per instruction (see `misc.c`),
  with a `"memory"` clobber where the original reloads between them.
- **Switches.** `objcmp.py` ignores relocated fields, so a switch with its case labels on the wrong bodies
  still matches there when the bodies have the same shape; the jump table in `.rdata` shows it (`DoMix`).

Three matches use contrived forms, commented in place: `GameCheckPaused` (an asm-label alias of `Paused`,
the only way found to stop gcc cross-jumping two returns the original keeps apart), `GameInitStuff`
(nested `do { } while (0)` to raise one value's allocation weight) and `ProcessWaterEffect` (two constant
locals for its loop steps, whose set insns change sched1's pick order before reload deletes them).
