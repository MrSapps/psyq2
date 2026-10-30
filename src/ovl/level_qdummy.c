/* An older build of the intermission, FMV, logo and multiplayer results module (see
 * level_qendmis1.c), against an older engine level interface: the interface is
 * { InitLevel, Engine } and the engine's QENGINELEVINTERFACE has another layout
 * (level_qdummy_engine.h). The Intro FMV plays three movies, QLogos fades out on fire and the
 * results screen lists the players without team scores. */
/* ovl-libs: libc2/MEMSET libgpu/P00 libgpu/P33 libpress/LIBPRESS libpress/VLC_C libpress/BUILD libpress/DCT002 */
#include "ovl.h"
#include "bss.h"

/* the older types (only what this module touches is laid out) */
struct DB_OLD {  /* size 0x7e24 */
    char Unknown0[0x2a50];
    struct DISPENV DispEnv;  /* 0x2a50 */
    struct DR_MOVE WaterMove;  /* 0x2a64 */
    unsigned long Ot[4][51];  /* 0x2a7c */
    unsigned long BigOt[11];  /* 0x2dac */
    char Unknown2dd8[0x7e24 - 0x2dd8];
};
struct WINDOW_OLD {
    char Unknown0[0x298];
    struct CTRLER Input;  /* 0x298 */
};
#include "level_qdummy_engine.h"

struct QLEVELINTERFACE_OLD {
    void (*InitLevFn)();
    struct QENGINELEVINTERFACE_OLD *Engine;
};

void InitLevel(void);
struct QLEVELINTERFACE_OLD LevelInterface OVL_HEADER = { InitLevel, 0 };

#define ENGINE LevelInterface.Engine

/* the shared code's differences for this build */
#define QEND_OLD_PLAYER
#define QEND_QDUMMY

#include "level_qend_common.h"

int ResetCallback(void);
void DummyUpdate(void);
void IntroUpdate(void);
void ExtroUpdate(void);
void MagazineIntrUpdate(void);
void MagazineExtrUpdate(void);
void SetClutSemiTrans(short *clut, int n);
void LoadClut(unsigned short *clut, short slot, int is256);
int DMACallback(int dma, void (*func)());
void strKickCD(void);
void strInit(unsigned long *ring, void (*callback)(), int endframe);
void strSetDefDecEnv(int x0, int y0, int x1, int y1);
int strNextVlc(void);
void strSync(void);

static void EndMissionUpdate(void);
static void LogosUpdate(void);
static void MPResultsUpdate(void);
static void InitScoreScreen(void);
static short PlayMovie(char *name, int endframe, int vlcsize, int clear, int volume);

/* .bss, in the original's order */
/* the word after LIBPRESS's .data: the original image holds 4 junk bytes there (the start of
 * this object's .bss, left over from the build machine's memory); a .sdata word lands there */
static int BssJunk __attribute__((section(".sdata"))) = 0x656b6166;   /* "fake" */
static int UnusedBss0[2] BSS;
static int LogosTime BSS;
static int UnusedBss1 BSS;
static int LogosFade[3] BSS;
static int UnusedBss2 BSS;
static int PictureTime BSS;
static int VlcSize BSS;
static struct CdlLOC Loc BSS;
static int UnusedBss3 BSS;
static struct CdlFILE File BSS;
static int FileSize BSS;
static int EndFrame BSS;
static int isEndOfFlag BSS;
static int IsRGB24Start BSS;
static int UnusedBss4[2] BSS;
static struct DECENV dec BSS;
static unsigned long *RingBuf BSS;
static unsigned short *VlcTable BSS;
static short ScreenX BSS;
static short ScreenY __attribute__((section(".bss")));
static int UnusedBss5 BSS;
static struct DR_TPAGE TPages[2][5] BSS;
static struct DR_TPAGE *TPageEnd BSS;
static int UnusedBss6 BSS;
static struct SPRT Sprites[2][5] BSS;
static struct DR_TPAGE *TPagePtr BSS;
static int UnusedBss7 BSS;
static struct SPRT *SprtEnd BSS;
static struct SPRT *SprtPtr BSS;

/* .data defined before InitLevel's users */
extern unsigned short Cluts[7][16];
extern unsigned short Clut256[5][256];
static int YOffset;
static int MaxVlcSize;
static int MovieWidth;
static int MovieHeight;
static int Done;

void InitLevel(void)
{
    ENGINE->PrintF("\n\n\n\n\n");
    ENGINE->PrintF("Initialise %s\n", ENGINE->PrintIdentityName(*ENGINE->ThisLevelName));
    ENGINE->PrintF("\n\n\n\n\n");
    ENGINE->CantPause = 1;
    if (ENGINE->SizeOf != sizeof(struct QENGINELEVINTERFACE_OLD)) {
        for (;;)
            ENGINE->PrintF("Please get a programmer to recompile level %s\n",
                ENGINE->PrintIdentityName(*ENGINE->ThisLevelName));
        ENGINE->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINELEVINTERFACE_OLD), ENGINE->SizeOf);
    }
    ENGINE->PlayThisXA(2);
    if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 1"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[2], 0, 0);
        LoadClut(Cluts[1], 1, 0);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 2"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[3], 0, 0);
        LoadClut(Cluts[1], 1, 0);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 3"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[4], 0, 0);
        LoadClut(Cluts[1], 1, 0);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 4"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[5], 0, 0);
        LoadClut(Cluts[1], 1, 0);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 5"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[6], 0, 0);
        LoadClut(Cluts[1], 1, 0);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Intro FMV"}) == 1) {
        ENGINE->PrintF("Do Intro\n");
        PlayMovie("TAKE1B.STX", 1539, 0x10000, 1, 255);
        PlayMovie("ROGUEIN1.STX", 2950, 0x10000, 1, 255);
        PlayMovie("OUTRO1.STX", 1812, 0x10000, 1, 255);
        *ENGINE->LevelSpecific = IntroUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Extro FMV"}) == 1) {
        ENGINE->PrintF("Do Extro\n");
        PlayMovie("OUTRO1.STX", 1812, 0x10000, 1, 255);
        *ENGINE->LevelSpecific = ExtroUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"QLogos"}) == 1) {
        *ENGINE->LevelSpecific = LogosUpdate;
        ENGINE->palette[0] = 0x3fc0;
        ENGINE->palette[1] = 0x3fd0;
        ENGINE->palette[2] = 0x3fe0;
        SetClutSemiTrans((short *)Clut256[0], 768);
        LoadClut(Clut256[0], 0, 1);
        LoadClut(Clut256[1], 1, 1);
        LoadClut(Clut256[2], 2, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Dummy"}) == 1) {
        *ENGINE->LevelSpecific = DummyUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MagazineIntr"}) == 1) {
        *ENGINE->LevelSpecific = MagazineIntrUpdate;
        ENGINE->palette[0] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[3], 256);
        LoadClut(Clut256[3], 0, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MagazineExtr"}) == 1) {
        *ENGINE->LevelSpecific = MagazineExtrUpdate;
        ENGINE->palette[0] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[4], 256);
        LoadClut(Clut256[4], 0, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MPResults"}) == 1) {
        *ENGINE->LevelSpecific = MPResultsUpdate;
        ENGINE->palette[0] = 0x3fc0;
        ENGINE->palette[1] = 0x3fd0;
        ENGINE->palette[2] = 0x3fe0;
        SetClutSemiTrans((short *)Clut256[0], 768);
        LoadClut(Clut256[0], 0, 1);
        LoadClut(Clut256[1], 1, 1);
        LoadClut(Clut256[2], 2, 1);
        InitScoreScreen();
    }
    ENGINE->PrintF("Done Initialise\n");
}

struct FET_MESSAGE ScoreMenu[] = {
    { "PLAYER 1", 256, 111, 0, 0, 0, 0 },
    { "PLAYER 2", 256, 137, 0, 0, 0, 0 },
    { "PLAYER 3", 256, 163, 0, 0, 0, 0 },
    { "PLAYER 4", 256, 189, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};
static int ScrollY = 0;
static int UnusedData0 = 0;
static int LogosFrame = 0;
static int LogosState = 0;
static int UnusedData1 = 0;
static int PictureFrame = 0;
#include "level_qdummy_cluts.h"
static int UnusedData2 = 0;
static int YOffset = 0;
static int MaxVlcSize = 0;
static int MovieWidth = 0;
static int MovieHeight = 0;
static int UnusedData3 = 0;
static int Done = 0;

static void InitScoreScreen(void)
{
    ENGINE->cursorpos = ENGINE->FrontEndRestoreCursor();
    ENGINE->InGameInitScreen(41);
    switch (ENGINE->GameConfig.sMultiMode) {
    case 0:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "DM SCORES", 256, 32, 32, 0);
        break;
    case 1:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "TEAM DM SCORES", 256, 32, 32, 0);
        break;
    case 2:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "CTF SCORES", 256, 32, 32, 0);
        break;
    case 3:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "TAG SCORES", 256, 32, 32, 0);
        break;
    case 4:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "TEAM TAG SCORES", 256, 32, 32, 0);
        break;
    case 5:
        ENGINE->FrontEndAddText(ENGINE->MyStringTable, "VERSUS SCORES", 256, 32, 32, 0);
        break;
    }
    ENGINE->MyStringTable->cursorItem = 1;
    ENGINE->cursorupper = 1;
    ENGINE->FrontEndCreateMenu(ScoreMenu, 16);
}

static void DrawSprite(struct DVECTOR xy, struct DVECTOR wh, short clut, short tpage, struct COLOUR col, int abr);
static void DrawPicture(short y, short clut, short tpage, int is24, struct COLOUR col, int semitrans);

static void MPResultsUpdate(void)
{
    short clut = 1;     /* as in level_qendmis1 */

    ENGINE->DoFrontEndStuff();
    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    DrawPicture(0, clut, 4, 1, (struct COLOUR){64, 64, 64}, 1);
}

static void EndMissionUpdate(void)
{
    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    DrawSprite((struct DVECTOR){40, 20}, (struct DVECTOR){192, 128}, 1, 4, (struct COLOUR){128, 128, 128}, *ENGINE->ChrisCount & 0xf);
    DrawPicture(ScrollY, 0, 0, 0, (struct COLOUR){128, 128, 128}, 0);
    if (++ScrollY >= 240) {
        ENGINE->PrintF("Mission completed %d\n", *ENGINE->Mission);
        if (*ENGINE->Mission == 5)
            *ENGINE->LevelComplete = 13;
        else {
            *ENGINE->LevelComplete = 4;
            *ENGINE->NewMission = 1;
            *ENGINE->NextMission = *ENGINE->Mission + 1;
        }
    }
}

static void LogosUpdate(void)
{
    int t;
    int u;
    int v;
    int i;
    int *fade;

    if (LogosFrame > 150 && (ENGINE->GameWindows->Input.Original & 8))
        LogosState = 1;
    if (LogosState) {
        if (LogosState == 1) {
            i = 0;
            if (!(LogosFade[0] | LogosFade[1] | LogosFade[2])) {
                LogosState = 2;
                return;
            }
            fade = LogosFade;
            do {
                if (*fade) {
                    *fade -= 16;
                    if (*fade <= 0)
                        *fade = 0;
                    else
                        DrawPicture(0, i, i * 4, 1, (struct COLOUR){*fade, *fade, *fade}, 1);
                }
                i++;
                fade++;
            } while (i < 3);
        } else if (LogosState == 5)
            *ENGINE->LevelComplete = 6;
        else
            LogosState++;
        return;
    }
    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    LogosFade[0] = LogosFade[1] = LogosFade[2] = 0;
    t = ++LogosFrame;
    if (t >= 0) {
        LogosTime = t;
        if (t < 8)
            LogosFade[0] = t * 16;
        else if (t < 98)
            LogosFade[0] = 128;
        else if (t < 106)
            LogosFade[0] = 128 - (t - 98) * 16;
        if (LogosFade[0]) {
            DrawPicture(0, 0, 0, 1, (struct COLOUR){LogosFade[0], LogosFade[0], LogosFade[0]}, 1);
        }
        if (LogosTime >= 98) {
            u = LogosTime -= 98;
            if (u < 8)
                LogosFade[1] = u * 16;
            else if (u < 98)
                LogosFade[1] = 128;
            else if (u < 106)
                LogosFade[1] = 128 - (u - 98) * 16;
            if (LogosFade[1]) {
                DrawPicture(0, 1, 4, 1, (struct COLOUR){LogosFade[1], LogosFade[1], LogosFade[1]}, 1);
            }
            if (LogosTime >= 98) {
                v = LogosTime -= 98;
                if (v < 8)
                    LogosFade[2] = v * 16;
                else if (v < 98)
                    LogosFade[2] = 128;
                else if (v < 106)
                    LogosFade[2] = 128 - (v - 98) * 16;
                if (LogosFade[2]) {
                    DrawPicture(0, 2, 8, 1, (struct COLOUR){LogosFade[2], LogosFade[2], LogosFade[2]}, 1);
                }
                if (LogosTime >= 108)
                    *ENGINE->LevelComplete = 12;
            }
        }
    }
}

#include "level_qend_player.h"

/* the byte the original link left between this object's .rdata and LIBPRESS's */
__asm__(".rdata\n .byte 0x82\n .text");
