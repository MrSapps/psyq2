#include "types.h"

/* defined in this file (Quake.obj), so $gp-relative */
int Paused;

int GameIsPaused(void)
{
    return Paused;
}

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

char GetCurrentLevelNumber(void)
{
    return ThisLevelData - QuakeLevelData;
}

void GameEveryFrame(void)
{
    NextQuakeLine = &currdb->GameStuff->QuakeLines[0];
    LastQuakeLine = &currdb->GameStuff->QuakeLines[4];
}

void GameLevelStuff(void)
{
    QEntsEveryLevel();
}

void GameTerminateStuff(void)
{
    FreePathTList();
    FreeEdictTList();
}

void GameNewGame(void)
{
    KillDualShocks();
    LevelComplete = 6;
}

void SetupGameSpecific(void)
{
    GlobalLevelData = QuakeLevelData;
    db[0].GameStuff = &QuakeGameBuffs[0];
    db[1].GameStuff = &QuakeGameBuffs[1];
}

void GameReRunMPLevel(void)
{
    NextMission = 7;
    NewMission = 0;
}

void GameQuitGame(void)
{
    LevelComplete = 6;
    DeleteAllMatchingMemory(3);
}

void KillDualShocks(void)
{
    int Win;
    int Slot;

    for (Win = 0; Win < MaxWindows; Win++)
        for (Slot = 0; Slot < 4; Slot++)
            GameWindows[Win].Input.ShockSlots[Slot].ShockData = 0;
}

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

int QuakeNextDemoLevel(void)
{
    dprintf(0, "@@@@c:\\Log%d.Txt", LevelChange);
    if (GameRunningMode == 1)
        LevelComplete = 8;
    else
        LevelComplete = 6;
}

void GameTitle(void)
{
    DirectAccessLevelName = IDNAME("QFront");
    NextMission = 0;
    NewMission = 0;
    NextMission = 0;
    DirectAccessZoneName = IDNAME("Default");
}

void GameDisplayPausedMenu(void)
{
    struct RECT Rect = { 0x2e, 0x10, 0x1a4, 0xd0 };

    DoFrontEndStuff();
    DrawDarkenedBox(Rect);
}

void PreInitNotNeeded(void)
{
}

void GameDemoExit(void)
{
    GameRunningMode = 3;
    NextLevelName = IDNAME("MagazineExtr");
    NewMission = 0;
    MainStartName = IDNAME("Default");
    InSelfPlayMode = 0;
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

extern int LevelIsMultiPlayer;
void LoadSaveGameConfigData(int Write);

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

extern int MaxWindows;
void SelfPlayRecOff(int Win);

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

extern struct IDENTITYNAME DirectAccessLevelName;
extern struct IDENTITYNAME NextLevelName;
extern int NextMission;
extern struct QENGINELEVINTERFACE Engine;
extern char *MissionSpecificMemory;
extern struct MISSIONEVENT *MissionEvents;
extern void (*MissionPrintStatus)();
extern void (*GameHubLoadSaveData)();
char *PrintIdentityName(struct IDENTITYNAME Name);

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

extern struct DB *currdb;
struct RECT gDarkBoxRect;
void DrawBorder(struct RECT Rect, long *Ot, int OffsetY);
void SetTile(struct TILE *Tile);
void AddPrim_(void *Ot, void *Prim);

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
