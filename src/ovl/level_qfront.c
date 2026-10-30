/* level_qfront: the front end (title, menus, options, credits, the movie player). */
/* ovl-libs: libgpu/P00 libgpu/P33 libpress/LIBPRESS libpress/VLC_C libpress/BUILD libpress/DCT002 */
#include "ovl.h"

/* TODO: the functions not decompiled yet are their original instructions, generated into
 * level_qfront_asm.h (with the strings and jump tables they own):
 *   ASM_FUNC(f)      a function gcc emits where it is defined
 *   ASM_RDATA(f)     the .rdata a not-yet-C small function owns, at its place in the file
 *   ASM_DEFERRED(f)  a not-yet-C small function, as a C function whose body is its instructions
 *                    but for the final "jr $31" (gcc emits the small ones, inline candidates,
 *                    after everything else in prototype order).
 * Labels: LevelInterface, the C names below, S_<addr> for .rdata, Lq_<addr> inside functions. */
#include "level_qfront_asm.h"
#define ASM_FUNC(name) ASM_##name
#define ASM_RDATA(name) ASMR_##name
/* uninitialised data goes to .bss in definition order (no commons, no .lcomm) */
#define BSS __attribute__((section(".bss")))

/* a char field the original reads as signed */
#define SCHAR(x) (*(signed char *)&(x))

/* MODWIN's first byte of flags, accessed as a word */
struct MODWINFLAGS {
    unsigned int GotPackets : 2;
    unsigned int GotOpaquePackets : 2;
    unsigned int UnUsed : 3;
    unsigned int NoDraw : 1;
};
#define WINFLAGS(cre, w) ((struct MODWINFLAGS *)&(cre)->WinData[w])

/* the word at gFrontEndPadShot, with gFrontEndLoadSubType and gFrontEndLoadIsSpecial as the int
 * bitfields this module was built with (word accesses) */
struct LOADFLAGS {
    unsigned int PadShot : 16;
    unsigned int LoadType : 8;
    unsigned int LoadSubType : 7;
    unsigned int LoadIsSpecial : 1;
};
#define LOADFLAGS(e) ((struct LOADFLAGS *)&(e)->gFrontEndPadShot)

/* libgpu.h */
#define setRECT(r, _x, _y, _w, _h) ((r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h))

/* libpress (linked after this file) */
void DecDCTReset(int mode);
int DecDCToutCallback(void (*func)());
int DecDCTvlc2(unsigned long *bs, unsigned long *buf, unsigned short *table);
int DecDCTBufSize(unsigned long *bs);

/* the strplay sample's decode environment (as in level_qendmis1) */
struct DECENV {
    unsigned long *vlcbuf[2];   /* 0x0 */
    unsigned short *imgbuf;     /* 0x8 */
    int vlcid;                  /* 0xc */
    struct RECT rect[2];        /* 0x10 */
    int rectid;                 /* 0x20 */
    struct RECT slice;          /* 0x24 */
    int isdone;                 /* 0x2c */
};
/* the strplay variables, over the .bss words they occupy */
#define StrLoc ((struct CdlLOC *)D_12a68)
#define StrEndFlag D_12a98[0]
#define StrDec (*(struct DECENV *)D_12aa8)
#define StrRingBuf ((unsigned long *)D_12ad8[0])
#define StrVlcTable ((unsigned short *)D_12adc[0])

/* STRING_TABLE's flags, accessed as a word */
struct STRFLAGS {
    unsigned int UnUsed : 6;
    unsigned int bannerColour : 3;
    unsigned int window : 4;
    unsigned int onRelease : 1;
    unsigned int dontWrap : 1;
    unsigned int dontDraw : 1;
};
#define STRFLAGS(st) ((struct STRFLAGS *)((char *)(st) + 0x44))

/* libgpu.h */
typedef struct {
    unsigned addr : 24;
    unsigned len : 8;
    unsigned char r0, g0, b0, code;
} P_TAG;
#define setlen(p, _len) (((P_TAG *)(p))->len = (unsigned char)(_len))
#define setcode(p, _code) (((P_TAG *)(p))->code = (unsigned char)(_code))
#define setPolyFT4(p) setlen(p, 9), setcode(p, 0x2c)
#define setRGB0(p, _r0, _g0, _b0) (p)->r0 = _r0, (p)->g0 = _g0, (p)->b0 = _b0
#define setXY4(p, _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x1), (p)->y1 = (_y1), \
    (p)->x2 = (_x2), (p)->y2 = (_y2), (p)->x3 = (_x3), (p)->y3 = (_y3)
#define setUV4(p, _u0, _v0, _u1, _v1, _u2, _v2, _u3, _v3) \
    (p)->u0 = (_u0), (p)->v0 = (_v0), (p)->u1 = (_u1), (p)->v1 = (_v1), \
    (p)->u2 = (_u2), (p)->v2 = (_v2), (p)->u3 = (_u3), (p)->v3 = (_v3)
#define setXYWH(p, _x0, _y0, _w, _h) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x0) + (_w), (p)->y1 = (_y0), \
    (p)->x2 = (_x0), (p)->y2 = (_y0) + (_h), (p)->x3 = (_x0) + (_w), (p)->y3 = (_y0) + (_h)
#define setUVWH(p, _u0, _v0, _w, _h) \
    (p)->u0 = (_u0), (p)->v0 = (_v0), (p)->u1 = (_u0) + (_w), (p)->v1 = (_v0), \
    (p)->u2 = (_u0), (p)->v2 = (_v0) + (_h), (p)->u3 = (_u0) + (_w), (p)->v3 = (_v0) + (_h)

/* a palette to load: Mask has the colours that get the semi-transparency bit */
struct CLUTENTRY {
    short *Data;
    short Mask;
    short PalIndex;
};

/* the small functions gcc emits at the end of the file, in this order */
void ResetCallback();
void FrontEndNothing();
void FrontEndResetWindows();
void FrontEndDrawSortedObjs();
void FrontEndSpinPickup();
void FrontEndSpawnPickups();
void func_c6c0();
void CHECK_FOR_START();
void func_c7bc();
void GOTO_MISSIONCD_SPINDOWN();
void GOTO_MISSIONCD_CHECKING();
void GOTO_MISSIONCD_SWAP();
void GOTO_MISSIONCD_LOADING();
void GOTO_MISSIONCD_INVALID();
void GOTO_PLAYER();
void func_cb88();
void func_cc20();
void GOTO_OPTIONS();
void func_cd54();
void func_cdd8();
void GOTO_TITLE();
void GOTO_MULTIPLAYER();
void func_d010();
void func_d0c0();
void func_d15c();
void func_d210();
void func_d2d8();
void func_d394();
void func_d3bc();
void func_d3e8();
void func_d414();
void func_d4d0();
void func_d58c();
void func_d60c();
void func_d644(unsigned short *clut, short slot, int is256);
void func_d6fc();
void func_d734();
void GOTO_REINSERT_WAIT();
void func_d8c0();
void func_d9b8();
short func_dab0(short pad, char **type, char **port);
void func_db64();
void func_dbe0();
int DMACallback(int dma, void (*func)());
void func_dc4c();
void func_dcc8();
void func_ddbc();
int func_dec4(void);
void func_df90();
/* the others */
void FrontEndFindLimitIndices();
void CHECK_FOR_SCREENPOSITION();
void CHECK_FOR_MISSIONCD();
void FrontEndPatchEngineVariables();
void GOTO_DEMO();
void IntroDelayElapsed();
void GOTO_INTRO();
void GOTO_START_MULTIPLAYER();
void FrontEndLoadCluts();
void func_24fc();
void FrontEndAddQuad(struct POLY_FT4 **pp, short x, short y, short u, short v, short w, short h, short tpage, short clut, short col, int xoff);
void FrontEndDraw256();
void FrontEndLighting();
void FrontEndMainLoop();
void InitLevel();
void FrontEndCheckController();
void FrontEndInitScreen(char *Title, short screen);
void GOTO_CREDITS();
void CreditsScroll();
void GOTO_CONTROLS();
void FrontEndShowControls();
void CHECK_FOR_CONTROLS();
void GOTO_UNLOCK_SAVE();
void CHECK_FOR_MULTI_MODE();
void GOTO_RULES();
void GOTO_GAME_VARIABLES();
void GOTO_MULTIPLAYER_SETUP();
void func_4d78();
void func_50e4();
void func_59e0();
void func_5da8();
void func_7184();
void func_75dc();
void func_9c38(short w);
void PickupSpinTitle();
void PickupSpinMenu();
void func_9f08();
int printf(char *fmt, ...);
void func_ae00();
unsigned long *func_b00c();
void func_b1c4();
void PlayMovie();
void func_bdf4();

struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* bss */
static short D_12a60 BSS;
static short D_12a62 BSS;
int D_12a64[1] BSS;
int D_12a68[4] BSS;
int D_12a78[6] BSS;
int D_12a90[1] BSS;
int D_12a94[1] BSS;
int D_12a98[1] BSS;
int D_12a9c[3] BSS;
int D_12aa8[4] BSS;
int D_12ab8[5] BSS;
int D_12acc[2] BSS;
int D_12ad4[1] BSS;
int D_12ad8[1] BSS;
int D_12adc[2] BSS;
short D_12ae4 BSS;
struct POLY_FT4 D_12ae8[2] BSS;
struct CREATURE *Pickups[5] BSS;
struct POLY_FT4 *D_12b4c[3] BSS;
char D_12b58[2][100] BSS;
unsigned char D_12c20[4] BSS;
struct POLY_FT4 *D_12c24[48] BSS;
unsigned long SavedSp BSS;
unsigned char D_12ce8[8] BSS;
DRAWENV CreditsEnv[2] BSS;
short IntroDelay BSS;
char *D_12dac BSS;
unsigned long MainLoopSp BSS;
int Unused_12db4 BSS;
int D_12db8[8] BSS;
int D_12dd8[1] BSS;
char *D_12ddc[1] BSS;
struct POLY_FT4 PolyStack0[512] BSS;
struct POLY_FT4 PolyStack1[512] BSS;

short TimeLimits[12] = {
    1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 60, -1,
};

short ScoreLimits[5] = {
    1, 2, 3, 5, 10,
};

short FragLimits[9] = {
    1, 2, 3, 4, 5, 10, 15, 20, -1,
};

short FragLimitIndex = 5;

short ScoreLimitIndex = 2;

short TimeLimitIndex = 5;

struct FET_MESSAGE FET_DEMO[2] = {
    { "DEMO OF GAME", 256, 107, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_STARTING[3] = {
    { "STARTING", 256, 107, 0, 0, 0, 0 },
    { "GAME", 256, 133, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_START[3] = {
    { "START", 256, 147, (void *)func_cdd8, 0, 0, 0 },
    { "OPTIONS", 256, 173, (void *)GOTO_OPTIONS, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_SINGLE_MULTI[3] = {
    { "SINGLE PLAYER", 256, 107, (void *)func_cd54, 0, 0, 0 },
    { "MULTI PLAYER", 256, 133, (void *)GOTO_MULTIPLAYER, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_DO_YOU_WANT_TO_LOAD[2] = {
    { "DO YOU WANT TO LOAD?", 256, 94, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_YES[3] = {
    { "YES", 256, 120, 0, 0, 0, 0 },
    { "NO", 256, 146, (void *)func_cd54, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_PLAYER_OPTIONS[5] = {
    { "PLAYER OPTIONS", 256, 81, (void *)GOTO_PLAYER, 0, 0, 0 },
    { "SOUND OPTIONS", 256, 107, (void *)func_cb88, 0, 0, 0 },
    { "VIDEO OPTIONS", 256, 133, (void *)func_cc20, 0, 0, 0 },
    { "VIEW CREDITS", 256, 159, (void *)GOTO_CREDITS, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_YES_2[3] = {
    { "YES", 256, 120, 0, 0, 0, 0 },
    { "NO", 256, 146, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_PLAYER[5] = {
    { "CROSSHAIR", 256, 81, 0, 0, 0, 0 },
    { "AUTOCENTER", 256, 107, 0, 0, 0, 0 },
    { "CONTROLLER", 256, 133, (void *)GOTO_CONTROLS, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 159, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_SOUND[5] = {
    { "      MUSIC", 168, 81, 0, 0, 0, 0 },
    { "   SOUND FX", 168, 107, 0, 0, 0, 0 },
    { "STEREO", 256, 133, 0, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 159, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_VIDEO[4] = {
    { "HORIZONTAL SPLIT", 256, 94, 0, 0, 0, 0 },
    { "SCREEN POSITION", 256, 120, (void *)func_d010, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 146, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_USE_DIRECTIONAL_BUTTONS[3] = {
    { "USE DIRECTIONAL BUTTONS", 256, 107, 0, 0, 0, 0 },
    { "TO ADJUST DISPLAY", 256, 133, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_NEW_GAME[3] = {
    { "NEW GAME", 256, 107, (void *)func_d0c0, 0, 0, 0 },
    { "LOAD GAME", 256, 133, (void *)func_d414, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_EASY[4] = {
    { "EASY", 256, 94, (void *)func_d394, 0, 0, 0 },
    { "MEDIUM", 256, 120, (void *)func_d3bc, 0, 0, 0 },
    { "HARD", 256, 146, (void *)func_d3e8, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_WELL_DONE[5] = {
    { "WELL DONE", 256, 57, 0, 0, 0, 0 },
    { "DO YOU WISH TO SAVE", 256, 78, 0, 0, 0, 0 },
    { "AN UNLOCK FILE", 256, 99, 0, 0, 0, 0 },
    { "FOR BRONZE CHEATS?", 256, 120, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_SAVE_UNLOCK_YESNO[3] = {
    { "YES", 256, 162, (void *)func_d2d8, 0, 0, 0 },
    { "NO", 256, 183, (void *)GOTO_TITLE, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_DEATHMATCH[6] = {
    { "DEATHMATCH", 256, 76, (void *)GOTO_MULTIPLAYER_SETUP, 0, 0, 0 },
    { "TEAM DEATHMATCH", 256, 98, (void *)GOTO_MULTIPLAYER_SETUP, 0, 0, 0 },
    { "VERSUS", 256, 120, (void *)GOTO_MULTIPLAYER_SETUP, 0, 0, 0 },
    { "LOAD SETTINGS", 256, 142, (void *)func_d15c, 0, 0, 0 },
    { "SAVE SETTINGS", 256, 164, (void *)func_d210, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_MULTI_CHEAT_NONE[4] = {
    { "    GRAVITY", 168, 93, 0, 0, 0, 0 },
    { "FALLING DAMAGE", 256, 120, 0, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 147, (void *)func_c7bc, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_MULTI_CHEAT_BRONZE[6] = {
    { "    GRAVITY", 168, 72, 0, 0, 0, 0 },
    { "WEAPON STAY", 256, 96, 0, 0, 0, 0 },
    { "FALLING DAMAGE", 256, 120, 0, 0, 0, 0 },
    { "ONE SHOT KILL", 256, 144, 0, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 168, (void *)func_c7bc, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_MULTI_CHEAT_SILVER[8] = {
    { "    GRAVITY", 168, 54, 0, 0, 0, 0 },
    { " GAME SPEED", 168, 76, 0, 0, 0, 0 },
    { "BLAST FORCE", 168, 98, 0, 0, 0, 0 },
    { "WEAPON STAY", 256, 120, 0, 0, 0, 0 },
    { "FALLING DAMAGE", 256, 142, 0, 0, 0, 0 },
    { "ONE SHOT KILL", 256, 164, 0, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 186, (void *)func_c7bc, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_MULTI_CHEAT_GOLD[10] = {
    { "    GRAVITY", 168, 52, 0, 0, 0, 0 },
    { " GAME SPEED", 168, 69, 0, 0, 0, 0 },
    { "BLAST FORCE", 168, 86, 0, 0, 0, 0 },
    { "WEAPON STAY", 256, 103, 0, 0, 0, 0 },
    { "INFINITE AMMO", 256, 120, 0, 0, 0, 0 },
    { "ALL WEAPONS", 256, 137, 0, 0, 0, 0 },
    { "FALLING DAMAGE", 256, 154, 0, 0, 0, 0 },
    { "ONE SHOT KILL", 256, 171, 0, 0, 0, 0 },
    { "RESET TO DEFAULTS", 256, 188, (void *)func_c7bc, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES[7] = {
    { "RULES", 256, 75, 0, 0, 0, 0 },
    { "THE ONE AND ONLY! KILL YOUR", 256, 93, 0, 0, 0, 0 },
    { "FRIENDS AS MANY TIMES AS", 256, 111, 0, 0, 0, 0 },
    { "YOU CAN. ONE FRAG POINT PER", 256, 129, 0, 0, 0, 0 },
    { "KILL. THE PLAYER WITH THE", 256, 147, 0, 0, 0, 0 },
    { "MOST FRAGS WINS.", 256, 165, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES_2[8] = {
    { "RULES", 256, 66, 0, 0, 0, 0 },
    { "KILL THE MEMBERS OF THE", 256, 84, 0, 0, 0, 0 },
    { "OTHER TEAM AS MANY TIMES AS", 256, 102, 0, 0, 0, 0 },
    { "YOU CAN. ONE FRAG POINT PER", 256, 120, 0, 0, 0, 0 },
    { "KILL, A FRAG IS LOST FOR", 256, 138, 0, 0, 0, 0 },
    { "KILLING TEAMMATES. THE TEAM", 256, 156, 0, 0, 0, 0 },
    { "WITH THE MOST FRAGS WINS.", 256, 174, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES_3[10] = {
    { "RULES", 256, 48, 0, 0, 0, 0 },
    { "TOUCH THE OTHER TEAM'S FLAG", 256, 66, 0, 0, 0, 0 },
    { "TO TAKE IT. BRING IT TO", 256, 84, 0, 0, 0, 0 },
    { "YOUR FLAG AT HOME BASE TO", 256, 102, 0, 0, 0, 0 },
    { "SCORE A CAPTURE. TOUCH YOUR", 256, 120, 0, 0, 0, 0 },
    { "OWN FLAG TO RETURN IT TO", 256, 138, 0, 0, 0, 0 },
    { "YOUR BASE. THE TEAM OR", 256, 156, 0, 0, 0, 0 },
    { "PLAYER WITH THE MOST FLAG", 256, 174, 0, 0, 0, 0 },
    { "CAPTURES WINS.", 256, 192, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES_4[7] = {
    { "RULES", 256, 75, 0, 0, 0, 0 },
    { "TOUCH THE RED FLAG TO TAKE", 256, 93, 0, 0, 0, 0 },
    { "IT. KILL ANYBODY WHO HAS", 256, 111, 0, 0, 0, 0 },
    { "THE RED FLAG. THE PLAYER", 256, 129, 0, 0, 0, 0 },
    { "THAT HOLDS THE FLAG FOR THE", 256, 147, 0, 0, 0, 0 },
    { "LONGEST TIME WINS.", 256, 165, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES_5[10] = {
    { "RULES", 256, 48, 0, 0, 0, 0 },
    { "TOUCH THE RED FLAG TO TAKE", 256, 66, 0, 0, 0, 0 },
    { "IT. WHEN YOU TAKE THE FLAG", 256, 84, 0, 0, 0, 0 },
    { "YOU WILL LOSE YOUR WEAPONS", 256, 102, 0, 0, 0, 0 },
    { "AND HAVE TO RELY ON YOUR", 256, 120, 0, 0, 0, 0 },
    { "TEAMMATES TO DEFEND YOU. YOU", 256, 138, 0, 0, 0, 0 },
    { "CANNOT HURT YOUR TEAMMATES.", 256, 156, 0, 0, 0, 0 },
    { "THE TEAM THAT HOLDS THE", 256, 174, 0, 0, 0, 0 },
    { "FLAG THE LONGEST WINS.", 256, 192, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_RULES_6[10] = {
    { "RULES", 256, 48, 0, 0, 0, 0 },
    { "THERE ARE NO AMMO OR", 256, 66, 0, 0, 0, 0 },
    { "HEALTH POWER-UPS IN THE", 256, 84, 0, 0, 0, 0 },
    { "LEVEL. WHEN YOU DIE, YOU", 256, 102, 0, 0, 0, 0 },
    { "ARE OUT FOR THE ROUND. IF", 256, 120, 0, 0, 0, 0 },
    { "YOU ARE THE LAST ONE ALIVE", 256, 138, 0, 0, 0, 0 },
    { "IN THE ROUND, YOU GET A", 256, 156, 0, 0, 0, 0 },
    { "POINT. THE FIRST PLAYER TO", 256, 174, 0, 0, 0, 0 },
    { "REACH SCORE LIMIT WINS.", 256, 192, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_2_3_4_PLAYERS[7] = {
    { "2 3 4 PLAYERS", 256, 60, 0, 0, 0, 0 },
    { "", 256, 77, 0, 0, 0, 0 },
    { "TIME LIMIT   10", 256, 151, 0, 0, 0, 0 },
    { "FRAG LIMIT   10", 256, 168, 0, 0, 0, 0 },
    { "GAME VARIABLES", 256, 185, (void *)GOTO_GAME_VARIABLES, 0, 0, 0 },
    { "PROCEED", 256, 202, (void *)func_59e0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_2_3_4_PLAYERS_2[6] = {
    { "2 3 4 PLAYERS", 256, 59, 0, 0, 0, 0 },
    { "", 256, 78, 0, 0, 0, 0 },
    { "ROUND LIMIT   3", 256, 149, 0, 0, 0, 0 },
    { "GAME VARIABLES", 256, 168, (void *)GOTO_GAME_VARIABLES, 0, 0, 0 },
    { "PROCEED", 256, 187, (void *)func_59e0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_CONTROLLER[6] = {
    { "STYLE A", 256, 74, 0, 0, 0, 0 },
    { "VIBRATE", 256, 97, 0, 0, 0, 0 },
    { "SWAP Y", 256, 120, 0, 0, 0, 0 },
    { "MOUSE", 256, 143, 0, 0, 0, 0 },
    { "MOUSE SPEED", 168, 166, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_NULL[1] = {
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_CREDITS_TITLE[2] = {
    { "DEVELOPED BY", 256, 210, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_CREDITS[178] = {
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "HAMMERHEAD LTD", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "EXECUTIVE PRODUCED", 256, 210, 0, 0, 0, 0 },
    { "AND DIRECTED BY", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ID SOFTWARE", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "PUBLISHED BY", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ACTIVISION", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "HAMMERHEAD IS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "CHRIS ALIZON", 256, 210, 0, 0, 0, 0 },
    { "JASON ALLEN", 256, 210, 0, 0, 0, 0 },
    { "NEIL BARNES", 256, 210, 0, 0, 0, 0 },
    { "RAOUL BARNETT", 256, 210, 0, 0, 0, 0 },
    { "MARTIN BLEVIN", 256, 210, 0, 0, 0, 0 },
    { "STE CORK", 256, 210, 0, 0, 0, 0 },
    { "JO DALY", 256, 210, 0, 0, 0, 0 },
    { "NICOLA DALY", 256, 210, 0, 0, 0, 0 },
    { "RAY DONNELLY", 256, 210, 0, 0, 0, 0 },
    { "ROB DRIVER", 256, 210, 0, 0, 0, 0 },
    { "DARREN FAHY", 256, 210, 0, 0, 0, 0 },
    { "MIKE FOLLIN", 256, 210, 0, 0, 0, 0 },
    { "PAUL HESKETH", 256, 210, 0, 0, 0, 0 },
    { "DARREN HOLT", 256, 210, 0, 0, 0, 0 },
    { "PAUL HUNTER", 256, 210, 0, 0, 0, 0 },
    { "ANDY INGRAM", 256, 210, 0, 0, 0, 0 },
    { "SIMON JAMES", 256, 210, 0, 0, 0, 0 },
    { "PAUL JOHNSON", 256, 210, 0, 0, 0, 0 },
    { "MARC JONES", 256, 210, 0, 0, 0, 0 },
    { "CHARLES MCNAIR", 256, 210, 0, 0, 0, 0 },
    { "PHIL MERRICKS", 256, 210, 0, 0, 0, 0 },
    { "KARIM MEZELI", 256, 210, 0, 0, 0, 0 },
    { "PAUL MORETON", 256, 210, 0, 0, 0, 0 },
    { "CHRIS STANFORTH", 256, 210, 0, 0, 0, 0 },
    { "FIONA STANFORTH", 256, 210, 0, 0, 0, 0 },
    { "ALLAN WALES", 256, 210, 0, 0, 0, 0 },
    { "CRAIG WHITTLE", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ID SOFTWARE IS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "PROGRAMMING", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "JOHN CARMACK", 256, 210, 0, 0, 0, 0 },
    { "JOHN CASH", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ART", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ADRIAN CARMACK", 256, 210, 0, 0, 0, 0 },
    { "KEVIN CLOUD", 256, 210, 0, 0, 0, 0 },
    { "PAUL STEED", 256, 210, 0, 0, 0, 0 },
    { "KENNETH SCOTT", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "LEVEL DESIGN", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "TIM WILLITS", 256, 210, 0, 0, 0, 0 },
    { "CHRISTIAN ANTKOW", 256, 210, 0, 0, 0, 0 },
    { "PAUL JAQUAYS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "DESIGN", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "GRAEME DEVINE", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "BUSINESS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "TODD HOLLENSHEAD", 256, 210, 0, 0, 0, 0 },
    { "KATHERINE ANNA KANG", 256, 210, 0, 0, 0, 0 },
    { "DONNA JACKSON", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "LEVEL MUSIC BY", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "SONIC MAYHEM", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ACTIVISION", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ADDITIONAL PROGRAMMING", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "GARY JESDANUN", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "PRODUCER", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "MARTY STRATTON", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ASSOCIATE PRODUCER", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "STEVE ELWELL", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "DIRECTOR OF", 256, 210, 0, 0, 0, 0 },
    { "GLOBAL BRAND MANAGEMENT", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "DUSTY WELCH", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ASSOCIATE BRAND MANAGEMENT", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "KEVIN KRAFF", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "SENIOR PUBLICIST", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "JULIAN ROETHER", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "PRODUCTION TESTING", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "STEVE ROSENTHAL", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "QUALITY ASSURANCE", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "MANAGER OF CONSOLE TESTING", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "MARILENA MORINI", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "QA PROJECT LEAD", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ERIC KOCH", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "QA TESTERS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "DAMIEN FISCHER", 256, 210, 0, 0, 0, 0 },
    { "ED PUCCIO", 256, 210, 0, 0, 0, 0 },
    { "NELSON PRINCE", 256, 210, 0, 0, 0, 0 },
    { "JOE SHACKELFORD", 256, 210, 0, 0, 0, 0 },
    { "JEF SEDIVY", 256, 210, 0, 0, 0, 0 },
    { "BRYAN ANDERSON", 256, 210, 0, 0, 0, 0 },
    { "JEFF MCLEAN", 256, 210, 0, 0, 0, 0 },
    { "MIKE DENNY", 256, 210, 0, 0, 0, 0 },
    { "JEREMY LUYTIE", 256, 210, 0, 0, 0, 0 },
    { "ANDY MITCHELL", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "ABSOLUTE QUALITY", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "TROY KUPISCH", 256, 210, 0, 0, 0, 0 },
    { "JESSE SMITH", 256, 210, 0, 0, 0, 0 },
    { "TOBY WATKINS", 256, 210, 0, 0, 0, 0 },
    { "SEAN DAVIS", 256, 210, 0, 0, 0, 0 },
    { "TIM SMITH", 256, 210, 0, 0, 0, 0 },
    { "JEFF CATHER", 256, 210, 0, 0, 0, 0 },
    { "SCOTT WOOD", 256, 210, 0, 0, 0, 0 },
    { "ADAM COLEMAN", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "SPECIAL THANKS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 210, 0, 0, 0, 0 },
    { "MITCH LASKY", 256, 210, 0, 0, 0, 0 },
    { "DEREK MCLEISH", 256, 210, 0, 0, 0, 0 },
    { "JIM SUMMERS", 256, 210, 0, 0, 0, 0 },
    { "MARC BENNET", 256, 210, 0, 0, 0, 0 },
    { "DAVID GRIJNS", 256, 210, 0, 0, 0, 0 },
    { "", 256, 219, (void *)GOTO_OPTIONS, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_PLEASE_WAIT_WHILE[3] = {
    { "PLEASE WAIT WHILE", 256, 107, 0, 0, 0, 0 },
    { "CD SPINS DOWN", 256, 133, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_IT_IS_NOW_SAFE[4] = {
    { "IT IS NOW SAFE", 256, 94, 0, 0, 0, 0 },
    { "TO SWAP THE", 256, 120, 0, 0, 0, 0 },
    { "MISSION DISC", 256, 146, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_CHECKING[3] = {
    { "CHECKING", 256, 107, 0, 0, 0, 0 },
    { "MISSION DISC", 256, 133, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_LOADING[3] = {
    { "LOADING", 256, 107, 0, 0, 0, 0 },
    { "MISSION DISC", 256, 133, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

struct FET_MESSAGE FET_THIS_IS_NOT_A[4] = {
    { "THIS IS NOT A", 256, 94, (void *)func_cd54, 0, 0, 0 },
    { "VALID QUAKE 2", 256, 120, (void *)func_cd54, 0, 0, 0 },
    { "MISSION DISC", 256, 146, (void *)func_cd54, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

/* the menus show the limits as indices into these tables */
void FrontEndFindLimitIndices(void)
{
    int i;

    for (i = 0; i < 12; i++)
        if (TimeLimits[i] == LevelInterface.Engine->GameConfig.sTimeLimit)
            TimeLimitIndex = i;
    for (i = 0; i < 5; i++)
        if (ScoreLimits[i] == LevelInterface.Engine->GameConfig.sScoreLimit)
            ScoreLimitIndex = i;
    for (i = 0; i < 9; i++)
        if (FragLimits[i] == LevelInterface.Engine->GameConfig.sFragLimit)
            FragLimitIndex = i;
}


void CHECK_FOR_SCREENPOSITION(void)
{
    short pad;
    static short TimeLeft BSS;
    static short TimeRight BSS;
    static short TimeUp BSS;
    static short TimeDown BSS;

    pad = LevelInterface.Engine->gFrontEndPad[0];
    if (pad & 0x80) {
        TimeLeft += TimeLeft < 5;
        if ((TimeLeft == 5 || TimeLeft == 1) && LevelInterface.Engine->GameConfig.sScreenPosX >= -19)
            LevelInterface.Engine->GameConfig.sScreenPosX--;
    } else {
        TimeLeft = 0;
    }
    if (pad & 0x20) {
        TimeRight += TimeRight < 5;
        if ((TimeRight == 5 || TimeRight == 1) && LevelInterface.Engine->GameConfig.sScreenPosX < 20)
            LevelInterface.Engine->GameConfig.sScreenPosX++;
    } else {
        TimeRight = 0;
    }
    if (pad & 0x10) {
        TimeUp += TimeUp < 5;
        if ((TimeUp == 5 || TimeUp == 1) && LevelInterface.Engine->GameConfig.sScreenPosY >= -31)
            LevelInterface.Engine->GameConfig.sScreenPosY--;
    } else {
        TimeUp = 0;
    }
    if (pad & 0x40) {
        TimeDown += TimeDown < 5;
        if ((TimeDown == 5 || TimeDown == 1) && LevelInterface.Engine->GameConfig.sScreenPosY < 64)
            LevelInterface.Engine->GameConfig.sScreenPosY++;
    } else {
        TimeDown = 0;
    }
    LevelInterface.Engine->Config_InstallVideo();
}

void CHECK_FOR_MISSIONCD(void)
{
    switch (LevelInterface.Engine->MissionCDStatus) {
    case 0:
        if (LevelInterface.Engine->currentscreen != 21)
            GOTO_MISSIONCD_SPINDOWN();
        break;
    case 1:
        if (LevelInterface.Engine->currentscreen != 24)
            GOTO_MISSIONCD_SWAP();
        break;
    case 2:
        if (LevelInterface.Engine->currentscreen != 22)
            GOTO_MISSIONCD_CHECKING();
        break;
    case 3:
        if (LevelInterface.Engine->currentscreen != 23)
            GOTO_MISSIONCD_LOADING();
        break;
    case 4:
        if (LevelInterface.Engine->currentscreen != 25)
            GOTO_MISSIONCD_INVALID();
        break;
    case 5:
        break;
    }
}

void GOTO_MISSIONCD_SPINDOWN(void)
{
    FrontEndInitScreen("MISSION CD", 21);
    LevelInterface.Engine->FrontEndCreateMenu(FET_PLEASE_WAIT_WHILE, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MISSIONCD;
    LevelInterface.Engine->Backfunc = func_cdd8;
}

/* point the menus at the engine's settings */
void FrontEndPatchEngineVariables(void)
{
    FET_PLAYER[0].pVar = &LevelInterface.Engine->GameConfig.bCrossHairs;
    FET_PLAYER[1].pVar = &LevelInterface.Engine->GameConfig.bAutoCenter;
    FET_PLAYER[3].func = LevelInterface.Engine->FrontEndResetPlayerDefaults;
    FET_CONTROLLER[1].pVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].vibration;
    FET_CONTROLLER[2].pVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].invertY;
    FET_CONTROLLER[3].pVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].useMouse;
    FET_CONTROLLER[4].pSlider = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].mouseSensitivity;
    FET_SOUND[0].pSlider = &LevelInterface.Engine->GameConfig.sMusic;
    FET_SOUND[1].pSlider = &LevelInterface.Engine->GameConfig.sSoundFX;
    FET_SOUND[2].pVar = &LevelInterface.Engine->GameConfig.bStereo;
    FET_SOUND[3].func = LevelInterface.Engine->FrontEndResetSoundDefaults;
    FET_VIDEO[0].pVar = &LevelInterface.Engine->GameConfig.bSplitHoriz;
    FET_VIDEO[2].func = LevelInterface.Engine->FrontEndResetVideoDefaults;
    FET_MULTI_CHEAT_NONE[0].pSlider = &LevelInterface.Engine->GameConfig.sGravity;
    FET_MULTI_CHEAT_NONE[1].pVar = &LevelInterface.Engine->GameConfig.bFallingDamage;
    FET_MULTI_CHEAT_BRONZE[0].pSlider = &LevelInterface.Engine->GameConfig.sGravity;
    FET_MULTI_CHEAT_BRONZE[1].pVar = &LevelInterface.Engine->GameConfig.bWeaponStay;
    FET_MULTI_CHEAT_BRONZE[2].pVar = &LevelInterface.Engine->GameConfig.bFallingDamage;
    FET_MULTI_CHEAT_BRONZE[3].pVar = &LevelInterface.Engine->GameConfig.bOneShotKill;
    FET_MULTI_CHEAT_SILVER[0].pSlider = &LevelInterface.Engine->GameConfig.sGravity;
    FET_MULTI_CHEAT_SILVER[1].pSlider = &LevelInterface.Engine->GameConfig.sGameSpeed;
    FET_MULTI_CHEAT_SILVER[2].pSlider = &LevelInterface.Engine->GameConfig.sBlastScale;
    FET_MULTI_CHEAT_SILVER[3].pVar = &LevelInterface.Engine->GameConfig.bWeaponStay;
    FET_MULTI_CHEAT_SILVER[4].pVar = &LevelInterface.Engine->GameConfig.bFallingDamage;
    FET_MULTI_CHEAT_SILVER[5].pVar = &LevelInterface.Engine->GameConfig.bOneShotKill;
    FET_MULTI_CHEAT_GOLD[0].pSlider = &LevelInterface.Engine->GameConfig.sGravity;
    FET_MULTI_CHEAT_GOLD[1].pSlider = &LevelInterface.Engine->GameConfig.sGameSpeed;
    FET_MULTI_CHEAT_GOLD[2].pSlider = &LevelInterface.Engine->GameConfig.sBlastScale;
    FET_MULTI_CHEAT_GOLD[3].pVar = &LevelInterface.Engine->GameConfig.bWeaponStay;
    FET_MULTI_CHEAT_GOLD[4].pVar = &LevelInterface.Engine->GameConfig.bInfiniteAmmo;
    FET_MULTI_CHEAT_GOLD[5].pVar = &LevelInterface.Engine->GameConfig.bAllWeapons;
    FET_MULTI_CHEAT_GOLD[6].pVar = &LevelInterface.Engine->GameConfig.bFallingDamage;
    FET_MULTI_CHEAT_GOLD[7].pVar = &LevelInterface.Engine->GameConfig.bOneShotKill;
}

char * D_10d80[4] = {
    "BLUE", "RED", "PURPLE", "GREEN",
};

int PickupTypes[6] = {
    57, 59, 60, 61, 62, -1,
};

short Clut256_0[256] = {
    14833, 7634, 2635, 4471, 11664, 16781, 6514, 13709, 2539, 1514, 9613, 4525, 15785, 6512, 4405, 13677,
    4463, 6509, 10664, 9581, 1481, 11566, 6479, 9669, 7502, 10572, 4521, 4429, 11595, 11593, 4367, 7468,
    6444, 6475, 1416, 9515, 5389, 7497, 5419, 4365, 3371, 10538, 2317, 4484, 9514, 7467, 6413, 8490,
    7466, 2226, 6411, 6442, 11590, 5417, 6440, 5387, 3339, 8458, 8488, 9481, 6410, 7433, 8457, 3368,
    5386, 5356, 6409, 2284, 5385, 4361, 5355, 8455, 10533, 7431, 5384, 3277, 7461, 6407, 3336, 4299,
    4330, 6377, 5322, 7339, 7369, 5383, 5323, 5353, 2251, 4329, 6376, 264, 3305, 4268, 8452, 8391,
    5352, 3274, 7399, 6405, 4298, 4328, 3333, 6345, 1226, 6375, 5321, 5351, 3303, 4297, 2188, 4327,
    6344, 3273, 1225, 5320, 5350, 4296, 6343, 3272, 231, 4234, 2248, 2218, 5319, 6341, 4265, 9412,
    4295, 2277, 1223, 5348, 3241, 6372, 2187, 3271, 2217, 5318, 7364, 4264, 4294, 169, 199, 6311,
    3240, 5287, 2216, 2245, 5317, 4263, 4293, 3239, 6339, 5286, 2215, 10404, 167, 4262, 197, 4292,
    4231, 3208, 3238, 2184, 5285, 1130, 1160, 4261, 1190, 1129, 3207, 6308, 3237, 5284, 2183, 4230,
    135, 1159, 165, 4259, 3206, 3235, 2182, 5283, 4229, 134, 164, 7299, 5252, 3205, 2181, 5282,
    4228, 133, 3204, 2180, 2150, 4227, 132, 1126, 1095, 3173, 3203, 102, 2149, 2178, 4226, 4196,
    1125, 131, 3202, 3172, 101, 5187, 2148, 1124, 4194, 100, 3171, 2117, 2147, 69, 1123, 3170,
    99, 2116, 2146, 1092, 1122, 68, 3138, 2115, 37, 1061, 3108, 1091, 67, 2084, 66, 2114,
    1090, 2113, 4, 1059, 35, 2082, 64, 1058, 34, 2051, 2081, 1057, 2, 1056, 1025, 0,
};

short Clut256_1[256] = {
    3582, 22126, 2462, 2430, 2399, 12784, 15854, 3482, 2365, 12752, 11728, 13775, 12751, 10673, 11727, 10702,
    12750, 2363, 2423, 11726, 9649, 11695, 2301, 10671, 4470, 12718, 9678, 9647, 11694, 10670, 2299, 11693,
    10639, 10669, 3354, 8591, 4466, 9615, 11662, 8621, 3383, 10638, 9614, 10637, 9613, 11661, 9583, 14730,
    8559, 10636, 10606, 12652, 2265, 8588, 9582, 7564, 6510, 8558, 2295, 10605, 9581, 8557, 10604, 6479,
    7533, 9550, 9580, 8526, 2293, 8556, 7502, 8555, 7532, 9549, 8525, 7501, 3346, 7531, 10540, 9548,
    6446, 6507, 8524, 6476, 3255, 9547, 4399, 7470, 7500, 4429, 10570, 8523, 7469, 7499, 9516, 6445,
    8492, 6475, 7468, 2229, 7498, 6444, 9545, 6474, 4396, 8491, 3343, 7467, 6443, 8490, 7436, 7466,
    5418, 2227, 6442, 6412, 8489, 7435, 7465, 6411, 6441, 3308, 3339, 5387, 5417, 3248, 7434, 8457,
    6410, 12522, 2314, 5386, 7433, 4361, 6409, 8456, 5385, 7432, 6408, 9449, 6378, 2192, 3337, 5354,
    5384, 4330, 7401, 3306, 5382, 6377, 8393, 5353, 7400, 4329, 2220, 6376, 4298, 9445, 5352, 3304,
    4328, 7399, 2250, 6345, 2280, 6375, 4326, 4297, 5350, 6344, 2249, 6373, 5320, 4296, 3242, 6343,
    3272, 5319, 2248, 4265, 4295, 3271, 3241, 6342, 7364, 2277, 6371, 5318, 2247, 3270, 4264, 2217,
    4294, 3240, 6341, 5287, 4263, 4293, 3209, 6340, 2185, 3239, 5316, 5286, 2215, 4262, 1220, 3208,
    8325, 3238, 6339, 2184, 5285, 6309, 2214, 5315, 4261, 3207, 3237, 5284, 2183, 4230, 4260, 3206,
    2182, 5283, 2212, 4229, 4259, 1188, 5222, 3205, 2181, 4228, 5220, 3204, 2150, 2180, 4227, 1156,
    4197, 3203, 3173, 2149, 1155, 4196, 3172, 2148, 4195, 3171, 2147, 3170, 2146, 1090, 2114, 1024,
};

short Clut256_2[256] = {
    31710, 29597, 28539, 27482, 26426, 25369, 25368, 24312, 24312, 24311, 24312, 24311, 24279, 23255, 23255, 23254,
    23255, 23254, 23222, 22198, 22198, 22198, 22198, 22198, 22197, 22197, 22197, 21141, 22165, 21141, 21141, 21141,
    21141, 21141, 21141, 21140, 21141, 21140, 21140, 21140, 21140, 21140, 20084, 20084, 20084, 20084, 20084, 20084,
    20084, 20084, 20084, 20084, 20084, 20084, 20083, 20083, 20083, 20083, 20083, 20083, 20083, 19027, 19027, 19027,
    19027, 19027, 19027, 19026, 19026, 19026, 19026, 19026, 17970, 17970, 18994, 17970, 17970, 17970, 17970, 17970,
    17970, 17970, 17970, 17970, 17970, 17969, 17969, 17969, 17969, 16913, 16913, 16913, 16913, 16912, 16913, 16913,
    16912, 16912, 16913, 16912, 16912, 16912, 16880, 16880, 15856, 15856, 15856, 15855, 15855, 15823, 14799, 14799,
    14799, 14798, 14799, 14798, 14798, 13742, 8474, 14765, 13742, 8473, 13741, 13741, 13741, 9526, 13709, 12685,
    12685, 13678, 13708, 12684, 12684, 12684, 12684, 12684, 11628, 4283, 11628, 12651, 11628, 11627, 2204, 11627,
    11627, 11627, 11627, 11595, 10571, 10570, 11593, 10570, 10539, 10538, 9514, 10537, 9513, 9513, 11501, 9513,
    9513, 9513, 9513, 9513, 9513, 9513, 9513, 9513, 9513, 8457, 10564, 8457, 8457, 8456, 9480, 8456,
    8456, 8456, 8456, 8456, 9419, 8456, 9358, 9417, 7400, 8333, 7400, 7400, 7399, 8423, 7399, 8422,
    7399, 7399, 7399, 7367, 6343, 7366, 7368, 6342, 7366, 6342, 6342, 6342, 6342, 6342, 6342, 5286,
    6309, 5285, 6309, 5285, 6309, 5285, 5285, 5285, 5285, 5285, 4229, 6276, 5252, 4228, 5252, 5252,
    5252, 4228, 4228, 5252, 4228, 4228, 4228, 5252, 4228, 4228, 4228, 4228, 4228, 4228, 4195, 4195,
    3171, 3171, 3171, 3171, 3138, 2114, 2114, 2114, 2114, 2114, 1057, 1057, 1057, 0, 0, 0,
};

short Clut256_3[256] = {
    0, 24065, 16, 5351, 9, 4329, 18665, 13472, 7498, 5250, 16913, 2184, 273, 3204, 9513, 28303,
    2103, 3307, 5285, 13773, 16584, 22989, 9580, 2156, 4, 21760, 3272, 6440, 18793, 15683, 22090, 13,
    145, 7301, 32529, 11491, 11660, 2182, 7400, 2113, 1045, 1176, 1274, 11627, 2116, 7365, 2250, 2429,
    22032, 17969, 24078, 23948, 16944, 6443, 18893, 1097, 2187, 10469, 7432, 24841, 1101, 9410, 16716, 1105,
    12717, 10571, 18860, 12652, 1095, 1309, 4261, 23087, 3238, 1228, 1180, 28207, 2, 6375, 10603, 6344,
    2114, 32401, 20779, 6380, 20083, 14831, 6342, 8321, 9546, 2138, 5318, 27150, 2216, 8490, 6847, 12683,
    29295, 1075, 4360, 13603, 13742, 15, 1109, 1098, 4195, 4364, 26061, 282, 3172, 7434, 1143, 7,
    19842, 4297, 116, 4298, 21932, 11, 20713, 1239, 14698, 2147, 13511, 6378, 12581, 13741, 7431, 10603,
    5318, 2559, 26425, 22197, 4703, 247, 21799, 16774, 30416, 5564, 2392, 19648, 20999, 17903, 25004, 24311,
    25888, 15552, 28539, 15748, 208, 2463, 19874, 19026, 21102, 3647, 26923, 350, 220, 1206, 21835, 7331,
    175, 15856, 16912, 111, 383, 30384, 6815, 15625, 30028, 15689, 19027, 1102, 13507, 16945, 5251, 7333,
    20973, 23840, 14660, 1107, 17607, 1171, 1178, 251, 1241, 1203, 18697, 10604, 21140, 23254, 14798, 6445,
    8387, 1146, 7531, 1077, 11660, 6412, 7329, 22891, 31376, 4193, 27914, 3277, 7935, 31504, 3171, 6310,
    10468, 11694, 10471, 17837, 12580, 16739, 10503, 3243, 17803, 1438, 22990, 8489, 22065, 8491, 26158, 30255,
    21803, 2134, 26062, 22957, 25368, 4302, 1057, 5383, 2220, 1060, 1058, 21770, 1303, 2282, 5357, 9550,
    30320, 2314, 19722, 9514, 2105, 1340, 1174, 221, 1073, 41, 45, 1070, 38, 43, 27450, 1145,
};

short Clut16_0[16] = {
    0, 8352, 10433, 11523, 13603, 14692, 16773, 18886, 19974, 22057, 23147, 25262, 26384, 28499, 29589, 31704,
};

short Clut16_1[16] = {
    0, 7364, 10567, 12746, 15949, 21136, 25331, 29494, 0, 5318, 2147, 10580, 22027, 10792, 4373, 12937,
};

short Clut16_2[16] = {
    0, 2080, 3136, 5217, 6273, 7362, 9443, 10499, 12580, 13670, 15784, 16874, 18988, 21102, 22192, 24306,
};

short Clut16_3[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 32767, 32767, 32767, 32767, 0, 0, 0, 0,
};

void GOTO_DEMO(void)
{
    int i;

    FrontEndInitScreen(0, 19);
    for (i = 0; i < 5; i++) {
        WINFLAGS(Pickups[i], 0)->NoDraw = 1;
        WINFLAGS(Pickups[i], 1)->NoDraw = 1;
        WINFLAGS(Pickups[i], 2)->NoDraw = 1;
        WINFLAGS(Pickups[i], 3)->NoDraw = 1;
    }
    LevelInterface.Engine->maxcursor = 0;
    LevelInterface.Engine->cursorupper = 0;
    LevelInterface.Engine->FrontEndCreateMenu(FET_DEMO, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->FrontEndResetPlayerDefaults();
    LevelInterface.Engine->FrontEndResetSoundDefaults();
    LevelInterface.Engine->FrontEndResetVideoDefaults();
    LevelInterface.Engine->FrontEndResetMultiplayerDefaults();
    LevelInterface.Engine->Config_InstallVideo();
    LevelInterface.Engine->PatchEngineVariables(0);
    LevelInterface.Engine->gFrontEndLevelComplete = 10;
    LevelInterface.Engine->gFrontEndTimeout = 12;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->FrontEndLevelCompleteTimeout;
    LevelInterface.Engine->GameSetup(0);
}

void IntroDelayElapsed(void)
{
    IntroDelay -= *LevelInterface.Engine->NewVcounts;
    if (IntroDelay < 0) {
        SavedSp = LevelInterface.Engine->SetSp(*LevelInterface.Engine->usp);
        PlayMovie("ROGUEIN1.STX", 2949, 0x10000, 1, 255);
        LevelInterface.Engine->SetSp(SavedSp);
        LevelInterface.Engine->maxcursor = 0;
        LevelInterface.Engine->cursorupper = 0;
        *LevelInterface.Engine->DoSnapShot = 1;
        LevelInterface.Engine->FrontEndCreateMenu(FET_STARTING, 16);
        LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
        LevelInterface.Engine->GameConfig.cContinues = 3;
        LevelInterface.Engine->PatchEngineVariables(0);
        LevelInterface.Engine->gFrontEndLevelComplete = 1;
        LevelInterface.Engine->gFrontEndTimeout = 12;
        LevelInterface.Engine->Specialfunc = LevelInterface.Engine->FrontEndLevelCompleteTimeout;
        LevelInterface.Engine->LoadSaveGameConfigData(1);
        LevelInterface.Engine->GameSetup(0);
    }
}

void GOTO_INTRO(void)
{
    int i;

    IntroDelay = 150;
    FrontEndInitScreen(0, 19);
    for (i = 0; i < 5; i++) {
        WINFLAGS(Pickups[i], 0)->NoDraw = 1;
        WINFLAGS(Pickups[i], 1)->NoDraw = 1;
        WINFLAGS(Pickups[i], 2)->NoDraw = 1;
        WINFLAGS(Pickups[i], 3)->NoDraw = 1;
    }
    LevelInterface.Engine->maxcursor = 0;
    LevelInterface.Engine->cursorupper = 0;
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = IntroDelayElapsed;
}

void GOTO_START_MULTIPLAYER(void)
{
    short i, j;
    int k;

    LevelInterface.Engine->SetupScreen1by1(0);
    *LevelInterface.Engine->DirectAccessLevelName =
        ((struct LEVELDATA *)LevelInterface.Engine->GetLevelDataFromIdentityName("COLD STORAGE"))[LevelInterface.Engine->GameConfig.bArena].Name;
    for (i = 0; i < LevelInterface.Engine->GameConfig.sNumPlayers; i++) {
        LevelInterface.Engine->Frags[i] = 0;
        LevelInterface.Engine->Scores[i] = 0;
        for (j = 0; j < LevelInterface.Engine->GameConfig.sNumPlayers; j++)
            LevelInterface.Engine->KillsMatrix[i][j] = 0;
    }
    FrontEndInitScreen(0, 19);
    for (k = 0; k < 5; k++) {
        WINFLAGS(Pickups[k], 0)->NoDraw = 1;
        WINFLAGS(Pickups[k], 1)->NoDraw = 1;
        WINFLAGS(Pickups[k], 2)->NoDraw = 1;
        WINFLAGS(Pickups[k], 3)->NoDraw = 1;
    }
    LevelInterface.Engine->maxcursor = 0;
    LevelInterface.Engine->cursorupper = 0;
    LevelInterface.Engine->FrontEndCreateMenu(FET_STARTING, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    if (LevelInterface.Engine->GameConfig.sMultiMode == 5)
        LevelInterface.Engine->GameConfig.sTimeLimit = -1;
    else
        LevelInterface.Engine->GameConfig.sTimeLimit = TimeLimits[TimeLimitIndex];
    if (LevelInterface.Engine->GameConfig.sMultiMode == 5)
        LevelInterface.Engine->GameConfig.sScoreLimit = ScoreLimits[ScoreLimitIndex];
    else
        LevelInterface.Engine->GameConfig.sFragLimit = FragLimits[FragLimitIndex];
    LevelInterface.Engine->gFrontEndLevelComplete = 4;
    LevelInterface.Engine->gFrontEndTimeout = 12;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->FrontEndLevelCompleteTimeout;
    LevelInterface.Engine->LoadSaveGameConfigData(1);
    LevelInterface.Engine->PatchEngineVariables(1);
    LevelInterface.Engine->GameSetup(1);
}

struct CLUTENTRY SmallCluts[5] = {
    { Clut16_0, -2, 0x44 },
    { Clut16_3, -2, 0x46 },
    { Clut16_1, -2, 0x45 },
    { Clut16_2, -2, 0x47 },
    { 0, 0, 0 },
};

struct CLUTENTRY BigCluts[5] = {
    { Clut256_0, -1, 0 },
    { Clut256_1, -1, 3 },
    { Clut256_2, -2, 1 },
    { Clut256_3, -2, 2 },
    { 0, 0, 0 },
};

int FlickerLeft = 0;

int FlickerRight = 0;

int StartedInCredits = 0;

void FrontEndLoadCluts(struct CLUTENTRY *Big, struct CLUTENTRY *Small)
{
    RECT rect;
    short buf[16];
    int lasty;
    struct CLUTENTRY *clut;
    short *src;
    int i;

    i = 0;
    clut = Big;
    for (;;) {
        short *unused; /* a block with declarations: gcc leaves the exit test at the top */

        src = clut->Data;
        if (!src)
            break;
        rect.y = 255 - i;
        rect.w = 256;
        rect.x = 0;
        rect.h = 1;
        LevelInterface.Engine->LoadImage(&rect, clut->Data);
        i++;
        LevelInterface.Engine->palette[clut->PalIndex] = ((rect.x & 0x3ff) >> 4) + ((rect.y & 0x1ff) << 6);
        clut++;
        LevelInterface.Engine->DrawSync(0);
    }
    lasty = rect.y;
    rect.y = 248;
    rect.w = 16;
    rect.x = 0;
    rect.h = 1;
    clut = Small;
    for (;;) {
        int mask;
        int k;

        src = clut->Data;
        if (!src)
            break;
        mask = clut->Mask;
        for (k = 0; k < 16; k++) {
            if (mask & 1)
                buf[k] = *src++ | 0x8000;
            else
                buf[k] = *src++;
            mask >>= 1;
        }
        LevelInterface.Engine->LoadImage(&rect, buf);
        LevelInterface.Engine->palette[clut->PalIndex] = ((rect.x & 0x3ff) >> 4) + ((rect.y & 0x1ff) << 6);
        clut++;
        if (++rect.y >= lasty) {
            rect.y = 248;
            rect.x += 16;
        }
        LevelInterface.Engine->DrawSync(0);
    }
}

ASM_FUNC(func_24fc)

void FrontEndAddQuad(struct POLY_FT4 **pp, short x, short y, short u, short v, short w, short h,
                     short tpage, short clut, short col, int xoff)
{
    struct POLY_FT4 *p;
    unsigned long *ot;

    p = *pp;
    ot = (*LevelInterface.Engine->currdb)->BigOt;
    setPolyFT4(p);
    setXYWH(p, xoff + (x - (w >> 1)), y - (h >> 1), w, h);
    setUVWH(p, u, v, w, h);
    setRGB0(p, col, col, col);
    p->tpage = LevelInterface.Engine->GetTPage(1, 0, LevelInterface.Engine->tpscreenx[tpage], LevelInterface.Engine->tpscreeny[tpage]);
    p->clut = LevelInterface.Engine->palette[clut];
    LevelInterface.Engine->AddPrim_(ot, p);
    (*pp)++;
}

/* the controller layouts and the arena pictures (256 colour textures) */
void FrontEndDraw256(struct POLY_FT4 **pp)
{
    short n;

    if (LevelInterface.Engine->currentscreen == 6) {
        switch (LevelInterface.Engine->GameConfig.MultiPlayerParams[0].controllerSelected) {
        case 0:
        case 1:
        case 2:
            FrontEndAddQuad(pp, 256, 108, 0, 128, 224, 122, 6, 1, 127, 0);
            break;
        case 3:
            FrontEndAddQuad(pp, 256, 108, 0, 0, 224, 122, 10, 1, 127, 0);
            break;
        case 4:
            FrontEndAddQuad(pp, 256, 108, 0, 0, 224, 122, 6, 1, 127, 0);
            break;
        case 5:
            FrontEndAddQuad(pp, 256, 108, 0, 0, 112, 122, 6, 1, 127, -56);
            FrontEndAddQuad(pp, 256, 108, 112, 0, 112, 122, 10, 1, 127, 56);
            break;
        default:
            FrontEndAddQuad(pp, 256, 108, 0, 128, 224, 122, 10, 1, 127, 0);
            break;
        }
    } else if (LevelInterface.Engine->currentscreen == 13) {
        n = LevelInterface.Engine->GameConfig.bArena;
        if (n < 10)
            FrontEndAddQuad(pp, 256, 113, (n & 1) << 7, (n / 2) * 51, 127, 51, 8, 0, 128, 0);
        else
            FrontEndAddQuad(pp, 256, 113, 0, (n - 10) * 51, 127, 51, 12, 3, 128, 0);
    }
}

static const struct SHORTPAIR NoFlare = { 0, 0 };

void FrontEndLighting(void)
{
    struct COLOUR FlickerColour;
    struct POWER FlickerPower;
    struct COLOUR Cursor;
    struct POWER CursorPower;
    struct VECT Pos;
    int target;

    FlickerColour.r = 64;
    FlickerColour.g = 127;
    FlickerColour.b = 127;
    FlickerPower.Full = 500;
    FlickerPower.Fall = 1500;
    Cursor.r = 16;
    Cursor.g = 255;
    Cursor.b = 64;
    CursorPower.Full = 100;
    CursorPower.Fall = 200;
    Pos.vx = 0;
    Pos.vy = LevelInterface.Engine->cursorpos * 200 - 200;
    Pos.vz = 500;
    Pos.vy = -200;
    LevelInterface.Engine->AddLight(&Pos, Cursor, CursorPower, NoFlare);
    Pos.vy = 0;
    LevelInterface.Engine->AddLight(&Pos, Cursor, CursorPower, NoFlare);
    Pos.vy = 200;
    LevelInterface.Engine->AddLight(&Pos, Cursor, CursorPower, NoFlare);
    target = (LevelInterface.Engine->rand() & 0x1ff) + 200;
    FlickerRight = Pos.vx = FlickerRight + (target - FlickerRight) / 4;
    Pos.vy = 600;
    Pos.vz = 900;
    FlickerColour.g = (LevelInterface.Engine->rand() & 0x3f) + 64;
    LevelInterface.Engine->AddLight(&Pos, FlickerColour, FlickerPower, NoFlare);
    target = -((LevelInterface.Engine->rand() & 0x1ff) + 200);
    FlickerLeft = Pos.vx = FlickerLeft + (target - FlickerLeft) / 4;
    Pos.vy = 600;
    Pos.vz = 900;
    FlickerColour.g = (LevelInterface.Engine->rand() & 0x3f) + 64;
    LevelInterface.Engine->AddLight(&Pos, FlickerColour, FlickerPower, NoFlare);
}

void FrontEndMainLoop(void)
{
    MainLoopSp = LevelInterface.Engine->SetSp(0x1f8003fc);
    LevelInterface.Engine->ChrisInitEveryFrame();
    LevelInterface.Engine->DoFrontEndStuff();
    LevelInterface.Engine->ProcessMoveBufs();
    LevelInterface.Engine->MoveModels();
    LevelInterface.Engine->ProcessSounds();
    FrontEndResetWindows();
    LevelInterface.Engine->DrawAllScenes();
    LevelInterface.Engine->ProcessAllLensFlare();
    LevelInterface.Engine->DrawAllModels();
    LevelInterface.Engine->ProcessLights();
    LevelInterface.Engine->ProcessBlurs();
    FrontEndDrawSortedObjs();
    LevelInterface.Engine->FreeSubDivs();
    LevelInterface.Engine->ReallyFreePolyPackets();
    LevelInterface.Engine->ProcessMessageText();
    FrontEndNothing();
    LevelInterface.Engine->SetSp(MainLoopSp);
}

void InitLevel(void)
{
    int screen;

    FrontEndFindLimitIndices();
    LevelInterface.Engine->PolyStack0 = PolyStack0;
    LevelInterface.Engine->PolyStack1 = PolyStack1;
    FrontEndPatchEngineVariables();
    FrontEndLoadCluts(BigCluts, SmallCluts);
    LevelInterface.Engine->SetupScreen1by1(0);
    LevelInterface.Engine->AlterFocalSubDiv(0, 160, 4000);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->Creatures->Primary = 0;
    LevelInterface.Engine->FrontEndClearCursorMemory();
    LevelInterface.Engine->LoadSaveGameConfigData(0);
    FrontEndSpawnPickups(PickupTypes, Pickups);
    switch (screen = LevelInterface.Engine->FrontEndStartScreen) {
    case 0:
    default:
        GOTO_TITLE();
        break;
    case 1:
        StartedInCredits = screen;
        GOTO_CREDITS();
        break;
    case 2:
        GOTO_MULTIPLAYER();
        break;
    }
    *LevelInterface.Engine->MainLoop = FrontEndMainLoop;
    LevelInterface.Engine->Controllerfunc = FrontEndCheckController;
    LevelInterface.Engine->Lightingfunc = FrontEndLighting;
    LevelInterface.Engine->Draw256func = FrontEndDraw256;
}

struct FET_MESSAGE FET_REINSERT_CONTROLLER[4] = {
    { "PLEASE INSERT", 256, 94, 0, 0, 0, 0 },
    { "CONTROLLER INTO", 256, 120, 0, 0, 0, 0 },
    { "CONTROLLER PORT XX", 256, 146, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};

void * D_116e4[47] = {
    (void *)GOTO_TITLE, (void *)func_cdd8, (void *)GOTO_OPTIONS, (void *)GOTO_PLAYER, (void *)func_cb88, (void *)func_cc20, (void *)GOTO_CONTROLS, (void *)func_d010,
    (void *)func_cd54, (void *)func_d0c0, (void *)GOTO_MULTIPLAYER, 0, (void *)GOTO_GAME_VARIABLES, (void *)GOTO_MULTIPLAYER_SETUP, (void *)GOTO_RULES, (void *)GOTO_CREDITS,
    0, 0, (void *)func_d4d0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
};

short CreditsTimer = 0;

struct FET_MESSAGE *CreditsLine = 0;

void FrontEndCheckController(void)
{
    char buf[32];
    char port;
    short screen;

    screen = LevelInterface.Engine->currentscreen;
    if (screen != 11 && screen != 16 && screen != 17 && screen != 19 && screen != 20
        && LevelInterface.Engine->SnapShotPadViolation(0, &port) == 1) {
        FrontEndInitScreen(0, 20);
        LevelInterface.Engine->FrontEndCreateMenu(FET_REINSERT_CONTROLLER, 16);
        LevelInterface.Engine->sprintf(buf, "CONTROLLER PORT %s", LevelInterface.Engine->ControlGetPortName(port));
        LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[LevelInterface.Engine->cursorupper - 1], buf, 256, 146, 16, 0);
        LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
        LevelInterface.Engine->Specialfunc = GOTO_REINSERT_WAIT;
    }
}

void FrontEndSpawnPickups(int *Types, struct CREATURE **Out)
{
    short n = 0;
    struct PICKUPENTRY Entry = { { { 0, 0, 0 }, 0, 1 } };
    struct CREATURE *Pickup;

    for (;;) {
        short *unused;
        if ((short)*Types < 0)
            break;
        Entry.SimStart.Ident = *Types;
        Pickup = LevelInterface.Engine->InitialiseQuakePickup(&Entry);
        Out[n++] = Pickup;
        Pickup->Ang.vy = 2048;
        Pickup->Control = 0;
        Pickup->Draw.Location.vx = 0;
        Pickup->Draw.Location.vy = 0;
        Pickup->Draw.Location.vz = 1700;
        LevelInterface.Engine->RotMatrixYXZ(&Pickup->Ang, &Pickup->Matrix);
        Types++;
    }
}

void CHECK_FOR_START(void)
{
    char buf[32];

    if (*LevelInterface.Engine->NumPadds < 2) {
        LevelInterface.Engine->sprintf(buf, "%c%s", 'g', LevelInterface.Engine->MyStringTable[2].message);
        LevelInterface.Engine->FrontEndModifyText(LevelInterface.Engine->MyStringTable[2].message, buf);
        LevelInterface.Engine->cursorpos = LevelInterface.Engine->cursorlower;
    }
}

void GOTO_PLAYER(void)
{
    FrontEndInitScreen("PLAYER", 3);
    LevelInterface.Engine->FrontEndCreateMenu(FET_PLAYER, 16);
    LevelInterface.Engine->Backfunc = GOTO_OPTIONS;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->CHECK_FOR_SLIDER;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void func_cb88(void)
{
    FrontEndInitScreen("SOUND", 4);
    LevelInterface.Engine->FrontEndCreateMenu(FET_SOUND, 16);
    LevelInterface.Engine->Backfunc = GOTO_OPTIONS;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->CHECK_FOR_SLIDER;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void func_cc20(void)
{
    FrontEndInitScreen("VIDEO", 5);
    LevelInterface.Engine->FrontEndCreateMenu(FET_VIDEO, 16);
    LevelInterface.Engine->Backfunc = GOTO_OPTIONS;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->CHECK_FOR_SLIDER;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void FrontEndInitScreen(char *Title, short screen)
{
    short i;

    LevelInterface.Engine->InGameInitScreen(screen);
    for (i = 0; i < 5; i++) {
        WINFLAGS(Pickups[i], 0)->NoDraw = 1;
        WINFLAGS(Pickups[i], 1)->NoDraw = 1;
        WINFLAGS(Pickups[i], 2)->NoDraw = 1;
        WINFLAGS(Pickups[i], 3)->NoDraw = 1;
    }
    if (Title) {
        LevelInterface.Engine->FrontEndAddText(LevelInterface.Engine->MyStringTable, Title, 256, 32, 32, 0);
        LevelInterface.Engine->MyStringTable->cursorItem = 1;
        STRFLAGS(LevelInterface.Engine->MyStringTable)->bannerColour = 0;
        LevelInterface.Engine->cursorupper = 1;
    }
    if (LevelInterface.Engine->currentscreen != 11)
        WINFLAGS(Pickups[0], 0)->NoDraw = 0;
    if (LevelInterface.Engine->currentscreen == 0)
        Pickups[0]->Primary = PickupSpinTitle;
    else
        Pickups[0]->Primary = PickupSpinMenu;
}

void GOTO_CREDITS(void)
{
    FrontEndInitScreen("CREDITS", 15);
    LevelInterface.Engine->FrontEndCreateMenu(FET_CREDITS_TITLE, 16);
    CreditsTimer = 0;
    LevelInterface.Engine->cursorupper = LevelInterface.Engine->maxcursor = 17;
    CreditsLine = FET_CREDITS;
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    if (StartedInCredits) {
        StartedInCredits = 0;
        LevelInterface.Engine->Backfunc = GOTO_UNLOCK_SAVE;
    } else
        LevelInterface.Engine->Backfunc = GOTO_OPTIONS;
    LevelInterface.Engine->Specialfunc = CreditsScroll;
}

void CreditsScroll(void)
{
    struct SCRPNT pos;
    DRAWENV *env;
    unsigned long *ot;
    short i;
    short count;

    CreditsTimer = (CreditsTimer + 1) & 0xff;
    ot = &(*LevelInterface.Engine->currdb)->BigOt[6];
    if (*LevelInterface.Engine->DbFrame) {
        pos = (struct SCRPNT){LevelInterface.Engine->GameWindows->Size.sx + LevelInterface.Engine->GameWindows->Pos.sx,
                              LevelInterface.Engine->GameWindows->Pos.sy + 60};
        env = &CreditsEnv[0];
    } else {
        pos = (struct SCRPNT){LevelInterface.Engine->GameWindows->Pos.sx, LevelInterface.Engine->GameWindows->Pos.sy + 60};
        env = &CreditsEnv[1];
    }
    LevelInterface.Engine->SetDefDrawEnv(env, pos.sx, pos.sy, 512, 144);
    env->isbg = 0;
    env->ofs[1] -= 60;
    LevelInterface.Engine->SetDrawEnv(&env->dr_env, env);
    LevelInterface.Engine->AddPrim_(ot, &env->dr_env);
    if (!(CreditsTimer & 15)) {
        CreditsLine++;
        if (CreditsLine->menuName)
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[(short)(((CreditsTimer >> 4) & 15) + 1)],
                                                   CreditsLine->menuName, CreditsLine->x, 210, 16, 0);
        else
            CreditsLine--;
    }
    count = 0;
    for (i = 1; i < LevelInterface.Engine->cursorlower; i++) {
        LevelInterface.Engine->MyStringTable[i].y--;
        if (LevelInterface.Engine->MyStringTable[i].message[0])
            count++;
        if (LevelInterface.Engine->MyStringTable[i].y < 45) {
            LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[i].message, "%s", "");
            LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[i].originalMessage, "%s", "");
        }
    }
    if (!count && LevelInterface.Engine->Backfunc)
        ((void (*)(void))LevelInterface.Engine->Backfunc)();
}

void GOTO_CONTROLS(void)
{
    int i;

    FrontEndInitScreen("CONTROLS", 6);
    LevelInterface.Engine->cursorupper += 18;
    LevelInterface.Engine->maxcursor += 18;
    LevelInterface.Engine->FrontEndCreateMenu(FET_CONTROLLER, 16);
    LevelInterface.Engine->Backfunc = GOTO_PLAYER;
    LevelInterface.Engine->Specialfunc = CHECK_FOR_CONTROLS;
    for (i = LevelInterface.Engine->cursorlower; i < LevelInterface.Engine->cursorupper; i++)
        STRFLAGS(&LevelInterface.Engine->MyStringTable[i])->dontWrap = 1;
    FrontEndShowControls();
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

char *D_117a8[18] = {
    "RIGHT MOUSE", " NEXT WEAPON ", " PREV WEAPON ", 0, 0, 0, 0, 0,
    0, 0, 0, "   FORWARDS", "  STEP LEFT", "  BACKWARDS", " STEP RIGHT", "FIRE      ",
    "JUMP      ", "LOOK      ",
};

char *D_117f0[18] = {
    "RIGHT MOUSE 2", " NEXT WEAPON ", " PREV WEAPON ", 0, 0, 0, 0, 0,
    0, 0, 0, "   FORWARDS", "  STEP LEFT", "  BACKWARDS", " STEP RIGHT", "JUMP      ",
    "FIRE      ", "LOOK      ",
};

char *D_11838[18] = {
    "HUNTER MOUSE", " NEXT WEAPON ", " PREV WEAPON ", 0, 0, 0, 0, 0,
    0, 0, 0, "       JUMP", "  STEP LEFT", "  BACKWARDS", " STEP RIGHT", "FIRE      ",
    "FORWARDS  ", "LOOK      ",
};

char *D_11880[18] = {
    "RIGHT STICK", "        JUMP ", "        FIRE ", " PREV WEAPON", " NEXT WEAPON", 0, 0, 0,
    0, 0, " LOOK       ", "    FORWARDS ", "   STEP LEFT ", "   BACKWARDS ", "  STEP RIGHT ", 0,
    0, 0,
};

char *D_118c8[18] = {
    "LEFT STICK", " PREV WEAPON ", " NEXT WEAPON ", "JUMP      ", "FIRE      ", "FORWARDS  ", "STEP RIGHT", "BACKWARDS ",
    "STEP LEFT ", "       LOOK", 0, 0, 0, 0, 0, 0,
    0, 0,
};

char *D_11910[18] = {
    "BOTH STICKS", " NEXT WEAPON ", " PREV WEAPON ", "JUMP      ", "FIRE      ", 0, 0, 0,
    0, "TURN,FORWARDS  ", "STEP,LOOK", 0, 0, 0, 0, 0,
    0, 0,
};

char *D_11958[18] = {
    "STANDARD A", "   STEP LEFT ", "   LOOK DOWN ", " STEP RIGHT ", " LOOK UP    ", " NEXT WEAPON", " PREV WEAPON", " FIRE       ",
    " JUMP       ", 0, 0, "    FORWARDS ", "   TURN LEFT ", "   BACKWARDS ", "  TURN RIGHT ", 0,
    0, 0,
};

char *D_119a0[18] = {
    "STANDARD B", "   STEP LEFT ", " PREV WEAPON ", " STEP RIGHT ", " NEXT WEAPON", " LOOK UP    ", " LOOK DOWN  ", " FIRE       ",
    " JUMP       ", 0, 0, "    FORWARDS ", "   TURN LEFT ", "   BACKWARDS ", "  TURN RIGHT ", 0,
    0, 0,
};

char *D_119e8[18] = {
    "STANDARD C", "   TURN LEFT ", "   LOOK DOWN ", " TURN RIGHT ", " LOOK UP    ", " NEXT WEAPON", " PREV WEAPON", " FIRE       ",
    " JUMP       ", 0, 0, "    FORWARDS ", "   STEP LEFT ", "   BACKWARDS ", "  STEP RIGHT ", 0,
    0, 0,
};

char **ControllerConfigs[9] = {
    D_117a8, D_117f0, D_11838, D_11880, D_118c8, D_11910, D_11958, D_119a0, D_119e8,
};

short ControlsTextX[18] = {
    256, 92, 92, 412, 412, 412, 412, 412, 412, 92, 412, 92, 92, 92, 92, 412,
    412, 412,
};

short ControlsTextY[18] = {
    178, 48, 64, 48, 64, 80, 99, 124, 138, 162, 162, 80, 100, 124, 138, 72,
    96, 134,
};

void FrontEndShowControls(void)
{
    char buf[16];
    char **text;
    char **p;
    short i;

    switch (LevelInterface.Engine->GameWindows->Input.Method) {
    case 9:
    case 10:
        text = ControllerConfigs[SCHAR(LevelInterface.Engine->GameConfig.MultiPlayerParams[0].mouseConfigSelected)];
        break;
    case 5:
    case 7:
        text = ControllerConfigs[SCHAR(LevelInterface.Engine->GameConfig.MultiPlayerParams[0].analogConfigSelected)];
        break;
    case 4:
    case 6:
    case 8:
    default:
        text = ControllerConfigs[SCHAR(LevelInterface.Engine->GameConfig.MultiPlayerParams[0].standardConfigSelected)];
        break;
    }
    for (i = 1; i < 18; i++) {
        p = text + i;
        if (*p)
            LevelInterface.Engine->sprintf(buf, *p);
        else
            LevelInterface.Engine->sprintf(buf, "");
        LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[i], buf, ControlsTextX[i], ControlsTextY[i], 8, 0);
    }
    LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[LevelInterface.Engine->cursorlower], text[0], 256, 178, 16, 0);
}


void CHECK_FOR_CONTROLS(void)
{
    char buf[32];
    unsigned short pad;
    char *config;
    int lo, hi;
    int mouse;
    int analog;

    mouse = 0;
    pad = LevelInterface.Engine->gFrontEndPadShot;
    analog = 1;
    switch (LevelInterface.Engine->GameWindows->Input.Method) {
    case 9:
    case 10:
        lo = 0;
        hi = 3;
        mouse = 1;
        config = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].mouseConfigSelected;
        break;
    case 5:
    case 7:
        lo = 3;
        hi = 6;
        config = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].analogConfigSelected;
        break;
    case 4:
    case 6:
    case 8:
    default:
        lo = 6;
        hi = 9;
        analog = 0;
        config = &LevelInterface.Engine->GameConfig.MultiPlayerParams[0].standardConfigSelected;
        break;
    }
    if (LevelInterface.Engine->cursorpos == LevelInterface.Engine->cursorlower) {
        if (pad & 0x80) {
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
            (*config)--;
        }
        if (pad & 0x20) {
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
            (*config)++;
        }
    }
    if (SCHAR(*config) < lo)
        *config = hi - 1;
    if (SCHAR(*config) >= hi)
        *config = lo;
    LevelInterface.Engine->GameConfig.MultiPlayerParams[0].controllerSelected = SCHAR(*config);
    if (!LevelInterface.Engine->IsPlayerPadShockable(0)) {
        if (LevelInterface.Engine->MyStringTable[20].originalMessage[0] != 'g') {
            LevelInterface.Engine->sprintf(buf, "g%s", LevelInterface.Engine->MyStringTable[20].originalMessage);
            LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[20].originalMessage, "%s", buf);
            LevelInterface.Engine->MyStringTable[20].cursorDisabled = 1;
            if (LevelInterface.Engine->cursorpos == 20)
                LevelInterface.Engine->cursorpos--;
        }
    } else if (LevelInterface.Engine->MyStringTable[20].originalMessage[0] == 'g') {
        LevelInterface.Engine->sprintf(buf, "%s", &LevelInterface.Engine->MyStringTable[20].originalMessage[1]);
        LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[20].originalMessage, "%s", buf);
        LevelInterface.Engine->MyStringTable[20].cursorDisabled = 0;
    }
    if (!analog) {
        if (LevelInterface.Engine->MyStringTable[21].originalMessage[0] != 'g') {
            LevelInterface.Engine->sprintf(buf, "g%s", LevelInterface.Engine->MyStringTable[21].originalMessage);
            LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[21].originalMessage, "%s", buf);
            LevelInterface.Engine->MyStringTable[21].cursorDisabled = 1;
            if (LevelInterface.Engine->cursorpos == 21)
                LevelInterface.Engine->cursorpos--;
        }
    } else if (LevelInterface.Engine->MyStringTable[21].originalMessage[0] == 'g') {
        LevelInterface.Engine->sprintf(buf, "%s", &LevelInterface.Engine->MyStringTable[21].originalMessage[1]);
        LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[21].originalMessage, "%s", buf);
        LevelInterface.Engine->MyStringTable[21].cursorDisabled = 0;
    }
    if (!mouse && !LevelInterface.Engine->gFreeMice) {
        if (LevelInterface.Engine->MyStringTable[22].originalMessage[0] != 'g') {
            LevelInterface.Engine->sprintf(buf, "g%s", LevelInterface.Engine->MyStringTable[22].originalMessage);
            LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[22].originalMessage, "%s", buf);
            LevelInterface.Engine->MyStringTable[22].cursorDisabled = 1;
            if (LevelInterface.Engine->cursorpos == 22)
                LevelInterface.Engine->cursorpos--;
        }
    } else if (LevelInterface.Engine->MyStringTable[22].originalMessage[0] == 'g') {
        LevelInterface.Engine->sprintf(buf, "%s", &LevelInterface.Engine->MyStringTable[22].originalMessage[1]);
        LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[22].originalMessage, "%s", buf);
        LevelInterface.Engine->MyStringTable[22].cursorDisabled = 0;
    }
    if (!mouse) {
        LevelInterface.Engine->sprintf(buf, "%c%s", 'g', LevelInterface.Engine->MyStringTable[23].message);
        LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[23], buf);
        LevelInterface.Engine->MyStringTable[23].cursorDisabled = 1;
        if (LevelInterface.Engine->cursorpos == 23) {
            LevelInterface.Engine->cursorpos--;
            if (!LevelInterface.Engine->gFreeMice)
                LevelInterface.Engine->cursorpos--;
        }
    } else
        LevelInterface.Engine->MyStringTable[23].cursorDisabled = 0;
    FrontEndShowControls();
    LevelInterface.Engine->CHECK_FOR_SLIDER();
    LevelInterface.Engine->CHECK_FOR_MCFILEWINDOW();
}

void func_d0c0(void)
{
    FrontEndInitScreen("DIFFICULTY", 9);
    LevelInterface.Engine->FrontEndCreateMenu(FET_EASY, 16);
    LevelInterface.Engine->Backfunc = func_cd54;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->cursorlower + 1;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void func_d15c(void)
{
    FrontEndInitScreen("LOAD", 16);
    LevelInterface.Engine->Specialfunc = func_9f08;
    LevelInterface.Engine->gFrontEndLoadType = 0;
    LOADFLAGS(LevelInterface.Engine)->LoadSubType = 0;
    LevelInterface.Engine->Backfunc = GOTO_MULTIPLAYER;
    LevelInterface.Engine->MemoryCardSetStatus(1);
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

/* after the credits: offer to save a file unlocking the cheats this difficulty earned */
void GOTO_UNLOCK_SAVE(void)
{
    char buf[32];

    FrontEndInitScreen(0, 17);
    LevelInterface.Engine->Backfunc = 0;
    LevelInterface.Engine->FrontEndCreateMenu(FET_WELL_DONE, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_SAVE_UNLOCK_YESNO, 16);
    switch (LevelInterface.Engine->GameConfig.sDifficulty) {
    case 0:
        LevelInterface.Engine->GameConfig.cCheats = 1;
        LevelInterface.Engine->sprintf(buf, "FOR BRONZE CHEATS?");
        break;
    case 1:
        LevelInterface.Engine->GameConfig.cCheats = 2;
        LevelInterface.Engine->sprintf(buf, "FOR SILVER CHEATS?");
        break;
    case 2:
        LevelInterface.Engine->GameConfig.cCheats = 3;
        LevelInterface.Engine->sprintf(buf, "FOR GOLD CHEATS?");
        break;
    }
    LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[3].originalMessage, buf);
    LevelInterface.Engine->sprintf(LevelInterface.Engine->MyStringTable[3].message, buf);
}

void func_d4d0(void)
{
    if ((*(unsigned int *)LOADFLAGS(LevelInterface.Engine) & 0x7f000000) == 0x01000000) {
        FrontEndInitScreen(0, 18);
        LevelInterface.Engine->FrontEndCreateMenu(FET_DO_YOU_WANT_TO_LOAD, 16);
        LevelInterface.Engine->FrontEndCreateMenu(FET_YES, 16);
        LevelInterface.Engine->Backfunc = 0;
        LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
    } else
        GOTO_MULTIPLAYER();
}

void GOTO_MULTIPLAYER(void)
{
    FrontEndInitScreen("MULTIPLAYER", 10);
    LevelInterface.Engine->FrontEndCreateMenu(FET_DEATHMATCH, 16);
    LevelInterface.Engine->Backfunc = func_cdd8;
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MULTI_MODE;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void CHECK_FOR_MULTI_MODE(void)
{
    char unused[32]; /* the original's frame has room for it */
    int i;

    switch (LevelInterface.Engine->cursorpos - LevelInterface.Engine->cursorlower) {
    case 0:
        LevelInterface.Engine->GameConfig.sMultiMode = 0;
        break;
    case 1:
        LevelInterface.Engine->GameConfig.sMultiMode = 1;
        break;
    case 2:
        LevelInterface.Engine->GameConfig.sMultiMode = 5;
        break;
    }
    if (LevelInterface.Engine->cursorpos - LevelInterface.Engine->cursorlower < 3) {
        LevelInterface.Engine->MoveBottomPoly(2, 220);
        if (LevelInterface.Engine->gFrontEndPadShot & 0x8000) {
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}),
                                                   &Pickups[0]->Draw.Location);
            GOTO_RULES();
            return;
        }
    } else
        LevelInterface.Engine->MoveBottomPoly(2, 260);
    if (*LevelInterface.Engine->NumPadds < 2) {
        FrontEndInitScreen("START", 1);
        LevelInterface.Engine->FrontEndCreateMenu(FET_SINGLE_MULTI, 16);
        LevelInterface.Engine->SetupScreen1by1(0);
        for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
            LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
        LevelInterface.Engine->Specialfunc = CHECK_FOR_START;
        LevelInterface.Engine->Backfunc = GOTO_TITLE;
        LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
    }
}

void GOTO_RULES(void)
{
    switch (LevelInterface.Engine->GameConfig.sMultiMode) {
    case 0:
        FrontEndInitScreen("DEATHMATCH", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES, 16);
        break;
    case 1:
        FrontEndInitScreen("TEAM DEATHMATCH", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES_2, 16);
        break;
    case 2:
        FrontEndInitScreen("CAPTURE THE FLAG", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES_3, 16);
        break;
    case 3:
        FrontEndInitScreen("TAG", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES_4, 16);
        break;
    case 4:
        FrontEndInitScreen("TEAM TAG", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES_5, 16);
        break;
    case 5:
        FrontEndInitScreen("VERSUS", 14);
        LevelInterface.Engine->FrontEndCreateMenu(FET_RULES_6, 16);
        break;
    }
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Backfunc = GOTO_MULTIPLAYER;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void GOTO_GAME_VARIABLES(void)
{
    FrontEndInitScreen("GAME VARIABLES", 12);
    if (LevelInterface.Engine->GameConfig.cCheats == 3)
        LevelInterface.Engine->FrontEndCreateMenu(FET_MULTI_CHEAT_GOLD, 16);
    else if (LevelInterface.Engine->GameConfig.cCheats == 2)
        LevelInterface.Engine->FrontEndCreateMenu(FET_MULTI_CHEAT_SILVER, 16);
    else if (LevelInterface.Engine->GameConfig.cCheats == 1)
        LevelInterface.Engine->FrontEndCreateMenu(FET_MULTI_CHEAT_BRONZE, 16);
    else
        LevelInterface.Engine->FrontEndCreateMenu(FET_MULTI_CHEAT_NONE, 16);
    LevelInterface.Engine->Backfunc = GOTO_MULTIPLAYER_SETUP;
    LevelInterface.Engine->Specialfunc = LevelInterface.Engine->CHECK_FOR_SLIDER;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void GOTO_MULTIPLAYER_SETUP(void)
{
    char *name;
    char *dst;
    short j;
    short i;
    char c;

    LevelInterface.Engine->SetupScreen1by1(0);
    for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
        LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
    switch (LevelInterface.Engine->GameConfig.sMultiMode) {
    case 0:
        FrontEndInitScreen("DEATHMATCH", 13);
        break;
    case 1:
        FrontEndInitScreen("TEAM DEATHMATCH", 13);
        break;
    case 2:
        FrontEndInitScreen("CAPTURE THE FLAG", 13);
        break;
    case 3:
        FrontEndInitScreen("TAG", 13);
        break;
    case 4:
        FrontEndInitScreen("TEAM TAG", 13);
        break;
    case 5:
        FrontEndInitScreen("VERSUS", 13);
        break;
    }
    if (LevelInterface.Engine->GameConfig.sMultiMode != 5)
        LevelInterface.Engine->FrontEndCreateMenu(FET_2_3_4_PLAYERS, 16);
    else
        LevelInterface.Engine->FrontEndCreateMenu(FET_2_3_4_PLAYERS_2, 16);
    dst = (char *)D_12db8;
    name = ((struct LEVELDATA *)LevelInterface.Engine->GetLevelDataFromIdentityName("COLD STORAGE"))[LevelInterface.Engine->GameConfig.bArena].Name.Name;
    for (j = 0; (c = *name) != 0 && j < 12; name++) {
        if (c == ' ' || (unsigned char)(c - '0') < 10)
            dst[j++] = c;
        else
            dst[j++] = c & ~0x20;
    }
    dst[j] = 0;
    func_4d78();
    LevelInterface.Engine->Backfunc = GOTO_MULTIPLAYER;
    LevelInterface.Engine->Specialfunc = func_50e4;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->cursorlower;
}

void func_4d78(void)
{
    char buf[32];
    short line;

    line = LevelInterface.Engine->cursorlower;
    if (line == LevelInterface.Engine->cursorpos) {
        LevelInterface.Engine->sprintf(buf, "%c2 %c3 %c4 bPLAYERS",
            LevelInterface.Engine->GameConfig.sNumPlayers == 2 ? 'b' : 'd',
            *LevelInterface.Engine->NumPadds >= 3 ? (LevelInterface.Engine->GameConfig.sNumPlayers == 3 ? 'b' : 'd') : 'g',
            *LevelInterface.Engine->NumPadds >= 4 ? (LevelInterface.Engine->GameConfig.sNumPlayers == 4 ? 'b' : 'd') : 'g');
    } else {
        LevelInterface.Engine->sprintf(buf, "%d PLAYERS", LevelInterface.Engine->GameConfig.sNumPlayers);
    }
    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[line], buf);
    LevelInterface.Engine->sprintf(buf, D_12db8);
    line++;
    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[line], buf);
    line++;
    if (LevelInterface.Engine->GameConfig.sMultiMode == 5) {
        LevelInterface.Engine->sprintf(buf, "ROUNDS %2d", ScoreLimits[ScoreLimitIndex]);
    } else {
        LevelInterface.Engine->sprintf(buf, "FRAG LIMIT %2d", FragLimits[FragLimitIndex]);
        if (FragLimits[FragLimitIndex] == -1)
            LevelInterface.Engine->sprintf(buf, "FRAG LIMIT  i");
    }
    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[line], buf);
    line++;
    if (LevelInterface.Engine->GameConfig.sMultiMode != 5) {
        LevelInterface.Engine->sprintf(buf, "TIME LIMIT %2d", TimeLimits[TimeLimitIndex]);
        if (TimeLimits[TimeLimitIndex] == -1)
            LevelInterface.Engine->sprintf(buf, "TIME LIMIT  i");
        LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[line], buf);
    }
}

void func_50e4(void)
{
    char unused[64];
    unsigned short pressed;
    short max;
    short row;
    short old;
    unsigned char *src;
    unsigned char c;
    short j;
    char *name;
    int i;

    pressed = LevelInterface.Engine->gFrontEndPadShot;
    max = *LevelInterface.Engine->NumPadds > 4 ? 4 : *LevelInterface.Engine->NumPadds;
    if (*LevelInterface.Engine->NumPadds < 2) {
        FrontEndInitScreen("START", 1);
        LevelInterface.Engine->FrontEndCreateMenu(FET_SINGLE_MULTI, 16);
        LevelInterface.Engine->SetupScreen1by1(0);
        for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
            LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
        LevelInterface.Engine->Specialfunc = CHECK_FOR_START;
        LevelInterface.Engine->Backfunc = GOTO_TITLE;
        LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
        return;
    }
    row = LevelInterface.Engine->cursorlower;
    if (row == LevelInterface.Engine->cursorpos) {
        if (max < LevelInterface.Engine->GameConfig.sNumPlayers)
            LevelInterface.Engine->GameConfig.sNumPlayers--;
        old = LevelInterface.Engine->GameConfig.sNumPlayers;
        if (pressed & 0x80)
            LevelInterface.Engine->GameConfig.sNumPlayers--;
        if (pressed & 0x20)
            LevelInterface.Engine->GameConfig.sNumPlayers++;
        if (LevelInterface.Engine->GameConfig.sNumPlayers < 2)
            LevelInterface.Engine->GameConfig.sNumPlayers = max;
        else if (LevelInterface.Engine->GameConfig.sNumPlayers > max)
            LevelInterface.Engine->GameConfig.sNumPlayers = 2;
        if (old != LevelInterface.Engine->GameConfig.sNumPlayers)
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
    }
    if (++row == LevelInterface.Engine->cursorpos) {
        if (pressed & 0x80) {
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
            LevelInterface.Engine->GameConfig.bArena--;
        }
        if (pressed & 0x20) {
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
            LevelInterface.Engine->GameConfig.bArena++;
        }
        if (pressed & 0xa0) {
            if (LevelInterface.Engine->GameConfig.bArena < 0)
                LevelInterface.Engine->GameConfig.bArena = 11;
            else if (LevelInterface.Engine->GameConfig.bArena > 11)
                LevelInterface.Engine->GameConfig.bArena = 0;
            name = (char *)D_12db8;
            src = (unsigned char *)&((struct LEVELDATA *)LevelInterface.Engine->GetLevelDataFromIdentityName("COLD STORAGE"))[LevelInterface.Engine->GameConfig.bArena].Name;
            j = 0;
            for (c = *src; c && j < 12; c = *++src) {
                if (c == ' ' || (c >= '0' && c <= '9'))
                    name[j++] = c;
                else
                    name[j++] = c & 0xdf;
            }
            name[j] = 0;
        }
    }
    if (++row == LevelInterface.Engine->cursorpos) {
        if (LevelInterface.Engine->GameConfig.sMultiMode == 5) {
            old = ScoreLimitIndex;
            if (pressed & 0x80)
                ScoreLimitIndex--;
            if (pressed & 0x20)
                ScoreLimitIndex++;
            if (ScoreLimitIndex < 0)
                ScoreLimitIndex = 4;
            else if (ScoreLimitIndex > 4)
                ScoreLimitIndex = 0;
            if (old != ScoreLimitIndex)
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
        } else {
            old = FragLimitIndex;
            if (pressed & 0x80)
                FragLimitIndex--;
            if (pressed & 0x20)
                FragLimitIndex++;
            if (FragLimitIndex < 0)
                FragLimitIndex = 8;
            else if (FragLimitIndex > 8)
                FragLimitIndex = 0;
            if (old != FragLimitIndex)
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
        }
    }
    if (++row == LevelInterface.Engine->cursorpos && LevelInterface.Engine->GameConfig.sMultiMode != 5) {
        old = TimeLimitIndex;
        if (pressed & 0x80)
            TimeLimitIndex--;
        if (pressed & 0x20)
            TimeLimitIndex++;
        if (TimeLimitIndex < 0)
            TimeLimitIndex = 11;
        else if (TimeLimitIndex > 11)
            TimeLimitIndex = 0;
        if (old != TimeLimitIndex)
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                   &LevelInterface.Engine->Creatures->Draw.Location);
    }
    func_4d78();
}

void * D_11a9c[3] = {
    "CONTROLLER", "MOUSE", "MULTI TAP",
};

short D_11aa8 = 0;

int D_11aac = 0;

int D_11ab0 = 0;

int D_11ab4 = 0;

int D_11ab8[2] = {
    0, 0,
};

int D_11ac0 = 0;

void func_59e0(void)
{
    short i, j;

    FrontEndInitScreen(0, 11);
    for (i = 0; i < 4; i++) {
        if (i >= LevelInterface.Engine->GameConfig.sNumPlayers) {
            for (j = 0; j < 4; j++)
                Pickups[j + 1]->WinData[i].NoDraw = 1;
        }
    }
    Pickups[1]->Primary = PickupSpinTitle;
    Pickups[2]->Primary = PickupSpinTitle;
    Pickups[3]->Primary = PickupSpinTitle;
    Pickups[4]->Primary = PickupSpinTitle;
    switch (LevelInterface.Engine->GameConfig.sNumPlayers) {
    case 2:
        LevelInterface.Engine->SetupScreen1by2(0);
        for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
            LevelInterface.Engine->AlterFocalSubDiv(i, 210, 4000);
        break;
    case 3:
        LevelInterface.Engine->SetupScreen2by2(0);
        for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
            LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
        break;
    case 4:
        LevelInterface.Engine->SetupScreen2by2(0);
        for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
            LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
        break;
    }
    for (i = 0; i < LevelInterface.Engine->GameConfig.sNumPlayers; i++) {
        LevelInterface.Engine->maxcursor += 7;
        LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 0;
        LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 1;
        LevelInterface.Engine->GameConfig.MultiPlayerParams[i].currentLetter = 0;
        D_12c20[i] = 0;
        D_12ce8[i] = 2;
        LevelInterface.Engine->gFrontEndOPad[i] = LevelInterface.Engine->gFrontEndPad[i];
    }
    LevelInterface.Engine->Specialfunc = func_5da8;
    *LevelInterface.Engine->DoSnapShot = 1;
}

#define ITM_PKUP (struct IDENTITYNAME){"itm_pkup"}

void func_5da8(void)
{
    short Unused;
    short *Sel;
    short Limit;
    short i;
    struct MULTIPLAYER *Mp;
    char *Letter;
    short Len;
    short Shot;
    short Old;

    D_12ae4 = 0;
    D_12ddc[0] = D_12b58[*LevelInterface.Engine->DbFrame];
    D_12dac = D_12ddc[0] + 100;
    D_12c24[0] = &D_12ae8[*LevelInterface.Engine->DbFrame];
    D_12b4c[0] = D_12c24[0] + 1;
    func_24fc(0, 2, 2, 1, (struct COLOUR){64, 64, 64}, 1);
    for (i = 0; i < LevelInterface.Engine->GameConfig.sNumPlayers; i++) {
        Old = LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos;
        Shot = LevelInterface.Engine->gFrontEndPad[i] & (LevelInterface.Engine->gFrontEndPad[i] ^ LevelInterface.Engine->gFrontEndOPad[i]);
        if (Shot & 0x10) {
            do {
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos--;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos < 0)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 6;
            } while (LevelInterface.Engine->MyStringTable[i * 7 + LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos].cursorDisabled);
        } else if (Shot & 0x40) {
            do {
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos++;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos >= 7)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
            } while (LevelInterface.Engine->MyStringTable[i * 7 + LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos].cursorDisabled);
        }
        if (Old != LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos)
            LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu2"}),
                                                   &Pickups[0]->Draw.Location);
        if (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode != 6 &&
            LevelInterface.Engine->SnapShotPadViolation(i, &Unused)) {
            D_12ce8[i] = LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos;
            D_12c20[i] = LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode;
            LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
            LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 6;
        }
        switch (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode) {
        case 0:
            if (Shot & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                GOTO_MULTIPLAYER_SETUP();
                return;
            }
            if (Shot & 0x4000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}),
                                                       &Pickups[0]->Draw.Location);
                switch (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos) {
                case 0:
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 1;
                    break;
                case 1:
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 5;
                    break;
                }
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
            }
            break;
        case 1:
            if (Shot & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 0;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
            } else if (Shot & 0x4000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}),
                                                       &Pickups[0]->Draw.Location);
                switch (LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos) {
                case 0:
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 2;
                    break;
                case 1:
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 3;
                    break;
                case 2:
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 4;
                    break;
                }
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
            }
            break;
        case 2:
            Mp = &LevelInterface.Engine->GameConfig.MultiPlayerParams[i];
            Letter = &Mp->playerName[Mp->currentLetter];
            for (Len = 0; Letter[-Mp->currentLetter + Len]; Len++)
                ;
            if (Shot & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 1;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 0;
                break;
            }
            if (Shot & 0x10) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}),
                                                       &Pickups[0]->Draw.Location);
                *Letter = *Letter + 1;
            } else if (Shot & 0x40) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}),
                                                       &Pickups[0]->Draw.Location);
                *Letter = *Letter - 1;
            }
            if (*Letter == '[')
                *Letter = '0';
            if (*Letter == ':')
                *Letter = ' ';
            if (*Letter == '!')
                *Letter = 'A';
            if (*Letter == '@')
                *Letter = ' ';
            if (*Letter == 0x1f)
                *Letter = '9';
            if (*Letter == '/')
                *Letter = 'Z';
            Old = Mp->currentLetter;
            if (Shot & 0x80) {
                Mp->currentLetter = Old - 1;
                if (Mp->currentLetter + 2 == Len && Mp->currentLetter + 2 >= 2 && *Letter == ' ')
                    *Letter = 0;
                if (Mp->currentLetter < 0)
                    Mp->currentLetter = Len - 1;
            } else if (Shot & 0x20) {
                Mp->currentLetter = Old + 1;
                if (Mp->currentLetter == Len && Mp->currentLetter != 8)
                    Letter[1] = ' ';
                if (Mp->currentLetter >= 8)
                    Mp->currentLetter = 0;
            }
            if (Old != Mp->currentLetter)
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum(ITM_PKUP),
                                                       &LevelInterface.Engine->Creatures->Draw.Location);
            break;
        case 3:
            switch (LevelInterface.Engine->GameConfig.sMultiMode) {
            case 0:
            case 3:
            case 5:
                Limit = 4;
                Sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[i].playerSelected;
                break;
            case 1:
            case 2:
            case 4:
                Limit = 2;
                Sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[i].teamSelected;
                break;
            }
            if (Shot & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 1;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 1;
            } else if (Shot & 0x80) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum(ITM_PKUP),
                                                       &LevelInterface.Engine->Creatures->Draw.Location);
                if (--*Sel < 0)
                    *Sel = Limit - 1;
            } else if (Shot & 0x20) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum(ITM_PKUP),
                                                       &LevelInterface.Engine->Creatures->Draw.Location);
                if (++*Sel >= Limit)
                    *Sel = 0;
            }
            break;
        case 5:
            if (Shot & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = 0;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = 1;
            }
            break;
        case 6:
            if (!LevelInterface.Engine->SnapShotPadViolation(i, &Unused)) {
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode = D_12c20[i];
                LevelInterface.Engine->GameConfig.MultiPlayerParams[i].cursorpos = D_12ce8[i];
            }
            break;
        }
        D_12ae4 += LevelInterface.Engine->GameConfig.MultiPlayerParams[i].waitingMode == 5;
    }
    func_75dc();
    if (D_12ae4 == LevelInterface.Engine->GameConfig.sNumPlayers)
        GOTO_START_MULTIPLAYER();
}

void func_db64(struct STRING_TABLE *st)
{
    LevelInterface.Engine->sprintf(st->message, " ");
    LevelInterface.Engine->FrontEndAddText(st, st->message, 0, 0, 16, 0);
    st->cursorDisabled = 1;
}

ASM_FUNC(func_7184)

/* "b" belongs to func_7184, still asm: its label (TODO: the literal once that is C) */
extern char S_12d8[];

/* the configuration list a player's controller type picks from: Configs[min..max-1] */
static void GetConfigRange(int player, signed char **cfg, short *min, short *max)
{
    switch (LevelInterface.Engine->GameWindows[player].Input.Method) {
    case 9:
    case 10:
        *cfg = (signed char *)&LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseConfigSelected;
        *min = 0;
        *max = 3;
        break;
    case 5:
    case 7:
        *cfg = (signed char *)&LevelInterface.Engine->GameConfig.MultiPlayerParams[player].analogConfigSelected;
        *min = 3;
        *max = 6;
        break;
    case 4:
    default:
        *cfg = (signed char *)&LevelInterface.Engine->GameConfig.MultiPlayerParams[player].standardConfigSelected;
        *min = 6;
        *max = 9;
        break;
    }
}

/* drop the trailing spaces of a player name */
static void TrimName(char *name)
{
    short i;

    for (i = 7; i > 0 && name[i] == ' '; i--)
        name[i] = 0;
}

/* why a player's pad was refused, and the port it is in */
static short GetPadViolation(short player, char **violation, char **portname)
{
    unsigned char port;
    short v;

    v = LevelInterface.Engine->SnapShotPadViolation(player, &port);
    if (v == 0)
        return 0;
    *violation = D_11a9c[v - 1];
    *portname = LevelInterface.Engine->ControlGetPortName(port);
    return v;
}

void func_75dc(void)
{
    char what[16];
    char buf[16];
    signed char *cfg;
    short min, max;
    char *violation;
    char *portname;
    short *sel;
    struct STRING_TABLE *st;
    short teamMode;
    short oneTeam;
    short str, first;
    short player, team, i, count;
    short colour;
    unsigned short pressed;
    char *name;
    short letter;

    teamMode = 0;
    str = 0;
    oneTeam = 0;
    switch (LevelInterface.Engine->GameConfig.sMultiMode) {
    case 0:
    case 3:
    case 5:
        LevelInterface.Engine->sprintf(what, "COLOR");
        teamMode = 0;
        break;
    case 1:
    case 2:
    case 4:
        LevelInterface.Engine->sprintf(what, "TEAM");
        teamMode = 1;
        break;
    }
    D_11aa8 = (D_11aa8 + *LevelInterface.Engine->NewVcounts / 10) & 15;
    if (teamMode) {
        count = 0;
        for (team = 0; team < 4; team++) {
            for (i = 0; i < LevelInterface.Engine->GameConfig.sNumPlayers; i++) {
                if (teamMode)
                    sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[i].teamSelected;
                else
                    sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[i].playerSelected;
                if (*sel == team) {
                    count++;
                    break;
                }
            }
        }
        oneTeam = count == 1;
    }
    for (player = 0; player < LevelInterface.Engine->GameConfig.sNumPlayers; player++) {
        first = str;
        pressed = LevelInterface.Engine->gFrontEndPad[player] & (LevelInterface.Engine->gFrontEndPad[player] ^ LevelInterface.Engine->gFrontEndOPad[player]);
        if (teamMode)
            sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[player].teamSelected;
        else
            sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[player].playerSelected;
        colour = *sel;
        switch (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].waitingMode) {
        case 0:
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "OPTIONS");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 60, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(buf, "%c%s", oneTeam ? 'g' : 'b', "START");
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], buf);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 86, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = oneTeam;
            if (oneTeam) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "THIS MODE REQUIRES 2 TEAMS");
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 100, 8, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos = 0;
            }
            LevelInterface.Engine->sprintf(buf, "%s", LevelInterface.Engine->GameConfig.MultiPlayerParams[player].playerName);
            TrimName(buf);
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "%s", buf);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 34, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            break;
        case 1:
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "NAME");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 34, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], what);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 60, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "CONTROLLER");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 86, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            break;
        case 2:
            letter = LevelInterface.Engine->GameConfig.MultiPlayerParams[player].currentLetter;
            name = LevelInterface.Engine->GameConfig.MultiPlayerParams[player].playerName;
#define LETTER(n) (letter == (n) ? (D_11aa8 & 8 ? "u" : S_12d8) : S_12d8)
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c",
                                           LETTER(0), name[0], LETTER(1), name[1], LETTER(2), name[2], LETTER(3), name[3],
                                           LETTER(4), name[4], LETTER(5), name[5], LETTER(6), name[6], LETTER(7), name[7]);
#undef LETTER
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 73, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "NAME");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 47, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "USE DIRECTIONAL BUTTONS");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 90, 8, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "TO CHANGE NAME");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 100, 8, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            break;
        case 3:
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], D_10d80[*sel]);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 73, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], what);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 47, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "LEFT AND RIGHT CHANGES %sS", what);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 100, 8, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            break;
        case 4:
            GetConfigRange(player, &cfg, &min, &max);
            if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str) {
                if (pressed & 0x80) {
                    LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                           &LevelInterface.Engine->Creatures->Draw.Location);
                    (*cfg)--;
                }
                if (pressed & 0x20) {
                    LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"itm_pkup"}),
                                                           &LevelInterface.Engine->Creatures->Draw.Location);
                    (*cfg)++;
                }
            }
            if (*cfg < min)
                *cfg = max - 1;
            if (*cfg >= max)
                *cfg = min;
            LevelInterface.Engine->GameConfig.MultiPlayerParams[player].controllerSelected = *cfg;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], ControllerConfigs[LevelInterface.Engine->GameConfig.MultiPlayerParams[player].controllerSelected][0]);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 26, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            if (!LevelInterface.Engine->IsPlayerPadShockable(player)) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "gVIBRATE");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 1;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos--;
            } else {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "VIBRATE");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 0;
            }
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 43, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str].pBoolVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[player].vibration;
            str++;
            if (LevelInterface.Engine->GameWindows[player].Input.Method == 9 || LevelInterface.Engine->GameWindows[player].Input.Method == 10 ||
                LevelInterface.Engine->GameWindows[player].Input.Method == 5 || LevelInterface.Engine->GameWindows[player].Input.Method == 7) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "SWAP Y");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 0;
            } else {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "gSWAP Y");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 1;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos--;
            }
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 60, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str].pBoolVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[player].invertY;
            str++;
            if (LevelInterface.Engine->gFreeMice > 0 || LevelInterface.Engine->GameWindows[player].Input.Method == 10 || LevelInterface.Engine->GameWindows[player].Input.Method == 9) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "MOUSE");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 0;
            } else {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "gMOUSE");
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 1;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos--;
            }
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 77, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str].pBoolVar = &LevelInterface.Engine->GameConfig.MultiPlayerParams[player].useMouse;
            str++;
            if (LevelInterface.Engine->GameWindows[player].Input.Method == 9 || LevelInterface.Engine->GameWindows[player].Input.Method == 10) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "MOUSE SPEED %d", LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity * 99 / 127);
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 0;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str) {
                    if (LevelInterface.Engine->gFrontEndPad[player] & 0x80) {
                        LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity--;
                        if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity < 0)
                            LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity = 0;
                    } else if (LevelInterface.Engine->gFrontEndPad[player] & 0x20) {
                        LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity++;
                        if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity >= 128)
                            LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity = 127;
                    }
                }
            } else {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "gMOUSE SPEED %d", LevelInterface.Engine->GameConfig.MultiPlayerParams[player].mouseSensitivity * 99 / 127);
                LevelInterface.Engine->MyStringTable[str].cursorDisabled = 1;
                if (LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos + player * 7 == str)
                    LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos--;
            }
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 94, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            str++;
            func_7184(player, first, str);
            if (pressed & 0x1000) {
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu3"}),
                                                       &Pickups[0]->Draw.Location);
                LevelInterface.Engine->GameConfig.MultiPlayerParams[player].waitingMode = 1;
                LevelInterface.Engine->GameConfig.MultiPlayerParams[player].cursorpos = 2;
            }
            break;
        case 5:
            /* (the two arms of the READY!/LOADING test are one block in the original, crossjumped) */
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 0, 0, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            if (D_12ae4 != *LevelInterface.Engine->MaxWindows) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "READY!");
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 73, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            } else {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "LOADING");
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 73, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            }
            LevelInterface.Engine->sprintf(buf, "%s", LevelInterface.Engine->GameConfig.MultiPlayerParams[player].playerName);
            TrimName(buf);
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "%s", buf);
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 47, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            break;
        case 6:
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 0, 0, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 0;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "PLEASE");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 25, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "RE-INSERT");
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 39, 16, player);
            STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
            LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            if (GetPadViolation(player, &violation, &portname)) {
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "%s", violation);
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 53, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "INTO");
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 67, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
                LevelInterface.Engine->sprintf(&LevelInterface.Engine->MyStringTable[str], "CONTROLLER");
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 128, 81, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
                LevelInterface.Engine->sprintf(buf, "PORT %s", portname);
                LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], buf, 128, 95, 16, player);
                STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->bannerColour = colour;
                LevelInterface.Engine->MyStringTable[str++].cursorDisabled = 1;
            }
            break;
        }
        for (; str < (player + 1) * 7; str++) {
            /* offset + table, in that order (&MyStringTable[str] adds them the other way round) */
            st = (struct STRING_TABLE *)(str * (int)sizeof(struct STRING_TABLE) + (int)LevelInterface.Engine->MyStringTable);
            LevelInterface.Engine->sprintf(st, " ");
            LevelInterface.Engine->FrontEndAddText(st, st, 0, 0, 16, 0);
            st->cursorDisabled = 1;
        }
        func_9c38(player);
    }
    if (LevelInterface.Engine->GameConfig.sNumPlayers == 2) {
        for (str = 7; str < 14; str++)
            LevelInterface.Engine->FrontEndAddText(&LevelInterface.Engine->MyStringTable[str], &LevelInterface.Engine->MyStringTable[str], 384,
                                                   LevelInterface.Engine->MyStringTable[str].y, LevelInterface.Engine->MyStringTable[str].fontSize,
                                                   STRFLAGS(&LevelInterface.Engine->MyStringTable[str])->window);
    }
}

void func_9c38(short w)
{
    short i;

    for (i = 0; i < 4; i++)
        Pickups[i + 1]->WinData[w].NoDraw = 1;
    if (LevelInterface.Engine->GameConfig.sMultiMode == 1)
        Pickups[LevelInterface.Engine->GameConfig.MultiPlayerParams[w].teamSelected + 1]->WinData[w].NoDraw = 0;
    else
        Pickups[LevelInterface.Engine->GameConfig.MultiPlayerParams[w].playerSelected + 1]->WinData[w].NoDraw = 0;
}

void FrontEndSpinPickup(struct CREATURE *Pickup)
{
    struct VECTOR Pos;
    short Unused[2];
    int Spin;
    int dx, dy, dz; /* never used: the code computing them is optimised away late, after it has
                     * split the function into blocks (the register allocation shows it) and made
                     * the callers that inline this function too big to be inlined themselves */

    Unused[0] = 2;
    Unused[1] = 3;
    LevelInterface.Engine->GetAbsoluteModelVert(Pickup, 1, &Pos);
    Pos.vz = 512;
    dx = LevelInterface.Engine->rand() % 64;
    dy = LevelInterface.Engine->rand() % 64;
    dz = Pos.vx % 4;
    Spin = *LevelInterface.Engine->NewVcounts << 2;
    Pickup->Ang.vy -= Spin;
    LevelInterface.Engine->RotMatrixYXZ(&Pickup->Ang, &Pickup->Matrix);
}

void PickupSpinTitle(struct CREATURE *Pickup)
{
    if (Pickup->ModelOpaque < 4096)
        Pickup->ModelOpaque += 256;
    else
        Pickup->ModelOpaque = 4096;
    FrontEndSpinPickup(Pickup);
}

void PickupSpinMenu(struct CREATURE *Pickup)
{
    if (Pickup->ModelOpaque > 1024)
        Pickup->ModelOpaque -= 256;
    else
        Pickup->ModelOpaque = 1024;
    FrontEndSpinPickup(Pickup);
}


/* memcard_data as the engine declared it (its char fields are signed) */
struct MEMCARD_DATA_S {
    long cmds;
    long result;
    long ret;
    long pendingStatus;
    struct DIRENTRY fileName[15];
    long cardInfo[8];
    char fileDescription[15][32];
    signed char numCards;
    signed char activeCardIndex;
    signed char numFiles;
    signed char numMpFiles;
    signed char numFreeFiles;
    signed char activeFileIndex;
    signed char testCardIndex;
    signed char testDescriptionIndex;
    char activeFileName[32];
};

#define MENU_SOUND() \
    LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"msc_menu1"}), \
                                           &Pickups[0]->Draw.Location)

void func_9f08(void)
{
    LevelInterface.Engine->MemoryCardUpdate();
    if (LevelInterface.Engine->gFrontEndPadShot || LevelInterface.Engine->MemoryCardGetStatus() == 13 ||
        LevelInterface.Engine->MemoryCardGetStatus() == 3 || LevelInterface.Engine->MemoryCardGetStatus() == 16) {
        switch (LevelInterface.Engine->MemoryCardGetStatus()) {
        case 1:
            break;
        case 9:
        case 12:
        case 17:
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                /* the original tests the masked word here, not the byte */
                if ((*(int *)LOADFLAGS(LevelInterface.Engine) & 0x7f000000) == 0x01000000) {
                    FrontEndInitScreen("SINGLE PLAYER", 8);
                    LevelInterface.Engine->FrontEndCreateMenu(FET_NEW_GAME, 16);
                    LevelInterface.Engine->Backfunc = func_cdd8;
                    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
                } else {
                    FrontEndInitScreen("MULTIPLAYER", 10);
                    LevelInterface.Engine->FrontEndCreateMenu(FET_DEATHMATCH, 16);
                    LevelInterface.Engine->Backfunc = func_cdd8;
                    LevelInterface.Engine->Specialfunc = CHECK_FOR_MULTI_MODE;
                    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
                }
            }
            break;
        case 16:
            if (LOADFLAGS(LevelInterface.Engine)->LoadType == 0 && LOADFLAGS(LevelInterface.Engine)->LoadSubType == 1) {
                LevelInterface.Engine->gFrontEndLevelComplete = 3;
                LevelInterface.Engine->gFrontEndTimeout = 12;
                LevelInterface.Engine->Specialfunc = LevelInterface.Engine->FrontEndLevelCompleteTimeout;
            } else if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                FrontEndInitScreen("MULTIPLAYER", 10);
                LevelInterface.Engine->FrontEndCreateMenu(FET_DEATHMATCH, 16);
                LevelInterface.Engine->Backfunc = func_cdd8;
                LevelInterface.Engine->Specialfunc = CHECK_FOR_MULTI_MODE;
                LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
            }
            break;
        case 3:
            LevelInterface.Engine->CHECK_FOR_MCFILEWINDOW();
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                LevelInterface.Engine->MemoryCardSetActiveCard(LevelInterface.Engine->cursorpos - LevelInterface.Engine->cursorlower);
                LevelInterface.Engine->MemoryCardSetStatus(4);
            }
            break;
        case 13: {
            char buf[32];
            char CheatNames[4][8] = { "NONE", "BRONZE", "SILVER", "GOLD" };
            char DifficultyNames[3][8] = { "EASY", "MEDIUM", "HARD" };
            short i;
            struct MEMCARD_DATA_S *md;
            struct STRING_TABLE *st;
            int cheats, continues, difficulty;
            int idx;

            for (i = LevelInterface.Engine->cursorlower; i < LevelInterface.Engine->cursorupper; i++)
                LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[i],
                                                          &LevelInterface.Engine->MyStringTable[i].originalMessage[3]);
            md = LevelInterface.Engine->memcard_data;
            i = LevelInterface.Engine->cursorpos - LevelInterface.Engine->cursorlower;
            LevelInterface.Engine->MemoryCardSetActiveFile(i);
            idx = md->activeFileIndex;
            LevelInterface.Engine->PDHCheats = (md->fileName[idx].name[13] - '0') & 3;
            LevelInterface.Engine->PDHContinues = (md->fileName[idx].name[14] - '0') & 3;
            LevelInterface.Engine->PDHDifficulty = (md->fileName[idx].name[15] - '0') & 3;
            continues = LevelInterface.Engine->PDHContinues;
            cheats = LevelInterface.Engine->PDHCheats;
            difficulty = LevelInterface.Engine->PDHDifficulty;
            if (i + 1 == LevelInterface.Engine->cursorupper - LevelInterface.Engine->cursorlower &&
                LevelInterface.Engine->MemoryCardGetNumFreeBlocks() && LevelInterface.Engine->gFrontEndLoadType == 1) {
                st = LevelInterface.Engine->MyStringTable;
                STRFLAGS(&st[4])->dontDraw = 1;
                STRFLAGS(&st[3])->dontDraw = 1;
                STRFLAGS(&st[2])->dontDraw = 1;
            } else if (LOADFLAGS(LevelInterface.Engine)->LoadType == 0 && LOADFLAGS(LevelInterface.Engine)->LoadSubType == 0) {
                STRFLAGS(&LevelInterface.Engine->MyStringTable[2])->dontDraw = 1;
                STRFLAGS(&LevelInterface.Engine->MyStringTable[4])->dontDraw = 1;
                if (cheats) {
                    LevelInterface.Engine->sprintf(buf, "CHEATS %s", CheatNames[cheats]);
                    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[3], buf);
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[3])->dontDraw = 0;
                } else
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[3])->dontDraw = 1;
            } else {
                if (LevelInterface.Engine->MyStringTable[i + LevelInterface.Engine->cursorlower].originalMessage[3] == 'M' &&
                    *(long *)&LevelInterface.Engine->MyStringTable[i + LevelInterface.Engine->cursorlower].originalMessage[4] == 0x49544c55) {
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[2])->dontDraw = 1;
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[3])->dontDraw = 1;
                } else {
                    LevelInterface.Engine->sprintf(buf, "RESUPPLIES %d", continues);
                    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[2], buf);
                    LevelInterface.Engine->sprintf(buf, "DIFFICULTY %s", DifficultyNames[difficulty]);
                    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[3], buf);
                }
                if (cheats) {
                    LevelInterface.Engine->sprintf(buf, "CHEATS %s", CheatNames[cheats]);
                    LevelInterface.Engine->FrontEndModifyText(&LevelInterface.Engine->MyStringTable[4], buf);
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[4])->dontDraw = 0;
                } else
                    STRFLAGS(&LevelInterface.Engine->MyStringTable[4])->dontDraw = 1;
            }
            LevelInterface.Engine->CHECK_FOR_MCFILEWINDOW();
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                LevelInterface.Engine->MemoryCardSetActiveFile(i);
                if (LevelInterface.Engine->gFrontEndLoadType == 0)
                    LevelInterface.Engine->MemoryCardSetStatus(21);
                if (LevelInterface.Engine->gFrontEndLoadType == 1) {
                    if (i + 1 == LevelInterface.Engine->cursorupper - LevelInterface.Engine->cursorlower &&
                        LevelInterface.Engine->MemoryCardGetNumFreeBlocks() && LevelInterface.Engine->gFrontEndLoadType == 1)
                        LevelInterface.Engine->MemoryCardSetStatus(22);
                    else
                        LevelInterface.Engine->MemoryCardSetStatus(19);
                }
            }
            break;
        }
        case 19:
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                if (LevelInterface.Engine->cursorpos == LevelInterface.Engine->cursorlower + 1)
                    LevelInterface.Engine->MemoryCardSetStatus(20);
                else if (LevelInterface.Engine->cursorpos == LevelInterface.Engine->cursorlower)
                    LevelInterface.Engine->MemoryCardSetStatus(13);
            }
            break;
        case 5:
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                if (LevelInterface.Engine->cursorpos == LevelInterface.Engine->cursorlower + 1)
                    LevelInterface.Engine->MemoryCardSetStatus(6);
                else if (LevelInterface.Engine->cursorpos == LevelInterface.Engine->cursorlower)
                    LevelInterface.Engine->MemoryCardSetStatus(1);
            }
            break;
        case 8:
            if (LevelInterface.Engine->gFrontEndPadShot & 0x4000) {
                MENU_SOUND();
                LevelInterface.Engine->MemoryCardSetStatus(1);
            }
            break;
        }
    }
}

/* the movie player: PSY-Q's strplay sample (as in level_qendmis1) */

#define ENGINE LevelInterface.Engine

/* libgpu's DR_MODE (not in types.h) */
struct DR_MODE {
    unsigned long tag;
    unsigned long code[2];
};

/* the player's variables, by their names in the sample */
#define ScreenX D_12a60
#define ScreenY D_12a62
#define VlcSize D_12a64[0]
#define Loc (*(struct CdlLOC *)D_12a68)
#define File (*(struct CdlFILE *)D_12a78)
#define FileSize D_12a90[0]
#define EndFrame D_12a94[0]
#define isEndOfFlag D_12a98[0]
#define IsRGB24Start D_12a9c[0]
#define dec (*(struct DECENV *)D_12aa8)
#define RingBuf (*(unsigned long **)D_12ad8)
#define VlcTable (*(unsigned short **)D_12adc)
#define YOffset D_11aac
#define MaxVlcSize D_11ab0
#define MovieWidth D_11ab4
#define MovieHeight D_11ab8[0]
#define Done D_11ac0

/* libpress, linked after this object */
unsigned short GetTPage(int tp, int abr, int x, int y);
void DecDCTReset(int mode);
void DecDCTin(unsigned long *buf, int mode);
void DecDCTout(unsigned long *buf, int size);
int DecDCToutCallback(void (*func)());
int DecDCTvlc2(unsigned long *bs, unsigned long *buf, unsigned short *table);
void DecDCTvlcBuild(unsigned short *table);
int DecDCTBufSize(unsigned long *bs);

int printf(char *fmt, ...)
{
}

void func_ae00(void)
{
    struct RECT snap_rect;  /* unused, but in the frame */
    int rest;

    if (*ENGINE->StCdIntrFlagAddr) {
        ENGINE->StCdInterrupt();
        *ENGINE->StCdIntrFlagAddr = 0;
    }
    ENGINE->LoadImage(&dec.slice, dec.imgbuf);
    rest = (short)(dec.rect[dec.rectid].w % 24);
    if (IsRGB24Start && rest) {
        IsRGB24Start = 0;
        dec.slice.x += rest * 3 / 2;
    } else
        dec.slice.x += 24;
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w)
        /* "* 3 << 7", not "* 384": gcc would reassociate the product */
        DecDCTout((unsigned long *)dec.imgbuf, ((dec.slice.w - 1) / 24 + 1) * (((dec.slice.h - 1) / 16 + 1) * 3 << 7) >> 1);
    else {
        dec.isdone = 1;
        dec.rectid = dec.rectid ? 0 : 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y + YOffset;
        IsRGB24Start = 1;
    }
}

unsigned long *func_b00c(void)
{
    unsigned long *addr;
    struct StHEADER *sector;
    int cnt;
    struct RECT rect;

    cnt = 0x800000;
    while (ENGINE->StGetNext(&addr, &sector)) {
        if (--cnt == 0)
            return 0;
    }
    if (addr[0] != ((unsigned long *)sector)[5] || addr[1] != ((unsigned long *)sector)[6]) {
        ENGINE->StFreeRing(addr);
        return 0;
    }
    if (((unsigned long *)sector)[2] >= EndFrame)
        isEndOfFlag = 1;
    if (MovieWidth != ((unsigned short *)sector)[8] || MovieHeight != ((unsigned short *)sector)[9]) {
        rect.w = 480;
        rect.x = 0;
        rect.y = 0;
        rect.h = 240;
        ENGINE->ClearImage(&rect, 0, 0, 0);
        MovieWidth = ((unsigned short *)sector)[8];
        MovieHeight = ((unsigned short *)sector)[9];
    }
    dec.rect[0].w = dec.rect[1].w = MovieWidth * 3 / 2;
    dec.slice.h = dec.rect[0].h = dec.rect[1].h = MovieHeight;
    YOffset = (240 - MovieHeight) / 2;
    return addr;
}

void func_b1c4(struct DISPENV *disp, struct DRAWENV *draw)
{
    int id;

    id = dec.rectid ? 0 : 1;
    ENGINE->SetDefDispEnv(disp, dec.rect[id].x, dec.rect[id].y, dec.rect[id].w, dec.rect[id].h);
    disp->screen.y = ScreenY + 8;
    disp->screen.x = ScreenX;
    ENGINE->SetDefDrawEnv(draw, dec.rect[id].x, dec.rect[id].y, dec.rect[id].w, dec.rect[id].h);
    disp->isrgb24 = 1;
    disp->disp.w = disp->disp.w * 2 / 3;
    ENGINE->PutDispEnv(disp);
    ENGINE->PutDrawEnv(draw);
    ENGINE->SetDispMask(1);
}

ASM_FUNC(PlayMovie)

ASM_FUNC(func_bdf4)








void GOTO_MISSIONCD_CHECKING(void)
{
    FrontEndInitScreen("MISSION CD", 22);
    LevelInterface.Engine->FrontEndCreateMenu(FET_CHECKING, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MISSIONCD;
    LevelInterface.Engine->ReReadToc();
}

void GOTO_MISSIONCD_SWAP(void)
{
    FrontEndInitScreen("MISSION CD", 24);
    LevelInterface.Engine->FrontEndCreateMenu(FET_IT_IS_NOW_SAFE, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MISSIONCD;
    LevelInterface.Engine->Backfunc = func_cdd8;
}

void GOTO_MISSIONCD_LOADING(void)
{
    FrontEndInitScreen("MISSION CD", 23);
    LevelInterface.Engine->FrontEndCreateMenu(FET_LOADING, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MISSIONCD;
}

void GOTO_MISSIONCD_INVALID(void)
{
    FrontEndInitScreen("MISSION CD", 25);
    LevelInterface.Engine->FrontEndCreateMenu(FET_THIS_IS_NOT_A, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_MISSIONCD;
    LevelInterface.Engine->Backfunc = func_cdd8;
}

void GOTO_OPTIONS(void)
{
    FrontEndInitScreen("OPTIONS", 2);
    LevelInterface.Engine->FrontEndCreateMenu(FET_PLAYER_OPTIONS, 16);
    LevelInterface.Engine->Backfunc = GOTO_TITLE;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
    *LevelInterface.Engine->targetcdvol = LevelInterface.Engine->GameConfig.sMusic * 2;
}

void func_cd54(void)
{
    FrontEndInitScreen("SINGLE PLAYER", 8);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NEW_GAME, 16);
    LevelInterface.Engine->Backfunc = func_cdd8;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}

void func_cdd8(void)
{
    char unused[32]; /* the original's frame has room for it */
    int i;

    FrontEndInitScreen("START", 1);
    LevelInterface.Engine->FrontEndCreateMenu(FET_SINGLE_MULTI, 16);
    LevelInterface.Engine->SetupScreen1by1(0);
    for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++)
        LevelInterface.Engine->AlterFocalSubDiv(i, 160, 4000);
    LevelInterface.Engine->Specialfunc = CHECK_FOR_START;
    LevelInterface.Engine->Backfunc = GOTO_TITLE;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}


void func_d010(void)
{
    FrontEndInitScreen("SCREEN POSITION", 7);
    LevelInterface.Engine->FrontEndCreateMenu(FET_USE_DIRECTIONAL_BUTTONS, 16);
    LevelInterface.Engine->FrontEndCreateMenu(FET_NULL, 16);
    LevelInterface.Engine->Backfunc = func_cc20;
    LevelInterface.Engine->Specialfunc = CHECK_FOR_SCREENPOSITION;
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}






void func_d414(void)
{
    FrontEndInitScreen("LOAD", 16);
    LevelInterface.Engine->Specialfunc = func_9f08;
    LevelInterface.Engine->gFrontEndLoadType = 0;
    LOADFLAGS(LevelInterface.Engine)->LoadSubType = 1;
    LevelInterface.Engine->Backfunc = func_cd54;
    LevelInterface.Engine->MemoryCardSetStatus(1);
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}



void func_d60c(void)
{
    PlayMovie("ROGUEIN1.STX", 2949, 0x10000, 1, 255);
}






void func_d9b8(char *out)
{
    unsigned char *s;
    unsigned char c;
    short i;

    s = (unsigned char *)((struct LEVELDATA *)LevelInterface.Engine->GetLevelDataFromIdentityName("COLD STORAGE"))[LevelInterface.Engine->GameConfig.bArena].Name.Name;
    i = 0;
    for (c = *s; c && i < 12; c = *++s) {
        if (c == ' ' || (c >= '0' && c <= '9'))
            out[i++] = c;
        else
            out[i++] = c & 0xdf;
    }
    out[i] = 0;
}









void ResetCallback(void)
{
    LevelInterface.Engine->ResetCallback();
}

void FrontEndNothing(void)
{
}

void FrontEndResetWindows(void)
{
    struct WINDOW *Win;
    short i;

    for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++) {
        Win = &LevelInterface.Engine->GameWindows[i];
        Win->CamPos.vx = 0;
        Win->CamPos.vy = 0;
        Win->CamPos.vz = 0;
        Win->CamAng.vx = 0;
        Win->CamAng.vy = 0;
        Win->CamAng.vz = 0;
        Win->ControlBits |= 7;
        LevelInterface.Engine->QGenWindowSortList(Win);
        LevelInterface.Engine->UpdateWindowStuff(Win);
    }
}

void FrontEndDrawSortedObjs(void)
{
    LevelInterface.Engine->DrawAllSortedObjs();
}





void func_c6c0(void)
{
    if (LevelInterface.Engine->gFrontEndPad[0]) {
        D_12dd8[0] = 9000;
        return;
    }
    if ((D_12dd8[0] -= *LevelInterface.Engine->NewVcounts) <= 0)
        GOTO_DEMO();
}
void func_c7bc(void)
{
    LevelInterface.Engine->GameConfig.sBlastScale = 64;
    LevelInterface.Engine->GameConfig.sGravity = 64;
    LevelInterface.Engine->GameConfig.sGameSpeed = 64;
    LevelInterface.Engine->GameConfig.sTimeLimit = 10;
    LevelInterface.Engine->GameConfig.sFragLimit = 10;
    LevelInterface.Engine->GameConfig.bWeaponStay = 0;
    LevelInterface.Engine->GameConfig.bInfiniteAmmo = 0;
    LevelInterface.Engine->GameConfig.bAllWeapons = 0;
    LevelInterface.Engine->GameConfig.bFallingDamage = 1;
    LevelInterface.Engine->GameConfig.bOneShotKill = 0;
}
void GOTO_REINSERT_WAIT(void)
{
    unsigned char port;
    void (*f)();

    if (LevelInterface.Engine->SnapShotPadViolation(0, &port) != 1) {
        f = D_116e4[LevelInterface.Engine->gFrontEndPrevScreen];
        if (f)
            f();
        else
            GOTO_TITLE();
    }
}

void GOTO_TITLE(void)
{
    short pos;

    FrontEndInitScreen(0, 0);
    LevelInterface.Engine->FrontEndCreateMenu(FET_START, 32);
    pos = LevelInterface.Engine->FrontEndRestoreCursor();
    D_12dd8[0] = 9000;
    LevelInterface.Engine->cursorpos = pos;
    LevelInterface.Engine->Specialfunc = func_c6c0;
}
void func_d210(void)
{
    FrontEndInitScreen(0, 17);
    LOADFLAGS(LevelInterface.Engine)->LoadIsSpecial = 0;
    LevelInterface.Engine->Specialfunc = func_9f08;
    LevelInterface.Engine->Backfunc = GOTO_MULTIPLAYER;
    LevelInterface.Engine->gFrontEndLoadType = 1;
    LOADFLAGS(LevelInterface.Engine)->LoadSubType = 0;
    LevelInterface.Engine->MemoryCardSetStatus(1);
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}
void func_d2d8(void)
{
    LOADFLAGS(LevelInterface.Engine)->LoadIsSpecial = 1;
    LevelInterface.Engine->Specialfunc = func_9f08;
    LevelInterface.Engine->Backfunc = GOTO_UNLOCK_SAVE;
    LevelInterface.Engine->gFrontEndLoadType = 1;
    LOADFLAGS(LevelInterface.Engine)->LoadSubType = 0;
    LevelInterface.Engine->MemoryCardSetStatus(1);
    LevelInterface.Engine->cursorpos = LevelInterface.Engine->FrontEndRestoreCursor();
}
void func_d394(void)
{
    LevelInterface.Engine->GameConfig.sDifficulty = 0;
    GOTO_INTRO();
}
void func_d3bc(void)
{
    LevelInterface.Engine->GameConfig.sDifficulty = 1;
    GOTO_INTRO();
}
void func_d3e8(void)
{
    LevelInterface.Engine->GameConfig.sDifficulty = 2;
    GOTO_INTRO();
}
void func_d58c(void)
{
    int i;

    for (i = 0; i < 5; i++) {
        WINFLAGS(Pickups[i], 0)->NoDraw = 1;
        WINFLAGS(Pickups[i], 1)->NoDraw = 1;
        WINFLAGS(Pickups[i], 2)->NoDraw = 1;
        WINFLAGS(Pickups[i], 3)->NoDraw = 1;
    }
}
void func_d644(unsigned short *clut, short slot, int is256)
{
    struct RECT rect;

    rect.x = (LevelInterface.Engine->palette[slot] & 0x3f) << 4;
    rect.y = LevelInterface.Engine->palette[slot] >> 6;
    rect.h = 1;
    rect.w = is256 ? 256 : 16;
    LevelInterface.Engine->LoadImage(&rect, clut);
    LevelInterface.Engine->DrawSync(0);
}
void func_d6fc(short *clut, int n)
{
    while (n) {
        *clut &= 0x7fff;
        if (*clut)
            *clut |= 0x8000;
        n--;
        clut++;
    }
}
void func_d734(void)
{
    struct WINDOW *w;
    short i;

    for (i = 0; i < *LevelInterface.Engine->MaxWindows; i++) {
        w = &LevelInterface.Engine->GameWindows[i];
        w->CamPos.vx = 0;
        w->CamPos.vy = 0;
        w->CamPos.vz = 0;
        w->CamAng.vx = 0;
        w->CamAng.vy = 0;
        w->CamAng.vz = 0;
        w->ControlBits |= 7;
        LevelInterface.Engine->QGenWindowSortList(w);
        LevelInterface.Engine->UpdateWindowStuff(w);
    }
}
void func_d8c0(int pad, char **sel, short *first, short *last)
{
    switch (LevelInterface.Engine->GameWindows[pad].Input.Method) {
    case 9:
    case 10:
        *sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[pad].mouseConfigSelected;
        *first = 0;
        *last = 3;
        break;
    case 5:
    case 7:
        *sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[pad].analogConfigSelected;
        *first = 3;
        *last = 6;
        break;
    case 4:
    default:
        *sel = &LevelInterface.Engine->GameConfig.MultiPlayerParams[pad].standardConfigSelected;
        *first = 6;
        *last = 9;
        break;
    }
}
short func_dab0(short pad, char **type, char **port)
{
    unsigned char p;
    short v;

    v = LevelInterface.Engine->SnapShotPadViolation(pad, &p);
    if (v == 0)
        return 0;
    *type = D_11a9c[v - 1];
    *port = LevelInterface.Engine->ControlGetPortName(p);
    return v;
}
void func_dbe0(char *s)
{
    short i;

    for (i = 7; i > 0; i--) {
        if (s[i] != ' ')
            break;
        s[i] = 0;
    }
}
int DMACallback(int dma, void (*func)())
{
    return LevelInterface.Engine->DMACallback(dma, func);
}
void func_dc4c(void)
{
    while (!LevelInterface.Engine->CdControl(0x15, StrLoc, 0))
        ;
    while (!LevelInterface.Engine->CdRead2(0x1c0))
        ;
}
void func_dcc8(unsigned long *ring, void (*callback)(), int endframe)
{
    DecDCTReset(0);
    StrEndFlag = 0;
    DecDCToutCallback(callback);
    LevelInterface.Engine->StSetRing(StrRingBuf, 64);
    LevelInterface.Engine->StSetStream(1, 1, ring, 0, endframe);
    func_dc4c();
}
void func_ddbc(int x0, int y0, int x1, int y1)
{
    setRECT(&StrDec.rect[0], x0, y0, 480, 240);
    setRECT(&StrDec.rect[1], x1, y1, 480, 240);
    LevelInterface.Engine->ClearImage(&StrDec.rect[0], 0, 0, 0);
    LevelInterface.Engine->ClearImage(&StrDec.rect[1], 0, 0, 0);
    LevelInterface.Engine->DrawSync(0);
    setRECT(&StrDec.slice, x0, y0, 24, 240);
    StrDec.vlcid = 0;
    StrDec.rectid = 0;
    StrDec.isdone = 0;
}
int func_dec4(void)
{
    int cnt;
    unsigned long *next;

    cnt = 0x800000;
    while ((next = func_b00c()) == 0) {
        if (--cnt == 0)
            return -1;
    }
    if (D_11ab0 < DecDCTBufSize(next))
        D_11ab0 = DecDCTBufSize(next);
    StrDec.vlcid = StrDec.vlcid ^ 1;
    DecDCTvlc2(next, StrDec.vlcbuf[StrDec.vlcid], StrVlcTable);
    LevelInterface.Engine->StFreeRing(next);
    return 0;
}
void func_df90(void)
{
    volatile int cnt;

    cnt = 0x800000;
    while (!StrDec.isdone) {
        if (--cnt == 0) {
            StrDec.isdone = 1;
            StrDec.rectid = StrDec.rectid ? 0 : 1;
            StrDec.slice.x = StrDec.rect[StrDec.rectid].x;
            StrDec.slice.y = StrDec.rect[StrDec.rectid].y + D_11aac;
        }
    }
    StrDec.isdone = 0;
}

/* the word the original link left after the libraries' .data (in .sdata, which follows it) */
__asm__(".section .sdata\n .word 0x0d7b0a0d\n .text");
