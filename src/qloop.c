#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void OnLeavingLevel(void); void QuakeLoop(void);
void QuakeInterMissionLoop(void);

/* defined in qloop.obj, so $gp-relative */
unsigned int usp;
static int IsPaused;

extern short ScreenShotOn;
extern int HelpComputerOn;
extern int QuakeLogicStartTime;
extern int QuakeLogicStopTime;
extern int TempTime;
extern int MoveModelTime;
extern void (*VBlankSpecific)();
extern int InSelfPlayMode;
extern void (*LevelSpecific)();
extern void (*LevelExtraDraw)();
extern int LightsTime;
extern int SortTime;
extern int DebugInfo;
extern short LevelComplete;

unsigned int SetSp(unsigned int sp);
long GetRCnt();
void ChrisInitEveryFrame(void);
int GameCheckPaused(void);
void GameDisplayPausedMenu(void);
void HelpComputer_DoDisplay(void);
void QuakeLogic(void);
void ProcessMoveBufs(void);
void CalcActiveAreaList(void);
void ProcessCreatureBatches(void);
void MoveModels(void);
void SpriteAnim(int Mode);
void ProcessSolidBoxs(void);
void ProcessSounds(void);
void DrawTelePorts(void);
void DrawLaserWalls(void);
void DrawLaserBeams(void);
void ProcessAllCameras(void);
void ProcessFlyingPickups(void);
void ProcessAllPanels(void);
void UndoMobileLights(void);
void DrawAllScenes(void);
void ProcessMissiles(void);
void ProcessAllLensFlare(void);
void ProcessParticles(void);
void DrawDebugBoxs(void);
void DrawAllModels(void);
void ProcessLights(void);
void ProcessPostDraw(void);
void ProcessBlurs(void);
void ProcessSplurges(void);
void ProcessThickLines(void);
void DrawAllSortedObjs(void);
void DrawSortedObjectsHeavens(void);
void FreeSubDivs(void);
void ReallyFreePolyPackets(void);
void PrintDebugInfoScene(int *DebugScreen);
void DrawPathCorners(void);
void ProcessMessageText(void);

extern volatile int DebugWatch;

void OnLeavingLevel(void);

void QuakeLoop(void)
{
    usp = SetSp(0x1f8003fc);
    ChrisInitEveryFrame();
    if (!ScreenShotOn)
        IsPaused = GameCheckPaused();
    if (IsPaused && !HelpComputerOn)
        GameDisplayPausedMenu();
    HelpComputer_DoDisplay();
    QuakeLogicStartTime = GetRCnt(0xf2000001);
    QuakeLogic();
    QuakeLogicStopTime = GetRCnt(0xf2000001);
    ProcessMoveBufs();
    CalcActiveAreaList();
    ProcessCreatureBatches();
    TempTime = GetRCnt(0xf2000001);
    MoveModels();
    MoveModelTime = GetRCnt(0xf2000001) - TempTime;
    if (!VBlankSpecific && InSelfPlayMode)
        SpriteAnim(1);
    ProcessSolidBoxs();
    ProcessSounds();
    DrawTelePorts();
    DrawLaserWalls();
    DrawLaserBeams();
    if (LevelSpecific)
        LevelSpecific();
    ProcessAllCameras();
    ProcessFlyingPickups();
    ProcessAllPanels();
    UndoMobileLights();
    DrawAllScenes();
    ProcessMissiles();
    ProcessAllLensFlare();
    ProcessParticles();
    DrawDebugBoxs();
    DrawAllModels();
    TempTime = GetRCnt(0xf2000001);
    ProcessLights();
    LightsTime += GetRCnt(0xf2000001) - TempTime;
    ProcessPostDraw();
    ProcessBlurs();
    ProcessSplurges();
    if (LevelExtraDraw)
        LevelExtraDraw();
    ProcessThickLines();
    TempTime = GetRCnt(0xf2000001);
    DrawAllSortedObjs();
    DrawSortedObjectsHeavens();
    SortTime = GetRCnt(0xf2000001) - TempTime;
    FreeSubDivs();
    ReallyFreePolyPackets();
    PrintDebugInfoScene(&DebugInfo);
    DrawPathCorners();
    ProcessMessageText();
    if (LevelComplete)
        OnLeavingLevel();
    SetSp(usp);
}

void QuakeInterMissionLoop(void)
{
    DebugWatch = 99;
    DebugWatch = 100;
    usp = SetSp(0x1f8003fc);
    ChrisInitEveryFrame();
    DebugWatch = 2;
    DebugWatch = 1;
    DebugWatch = 3;
    QuakeLogicStartTime = GetRCnt(0xf2000001);
    DebugWatch = 4;
    QuakeLogic();
    DebugWatch = 5;
    QuakeLogicStopTime = GetRCnt(0xf2000001);
    ProcessMoveBufs();
    DebugWatch = 6;
    CalcActiveAreaList();
    DebugWatch = 0x74;
    ProcessCreatureBatches();
    DebugWatch = 7;
    TempTime = GetRCnt(0xf2000001);
    MoveModels();
    MoveModelTime = GetRCnt(0xf2000001) - TempTime;
    ProcessSolidBoxs();
    DebugWatch = 9;
    ProcessSounds();
    DebugWatch = 10;
    DebugWatch = 11;
    DebugWatch = 12;
    ProcessAllCameras();
    DebugWatch = 13;
    ProcessMissiles();
    DebugWatch = 14;
    DebugWatch = 0x73;
    UndoMobileLights();
    DebugWatch = 15;
    DrawAllScenes();
    DebugWatch = 16;
    ProcessAllLensFlare();
    DebugWatch = 17;
    ProcessParticles();
    DebugWatch = 18;
    DebugWatch = 19;
    DrawDebugBoxs();
    DebugWatch = 20;
    DrawAllModels();
    TempTime = GetRCnt(0xf2000001);
    ProcessLights();
    LightsTime += GetRCnt(0xf2000001) - TempTime;
    DebugWatch = 21;
    ProcessPostDraw();
    DebugWatch = 22;
    ProcessBlurs();
    DebugWatch = 23;
    ProcessSplurges();
    DebugWatch = 24;
    DebugWatch = 25;
    if (LevelExtraDraw)
        LevelExtraDraw();
    ProcessThickLines();
    DebugWatch = 25;
    TempTime = GetRCnt(0xf2000001);
    DebugWatch = 26;
    DrawAllSortedObjs();
    DebugWatch = 27;
    DrawSortedObjectsHeavens();
    DebugWatch = 28;
    SortTime = GetRCnt(0xf2000001) - TempTime;
    DebugWatch = 29;
    FreeSubDivs();
    DebugWatch = 30;
    ReallyFreePolyPackets();
    DebugWatch = 31;
    PrintDebugInfoScene(&DebugInfo);
    DebugWatch = 32;
    DrawPathCorners();
    DebugWatch = 33;
    ProcessMessageText();
    DebugWatch = 35;
    SetSp(usp);
    DebugWatch = 36;
}
