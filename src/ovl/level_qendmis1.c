/* The intermission, FMV, logo and multiplayer results module: EndMission 1-5, Intro FMV,
 * Extro FMV, QLogos, QLogos2, Dummy, MagazineIntr, MagazineExtr and MPResults all run it.
 * The movie player is PSY-Q's streaming sample (strplay) driven through the engine's
 * libcd/libgpu pointers; libpress and three small libc/libgpu functions were linked into
 * the module after this object.
 *
 * TODO, not matching yet (same size or within a few instructions; the rest of the module,
 * .rdata, .data and .bss match): SetupScoreLines and FormatScoreLine (FormatScoreLine's name
 * loop: the original keeps the jump to the loop test instead of copying the test, and
 * strength-reduces the fill loop), Logos2Update, LogosUpdate and PictureUpdate (the original
 * loads the frame counter after the SprtPtr/SprtEnd stores; here sched1 hoists it), DrawSprite
 * and DrawPicture (the original reloads SprtPtr after more of the stores), strCallback
 * (scheduling of IsRGB24Start), PlayMovie (ret lives in the frame as a short) and MovieFreeze
 * (the original addresses src/dst through registers). */
/* ovl-libs: libc2/MEMSET libgpu/P00 libgpu/P33 libpress/LIBPRESS libpress/VLC_C libpress/BUILD libpress/DCT002 */
#include "ovl.h"
#include "bss.h"

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

#define ENGINE LevelInterface.Engine

/* libgpu's DR_MODE (not in types.h) */
struct DR_MODE {
    unsigned long tag;
    unsigned long code[2];
};

/* libgpu.h */
#define setRECT(r, _x, _y, _w, _h) ((r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h))

/* the strplay sample's decode environment */
struct DECENV {
    unsigned long *vlcbuf[2];   /* 0x0 */
    unsigned short *imgbuf;     /* 0x8 */
    int vlcid;                  /* 0xc */
    struct RECT rect[2];        /* 0x10 */
    int rectid;                 /* 0x20 */
    struct RECT slice;          /* 0x24 */
    int isdone;                 /* 0x2c */
};

/* STRING_TABLE with its flags as the int bitfields this module was built with */
struct SCORELINE {
    char message[32];
    char originalMessage[32];
    short x;
    short y;
    unsigned short UnUsed : 6;
    unsigned short bannerColour : 3;
    unsigned short window : 4;
    unsigned short onRelease : 1;
    unsigned short dontWrap : 1;
    unsigned short dontDraw : 1;
    short fontSize;
    short cursorItem;
    short cursorDisabled;
    void *selectFunc;
    short *pSliderVar;
    short sliderX;
    short *pBoolVar;
};

/* library functions linked into the module */
void *memset(void *s, int c, int n);
unsigned short GetTPage(int tp, int abr, int x, int y);
void SetDrawTPage(struct DR_TPAGE *p, int dfe, int dtd, int tpage);
void DecDCTReset(int mode);
void DecDCTin(unsigned long *buf, int mode);
void DecDCTout(unsigned long *buf, int size);
int DecDCToutCallback(void (*func)());
int DecDCTvlc2(unsigned long *bs, unsigned long *buf, unsigned short *table);
void DecDCTvlcBuild(unsigned short *table);
int DecDCTBufSize(unsigned long *bs);

int ResetCallback(void);
void DummyUpdate(void);
void IntroUpdate(void);
void ExtroUpdate(void);
void MagazineIntrUpdate(void);
void MagazineExtrUpdate(void);
void SetClutSemiTrans(short *clut, int n);
void LoadClut(unsigned short *clut, short slot, int is256);
void StaticCamera(void);
void FormatScoreLine(struct SCORELINE *line, char *name, short score);
int DMACallback(int dma, void (*func)());
void strKickCD(void);
void strInit(unsigned long *ring, void (*callback)(), int endframe);
void strSetDefDecEnv(int x0, int y0, int x1, int y1);
int strNextVlc(void);
void strSync(void);

static void EndMissionUpdate(void);
static void Logos2Update(void);
static void LogosUpdate(void);
static void MPResultsUpdate(void);
static void InitScoreScreen(void);
static short PlayMovie(char *name, int endframe, int vlcsize, int clear, int volume);

/* .bss, in the original's order */
static int UnusedBss0[3] BSS;
static int Logos2Time BSS;
static int UnusedBss1 BSS;
static int Logos2Fade[3] BSS;
static int UnusedBss2[2] BSS;
static int LogosTime BSS;
static int UnusedBss3[2] BSS;
static int LogosFade[3] BSS;
static int UnusedBss4[2] BSS;
static int PictureTime BSS;
static int VlcSize BSS;
static struct CdlLOC Loc BSS;
static struct CdlFILE File BSS;
static int FileSize BSS;
static int EndFrame BSS;
static int isEndOfFlag BSS;
static int IsRGB24Start BSS;
static int UnusedBss5[2] BSS;
static struct DECENV dec BSS;
static unsigned long *RingBuf BSS;
static unsigned short *VlcTable BSS;
static short ScreenX BSS;
static short ScreenY __attribute__((section(".bss")));
static int UnusedBss6 BSS;
static struct DR_TPAGE TPages[2][5] BSS;
static struct DR_TPAGE *TPageEnd BSS;
static int Wait BSS;
static struct SCORELINE *PlayerLines[4] BSS;
static int UnusedBss7[2] BSS;
static struct SPRT Sprites[2][5] BSS;
static struct DR_TPAGE *TPagePtr BSS;
static int UnusedBss8 BSS;
static int State BSS;
static struct SPRT *SprtEnd BSS;
static struct SPRT *SprtPtr BSS;

/* .data defined before InitLevel's users */
extern unsigned short Cluts[7][16];
extern unsigned short Clut256[7][256];
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
    if (ENGINE->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            ENGINE->PrintF("Please get a programmer to recompile level %s\n",
                ENGINE->PrintIdentityName(*ENGINE->ThisLevelName));
        ENGINE->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINELEVINTERFACE), ENGINE->SizeOf);
    }
    ENGINE->PlayThisXA(2);
    if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 1"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[2], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 2"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[3], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 3"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[4], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 4"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[5], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 5"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[6], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Intro FMV"}) == 1) {
        ENGINE->PrintF("Do Intro\n");
        PlayMovie("TAKE1B.STX", 1538, 0x10000, 1, 255);
        *ENGINE->LevelSpecific = IntroUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Extro FMV"}) == 1) {
        ENGINE->PrintF("Do Extro\n");
        PlayMovie("OUTRO1.STX", 1811, 0x10000, 1, 255);
        *ENGINE->LevelSpecific = ExtroUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"QLogos2"}) == 1) {
        *ENGINE->LevelSpecific = Logos2Update;
        ENGINE->palette[0] = 0x3fc0;
        ENGINE->palette[1] = 0x3fd0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
        SetClutSemiTrans((short *)Clut256[0], 256);
        SetClutSemiTrans((short *)Clut256[6], 256);
        LoadClut(Clut256[6], 0, 1);
        LoadClut(Clut256[0], 1, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"QLogos"}) == 1) {
        *ENGINE->LevelSpecific = LogosUpdate;
        ENGINE->palette[0] = 0x3fc0;
        ENGINE->palette[1] = 0x3fd0;
        ENGINE->palette[2] = 0x3fe0;
        ENGINE->palette[3] = 0x3ff0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
        SetClutSemiTrans((short *)Clut256[0], 1024);
        LoadClut(Clut256[0], 0, 1);
        LoadClut(Clut256[1], 1, 1);
        LoadClut(Clut256[2], 2, 1);
        LoadClut(Clut256[7], 3, 1);
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
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MPResults"}) == 1) {
        *ENGINE->LevelSpecific = MPResultsUpdate;
        ENGINE->palette[1] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[5], 256);
        LoadClut(Clut256[5], 1, 1);
        InitScoreScreen();
    }
    ENGINE->PrintF("Done Initialise\n");
}

/* bubble sort of the scores, carrying the player ids along */
static void SortScores(short *scores, short *ids, short n)
{
    int sorted;
    int i;

    do {
        sorted = 1;
        for (i = 0; i < n - 1; i++) {
            if (scores[i] < scores[i + 1]) {
                scores[i] ^= scores[i + 1];
                scores[i + 1] ^= scores[i];
                scores[i] ^= scores[i + 1];
                ids[i] ^= ids[i + 1];
                ids[i + 1] ^= ids[i];
                ids[i] ^= ids[i + 1];
                sorted = 0;
            }
        }
    } while (!sorted);
}

struct FET_MESSAGE ScoreMenu[] = {
    { "", 256, 0, 0, 0, 0, 0 },
    { "", 256, 0, 0, 0, 0, 0 },
    { "", 256, 0, 0, 0, 0, 0 },
    { "", 256, 0, 0, 0, 0, 0 },
    { "", 256, 0, 0, 0, 0, 0 },
    { "", 256, 0, 0, 0, 0, 0 },
    { "ALL PLAYERS PRESS", 256, 180, 0, 0, 0, 0 },
    { "FIRE TO CONTINUE", 256, 200, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
};
struct FET_MESSAGE NullMenu[] = { { 0, 0, 0, 0, 0, 0, 0 } };
static unsigned char PlayerReady[4] = { 0 };
static int ReadyTimeout = 150;
static int UnusedData0 = 0;
static int ScrollY = 0;
static int Fade = 0;
static int FadeOut = 0;
static int UnusedData1 = 0;
static int Logos2Frame = 0;
static int UnusedData2[2] = { 0 };
static int LogosFrame = 0;
static int UnusedData3[2] = { 0 };
static int PictureFrame = 0;
#include "level_qendmis1_cluts.h"
static int UnusedData4 = 0;
static int YOffset = 0;
static int MaxVlcSize = 0;
static int MovieWidth = 0;
static int MovieHeight = 0;
static int UnusedData5 = 0;
static int Done = 0;

void FormatScoreLine(struct SCORELINE *line, char *name, short score)
{
    char buf[8];
    int scorelen;
    int namelen;
    int i;
    char *p;

    ENGINE->sprintf(buf, "%d", score);
    scorelen = 0;
    p = buf;
    while (*p++)
        scorelen++;
    p = name;
    namelen = 0;
    while (*p++)
        namelen++;
    for (i = 0; i < 31; i++)
        line->message[i] = ' ';
    line->message[31] = 0;
    p = &line->message[12];
    i = 0;
    while (namelen--)
        *p++ = name[i++];
    p = &line->message[26];
    i = 0;
    while (scorelen--)
        *p++ = buf[i++];
}

static void SetupScoreLines(struct SCORELINE *line)
{
    int numplayers = ENGINE->GameConfig.sNumPlayers;
    short teamcount[4] = { 0 };
    short scores[4];
    short ids[4];
    short teamscores[4];
    short teamids[4];
    char *teamnames[4] = { "BLUE", "RED", "PURPLE", "GREEN" };
    int numteams;
    int i, j;
    int y;
    int n;

    for (i = 0; i < numplayers; i++) {
        if (ENGINE->GameConfig.sMultiMode == 5)
            scores[i] = ENGINE->Scores[i];
        else
            scores[i] = ENGINE->Frags[i];
        ids[i] = i;
    }
    SortScores(scores, ids, numplayers);
    n = ENGINE->GameConfig.sNumPlayers;
    if (ENGINE->GameConfig.sMultiMode == 1) {
        numteams = 0;
        for (i = 0; i < numplayers; i++) {
            short *t = &teamcount[ENGINE->GameConfig.MultiPlayerParams[i].teamSelected];

            if (*t == 0)
                numteams++;
            (*t)++;
        }
        n += 1 + numteams * 2;
        j = 0;
        for (i = 0; i < 4; i++) {
            if (teamcount[i]) {
                teamids[j] = i;
                teamscores[j] = ENGINE->Scores[i];
                j++;
            }
        }
        SortScores(teamscores, teamids, numteams);
    }
    y = (244 - n * 11) >> 1;
    if (ENGINE->GameConfig.sMultiMode == 1) {
        for (i = 0; i < numteams; i++) {
            ENGINE->sprintf(line->message, "%s TEAM SCORED %d", teamnames[teamids[i]], teamscores[i]);
            line->cursorItem = 1;
            line->bannerColour = teamids[i];
            ENGINE->FrontEndModifyText(line, line);
            line->x = 256;
            line->y = y;
            y += 15;
            line++;
            for (j = 0; j < numplayers; j++) {
                if (ENGINE->GameConfig.MultiPlayerParams[ids[j]].teamSelected == teamids[i]) {
                    FormatScoreLine(line, ENGINE->GameConfig.MultiPlayerParams[ids[j]].playerName, scores[j]);
                    ENGINE->FrontEndModifyText(line, line);
                    PlayerLines[ids[j]] = line;
                    ENGINE->PrintF("Adding Player %d to team table %d\nString is %s\n", ids[j], teamids[i], line);
                    line->y = y;
                    y += 15;
                    line->x = 256;
                    line->bannerColour = teamids[i];
                    line++;
                }
            }
            y += 15;
        }
    } else {
        for (i = 0; i < ENGINE->GameConfig.sNumPlayers; i++) {
            FormatScoreLine(line, ENGINE->GameConfig.MultiPlayerParams[ids[i]].playerName, scores[i]);
            ENGINE->FrontEndModifyText(line, line);
            PlayerLines[ids[i]] = line;
            line->x = 256;
            line->y = y;
            y += 15;
            line->cursorItem = 1;
            line->bannerColour = ENGINE->GameConfig.MultiPlayerParams[ids[i]].playerSelected;
            line++;
        }
    }
}

static void WaitForPlayers(void)
{
    int allready;
    int i;

    allready = 1;
    for (i = 0; i < ENGINE->GameConfig.sNumPlayers; i++) {
        if (ENGINE->MapPadInputToFire(i))
            PlayerReady[i] = 1;
        if (PlayerReady[i]) {
            PlayerLines[i]->message[2] = 'R';
            PlayerLines[i]->message[3] = 'E';
            PlayerLines[i]->message[4] = 'A';
            PlayerLines[i]->message[5] = 'D';
            PlayerLines[i]->message[6] = 'Y';
        } else {
            PlayerLines[i]->message[2] = ' ';
            PlayerLines[i]->message[3] = ' ';
            PlayerLines[i]->message[4] = ' ';
            PlayerLines[i]->message[5] = ' ';
            allready = 0;
            PlayerLines[i]->message[6] = ' ';
        }
    }
    if (allready) {
        ReadyTimeout -= *ENGINE->NewVcounts;
        if (ReadyTimeout < 0)
            *ENGINE->LevelComplete = 18;
    } else
        ReadyTimeout = 150;
}

static void InitScoreScreen(void)
{
    ENGINE->cursorupper = 0;
    ENGINE->cursorlower = 0;
    ENGINE->InGameInitScreen(44);
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
    ENGINE->FrontEndCreateMenu(NullMenu, 16);
    SetupScoreLines((struct SCORELINE *)&ENGINE->MyStringTable[1]);
    ENGINE->Specialfunc = WaitForPlayers;
}

static void DrawPicture(short y, short clut, short tpage, int is24, struct COLOUR col, int semitrans);
static void DrawSprite(struct DVECTOR xy, struct DVECTOR wh, short clut, short tpage, struct COLOUR col, int abr);

static void MPResultsUpdate(void)
{
    short clut = 1;     /* as a variable it takes the function to the 64 RTL insns that keep it out of
                           line (gcc 2.8 -O3 defers anything smaller to the end of the file) */

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
    if (Fade) {
        if (FadeOut) {
            Fade -= 2;
            if (Fade < 0)
                Fade = 0;
        } else {
            Fade += 2;
            if (Fade > 128)
                Fade = 128;
        }
        if (Fade)
            DrawSprite((struct DVECTOR){40, 20}, (struct DVECTOR){192, 128}, 1, 4, (struct COLOUR){Fade, Fade, Fade}, *ENGINE->ChrisCount & 0xf);
    }
    DrawPicture(ScrollY, 0, 0, 0, (struct COLOUR){128, 128, 128}, 0);
    if (*ENGINE->ChrisCount > 30 && ENGINE->MapPadInputToFire(0))
        State = 8;
    switch (State) {
    case 0:
        ENGINE->centreprintf(ENGINE->GameWindows, "establishing uplink....");
        Wait = 90;
        State++;
        break;
    case 2:
        ENGINE->centreprintf(ENGINE->GameWindows, "uplink established....");
        Wait = 64;
        State++;
        Fade = 2;
        break;
    case 4:
        ENGINE->centreprintf(ENGINE->GameWindows, "scanning area....");
        Wait = 19;
        State++;
        break;
    case 6:
        if (++ScrollY >= 240) {
            ENGINE->centreprintf(ENGINE->GameWindows, "closing uplink....");
            FadeOut = 1;
            Wait = 60;
            State++;
        }
        break;
    case 8:
        ENGINE->PrintF("Mission completed %d\n", *ENGINE->Mission);
        if (*ENGINE->Mission == 5)
            *ENGINE->LevelComplete = 13;
        else {
            *ENGINE->LevelComplete = 4;
            *ENGINE->NewMission = 1;
            *ENGINE->NextMission = *ENGINE->Mission + 1;
        }
        break;
    default:
        if (--Wait < 0)
            State++;
        break;
    }
}

static void Logos2Update(void)
{
    int t;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    Logos2Fade[2] = Logos2Fade[1] = Logos2Fade[0] = 0;
    t = ++Logos2Frame;
    if (t >= 0) {
        Logos2Time = t;
        if (t < 8)
            Logos2Fade[0] = t * 16;
        else if (t < 308)
            Logos2Fade[0] = 128;
        else if (t < 316)
            Logos2Fade[0] = 128 - (t - 308) * 16;
        if (Logos2Fade[0]) {
            DrawPicture(0, 0, 4, 1, (struct COLOUR){Logos2Fade[0], Logos2Fade[0], Logos2Fade[0]}, 1);
        }
        if (Logos2Time >= 308) {
            t = Logos2Time -= 308;
            if (t < 8)
                Logos2Fade[1] = t * 16;
            else if (t < 98)
                Logos2Fade[1] = 128;
            else if (t < 106)
                Logos2Fade[1] = 128 - (t - 98) * 16;
            if (Logos2Fade[1]) {
                DrawPicture(0, 1, 0, 1, (struct COLOUR){Logos2Fade[1], Logos2Fade[1], Logos2Fade[1]}, 1);
            }
            if (Logos2Time >= 110)
                *ENGINE->LevelComplete = 14;
        }
    }
}

static void LogosUpdate(void)
{
    int t;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    LogosFade[2] = LogosFade[1] = LogosFade[0] = 0;
    t = ++LogosFrame;
    if (t >= 0) {
        LogosTime = t;
        if (t < 8)
            LogosFade[1] = t * 16;
        else if (t < 98)
            LogosFade[1] = 128;
        else if (t < 106)
            LogosFade[1] = 128 - (t - 98) * 16;
        if (LogosFade[1]) {
            DrawPicture(0, 1, 4, 1, (struct COLOUR){LogosFade[1], LogosFade[1], LogosFade[1]}, 1);
        }
        if (LogosTime >= 98) {
            t = LogosTime -= 98;
            if (t < 8)
                LogosFade[2] = t * 16;
            else if (t < 98)
                LogosFade[2] = 128;
            else if (t < 106)
                LogosFade[2] = 128 - (t - 98) * 16;
            if (LogosFade[2]) {
                DrawPicture(0, 2, 8, 1, (struct COLOUR){LogosFade[2], LogosFade[2], LogosFade[2]}, 1);
            }
            if (LogosTime >= 108)
                *ENGINE->LevelComplete = 12;
        }
    }
}

static void PictureUpdate(int len, short clut, short next)
{
    int fade[3];
    int t;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    t = ++PictureFrame;
    PictureTime = t;
    fade[2] = fade[1] = fade[0] = 0;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    if (t < 8)
        fade[0] = t * 16;
    else if (t < len + 8)
        fade[0] = 128;
    else if (t < len + 16)
        fade[0] = 128 - (t - 8 - len) * 16;
    if (fade[0]) {
        DrawPicture(0, 0, 2, 1, (struct COLOUR){fade[0], fade[0], fade[0]}, 1);
    }
    if (PictureTime >= len + 18)
        *ENGINE->LevelComplete = next;
}

static void DrawSprite(struct DVECTOR xy, struct DVECTOR wh, short clut, short tpage, struct COLOUR col, int abr)
{
    unsigned long *ot = &(*ENGINE->currdb)->Ot[0][48];
    short tp = tpage + (abr >> 1);

    if (SprtPtr < SprtEnd) {
        *(struct COLOUR *)&SprtPtr->r0 = col;
        *(struct DVECTOR *)&SprtPtr->x0 = xy;
        SprtPtr->u0 = 0;
        SprtPtr->v0 = (abr & 1) << 7;
        SprtPtr->clut = ENGINE->palette[clut];
        *(struct DVECTOR *)&SprtPtr->w = wh;
        SprtPtr->code = 0x66;
        SprtPtr->tag = 0x04000000;
        ENGINE->AddPrim_(ot, SprtPtr++);
    }
    if (TPagePtr < TPageEnd) {
        SetDrawTPage(TPagePtr, 1, 0, GetTPage(0, 1, ENGINE->tpscreenx[tp], ENGINE->tpscreeny[tp]));
        ENGINE->AddPrim_(ot, TPagePtr++);
    }
}

static void DrawPicture(short y, short clut, short tpage, int is24, struct COLOUR col, int semitrans)
{
    unsigned long *ot;
    int v;
    int mode;
    int tpstep;
    int i;
    int x;
    int h;
    int w;

    v = y;
    x = 0;
    h = 240;
    if (is24) {
        tpstep = 2;
        mode = 1;
    } else {
        tpstep = 1;
        mode = 0;
    }
    w = 256 - y;
    ot = &(*ENGINE->currdb)->Ot[0][48];
    for (i = 0; i < 4; i++) {
        if (h) {
            if (h < w)
                w = h;
            if (SprtPtr < SprtEnd) {
                *(struct COLOUR *)&SprtPtr->r0 = col;
                SprtPtr->x0 = (i << 8) & 0x1ff;
                SprtPtr->y0 = x;
                SprtPtr->u0 = 0;
                SprtPtr->v0 = v;
                SprtPtr->clut = ENGINE->palette[clut];
                SprtPtr->w = 256;
                SprtPtr->h = w;
                SprtPtr->code = 0x64;
                if (semitrans)
                    SprtPtr->code |= 2;
                SprtPtr->tag = 0x04000000;
                ENGINE->AddPrim_(ot, SprtPtr++);
            }
            if (i == 1) {
                h -= w;
                w = 256;
                v = 0;
                x = x + 256 - y;
            }
            if (TPagePtr < TPageEnd) {
                SetDrawTPage(TPagePtr, 1, 0, GetTPage(mode, 1, ENGINE->tpscreenx[tpage], ENGINE->tpscreeny[tpage]));
                tpage += tpstep;
                ENGINE->AddPrim_(ot, TPagePtr++);
            }
        }
    }
}

int printf(char *fmt, ...)
{
}

static void strCallback(void)
{
    struct RECT snap_rect;
    short rest;

    if (*ENGINE->StCdIntrFlagAddr) {
        ENGINE->StCdInterrupt();
        *ENGINE->StCdIntrFlagAddr = 0;
    }
    ENGINE->LoadImage(&dec.slice, dec.imgbuf);
    rest = dec.rect[dec.rectid].w % 24;
    if (IsRGB24Start && rest) {
        IsRGB24Start = 0;
        dec.slice.x += rest * 3 / 2;
    } else
        dec.slice.x += 24;
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w)
        DecDCTout((unsigned long *)dec.imgbuf, ((dec.slice.w - 1) / 24 + 1) * (((dec.slice.h - 1) / 16 + 1) * 384) >> 1);
    else {
        dec.isdone = 1;
        dec.rectid = dec.rectid ? 0 : 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        IsRGB24Start = 1;
        dec.slice.y = dec.rect[dec.rectid].y + YOffset;
    }
}

static unsigned long *strNext(void)
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

static void strSetDispEnv(struct DISPENV *disp, struct DRAWENV *draw)
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

void strKickCD(void)
{
    while (!ENGINE->CdControl(0x15, &Loc, 0))
        ;
    while (!ENGINE->CdRead2(0x1c0))
        ;
}

void strInit(unsigned long *ring, void (*callback)(), int endframe)
{
    DecDCTReset(0);
    isEndOfFlag = 0;
    DecDCToutCallback(callback);
    ENGINE->StSetRing(RingBuf, 64);
    ENGINE->StSetStream(1, 1, ring, 0, endframe);
    strKickCD();
}

void strSetDefDecEnv(int x0, int y0, int x1, int y1)
{
    setRECT(&dec.rect[0], x0, y0, 480, 240);
    setRECT(&dec.rect[1], x1, y1, 480, 240);
    ENGINE->ClearImage(&dec.rect[0], 0, 0, 0);
    ENGINE->ClearImage(&dec.rect[1], 0, 0, 0);
    ENGINE->DrawSync(0);
    setRECT(&dec.slice, x0, y0, 24, 240);
    dec.vlcid = 0;
    dec.rectid = 0;
    dec.isdone = 0;
}

int strNextVlc(void)
{
    int cnt;
    unsigned long *next;

    cnt = 0x800000;
    while ((next = strNext()) == 0) {
        if (--cnt == 0)
            return -1;
    }
    if (MaxVlcSize < DecDCTBufSize(next))
        MaxVlcSize = DecDCTBufSize(next);
    dec.vlcid = dec.vlcid ^ 1;
    DecDCTvlc2(next, dec.vlcbuf[dec.vlcid], VlcTable);
    ENGINE->StFreeRing(next);
    return 0;
}

void strSync(void)
{
    volatile int cnt;

    cnt = 0x800000;
    while (!dec.isdone) {
        if (--cnt == 0) {
            dec.isdone = 1;
            dec.rectid = dec.rectid ? 0 : 1;
            dec.slice.x = dec.rect[dec.rectid].x;
            dec.slice.y = dec.rect[dec.rectid].y + YOffset;
        }
    }
    dec.isdone = 0;
}

static void MovieFreeze(void);

static short PlayMovie(char *name, int endframe, int vlcsize, int clear, int volume)
{
    long unused[12];    /* never used, but in the original's frame */
    char path[40];
    struct DISPENV disp;
    struct DRAWENV draw;
    int frame;
    int vol;
    int i;
    short ret;

    frame = 0;
    ret = 0;
    YOffset = 0;
    ScreenY = ENGINE->GameConfig.sScreenPosY;
    ScreenX = ENGINE->GameConfig.sScreenPosX;
    ENGINE->strcpy(path, "\\Q2DATA\\");
    ENGINE->strcat(path, "MOVIES\\");
    ENGINE->strcat(path, name);
    ENGINE->strcat(path, ";1");
    ENGINE->PrintF("Play movie %s %d\n", path, endframe);
    EndFrame = endframe;
    if (vlcsize)
        VlcSize = vlcsize;
    else
        VlcSize = 0x20400;
    MaxVlcSize = 0;
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    if (clear) {
        ENGINE->ClearImage(&ENGINE->Db[0].DispEnv.disp, 0, 0, 0);
        ENGINE->ClearImage(&ENGINE->Db[1].DispEnv.disp, 0, 0, 0);
    }
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    Done = 0;
    VlcTable = ENGINE->My_malloc_name(0x11000, "ring buffer");
    RingBuf = ENGINE->My_malloc_name(0x20000, "ring buffer");
    dec.vlcbuf[0] = ENGINE->My_malloc_name(VlcSize, "vlc buffer 0");
    dec.vlcbuf[1] = ENGINE->My_malloc_name(VlcSize, "vlc buffer 1");
    dec.imgbuf = ENGINE->My_malloc_name(0x5a00, "image buffer");
    DecDCTvlcBuild(VlcTable);
    IsRGB24Start = 1;
    if (!ENGINE->CdSearchFile(&File, path)) {
        ENGINE->PrintF("Movie not found\n");
        Done = 1;
    }
    strSetDefDecEnv(0, YOffset, 512, YOffset);
    Loc = File.pos;
    FileSize = File.size;
    strSetDispEnv(&disp, &draw);
    strInit((unsigned long *)0x0fffffff, strCallback, 0);
    if (!Done)
        strNextVlc();
    dec.slice.y = dec.rect[dec.rectid].y + YOffset;
    vol = 0;
    while (!isEndOfFlag && !Done) {
        frame++;
        vol += volume;
        if (frame <= 16)
            ENGINE->SetCDVolume(vol / 16);
        DecDCTin(dec.vlcbuf[dec.vlcid], 3);
        DecDCTout((unsigned long *)dec.imgbuf, ((dec.slice.w - 1) / 24 + 1) * (((dec.slice.h - 1) / 16 + 1) * 384) >> 1);
        if (strNextVlc() < 0) {
            Done = 1;
            ret = -1;
        }
        strSync();
        ENGINE->VSync(0);
        ENGINE->NewPadRead();
        if (ENGINE->GameWindows->Input.Original & 8) {
            Done = 1;
            ret = 2;
        }
        if (ENGINE->GameWindows->Input.Original & 0x4000) {
            Done = 1;
            ret = 1;
        }
        strSetDispEnv(&disp, &draw);
    }
    vol = volume * 4;
    for (i = 4; i > 0; i--) {
        ENGINE->SetCDVolume(vol / 4);
        ENGINE->VSync(0);
        vol -= volume;
    }
    ENGINE->StUnSetRing();
    ENGINE->CdControlB(9, 0, 0);
    DecDCToutCallback(0);
    DecDCToutCallback(0);
    ENGINE->CdDataCallback(0);
    ENGINE->CdReadyCallback(0);
    ENGINE->MyFree(&VlcTable);
    ENGINE->MyFree(&RingBuf);
    ENGINE->MyFree(&dec.vlcbuf[0]);
    ENGINE->MyFree(&dec.vlcbuf[1]);
    ENGINE->MyFree(&dec.imgbuf);
    ENGINE->DrawSync(0);
    MovieFreeze();
    ENGINE->PrintF("Max size of each VLC buffer = %d bytes.\n\n", MaxVlcSize * 4 + 1024);
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    ENGINE->PutDispEnv(&(*ENGINE->currdb)->DispEnv);
    ENGINE->VSync(0);
    ENGINE->DrawSync(0);
    ENGINE->ClearOTag(ENGINE->Db[0].InitOt, 0xd9);
    ENGINE->ClearOTag(ENGINE->Db[1].InitOt, 0xd9);
    return ret;
}

/* copies the last frame (24-bit) into a 15-bit picture in the display area and fades it out */
static void MovieFreeze(void)
{
    struct RECT src;
    struct RECT dst;
    struct TILE tile;
    struct DR_MODE mode;
    struct DISPENV disp;
    struct DRAWENV draw;
    unsigned char *buf;
    int yoff;
    int x, y;
    int i;
    unsigned char *s;
    unsigned short *d;

    yoff = YOffset;
    if (dec.rectid) {
        int id = dec.rectid ? 0 : 1;

        dec.rect[id].y += yoff;
        ENGINE->MoveImage(&dec.rect[id], dec.rect[1].x, dec.rect[1].y + yoff);
        dec.rect[id].y -= yoff;
        ENGINE->DrawSync(0);
    }
    dec.rectid = 0;
    strSetDispEnv(&disp, &draw);
    setRECT(&src, dec.rect[1].x, dec.rect[1].y + YOffset, dec.rect[1].w, dec.rect[1].h);
    setRECT(&dst, dec.rect[0].x, dec.rect[0].y, dec.rect[0].w * 2 / 3, dec.rect[0].h);
    buf = ENGINE->My_malloc_name(src.w * src.h * 2, "source buf");
    ENGINE->StoreImage(&src, buf);
    ENGINE->DrawSync(0);
    s = buf;
    d = (unsigned short *)buf;
    for (y = 0; y < src.h; y++) {
        for (x = 0; x < src.w; x += 3) {
            d[0] = (s[0] >> 3) + ((s[2] & 0xf8) << 7) + ((s[1] & 0xf8) << 2);
            d[1] = (s[3] >> 3) + ((s[5] & 0xf8) << 7) + ((s[4] & 0xf8) << 2);
            s += 6;
            d += 2;
        }
    }
    i = dst.h;
    dst.h = 240;
    ENGINE->ClearImage(&dst, 0, 0, 0);
    dst.h = i;
    dst.y += yoff;
    ENGINE->LoadImage(&dst, buf);
    dst.h = 240;
    dst.y -= yoff;
    ENGINE->DrawSync(0);
    ENGINE->SetDefDispEnv(&disp, dst.x, dst.y, dst.w, dst.h);
    ENGINE->SetDefDrawEnv(&draw, dst.x, dst.y, dst.w, dst.h);
    disp.isrgb24 = 1;
    disp.isinter = 0;
    disp.screen.x = ScreenX;
    disp.screen.y = ScreenY + 8;
    ENGINE->VSync(0);
    ENGINE->PutDispEnv(&disp);
    ENGINE->PutDrawEnv(&draw);
    tile.x0 = 0;
    tile.y0 = 0;
    tile.w = dst.w;
    tile.r0 = 8;
    tile.g0 = 8;
    tile.b0 = 8;
    tile.h = dst.h;
    ENGINE->SetTile(&tile);
    tile.code = (tile.code & 0xfe) | 2;
    ENGINE->SetDrawMode(&mode, 1, 0, GetTPage(0, 2, 512, 256), 0);
    for (i = 32; i > 0; i--) {
        ENGINE->DrawSync(0);
        ENGINE->ClearOTag(ENGINE->Db->BigOt, 11);
        ENGINE->AddPrim_(ENGINE->Db->BigOt, &tile);
        ENGINE->AddPrim_(ENGINE->Db->BigOt, &mode);
        ENGINE->VSync(0);
        ENGINE->DrawOTag(ENGINE->Db->BigOt);
    }
    ENGINE->MyFree(&buf);
    ENGINE->ClearImage(&ENGINE->Db[0].DispEnv.disp, 0, 0, 0);
    ENGINE->ClearImage(&ENGINE->Db[1].DispEnv.disp, 0, 0, 0);
}

int ResetCallback(void)
{
    return ENGINE->ResetCallback();
}

void DummyUpdate(void)
{
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 10;
}

void IntroUpdate(void)
{
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 6;
}

void ExtroUpdate(void)
{
    ENGINE->GameWindows->CameraRoutine = StaticCamera;
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 17;
}

void MagazineIntrUpdate(void)
{
    PictureUpdate(150, 1, 6);
}

void MagazineExtrUpdate(void)
{
    PictureUpdate(750, 0, 9);
}

/* sets the semi-transparency bit of every non-black colour */
void SetClutSemiTrans(short *clut, int n)
{
    while (n) {
        *clut &= 0x7fff;
        if (*clut)
            *clut |= 0x8000;
        n--;
        clut++;
    }
}

void LoadClut(unsigned short *clut, short slot, int is256)
{
    struct RECT rect;

    rect.x = (ENGINE->palette[slot] & 0x3f) << 4;
    rect.y = ENGINE->palette[slot] >> 6;
    rect.h = 1;
    rect.w = is256 ? 256 : 16;
    ENGINE->LoadImage(&rect, clut);
    ENGINE->DrawSync(0);
}

void StaticCamera(void)
{
    struct WINDOW *w;
    short i;

    for (i = 0; i < *ENGINE->MaxWindows; i++) {
        w = &ENGINE->GameWindows[i];
        w->CamPos.vx = 0;
        w->CamPos.vy = 0;
        w->CamPos.vz = 0;
        w->CamAng.vx = 0;
        w->CamAng.vy = 0;
        w->CamAng.vz = 0;
        w->ControlBits |= 7;
        ENGINE->QGenWindowSortList(w);
        ENGINE->UpdateWindowStuff(w);
    }
}

int DMACallback(int dma, void (*func)())
{
    return ENGINE->DMACallback(dma, func);
}

/* the byte the original link left between this object's .rdata and LIBPRESS's */
__asm__(".rdata\n .byte 0x3b\n .text");
