/* The intermission, FMV, logo and multiplayer results module: EndMission 1-5, Intro FMV,
 * Extro FMV, QLogos, QLogos2, Dummy, MagazineIntr, MagazineExtr and MPResults all run it.
 * The movie player is PSY-Q's streaming sample (strplay) driven through the engine's
 * libcd/libgpu pointers; libpress and three small libc/libgpu functions were linked into
 * the module after this object.
 */
/* ovl-libs: libc2/MEMSET libgpu/P00 libgpu/P33 libpress/LIBPRESS libpress/VLC_C libpress/BUILD libpress/DCT002 */
#include "ovl.h"
#include <libetc.h>
#include "bss.h"

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

#define ENGINE LevelInterface.Engine

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

#include "level_qend_common.h"

void DummyUpdate(void);
void IntroUpdate(void);
void ExtroUpdate(void);
void MagazineIntrUpdate(void);
void MagazineExtrUpdate(void);
void SetClutSemiTrans(short *clut, int n);
void LoadSlotClut(unsigned short *clut, short slot, int is256);
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
/* the word after LIBPRESS's .data: the original image holds 4 junk bytes there (the start of
 * this object's .bss, left over from the build machine's memory); a .sdata word lands there */
static int BssJunk __attribute__((section(".sdata"))) = 0x6e69202c;   /* ", in" */
static int UnusedBss0[2] BSS;
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
static CdlLOC Loc BSS;
static CdlFILE File BSS;
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
static DR_TPAGE TPages[2][5] BSS;
static DR_TPAGE *TPageEnd BSS;
static int Wait BSS;
static struct SCORELINE *PlayerLines[4] BSS;
static int UnusedBss7[2] BSS;
static SPRT Sprites[2][5] BSS;
static DR_TPAGE *TPagePtr BSS;
static int UnusedBss8 BSS;
static int State BSS;
static SPRT *SprtEnd BSS;
static SPRT *SprtPtr BSS;

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
        LoadSlotClut(Cluts[2], 0, 0);
        LoadSlotClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 2"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadSlotClut(Cluts[3], 0, 0);
        LoadSlotClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 3"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadSlotClut(Cluts[4], 0, 0);
        LoadSlotClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 4"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadSlotClut(Cluts[5], 0, 0);
        LoadSlotClut(Cluts[1], 1, 0);
        State = 0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 5"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadSlotClut(Cluts[6], 0, 0);
        LoadSlotClut(Cluts[1], 1, 0);
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
        LoadSlotClut(Clut256[6], 0, 1);
        LoadSlotClut(Clut256[0], 1, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"QLogos"}) == 1) {
        *ENGINE->LevelSpecific = LogosUpdate;
        ENGINE->palette[0] = 0x3fc0;
        ENGINE->palette[1] = 0x3fd0;
        ENGINE->palette[2] = 0x3fe0;
        ENGINE->palette[3] = 0x3ff0;
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
        SetClutSemiTrans((short *)Clut256[0], 1024);
        LoadSlotClut(Clut256[0], 0, 1);
        LoadSlotClut(Clut256[1], 1, 1);
        LoadSlotClut(Clut256[2], 2, 1);
        LoadSlotClut(Clut256[7], 3, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Dummy"}) == 1) {
        *ENGINE->LevelSpecific = DummyUpdate;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MagazineIntr"}) == 1) {
        *ENGINE->LevelSpecific = MagazineIntrUpdate;
        ENGINE->palette[0] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[3], 256);
        LoadSlotClut(Clut256[3], 0, 1);
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MagazineExtr"}) == 1) {
        *ENGINE->LevelSpecific = MagazineExtrUpdate;
        ENGINE->palette[0] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[4], 256);
        LoadSlotClut(Clut256[4], 0, 1);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"MPResults"}) == 1) {
        *ENGINE->LevelSpecific = MPResultsUpdate;
        ENGINE->palette[1] = 0x3fc0;
        SetClutSemiTrans((short *)Clut256[5], 256);
        LoadSlotClut(Clut256[5], 1, 1);
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
    p = name;   /* one pointer for every loop: cse keeps p, not name, in the copied loop test */
    namelen = 0;
    while (*p++)
        namelen++;
    for (i = 0; i < 31; i++)    /* loop.c reverses this loop */
        line->message[i] = ' ';
    line->message[i] = 0;
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
        for (j = 0; j < numteams; j++) {
            ENGINE->sprintf(line->message, "%s TEAM SCORED %d", teamnames[teamids[j]], teamscores[j]);
            line->cursorItem = 1;
            line->bannerColour = teamids[j];
            ENGINE->FrontEndModifyText(line, line);
            line->x = 256;
            line->y = y;
            y += 15;
            line++;
            for (i = 0; i < numplayers; i++) {
                if (ENGINE->GameConfig.MultiPlayerParams[ids[i]].teamSelected == teamids[j]) {
                    FormatScoreLine(line, ENGINE->GameConfig.MultiPlayerParams[ids[i]].playerName, scores[i]);
                    ENGINE->FrontEndModifyText(line, line);
                    PlayerLines[ids[i]] = line;
                    ENGINE->PrintF("Adding Player %d to team table %d\nString is %s\n", ids[i], teamids[j], line);
                    line->y = y;
                    y += 15;
                    line->x = 256;
                    line->bannerColour = teamids[j];
                    line++;
                }
            }
            y += 15;
        }
    } else {
        for (j = 0; j < ENGINE->GameConfig.sNumPlayers; j++) {
            FormatScoreLine(line, ENGINE->GameConfig.MultiPlayerParams[ids[j]].playerName, scores[j]);
            ENGINE->FrontEndModifyText(line, line);
            PlayerLines[ids[j]] = line;
            line->x = 256;
            line->y = y;
            y += 15;
            line->cursorItem = 1;
            line->bannerColour = ENGINE->GameConfig.MultiPlayerParams[ids[j]].playerSelected;
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
static void DrawSprite(DVECTOR xy, DVECTOR wh, short clut, short tpage, struct COLOUR col, int abr);

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
            DrawSprite((DVECTOR){40, 20}, (DVECTOR){192, 128}, 1, 4, (struct COLOUR){Fade, Fade, Fade}, *ENGINE->ChrisCount & 0xf);
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
    int u;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    Logos2Fade[0] = Logos2Fade[1] = Logos2Fade[2] = 0;
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
            u = Logos2Time -= 308;
            if (u < 8)
                Logos2Fade[1] = u * 16;
            else if (u < 98)
                Logos2Fade[1] = 128;
            else if (u < 106)
                Logos2Fade[1] = 128 - (u - 98) * 16;
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
    int u;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    LogosFade[0] = LogosFade[1] = LogosFade[2] = 0;
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
            u = LogosTime -= 98;
            if (u < 8)
                LogosFade[2] = u * 16;
            else if (u < 98)
                LogosFade[2] = 128;
            else if (u < 106)
                LogosFade[2] = 128 - (u - 98) * 16;
            if (LogosFade[2]) {
                DrawPicture(0, 2, 8, 1, (struct COLOUR){LogosFade[2], LogosFade[2], LogosFade[2]}, 1);
            }
            if (LogosTime >= 108)
                *ENGINE->LevelComplete = 12;
        }
    }
}

#include "level_qend_player.h"

/* the byte the original link left between this object's .rdata and LIBPRESS's */
__asm__(".rdata\n .byte 0x3b\n .text");
