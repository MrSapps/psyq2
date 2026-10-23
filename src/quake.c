#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
int QuakeEndSelfPlay(int Win, int Manual);
int QuakeNextDemoLevel(void);
char GetCurrentLevelNumber(void);
void GameTitle(void);
void GameSetup(int GameType);
void GameEveryFrame(void);
void GameLevelStuff(void);
void GameTerminateStuff(void);
void GameNewGame(void);
void KillDualShocks(void);
int GameIsPaused(void);
void GameDisplayPausedMenu(void);
void PreInitNotNeeded(void);
void SetupGameSpecific(void);
void GameSelfPlay(void);
void GameDemoExit(void);
void GameMagDemoLevel(void);
void GameMPlayerExit(void);
void GameLogos(void);
void GameLogos2(void);
void GameIntro(void);
void GameExtro(void);
void GameReRunMPLevel(void);
void GameQuitGame(void);
void InitialiseSingleGame(void);
int GameCheckPaused(void);
void DrawBorder(struct RECT Rect, long *Ot, int OffsetY);
void DrawDarkenedBox(struct RECT Rect);
void InitMissionNull(void);
void InitMissionNull(void); void GameNewMission(void);
void GameInitStuff(void);
struct LEVELDATA *GetLevelDataFromIdentityName(struct IDENTITYNAME *Name);
void RemoveVram(void); void LoadInGameVram(void);
void ReloadAutoSave(void);
void ReloadAutoSave(void); void RestartLevel(int Resupply);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
struct CHARPAIR ProcessShockData(int Win);

/* defined in this file (Quake.obj), so $gp-relative */
int Paused;

/* also defined in Quake.obj */
struct LINE_F2 *NextQuakeLine;
struct LINE_F2 *LastQuakeLine;
struct LEVELDATA QuakeLevelData[52];
struct GAMEDB QuakeGameBuffs[2];

extern struct DB *currdb;
extern struct DB db[];
extern short LevelComplete;
extern struct LEVELDATA *GlobalLevelData;
extern struct LEVELDATA *ThisLevelData;
extern int NextMission;
extern int NewMission;
extern int MaxWindows;
extern struct WINDOW GameWindows[];

void QEntsEveryLevel(void);
void FreePathTList(void);
void FreeEdictTList(void);
void KillDualShocks(void);
void DeleteAllMatchingMemory(int type);

extern struct IDENTITYNAME DirectAccessLevelName;
extern struct IDENTITYNAME DirectAccessZoneName;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
extern int InSelfPlayMode;
extern int NextMission;
extern int NewMission;
extern int GameRunningMode;
extern int LevelChange;
extern short LevelComplete;
void dprintf();
void DoFrontEndStuff(void);
void DrawDarkenedBox(struct RECT Rect);

#define IDNAME(s) ((struct IDENTITYNAME){s})

/* volatile: the original reloads the pointer after the 12-byte name copy and
   tests the old pointer's next entry before storing the increment */
extern struct IDENTITYNAME SelfPlayMapNames[];
struct IDENTITYNAME * volatile SelfPlayMapNamePntr;

extern int LevelIsMultiPlayer;
void LoadSaveGameConfigData(int Write);

extern int MaxWindows;
void SelfPlayRecOff(int Win);

extern struct IDENTITYNAME DirectAccessLevelName;
extern struct IDENTITYNAME NextLevelName;
extern int NextMission;
extern struct QENGINELEVINTERFACE Engine;
extern char *MissionSpecificMemory;
extern struct MISSIONEVENT *MissionEvents;
extern void (*MissionPrintStatus)();
extern void (*GameHubLoadSaveData)();
char *PrintIdentityName(struct IDENTITYNAME Name);

extern struct DB *currdb;
struct RECT gDarkBoxRect;
void DrawBorder(struct RECT Rect, long *Ot, int OffsetY);
void SetTile(struct TILE *Tile);
void AddPrim_(void *Ot, void *Prim);

int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
char *PrintIdentityName(struct IDENTITYNAME Name);

extern short MaxAmmo[];
extern struct ARMOUR_TYPE Armour_Types[];
extern int WeaponBits[];
extern int AmmoTypes[];
int VBLevelTicks;
void RestoreLevelStartState(void);
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);

int GlobalKey;
struct LEVELDATA *FindLevelData(struct IDENTITYNAME LevelName);

void DownloadTextures(void);
int LoadPicture256(char *Name, int TPage);
int loadtp16b_ConCat(char *name, int tpnum, int skipheight);

extern struct SHOCKDATA *ShockDataAvailable[];
int SquareRootNR(int a);

static struct SHOCKDATA *FindShockData(int Ident)
{
    struct SHOCKDATA **Pntr;

    for (Pntr = ShockDataAvailable; *Pntr; Pntr++)
        if (Ident == (*Pntr)->Ident)
            return *Pntr;
    return 0;
}

extern int PalNtscRate;
int StillDataDualShock;
void *memset(void *s, int c, unsigned int n);

static int GetShockPoint(struct SHOCKPOINT *Pntr, int Time)
{
    while (Time >= Pntr->Time) {
        if (!Pntr->Time)
            break;
        Time -= Pntr->Time;
        Pntr++;
    }
    if (Pntr->Time) {
        StillDataDualShock = 1;
        return Pntr->Point + ((Pntr[1].Point - Pntr->Point) * Time) / Pntr->Time;
    }
    if (Time)
        return 0;
    return Pntr->Point;
}

extern char *MissionSpecificMemory;
extern struct MISSIONEVENT *MissionEvents;
extern void (*MissionPrintStatus)();
extern int Mission;
void MyFree(void **Addr);
void InitMissionBase(void);
void InitMissionJail(void);
void InitMissionPower(void);
void InitMissionGrav(void);
void InitMissionBoss(void);
void InitMissionTribute(void);

extern struct STARTPOSITION ReloadStartPosition;

/* DrawBorder: defined after its caller DrawDarkenedBox so it is not inlined there */
#define setlen(p, _len) (*(((unsigned char *)(p)) + 3) = (unsigned char)(_len))
#define setcode(p, _code) (*(((unsigned char *)(p)) + 7) = (unsigned char)(_code))
#define getcode(p) (unsigned char)(*(((unsigned char *)(p)) + 7))
#define setPolyFT4(p) setlen(p, 9), setcode(p, 0x2c)
#define setShadeTex(p, tge) ((tge) ? setcode(p, getcode(p) | 1) : setcode(p, getcode(p) & ~1))
#define setXYWH(p, _x0, _y0, _w, _h) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), \
    (p)->x1 = (_x0) + (_w), (p)->y1 = (_y0), \
    (p)->x2 = (_x0), (p)->y2 = (_y0) + (_h), \
    (p)->x3 = (_x0) + (_w), (p)->y3 = (_y0) + (_h)
#define setXY4(p, _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x1), (p)->y1 = (_y1), \
    (p)->x2 = (_x2), (p)->y2 = (_y2), (p)->x3 = (_x3), (p)->y3 = (_y3)
#define setUVWH(p, _u0, _v0, _w, _h) \
    (p)->u0 = (_u0), (p)->v0 = (_v0), \
    (p)->u1 = (_u0) + (_w), (p)->v1 = (_v0), \
    (p)->u2 = (_u0), (p)->v2 = (_v0) + (_h), \
    (p)->u3 = (_u0) + (_w), (p)->v3 = (_v0) + (_h)

extern short tpscreenx[];
extern short tpscreeny[];
extern short palette[];
unsigned short GetTPage(int tp, int abr, int x, int y);

/* GameCheckPaused */
int Pauser;
short Console_Flags;
extern int ClearingNecessary;
extern short SceneValid;
extern int LastLevelComplete;
extern short targetcdvol;
int SnapShotPadViolation(short Port, char *PortId);
void InGame_GOTO_WANT_TO_SAVE(void);
void InGame_GOTO_REINSERT_CONTROLLER(int Violation, int PortId, int Win);
void InGame_GOTO_PAUSED(void);
void InGame_GOTO_RESUME(void);
void killallsounds(void);
void HelpComputer_StopDisplay(int a);

/* The original keeps the two `return Paused` tails after InGame_GOTO_REINSERT_CONTROLLER and
   InGame_GOTO_WANT_TO_SAVE as separate `lw v0,Paused; j epilogue`, instead of cross-jumping them
   into the final `return Paused` (jump.c find_cross_jump, which compares SYMBOL_REF names by
   pointer). Only a second SYMBOL_REF for Paused with its own name string does that: an asm-label
   alias of the same symbol. Plain C forms (inline GameIsPaused(), volatile, block-scope extern,
   temporaries, gotos) all get merged. */
extern int PausedSym2 __asm__("Paused");

/* GameInitStuff */
extern struct LEVELDATA *ThisLevelData;
extern char GlobalPerFrameMemory[];
extern char *GameEveryFrameMemory;
extern char *GameEveryFrameLast;
extern void (*MainLoop)();
void InitBlurPoints(void);
void SetupScreen1by1(void (*PanelRoutine)());
void SetupScreen1by2(void (*PanelRoutine)());
void SetupScreen2by1(void (*PanelRoutine)());
void SetupScreen2by2(void (*PanelRoutine)());
void ProcessQuakePanel1by1();
void ProcessQuakePanel1by2();
void ProcessQuakePanel2by1();
void ProcessQuakePanel2by2();
void MoveQuakeCamera();
void MallocEdictTList(int NumEdicts, int NumClients);
void MallocPathTList(int Num);
void ResetAllCameras(void);
void Init_PickupSounds(void);
void QuakeLoop();

static void InitGameWindowMemory(int NumEdicts, short NumClientT, int NumPaths)
{
    int a;

    GameEveryFrameMemory = GlobalPerFrameMemory;
    GameEveryFrameLast = GlobalPerFrameMemory + 0xc00;
    for (a = 0; a < MaxWindows; a++) {
        GameWindows[a].Items.Health = 0;
        GameWindows[a].FadeTime = 0;
    }
    MallocEdictTList(NumEdicts, NumClientT);
    MallocPathTList(NumPaths);
}

int QuakeNextDemoLevel(void)
{
    dprintf(0, "@@@@c:\\Log%d.Txt", LevelChange);
    if (GameRunningMode == 1)
        LevelComplete = 8;
    else
        LevelComplete = 6;
}

int QuakeEndSelfPlay(int Win, int Manual)
{
    int a;

    if (Manual) {
        for (a = 0; a < MaxWindows; a++)
            SelfPlayRecOff(a);
        if (GameRunningMode == 1)
            GameRunningMode = 2;
        return 1;
    }
    QuakeNextDemoLevel();
    return 0;
}

void InitialiseSingleGame(void)
{
    {
        struct LEVELDATA *LevelData;
        int e;

        if (*(int *)DirectAccessLevelName.Name) {
            {
                struct LEVELDATA *LevelData;

                LevelData = FindLevelData(DirectAccessLevelName);
                if (LevelData)
                    NextMission = LevelData->MissionIdent;
                else
                    NextMission = 0;
            }
            NextLevelName = DirectAccessLevelName;
            MainStartName = DirectAccessZoneName;
            DirectAccessLevelName = IDNAME("");
        } else {
            NextMission = 1;
            NextLevelName = IDNAME("");
        }
        GlobalKey = 0;
        LevelData = QuakeLevelData;
        while (1) {
            if (!LevelData->Name.Name[0])
                break;
            for (e = 0; e < 8; e++)
                LevelData->EventStates[e] = 0;
            LevelData++;
        }
        DeleteAllMatchingMemory(3);
    }
}

int GameIsPaused(void)
{
    return Paused;
}

int GameCheckPaused(void)
{
    struct WINDOW *Window;
    static int OldPaused;
    static int OldOldPaused;
    int a;
    static int OldLevel = -2;
    char dummyPortId;
    short Violation;

    if (Paused)
        Console_Flags |= 0x10;
    else
        Console_Flags &= ~0x10;
    if (OldOldPaused || OldPaused || Paused)
        ClearingNecessary = 1;
    OldOldPaused = OldPaused;
    OldPaused = Paused;
    for (a = 0; a < MaxWindows; a++) {
        short configSelected;

        switch (GameWindows[a].Input.Method) {
        case 9:
        case 10:
            configSelected = (signed char)Engine.GameConfig.MultiPlayerParams[a].mouseConfigSelected;
            break;
        case 5:
        case 7:
            configSelected = (signed char)Engine.GameConfig.MultiPlayerParams[a].analogConfigSelected;
            break;
        case 4:
        case 6:
        case 8:
        default:
            configSelected = (signed char)Engine.GameConfig.MultiPlayerParams[a].standardConfigSelected;
            break;
        }
        Engine.GameConfig.MultiPlayerParams[a].controllerSelected = configSelected;
    }
    if (Engine.CantPause || !SceneValid) {
        Paused = 0;
        return 0;
    }
    if (Paused && Engine.CantUnPause)
        return Engine.CantUnPause;
    for (a = 0; a < MaxWindows; a++) {
        Violation = SnapShotPadViolation(a, &dummyPortId);
        if (Violation) {
            targetcdvol = 0;
            Paused = 1;
            Pauser = a;
            killallsounds();
            InGame_GOTO_REINSERT_CONTROLLER(Violation, dummyPortId, a);
            return PausedSym2;
        }
    }
    if (LevelChange != OldLevel && !LevelIsMultiPlayer) {
        OldLevel = LevelChange;
        if (LastLevelComplete == 2 || LastLevelComplete == 4) {
            targetcdvol = 0;
            Paused = 1;
            Pauser = 0;
            InGame_GOTO_WANT_TO_SAVE();
            return PausedSym2;
        }
    }
    if (!Paused) {
        for (a = 0; a < MaxWindows; a++) {
            Window = &GameWindows[a];
            if ((Window->Input.Padd & 8) && !(Window->Input.OPadd & 8)) {
                targetcdvol = 0;
                Paused = 1;
                Pauser = a;
                InGame_GOTO_PAUSED();
                return 1;
            }
        }
    } else {
        Window = &GameWindows[Pauser];
        if ((Window->Input.Padd & 8) && !(Window->Input.OPadd & 8)) {
            if (Engine.currentscreen >= 16 && Engine.currentscreen <= 18)
                return Paused;
            Paused = 0;
            HelpComputer_StopDisplay(0);
            InGame_GOTO_RESUME();
        }
    }
    return Paused;
}

void DrawBorder(struct RECT Rect, long *Ot, int OffsetY)
{
    struct POLY_FT4 *polft4;
    short tpage;
    short clut;

    polft4 = currdb->PausedPolys;
    tpage = GetTPage(0, 1, tpscreenx[13], tpscreeny[13]);
    clut = palette[68];

#define BORDERPOLY(u, v, uw, vh, x, y, w, h) \
    setPolyFT4(polft4); \
    setShadeTex(polft4, 1); \
    polft4->clut = clut; \
    polft4->tpage = tpage; \
    setUVWH(polft4, u, v, uw, vh); \
    setXYWH(polft4, x, y, w, h); \
    AddPrim_(Ot, polft4++)

    BORDERPOLY(32, 224, 46, 29, Rect.x - 10, Rect.y - 6 + OffsetY, 46, 29);
    BORDERPOLY(32, 224, 46, 29, Rect.x + Rect.w + 10, Rect.y - 6 + OffsetY, -46, 29);
    BORDERPOLY(32, 223, 46, 29, Rect.x + Rect.w + 10, Rect.y + Rect.h + 6 + OffsetY, -46, -30);
    BORDERPOLY(32, 223, 46, 30, Rect.x - 10, Rect.y + Rect.h + 6 + OffsetY, 46, -30);
    BORDERPOLY(112, 247, 19, 6, Rect.x + 36, Rect.y - 6 + OffsetY, Rect.w - 70, 7);
    BORDERPOLY(112, 247, 19, 6, Rect.x + 36, Rect.y + Rect.h + OffsetY, Rect.w - 70, 7);
    BORDERPOLY(81, 241, 11, 14, Rect.x - 10, Rect.y + 22 + OffsetY, 12, Rect.h - 44);
    BORDERPOLY(81, 241, 11, 14, Rect.x + Rect.w - 1, Rect.y + 22 + OffsetY, 11, Rect.h - 44);
}

void DrawDarkenedBox(struct RECT Rect)
{
    struct TILE *Tile;
    struct DR_TPAGE *DrTPageS;
    struct DR_TPAGE *DrTPageO;
    long *Ot;
    short a;

    gDarkBoxRect = Rect;
    Tile = currdb->PausedTile;
    DrTPageS = &currdb->PausedDrTPageToSemi;
    DrTPageO = &currdb->PausedDrTPageToOpaque;
    ((unsigned char *)DrTPageS)[3] = 1;
    DrTPageS->code[0] = 0xe1000600;
    ((unsigned char *)DrTPageO)[3] = 1;
    DrTPageO->code[0] = 0xe1000600;
    DrawBorder(Rect, (long *)&currdb->BigOt[10], 0);
    Ot = (long *)&currdb->BigOt[4];
    AddPrim_(Ot, DrTPageO);
    for (a = 0; a < 2; a++) {
        SetTile(Tile);
        Tile->b0 = 0;
        Tile->g0 = 0;
        Tile->r0 = 0;
        Tile->code |= 2;
        Tile->x0 = Rect.x;
        Tile->y0 = Rect.y;
        Tile->w = Rect.w;
        Tile->h = Rect.h;
        AddPrim_(Ot, Tile);
        Tile++;
    }
    AddPrim_(Ot, DrTPageS);
}

void GameDisplayPausedMenu(void)
{
    struct RECT Rect = { 0x2e, 0x10, 0x1a4, 0xd0 };

    DoFrontEndStuff();
    DrawDarkenedBox(Rect);
}

void InitMissionNull(void)
{
    MissionSpecificMemory = 0;
    Engine.MissionData = 0;
    MissionEvents = 0;
    MissionPrintStatus = 0;
    GameHubLoadSaveData = 0;
    PrintIdentityName(DirectAccessLevelName);
    if (*(int *)&DirectAccessLevelName)
        NextLevelName = DirectAccessLevelName;
    NextMission = 1;
}

void InitMissionNull(void);

void GameNewMission(void)
{
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
    if (NewMission) {
        NextLevelName = IDNAME("");
        DeleteAllMatchingMemory(1);
    }
    MyFree((void **)&MissionSpecificMemory);
    if (!NextMission)
        InitialiseSingleGame();
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
    MissionEvents = 0;
    MissionPrintStatus = 0;
    Mission = NextMission;
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
    switch (Mission) {
    case 1:
        InitMissionBase();
        break;
    case 2:
        InitMissionJail();
        break;
    case 3:
        InitMissionPower();
        break;
    case 4:
        InitMissionGrav();
        break;
    case 5:
        InitMissionBoss();
        break;
    case 6:
        InitMissionTribute();
        break;
    case 7:
    default:
        InitMissionNull();
        break;
    }
}

void GameInitStuff(void)
{
    int a;
    int OldsNumPlayers;

    InitBlurPoints();
    OldsNumPlayers = Engine.GameConfig.sNumPlayers;
    if (!LevelIsMultiPlayer) {
        if (ThisLevelData->MaxPlayers)
            Engine.GameConfig.sNumPlayers = 1;
        else
            Engine.GameConfig.sNumPlayers = 0;
    }
    switch (Engine.GameConfig.sNumPlayers) {
    case 0:
        SetupScreen1by1(0);
        InitGameWindowMemory(0, 0, 0);
        /* Original allocation: the %hi(GameWindows+0x12C) pseudo gets $v0 ahead of the
           MoveQuakeCamera pair ($v1), the reverse of case 1, so the two tails don't
           cross-jump. That needs the high part to weigh more in local-alloc's priority
           (refs weighted by loop depth): computing the address inside nested
           do{}while(0)s does it; the original's actual form is unknown. */
        {
            void (**p)();

            do { do { do { do {
                p = &GameWindows[0].CameraRoutine;
            } while (0); } while (0); } while (0); } while (0);
            *p = MoveQuakeCamera;
        }
        break;
    case 1:
        SetupScreen1by1(ProcessQuakePanel1by1);
        InitGameWindowMemory(64, MaxWindows, 32);
        GameWindows[0].CameraRoutine = MoveQuakeCamera;
        break;
    case 2:
        if (Engine.GameConfig.bSplitHoriz)
            SetupScreen1by2(ProcessQuakePanel1by2);
        else
            SetupScreen2by1(ProcessQuakePanel2by1);
        InitGameWindowMemory(0, 0, 0);
        for (a = 0; a < MaxWindows; a++)
            GameWindows[a].CameraRoutine = MoveQuakeCamera;
        break;
    case 3:
        SetupScreen2by2(ProcessQuakePanel2by2);
        MaxWindows = 3;
        InitGameWindowMemory(0, 0, 0);
        for (a = 0; a < MaxWindows; a++)
            GameWindows[a].CameraRoutine = MoveQuakeCamera;
        break;
    case 4:
        SetupScreen2by2(ProcessQuakePanel2by2);
        InitGameWindowMemory(0, 0, 0);
        for (a = 0; a < MaxWindows; a++)
            GameWindows[a].CameraRoutine = MoveQuakeCamera;
        break;
    default:
        goto end; /* skips the shared tail below, straight to the restore */
    }
    ResetAllCameras();
    Init_PickupSounds();
    MainLoop = QuakeLoop;
end:
    Engine.GameConfig.sNumPlayers = OldsNumPlayers;
}

void GameTerminateStuff(void)
{
    FreePathTList();
    FreeEdictTList();
}

void PreInitNotNeeded(void)
{
}

struct LEVELDATA *GetLevelDataFromIdentityName(struct IDENTITYNAME *Name)
{
    struct LEVELDATA *Details;

    for (Details = QuakeLevelData; *(int *)Details->Name.Name; Details++) {
        if (ModNameCmp(&Details->Name, Name) == 1) {
            PrintIdentityName(*Name);
            return Details;
        }
    }
    PrintIdentityName(*Name);
    return 0;
}

char GetCurrentLevelNumber(void)
{
    return ThisLevelData - QuakeLevelData;
}

void GameEveryFrame(void)
{
    NextQuakeLine = &currdb->GameStuff->QuakeLines[0];
    LastQuakeLine = &currdb->GameStuff->QuakeLines[4];
}

void RemoveVram(void);

void LoadInGameVram(void)
{
    DownloadTextures();
    LoadPicture256("control.lbm", 10);
    LoadPicture256("mouse.lbm", 6);
    LoadPicture256("multipics.lbm", 8);
    LoadPicture256("multipic2.lbm", 12);
    loadtp16b_ConCat("frontend.lbm", 13, 0);
    loadtp16b_ConCat("Squiggle.lbm", 12, 159);
    loadtp16b_ConCat("chars.lbm", 15, 128);
    if (LevelIsMultiPlayer) {
        if (Engine.GameConfig.sNumPlayers >= 3)
            loadtp16b_ConCat("qkm_menu.lbm", 14, 0);
        else
            loadtp16b_ConCat("qk2_menu.lbm", 14, 0);
    } else
        loadtp16b_ConCat("qk_menu.lbm", 14, 0);
    loadtp16b_ConCat("EndDemo1.lbm", 2, 0);
    loadtp16b_ConCat("FrDemo1.lbm", 2, 0);
    loadtp16b_ConCat("DemoSp1.lbm", 11, 0);
    loadtp16b_ConCat("DemoSp2.lbm", 12, 0);
    loadtp16b_ConCat("HamLogo.lbm", 0, 0);
    loadtp16b_ConCat("IdLogo.lbm", 4, 0);
    loadtp16b_ConCat("wipteam1.lbm", 4, 0);
    loadtp16b_ConCat("ActLogo.lbm", 8, 0);
    loadtp16b_ConCat("Legal.lbm", 4, 0);
    loadtp16b_ConCat("background3.lbm", 2, 0);
    loadtp16b_ConCat("Screena.lbm", 0, 0);
    loadtp16b_ConCat("Screenb.lbm", 2, 0);
    loadtp16b_ConCat("Globe0.lbm", 4, 0);
    loadtp16b_ConCat("Globe1.lbm", 5, 0);
    loadtp16b_ConCat("Globe2.lbm", 6, 0);
    loadtp16b_ConCat("Globe3.lbm", 7, 0);
    loadtp16b_ConCat("Globe4.lbm", 8, 0);
    loadtp16b_ConCat("Globe5.lbm", 9, 0);
    loadtp16b_ConCat("Globe6.lbm", 10, 0);
    loadtp16b_ConCat("Globe7.lbm", 11, 0);
    RemoveVram();
}

void GameLevelStuff(void)
{
    QEntsEveryLevel();
}

void SetupGameSpecific(void)
{
    GlobalLevelData = QuakeLevelData;
    db[0].GameStuff = &QuakeGameBuffs[0];
    db[1].GameStuff = &QuakeGameBuffs[1];
}

void GameSelfPlay(void)
{
    if (GameRunningMode) {
        NewMission = 0;
        NextMission = 0;
        DirectAccessLevelName = IDNAME("MagDemo");
        DirectAccessZoneName = IDNAME("SelfPlay0");
        InSelfPlayMode = 1;
        return;
    }
    if (!SelfPlayMapNamePntr)
        SelfPlayMapNamePntr = SelfPlayMapNames;
    NewMission = 0;
    NextMission = 0;
    DirectAccessLevelName = *SelfPlayMapNamePntr;
    DirectAccessZoneName = IDNAME("SelfPlay0");
    InSelfPlayMode = 1;
    if (*(int *)SelfPlayMapNamePntr++[1].Name == 0)
        SelfPlayMapNamePntr = SelfPlayMapNames;
}

void GameDemoExit(void)
{
    GameRunningMode = 3;
    NextLevelName = IDNAME("MagazineExtr");
    NewMission = 0;
    MainStartName = IDNAME("Default");
    InSelfPlayMode = 0;
}

void GameTitle(void)
{
    DirectAccessLevelName = IDNAME("QFront");
    NextMission = 0;
    NewMission = 0;
    NextMission = 0;
    DirectAccessZoneName = IDNAME("Default");
}

void GameMagDemoLevel(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("MagDemo");
    NextMission = 0;
    DirectAccessZoneName = IDNAME("Default");
    InSelfPlayMode = 0;
}

void GameMPlayerExit(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("MPResults");
    NextMission = 0;
    DirectAccessZoneName = IDNAME("Default");
}

void GameLogos(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("QLogos");
    DirectAccessZoneName = IDNAME("Default");
    NextMission = 0;
    LevelComplete = 6;
}

void GameLogos2(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("QLogos2");
    DirectAccessZoneName = IDNAME("Default");
    NextMission = 0;
    LevelComplete = 6;
}

void GameIntro(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("Intro FMV");
    DirectAccessZoneName = IDNAME("Default");
    NextMission = 0;
    LevelComplete = 6;
}

void GameExtro(void)
{
    NewMission = 1;
    DirectAccessLevelName = IDNAME("Extro FMV");
    DirectAccessZoneName = IDNAME("Default");
    NextMission = 0;
    LevelComplete = 6;
}

void GameReRunMPLevel(void)
{
    NextMission = 7;
    NewMission = 0;
}

void GameSetup(int GameType)
{
    NextMission = 0;
    LoadSaveGameConfigData(0);
    if (GameType == 0) {
        LevelIsMultiPlayer = 0;
        if (!InSelfPlayMode) {
            NewMission = 1;
            DirectAccessLevelName = IDNAME("");
        }
    } else {
        LevelIsMultiPlayer = 1;
        NewMission = 1;
    }
}

void GameNewGame(void)
{
    KillDualShocks();
    LevelComplete = 6;
}

void GameQuitGame(void)
{
    LevelComplete = 6;
    DeleteAllMatchingMemory(3);
}

void ReloadAutoSave(void)
{
    struct SAVEFILEHDR *FileHdr;
    struct LEVELDATA *ThisLevelData;

    {
        struct IDENTITYNAME SaveName;

        SaveName = *(struct IDENTITYNAME *)"PlayerSave0";
        ReloadStartPosition = *&(struct STARTPOSITION){{"Reload Start"}};
        FileHdr = OpenSaveMemory(SaveName, 0);
    }
    if (FileHdr) {
        struct QPLAYERSTATUS *PlayerStatus;

        PlayerStatus = (struct QPLAYERSTATUS *)(FileHdr + 1);
        ReloadStartPosition.Location = PlayerStatus->Location;
        ReloadStartPosition.Location.vy += 0x11e;
        ReloadStartPosition.AngY = PlayerStatus->Ang.vy;
        ReloadStartPosition.Zone = PlayerStatus->DestZoneName.Name[4] - '0';
        LevelComplete = 3;
        NextLevelName = PlayerStatus->DestLevelName;
        ThisLevelData = FindLevelData(NextLevelName);
        if (ThisLevelData)
            NextMission = ThisLevelData->MissionIdent;
        if (!PlayerStatus->DestStartName.Name[0])
            MainStartName = IDNAME("Reload Start");
        else
            MainStartName = PlayerStatus->DestStartName;
        PrintIdentityName(PlayerStatus->DestLevelName);
        PrintIdentityName(PlayerStatus->DestStartName);
        PrintIdentityName(PlayerStatus->DestZoneName);
        PrintIdentityName(MainStartName);
        VBLevelTicks = PlayerStatus->VBLevelTicks;
    }
}

void ReloadAutoSave(void);

void RestartLevel(int Resupply)
{
    struct SAVEFILEHDR *FileHdr;

    RestoreLevelStartState();
    {
        struct IDENTITYNAME SaveName;

        SaveName = *(struct IDENTITYNAME *)"PlayerSave0";
        FileHdr = OpenSaveMemory(SaveName, 0);
    }
    if (FileHdr && Resupply) {
        struct QPLAYERSTATUS *PlayerStatus;

        PlayerStatus = (struct QPLAYERSTATUS *)(FileHdr + 1);
        {
            int a;

            for (a = 2; a < 12; a++) {
                if (PlayerStatus->PlayerExtras.WeaponsAvailable & WeaponBits[a]) {
                    if (PlayerStatus->PlayerExtras.AmmoCount[AmmoTypes[a]] < MaxAmmo[AmmoTypes[a]] >> 1)
                        PlayerStatus->PlayerExtras.AmmoCount[AmmoTypes[a]] = MaxAmmo[AmmoTypes[a]] >> 1;
                }
            }
        }
        PlayerStatus->Health = PlayerStatus->PlayerExtras.Max_Health;
        if (PlayerStatus->PlayerExtras.Max_ArmourType != 1 && PlayerStatus->PlayerExtras.Max_ArmourType != 2) {
            PlayerStatus->PlayerExtras.Max_ArmourType = 0;
            PlayerStatus->PlayerExtras.ExtraBits &= ~0x4000;
            PlayerStatus->PlayerExtras.ExtraBits &= ~0x2000;
            PlayerStatus->PlayerExtras.ExtraBits |= 0x1000;
        }
        if (PlayerStatus->PlayerExtras.Armour_Count < Armour_Types[0].Max_Count)
            PlayerStatus->PlayerExtras.Armour_Count = Armour_Types[0].Max_Count;
        VBLevelTicks = PlayerStatus->VBLevelTicks;
    }
    ReloadAutoSave();
}

void KillDualShocks(void)
{
    int Win;
    int Slot;

    for (Win = 0; Win < MaxWindows; Win++)
        for (Slot = 0; Slot < 4; Slot++)
            GameWindows[Win].Input.ShockSlots[Slot].ShockData = 0;
}

void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power)
{
    int Slot;
    int Scale;
    int Win;
    int StartWin;
    int StopWin;
    int EmptySlot;
    struct SHOCKDATA *ShockData;

    if (InputWin == 4) {
        StartWin = 0;
        StopWin = MaxWindows;
    } else {
        StartWin = InputWin;
        StopWin = InputWin + 1;
    }
    for (Win = StartWin; Win < StopWin; Win++) {
        if (Location) {
            int DistX;
            int DistY;
            int DistZ;
            int Dist2;
            int Fall2;

            DistX = abs(Location->vx - GameWindows[Win].CamPos.vx);
            if (DistX > Power.Fall)
                continue;
            DistY = abs(Location->vy - GameWindows[Win].CamPos.vy);
            if (DistY > Power.Fall)
                continue;
            DistZ = abs(Location->vz - GameWindows[Win].CamPos.vz);
            if (DistZ > Power.Fall)
                continue;
            Dist2 = DistX * DistX + DistY * DistY + DistZ * DistZ;
            Fall2 = Power.Fall * Power.Fall;
            if (Dist2 > Fall2)
                continue;
            if (Dist2 > Power.Full * Power.Full) {
                Scale = InputScale * (Power.Fall - SquareRootNR(Dist2)) / (Power.Fall - Power.Full);
                if (Scale > InputScale)
                    Scale = InputScale;
                else if (Scale < 0)
                    Scale = 0;
            } else
                Scale = InputScale;
        } else
            Scale = InputScale;
        if (Scale <= 0x200)
            continue;
        ShockData = FindShockData(Ident);
        if (!ShockData)
            continue;
        if (ShockData->Flags & 1) {
            EmptySlot = 4;
            for (Slot = 0; Slot < 4; Slot++) {
                if (GameWindows[Win].Input.ShockSlots[Slot].ShockData->Ident == ShockData->Ident)
                    break;
                if (!GameWindows[Win].Input.ShockSlots[Slot].ShockData)
                    EmptySlot = Slot;
            }
            if (Slot == 4)
                Slot = EmptySlot;
        } else {
            for (Slot = 0; Slot < 4; Slot++)
                if (!GameWindows[Win].Input.ShockSlots[Slot].ShockData)
                    break;
        }
        if (Slot != 4) {
            GameWindows[Win].Input.ShockSlots[Slot].ShockData = ShockData;
            GameWindows[Win].Input.ShockSlots[Slot].Time = 0;
            GameWindows[Win].Input.ShockSlots[Slot].Scale = Scale;
        }
    }
}

struct CHARPAIR ProcessShockData(int Win)
{
    int Big;
    int Little;
    int Slot;
    struct CHARPAIR Return;

    Big = 0;
    Little = 0;
    for (Slot = 0; Slot < 4; Slot++) {
        if (GameWindows[Win].Input.ShockSlots[Slot].ShockData) {
            StillDataDualShock = 0;
            if (GameWindows[Win].Input.ShockSlots[Slot].ShockData->BigPntr)
                Big += (GetShockPoint(GameWindows[Win].Input.ShockSlots[Slot].ShockData->BigPntr, GameWindows[Win].Input.ShockSlots[Slot].Time) * GameWindows[Win].Input.ShockSlots[Slot].Scale) / 4096;
            if (GameWindows[Win].Input.ShockSlots[Slot].ShockData->LittlePntr)
                Little += (GetShockPoint(GameWindows[Win].Input.ShockSlots[Slot].ShockData->LittlePntr, GameWindows[Win].Input.ShockSlots[Slot].Time) * GameWindows[Win].Input.ShockSlots[Slot].Scale) / 4096;
            if (StillDataDualShock)
                GameWindows[Win].Input.ShockSlots[Slot].Time += PalNtscRate;
            else
                GameWindows[Win].Input.ShockSlots[Slot].ShockData = 0;
        }
    }
    if (Big > 0x1000)
        Big = 0x1000;
    Return.x = (Big * 255) / 4096;
    if (Little > 0x1000)
        Little = 0x1000;
    GameWindows[Win].Input.LittlePrecision += (Little * 255) / 4096;
    if (GameWindows[Win].Input.LittlePrecision > 0x80) {
        GameWindows[Win].Input.LittlePrecision -= 0x100;
        Return.y = 1;
    } else
        Return.y = 0;
    if (!GameWindows[Win].Creature || ((struct CREATURE *)GameWindows[Win].Creature)->Health <= 0)
        return (struct CHARPAIR){0, 0};
    return Return;
}
