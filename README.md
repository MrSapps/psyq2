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

    src/<obj>.c        gcc -E -> CC1PSX 4.4 (-O3 -G8 -g) -> ASPSX 2.81    -> build/obj/<obj>.obj
    src/hand_asm.s, src/libsn/*.s, src/libbss/*.s       ASPSX 2.81       -> build/obj/...
    PSY-Q 4.5 libraries                                 PSYLIB /x        -> build/lib/<lib>/*.OBJ
    config/SLUS_007.lnk                                 PSYLINK          -> build/SLUS_007.cpe
                                                        CPE2EXE          -> build/SLUS_007.57

## Layout

- `src/<obj>.c`     one file per object of the original link: `event.c` is EVENT.C and QEVENT.C (QEVENT is
                    included into it), `models.c` is QUATANIM.C and MODELS.C. Data-only objects (`qlevifce`,
                    `dbuffer`, `weapons`, `window`, `skybox`, `endgroup`) are C files with just their data.
- `src/include/`    `types.h` and `globals.h` (decoded from `MAIN.SYM`), `bss.h`
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
