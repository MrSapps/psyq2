/* The MagazineExtr build of the intermission/FMV/logo/results module (see level_qendmis1.c): an
 * earlier version of the same program, built against an older engine interface (8 bytes shorter,
 * see struct QENGINEOLD) and an older struct DB. It has QLogos (three logos and a skip) but no
 * QLogos2, a plain EndMission screen, and sprintf'd score lines. */
/* ovl-libs: libc2/MEMSET libgpu/P00 libgpu/P33 libpress/LIBPRESS libpress/VLC_C libpress/BUILD libpress/DCT002 */
#include "ovl.h"
#include "bss.h"

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* the struct DB of this interface: its DispEnv..BigOt 16 bytes earlier, size 0x7e88 */
struct DBOLD {
    char Unknown0[0x2aac];
    struct DISPENV DispEnv;     /* 0x2aac */
    struct DR_MOVE WaterMove;   /* 0x2ac0 */
    unsigned long InitOt[2];    /* 0x2ad8 */
    unsigned long Ot[4][51];    /* 0x2ae0 */
    unsigned long BigOt[11];    /* 0x2e10 */
    char Unknown1[0x7e88 - 0x2e3c];
};

/* QENGINELEVINTERFACE as this module was built: one of the pointers 0x94-0xb8 missing, one more
 * field after LevelComplete, CantPause an int and no memcard/PDH fields at the end */
struct QENGINEOLD {  /* size 0x4ec */
    int SizeOf;
    int (*PrintF)();
    struct IDENTITYNAME *ThisLevelName;
    struct POLY_FT4 *PolyStack0;
    struct POLY_FT4 *PolyStack1;
    void *MissionData;
    unsigned int *usp;
    struct CREATURE *(*InitialiseQuakePlayer)();
    void (*QuakeSinglePlayerInit)();
    int (*ActivateCreatureBatch)();
    void (*LoadSaveCreatureBatches)();
    struct CREATURE *(*InitialiseQuakePickup)();
    void (*InitMultiQuakePlayer)();
    int (*PrimaryQuakePickup)();
    void (*AddCross)();
    int (*AddLight)();
    struct CREATURE *(*AddLaser)();
    int (*AddThickLine)();
    struct PARTICLE *(*AddParticlesSL)();
    void (*SceneObjFragments)();
    void (*QuakeGenExplosion)();
    void (*SelfRecordOn)();
    void (*SelfPlayOn)();
    int (*QuakeEndSelfPlay)();
    int (*QuakeNextDemoLevel)();
    void (**LevelSpecific)();
    void (**LevelExtraDraw)();
    int *NoInputTime;
    short *(*SetNoInputTimeOutRoutine)();
    int (*IsPlayerPadShockable)();
    int *InSelfPlayMode;
    struct BRUSHMOVEBUF *BrushMoveBufs;
    struct SOLIDBOX *(*GenSolidBox)();
    int (*GetSceneNum)();
    int (*Find3dSpaceIndxLink)();
    struct COLLHEADER *CollMap;
    struct COLLHEADER *MoveCollMap;
    char *GameDeathWatchA;
    char *GameDeathWatchB;
    int (*Event_SetDisableNamed)();
    int (*Event_GetDisableNamed)();
    int (*Event_DoNamed)();
    int (*Event_GetNamedOffset)();
    struct PSXMAINEVENTHDR **eventlistbase;
    int (*SquareRootNR)();
    long *DummyMemory;
    struct DBOLD *Db;
    unsigned char *DbFrame;
    void (*RemoveMeRoutine)();
    void (*SetGeomStuffWin)();
    int (*AreaValid)();
    void (*SetupScreenGivenArea)();
    int *NewVcounts;
    int *MaxWindows;
    struct PARTCOLOURS **ParticleColoursTable;
    struct CREATURE *Creatures;
    struct SCENE2 *Scene2Data;
    int *ChrisCount;
    unsigned int *LevelTicks;
    struct IDENTITYNAME *ZoneName;
    struct STARTPOSITION **MainPlayerStart;
    struct CSTBL *rcossin_tbl;
    int *GlobalKey;
    struct MISSIONEVENT *LevelEvents;
    struct WINDOW *GameWindows;
    void ***OnZoneLoadTable;
    int (*rand)();
    int (*sprintf)();
    struct MATRIX *(*RotMatrixYXZ)();
    char *(*PrintIdentityName)();
    void (*centreprintf)();
    void (*dprintf)();
    char *(*GetStringFromStringtable)();
    void (*PlayThisXA)();
    int (*SoundByCreature)();
    int (*GetSoundNum)();
    int (*GetSoundClone)();
    void (*SoundSetVolume)();
    void (*SoundSetPitch)();
    void (*SoundSetRelativeVolume)();
    void (*SoundSetRelativePitch)();
    int (*SoundTestIfRunning)();
    int (*SoundStopSoundID)();
    int (*SoundAtLocation)();
    int (*SoundHere)();
    int (*MapPadInputToFire)();
    void *(*MY_malloc_namehb)();
    long (*VectorNormalSS)();
    void (*SetupScreen2by2)();
    void (*SetupScreen1by2)();
    void (*SetupScreen2by1)();
    void (*SetupScreen1by1)();
    void (*AlterFocalSubDiv)();
    void (*GetAbsoluteModelVert)();
    int (*AddBlurPoints)();
    struct LEVELDATA *(*GetLevelDataFromIdentityName)();
    int (*LoadImage)();
    int (*DrawSync)();
    unsigned short (*GetTPage)();
    void (*AddPrim_)();
    void (*ChrisInitEveryFrame)();
    void (*ProcessMoveBufs)();
    void (*ProcessSounds)();
    void (*DrawAllScenes)();
    void (*DrawAllSortedObjs)();
    void (*ProcessAllLensFlare)();
    void (*DrawAllModels)();
    void (*ProcessLights)();
    void (*ProcessBlurs)();
    void (*FreeSubDivs)();
    void (*ReallyFreePolyPackets)();
    void (*QGenWindowSortList)();
    void (*UpdateWindowStuff)();
    void (*MoveModels)();
    unsigned long (*SetSp)();
    void (*ReloadAutoSave)();
    short *tpscreenx;
    short *tpscreeny;
    short *palette;
    struct DBOLD **currdb;
    void (*InGameInitScreen)();
    short (*FrontEndRestoreCursor)();
    void (*FrontEndClearCursorMemory)();
    void (*FrontEndResetPlayerDefaults)();
    void (*FrontEndResetSoundDefaults)();
    void (*FrontEndResetVideoDefaults)();
    void (*FrontEndResetMultiplayerDefaults)();
    void (*FrontEndCreateMenu)();
    void (*FrontEndAddText)();
    void (*FrontEndModifyText)();
    void (*DoFrontEndStuff)();
    void (*MoveBottomPoly)();
    void (*CHECK_FOR_SLIDER)();
    void (*CHECK_FOR_MCFILEWINDOW)();
    void (*MemCardInit)();
    void (*MemCardStart)();
    long (*MemCardAccept)();
    long (*MemCardFormat)();
    long (*MemCardSync)();
    long (*MemCardGetDirentry)();
    long (*MemCardReadFile)();
    long (*MemCardWriteFile)();
    long (*MemCardCreateFile)();
    void (*FrontEndInitText)();
    void (*MemoryCardInit)();
    void (*MemoryCardStart)();
    void (*MemoryCardStop)();
    long (*MemoryCardGetStatus)();
    void (*MemoryCardSetStatus)();
    char *(*MemoryCardGetFilename)();
    long (*MemoryCardGetNumFiles)();
    void (*MemoryCardGetCardname)();
    long (*MemoryCardGetNumCards)();
    void (*MemoryCardSetActiveCard)();
    void (*MemoryCardSetActiveFile)();
    long (*MemoryCardGetNumFreeBlocks)();
    void (*MemoryCardUpdate)();
    void (*InGame_PrintMemCardMessage)();
    void (*FrontEndLevelCompleteTimeout)();
    void (*GameSetup)();
    void (*ReReadToc)();
    int (*MyGetCDStatus)();
    void *Backfunc;
    void *Specialfunc;
    void *Lightingfunc;
    void *Controllerfunc;
    void *Draw256func;
    struct STRING_TABLE *MyStringTable;
    short *NumPadds;
    short *MultiTapExists;
    short gFrontEndPad[4];
    short gFrontEndOPad[4];
    short gFrontEndPadShot;
    char gFrontEndLoadType;
    unsigned char gFrontEndLoadSubType : 7;
    unsigned char gFrontEndLoadIsSpecial : 1;
    short gFrontEndTimeout;
    short gFrontEndLevelComplete;
    short gFrontEndPrevScreen;
    short currentscreen;
    short cursorpos;
    short cursorlower;
    short cursorupper;
    short maxcursor;
    struct QGAMECONFIG GameConfig;
    short MissionCDStatus;
    void (*PatchEngineVariables)();
    struct IDENTITYNAME *DirectAccessLevelName;
    void (**MainLoop)();
    void (*InitialiseQuakePickups)();
    struct CREATURE *(*SpawnGeneral)();
    void (*KillModel)();
    struct IDENTITYNAME *MainStartName;
    short *LevelComplete;
    void *Unknown;              /* not in the later interface */
    void (*GenVectBox)();
    void (*OrderedSort)();
    struct AREA *AreaData;
    struct SHIELDGLINT *ReactorShieldGlints;
    void (*GenShieldMatrix)();
    void (*Rotate)();
    void (*SetWinFade)();
    struct ANIMHDR *(*FindAnimHdr)();
    void (*DoDamage)();
    void (*GetToc)();
    void (*HelpComputer_NewMessage)();
    short *DoSnapShot;
    short (*SnapShotPadViolation)();
    char *(*ControlGetPortName)();
    void (*Config_InstallVideo)();
    void *(*My_malloc_name)();
    void (*MyFree)();
    void (*StCdInterrupt)();
    int *StCdIntrFlagAddr;
    int (*ClearImage)();
    int (*MoveImage)();
    int (*StoreImage)();
    unsigned long (*StGetNext)();
    unsigned long (*StFreeRing)();
    void (*StUnSetRing)();
    void (*StSetRing)();
    void (*StSetStream)();
    void (*SetDispMask)();
    struct DISPENV *(*SetDefDispEnv)();
    struct DRAWENV *(*SetDefDrawEnv)();
    struct DISPENV *(*PutDispEnv)();
    struct DRAWENV *(*PutDrawEnv)();
    void (*SetDrawMode)();
    void (*SetTile)();
    unsigned long *(*ClearOTag)();
    void (*DrawOTag)();
    int (*VSync)();
    char *(*strcpy)();
    char *(*strcat)();
    int (*CdControl)();
    int (*CdControlB)();
    void *(*CdDataCallback)();
    void (*(*CdReadyCallback)())();
    int (*CdRead2)();
    struct CdlFILE *(*CdSearchFile)();
    void (*SetCDVolume)();
    void (*NewPadRead)();
    int (*ResetCallback)();
    int (*DMACallback)();
    char (*AddLevelInterMission)();
    void (*SetDrawEnv)();
    int (*ModNameCmp)();
    short *TPages;
    int *NewMission;
    int *NextMission;
    int *Mission;
    int *DebugWatch;
    void (*LoadSaveGameConfigData)();
    void (*ProcessMessageText)();
    void (*PrintDebugInfoScene)();
    int *DebugInfoPntr;
    int CantPause;
    int FrontEndStartScreen;
    void (*DrawBorder)();
    void (*SetShockData)();
    short Frags[4];
    short Scores[4];
    short KillsMatrix[4][4];
    short *targetcdvol;
    short gFreeMice;
};

#define ENGINE ((struct QENGINEOLD *)LevelInterface.Engine)
#define QEND_OLD_PLAYER

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

int ResetCallback(void);
void DummyUpdate(void);
void IntroUpdate(void);
void ExtroUpdate(void);
void MagazineIntrUpdate(void);
void MagazineExtrUpdate(void);
void SetClutSemiTrans(short *clut, int n);
void LoadClut(unsigned short *clut, short slot, int is256);
void StaticCamera(void);
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
static int UnusedBss5[2] BSS;
static struct DECENV dec BSS;
static unsigned long *RingBuf BSS;
static unsigned short *VlcTable BSS;
static short ScreenX BSS;
static short ScreenY __attribute__((section(".bss")));
static int UnusedBss6 BSS;
static struct DR_TPAGE TPages[2][5] BSS;
static struct DR_TPAGE *TPageEnd BSS;
static int UnusedBss7 BSS;
static struct SCORELINE *PlayerLines[4] BSS;
static int UnusedBss8[2] BSS;
static struct SPRT Sprites[2][5] BSS;
static struct DR_TPAGE *TPagePtr BSS;
static int UnusedBss9 BSS;
static struct SPRT *SprtEnd BSS;
static struct SPRT *SprtPtr BSS;

/* .data defined before InitLevel's users */
extern unsigned short Cluts[7][16];
extern unsigned short Clut256[6][256];
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
    if (ENGINE->SizeOf != sizeof(struct QENGINEOLD)) {
        for (;;)
            ENGINE->PrintF("Please get a programmer to recompile level %s\n",
                ENGINE->PrintIdentityName(*ENGINE->ThisLevelName));
        ENGINE->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINEOLD), ENGINE->SizeOf);
    }
    ENGINE->PlayThisXA(2);
    if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 1"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[2], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 2"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[3], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 3"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[4], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 4"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[5], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"EndMission 5"}) == 1) {
        *ENGINE->LevelSpecific = EndMissionUpdate;
        ENGINE->palette[0] = 0x4000;
        ENGINE->palette[1] = 0x4040;
        LoadClut(Cluts[6], 0, 0);
        LoadClut(Cluts[1], 1, 0);
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
    } else if (ENGINE->ModNameCmp(ENGINE->ThisLevelName, &(struct IDENTITYNAME){"Intro FMV"}) == 1) {
        ENGINE->PrintF("Do Intro\n");
        PlayMovie("TAKE1B.STX", 1539, 0x10000, 1, 255);
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
        ENGINE->GameWindows->CameraRoutine = StaticCamera;
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
    { 0, 0, 0, 0, 0, 0, 0 },
};
struct FET_MESSAGE NullMenu[] = { { 0, 0, 0, 0, 0, 0, 0 } };
static unsigned char PlayerReady[4] = { 0 };
static int ReadyTimeout = 150;
static int UnusedData0 = 0;
static int ScrollY = 0;
static int UnusedData1 = 0;
static int LogosFrame = 0;
static int LogosSkip = 0;
static int UnusedData2 = 0;
static int PictureFrame = 0;
#include "level_qmagextr_cluts.h"
static int UnusedData4 = 0;
static int YOffset = 0;
static int MaxVlcSize = 0;
static int MovieWidth = 0;
static int MovieHeight = 0;
static int UnusedData5 = 0;
static int Done = 0;

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
        n += numteams * 2;
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
    y = (252 - n * 11) >> 1;
    if (ENGINE->GameConfig.sMultiMode == 1) {
        for (j = 0; j < numteams; j++) {
            ENGINE->sprintf(line->message, "%s TEAM SCORED %d", teamnames[teamids[j]], teamscores[j]);
            line->cursorItem = 1;
            line->bannerColour = teamids[j];
            ENGINE->FrontEndModifyText(line, line);
            line->x = 256;
            line->y = y;
            y += 15;
            ENGINE->PrintF("Y is at %d\n", y);
            line++;
            for (i = 0; i < numplayers; i++) {
                if (ENGINE->GameConfig.MultiPlayerParams[ids[i]].teamSelected == teamids[j]) {
                    ENGINE->sprintf(line->message, "%14s       %d", ENGINE->GameConfig.MultiPlayerParams[ids[i]].playerName, scores[i]);
                    ENGINE->FrontEndModifyText(line, line);
                    PlayerLines[ids[i]] = line;
                    ENGINE->PrintF("Adding Player %d to team table %d\nString is %s\n", ids[i], teamids[j], line);
                    line->y = y;
                    y += 15;
                    line->x = 256;
                    ENGINE->PrintF("Y is at %d\n", y);
                    line->bannerColour = teamids[j];
                    line++;
                }
            }
            y += 15;
        }
    } else {
        for (j = 0; j < ENGINE->GameConfig.sNumPlayers; j++) {
            ENGINE->sprintf(line->message, "%18s    %d ", ENGINE->GameConfig.MultiPlayerParams[ids[j]].playerName, scores[j]);
            ENGINE->FrontEndModifyText(line, line);
            PlayerLines[ids[j]] = line;
            line->x = 256;
            line->y = y;
            y += 15;
            ENGINE->PrintF("Y is at %d\n", y);
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
        if (ENGINE->gFrontEndPad[i] && !ENGINE->gFrontEndOPad[i])
            PlayerReady[i] = 1 - PlayerReady[i];
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
            *ENGINE->LevelComplete = 17;
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
    DrawSprite((struct DVECTOR){40, 20}, (struct DVECTOR){192, 128}, 1, 4, (struct COLOUR){128, 128, 128}, *ENGINE->ChrisCount & 0xf);
    DrawPicture(ScrollY, 0, 0, 0, (struct COLOUR){128, 128, 128}, 0);
    if (++ScrollY >= 240 || ENGINE->MapPadInputToFire(0)) {
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

static void DrawPicture(short y, short clut, short tpage, int is24, struct COLOUR col, int semitrans);
/* the three logos, 83 frames each; fire after frame 126 fades them out and leaves */
static void LogosUpdate(void)
{
    int t;
    int u;
    int w;
    int i;
    int *fade;

    if (LogosFrame >= 126 && (ENGINE->GameWindows->Input.Original & 8))
        LogosSkip = 1;
    if (LogosSkip) {
        if (LogosSkip == 1) {
            if ((LogosFade[0] | LogosFade[1] | LogosFade[2]) == 0) {
                LogosSkip = 2;
                return;
            }
            for (i = 0, fade = LogosFade; i < 3; i++) {
                if (*fade) {
                    *fade -= 16;
                    if (*fade <= 0)
                        *fade = 0;
                    else
                        DrawPicture(0, i, i * 4, 1, (struct COLOUR){*fade, *fade, *fade}, 1);
                }
                fade++;
            }
        } else if (LogosSkip == 5)
            *ENGINE->LevelComplete = 6;
        else
            LogosSkip++;
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
        else if (t < 83)
            LogosFade[0] = 128;
        else if (t < 91)
            LogosFade[0] = 128 - (t - 83) * 16;
        if (LogosFade[0]) {
            DrawPicture(0, 0, 0, 1, (struct COLOUR){LogosFade[0], LogosFade[0], LogosFade[0]}, 1);
        }
        if (LogosTime >= 83) {
            u = LogosTime -= 83;
            if (u < 8)
                LogosFade[1] = u * 16;
            else if (u < 83)
                LogosFade[1] = 128;
            else if (u < 91)
                LogosFade[1] = 128 - (u - 83) * 16;
            if (LogosFade[1]) {
                DrawPicture(0, 1, 4, 1, (struct COLOUR){LogosFade[1], LogosFade[1], LogosFade[1]}, 1);
            }
            if (LogosTime >= 83) {
                w = LogosTime -= 83;
                if (w < 8)
                    LogosFade[2] = w * 16;
                else if (w < 83)
                    LogosFade[2] = 128;
                else if (w < 91)
                    LogosFade[2] = 128 - (w - 83) * 16;
                if (LogosFade[2]) {
                    DrawPicture(0, 2, 8, 1, (struct COLOUR){LogosFade[2], LogosFade[2], LogosFade[2]}, 1);
                }
                if (LogosTime >= 93)
                    *ENGINE->LevelComplete = 12;
            }
        }
    }
}

#include "level_qend_player.h"

/* the byte the original link left between this object's .rdata and LIBPRESS's */
__asm__(".rdata\n .byte 0xc5\n .text");
