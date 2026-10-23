#include "types.h"

extern struct QENGINELEVINTERFACE Engine;
extern int TitleLoad;
extern char WaitingCutScene;
extern volatile char FinishedCutScene;
extern void (*VBlankSpecific)();
extern void (*GameHubLoadSaveData)();
struct LEVELDATA *GlobalLevelData;
extern char levelpath[];
char LevelPath[40];
int timeoutdisable;
unsigned char savelist[40];
int levellistpos;
extern int LevelCounterLast;
extern int LevelCounter;
extern int ChrisCount;
extern int ChrisCount1;
short saveenabled;
int cddemotimeout;
int cddemotimeout2;
char *CommonCreAiBinAddr; /* globals.json says QLevIfce.obj, but only loadlev uses it, $gp-relative */
struct FILEHDR *CommonFiles;
char *ZoneCreAiBinAddr;
struct FILEHDR *CurrentZoneFiles;
extern unsigned short *GamePals;
extern struct COLLHEADER CollMap;
extern struct SCENE2 *Scene2Data;
extern short *SceneFlashData;
extern struct LIGHTS *FixedLightData;
extern short *SceneModData;
extern unsigned char *SceneSortData;
extern struct MODELREFINDX *ModelRefIndx;
extern int LevelIsMultiPlayer;

char *strcpy();
void *memset();
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
void MyFreeRep(void **Addr, char *Name);
void DumpCreAi(char *Code);
void TurnOffScene(void);
void Event_KillTimers(void);
int RestoreEvents(void);
void RestartXA(void);
void InitLevel(void);
void SetupDrawDisplay3D(void);
void OnLeavingZone(void);

struct LEVELDATA *FindLevelData(struct IDENTITYNAME LevelName)
{
    struct LEVELDATA *LevelData;

    LevelData = GlobalLevelData;
    for (;;) {
        switch (ModNameCmp(&LevelName, &LevelData->Name)) {
        case 0:
            LevelData++;
            break;
        case 1:
            return LevelData;
        case 2:
            return 0;
        }
    }
}

void FadeOutLoading(void)
{
    if (!TitleLoad) {
        WaitingCutScene = 1;
        do {
        } while (!FinishedCutScene);
    }
    VBlankSpecific = 0;
}

void LoadLevel(void)
{
    timeoutdisable = 1;
    strcpy(levelpath, LevelPath);
    saveenabled = savelist[levellistpos];
    LevelCounterLast = 0;
    LevelCounter = 0;
    ChrisCount = 0;
    ChrisCount1 = -1;
    RestartXA();
    InitLevel();
    timeoutdisable = 0;
    cddemotimeout2 = 400;
    timeoutdisable = 0;
    cddemotimeout2 = cddemotimeout;
    SetupDrawDisplay3D();
}

void FreeScene(void)
{
    struct FILEDATA FileData;

    TurnOffScene();
    DumpCreAi(CommonCreAiBinAddr);
    Event_KillTimers();
    MyFreeRep((void **)&CommonFiles, "Common Files");
    DumpCreAi(ZoneCreAiBinAddr);
    MyFreeRep((void **)&CurrentZoneFiles, "Current Zone Files");
    MyFreeRep((void **)&GamePals, "Game Pal's");
    CollMap.CollMap3d = 0;
    Scene2Data = 0;
    SceneFlashData = 0;
    FixedLightData = 0;
    SceneModData = 0;
    SceneSortData = 0;
    ModelRefIndx = 0;
}

void OnLeavingLevel(void)
{
    OnLeavingZone();
}

void RestoreSavedData(void)
{
    if (!LevelIsMultiPlayer) {
        RestoreEvents();
        if (GameHubLoadSaveData)
            GameHubLoadSaveData(0);
    }
}

struct FILEDATA GetFileData(struct FILEHDR *BaseHdr, struct IDENTITYNAME Name)
{
    struct FILEHDR *FileHdr;
    char *Temp;

    FileHdr = BaseHdr;
    for (;;) {
        switch (ModNameCmp(&Name, &FileHdr->Name)) {
        case 0:
            FileHdr++;
            break;
        case 1:
            Temp = (char *)BaseHdr + FileHdr->Offset;
            return (struct FILEDATA){Temp, FileHdr[1].Offset - FileHdr->Offset};
        case 2:
            return (struct FILEDATA){0};
        }
    }
}

void SetFrontEndStartScreen(int StartScreen)
{
    Engine.FrontEndStartScreen = StartScreen;
}

int GetDifficultySetting(void)
{
    return Engine.GameConfig.sDifficulty;
}

void SetController(int Win, int Method)
{
    Engine.GameConfig.MultiPlayerParams[Win].controllerSelected = Method;
}

void NoLevelFunc(void)
{
}

extern struct QENGINELEVINTERFACE Engine;
extern int LevelCounterLast;
extern int LevelCounter;
extern int ChrisCount;
extern int ChrisCount1;
extern struct DB db[];
struct IDENTITYNAME ThisLevelName;
struct QLEVELINTERFACE *LevelInterface;
void (*LevelInitFunc)();
extern void (*LevelSpecific)();
extern struct GLOBMODHDR *CastList;
extern struct CREATURE Creatures[48];
extern struct GLOBMODHDR *NextCommonCastList;
extern struct GLOBMODHDR *NextZoneCastList;
extern short SceneValid;
extern char SkyClear;
extern short LevelComplete;
extern int Paused;
extern short quitgame;
extern struct GLOBMODHDR *CommonCastList;
extern struct GLOBMODHDR *ZoneCastList;
void *memset();
void MyFreeRep(void **Addr, char *Name);
struct LEVELDATA *GetLevelDataFromIdentityName(struct IDENTITYNAME *Name);
void QuakeFillInLevelEngineSideInterface(void);
void __SN_RELOCATE(unsigned int baseaddr, unsigned int *relocs);
char *PrintIdentityName(struct IDENTITYNAME Name);
void FreeWaterMoves(void);
void InitCreFreeList(void);
void LinkCastLists(void);
void InitAiLookupTables(void);
void RemapModels(void);

void LoadRelocateLevel(char *Code, char *Rel, int Size)
{
    struct LEVELDATA *Details;

    Details = GetLevelDataFromIdentityName(&ThisLevelName);
    QuakeFillInLevelEngineSideInterface();
    if (Details) {
        __SN_RELOCATE((unsigned int)Code, (unsigned int *)Rel);
        PrintIdentityName(Details->Name);
        LevelInterface = (struct QLEVELINTERFACE *)Code;
        LevelInitFunc = Details->LevelInitFunc = LevelInterface->InitLevFn;
        LevelInterface->Engine = &Engine;
    }
}

void FinishedLoading(void)
{
    LevelSpecific = 0;
    MyFreeRep((void **)&db[0].modpolsg4, "ModPols");
    MyFreeRep((void **)&db[0].PartFt4, "Particles");
    FreeWaterMoves();
    MyFreeRep((void **)&CastList, "CastList");
    memset(Creatures, 0, sizeof(Creatures));
    InitCreFreeList();
    SceneValid = 1;
    SkyClear = 0;
    LevelComplete = 0;
    Paused = 0;
    quitgame = 0;
    ChrisCount = 0;
    ChrisCount1 = -1;
    LevelCounterLast = 0;
    LevelCounter = 0;
    CommonCastList = NextCommonCastList;
    ZoneCastList = NextZoneCastList;
    LinkCastLists();
    InitAiLookupTables();
    RemapModels();
}

struct IDENTITYNAME ZoneName;
struct ZONEDATA *ZoneData;
struct ZONEDATA *ThisZoneData;
extern struct STARTPOSITION *StartPositions;
extern struct STARTPOSITION ReloadStartPosition;
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
char *PrintIdentityName(struct IDENTITYNAME Name);

#define IDNAME(s) ((struct IDENTITYNAME){s})

void SetCurrentZoneData(void)
{
    ThisZoneData = ZoneData;
    for (;;) {
        switch (ModNameCmp(&ZoneName, &ThisZoneData->ZoneName)) {
        case 0:
            ThisZoneData++;
            break;
        case 1:
            return;
        case 2:
            PrintIdentityName(ZoneName);
            return;
        }
    }
}

struct STARTPOSITION *FindStartPosition(struct IDENTITYNAME Name)
{
    struct STARTPOSITION *Position;
    static struct IDENTITYNAME Default = {"Default"};

    Position = StartPositions;
    if (ModNameCmp(&Name, &IDNAME("Reload Start")) == 1)
        return &ReloadStartPosition;
    for (;;) {
        switch (ModNameCmp(&Name, &Position->Name)) {
        case 0:
            Position++;
            break;
        case 1:
            return Position;
        case 2:
            PrintIdentityName(Name);
            if (ModNameCmp(&Name, &Default))
                for (;;)
                    ;
            return 0;
        }
    }
}

extern struct IDENTITYNAME MainStartName;
struct IDENTITYNAME NextZoneName;
extern struct DB *olddb;
extern void (*EndSpecific)();
struct LEVELDATA *ThisLevelData;
extern struct PLAYER Players[];
extern short HackyLevelObj;
extern int GlobalKey;
void (**OnZoneLoadTable)();
int DrawSync(int mode);
struct DISPENV *PutDispEnv(struct DISPENV *env);
void ReloadLevelData(struct IDENTITYNAME ThisZoneName);
void RestoreCreatures(struct IDENTITYNAME LevelName, struct IDENTITYNAME ZoneName);
void SaveCreatures(struct IDENTITYNAME LevelName, struct IDENTITYNAME ZoneName);
void LoadSaveCreatureBatches(struct IDENTITYNAME *SaveName, int Save);
int StoreEventStates(void);
void FinishDying(void);
void GamePlayerLoadSave(int PlayerIndx, int Save);
void KillDualShocks(void);
void OnLoadZone(void);

void OnLeavingZone(void)
{
    KillDualShocks();
    FinishDying();
    if (ThisLevelData->MaxPlayers && LevelComplete != 3) {
        if (GameHubLoadSaveData)
            GameHubLoadSaveData(1);
        SaveCreatures(ThisLevelName, ZoneName);
        LoadSaveCreatureBatches(0, 1);
        StoreEventStates();
        GamePlayerLoadSave(0, 1);
    }
    Players[0].TargetPitchSeek = 0;
}

void OnLoadZone(void)
{
    int ZoneNum;
    void (*Routine)();

    HackyLevelObj = -100;
    if (!LevelIsMultiPlayer) {
        RestoreCreatures(ThisLevelName, ZoneName);
        if (Creatures[0].Player)
            Creatures[0].Player->Extras.ExtraBits |= GlobalKey & 0xfff;
        if (OnZoneLoadTable) {
            ZoneNum = ZoneName.Name[4] - '0';
            Routine = OnZoneLoadTable[ZoneNum];
            if (Routine)
                Routine();
        }
    }
}

void LoadZone(void)
{
    DrawSync(0);
    PutDispEnv(&olddb->DispEnv);
    MainStartName = IDNAME("");
    OnLeavingZone();
    ReloadLevelData(NextZoneName);
    RestoreSavedData();
    OnLoadZone();
    RestartXA();
    EndSpecific = 0;
}

extern int MusicCheck;
extern int cdtime;
extern char PolsToFreeList[];
extern short *SubToFreeList;
extern char *PolsToFreeListPntr;
extern int ObjSortIndx;
extern short *SubToFreeListPntr;
extern unsigned int LevelTicks;
extern unsigned int OldLevelTicks;
struct FET_MESSAGE FET_LOADING[2];
struct FET_MESSAGE FET_NULL[1];
extern struct STRING_TABLE MyStringTable[];
void killallsounds(void);
void FreeAllModelPolys(void);
void ReallyFreePolyPackets(void);
void UnLinkCastLists(void);
void ReadZoneFiles(char *Path, struct IDENTITYNAME ThisZoneName);
void UpdateCamerasOnReload(void);
void AquireResources(void);
void ResetLightBackList(void);
void InGameInitScreen(short screen);
void FrontEndCreateMenu(struct FET_MESSAGE *fetMenu, short fontSize);
void DoFrontEndStuff(void);

static int ZoneChanged(struct IDENTITYNAME Name)
{
    return ModNameCmp(&Name, &ZoneName) != 1;
}

int MaybeLoadZoneName(struct IDENTITYNAME name)
{
    if (LevelIsMultiPlayer || !ZoneChanged(name))
        return 0;
    OldLevelTicks = LevelTicks;
    PrintIdentityName(ZoneName);
    PrintIdentityName(name);
    NextZoneName = name;
    EndSpecific = LoadZone;
    InGameInitScreen(0x2e);
    FrontEndCreateMenu(FET_LOADING, 16);
    FrontEndCreateMenu(FET_NULL, 16);
    MyStringTable[0].cursorItem = 1;
    DoFrontEndStuff();
    return 1;
}

void ReloadLevelData(struct IDENTITYNAME ThisZoneName)
{
    struct FILEDATA FileData;

    MusicCheck = 0;
    killallsounds();
    DumpCreAi(ZoneCreAiBinAddr);
    Event_KillTimers();
    FreeWaterMoves();
    MyFreeRep((void **)&CurrentZoneFiles, "Zone Files");
    TurnOffScene();
    FreeAllModelPolys();
    ReallyFreePolyPackets();
    UnLinkCastLists();
    MyFreeRep((void **)&db[0].modpolsg4, "ModPols");
    MyFreeRep((void **)&db[0].PartFt4, "Particles");
    ReadZoneFiles(levelpath, ThisZoneName);
    MusicCheck = 1;
    cdtime = 0;
    ZoneCastList = NextZoneCastList;
    LinkCastLists();
    RemapModels();
    UpdateCamerasOnReload();
    SetCurrentZoneData();
    AquireResources();
    ResetLightBackList();
    PolsToFreeListPntr = PolsToFreeList;
    ObjSortIndx = 0;
    SubToFreeListPntr = SubToFreeList;
}
