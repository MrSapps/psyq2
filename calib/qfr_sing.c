#include "types.h"

extern struct BOTTOMPOLY bottomPoly[];
extern struct QENGINELEVINTERFACE Engine;
extern short targetcdvol;

/* defined in QFr_Sing.Obj, so $gp-relative */
short WaitTime;
char gViolatingPlayer;

short SnapShotPadViolation(short winNum, char *portId);
void killallsounds(void);
void Config_InstallVideo(void);
void InGame_GOTO_PAUSEDBACK(void);
void InGame_GOTO_PAUSED(void);
void InGame_GOTO_DIED(void);
void CHECK_FOR_SLIDER(void);
void CHECK_FOR_MCFILEWINDOW(void);

extern struct QENGINELEVINTERFACE Engine;
extern struct CURSOR_MEMORY CursorMemory[];
void FrontEndInitText(void);
void MoveBottomPoly(short polyNum, short y);

void InGameInitScreen(short screen)
{
    short i;

    CursorMemory[Engine.currentscreen].cursorpos = Engine.cursorpos;
    Engine.gFrontEndPrevScreen = Engine.currentscreen;
    Engine.currentscreen = screen;
    Engine.cursorupper = 0;
    Engine.cursorlower = 0;
    Engine.maxcursor = 0;
    FrontEndInitText();
    Engine.Backfunc = 0;
    Engine.Specialfunc = 0;
    for (i = 0; i < 3; i++) {
        MoveBottomPoly(i, 0x104);
        if (Engine.currentscreen == 0x2e)
            bottomPoly[i].y = bottomPoly[i].idealY;
    }
    Engine.gFrontEndTimeout = 0;
}

extern struct STRING_TABLE MyStringTable[];
void FrontEndAddText(struct STRING_TABLE *fetString, char *message, short x, short y, int fontSize, unsigned short window);

void FrontEndCreateMenu(struct FET_MESSAGE *fetMenu, short fontSize)
{
    short index;

    index = Engine.cursorupper;
    Engine.cursorlower = index;
    Engine.cursorpos = index;
    while (1) {
        if (!fetMenu->menuName)
            break;
        FrontEndAddText(&MyStringTable[index], fetMenu->menuName, fetMenu->x, fetMenu->y, fontSize, 0);
        MyStringTable[index].bannerColour = 0;
        MyStringTable[index].selectFunc = fetMenu->func;
        MyStringTable[index].pSliderVar = fetMenu->pSlider;
        MyStringTable[index].pBoolVar = fetMenu->pVar;
        MyStringTable[index].onRelease = fetMenu->onRelease;
        MyStringTable[index].dontWrap = 0;
        MyStringTable[index].dontDraw = 0;
        index++;
        fetMenu++;
    }
    Engine.maxcursor = index;
    Engine.cursorupper = index;
}

void MoveBottomPoly(short polyNum, short y)
{
    bottomPoly[polyNum].idealY = y - 8;
}

void FrontEndResetVideoDefaults(void)
{
    Engine.GameConfig.bSplitHoriz = 1;
    Engine.GameConfig.sScreenPosX = 0;
    Engine.GameConfig.sScreenPosY = 0;
    Config_InstallVideo();
}

void FrontEndResetSoundDefaults(void)
{
    Engine.GameConfig.bStereo = 1;
    Engine.GameConfig.sMusic = 0x30;
    Engine.GameConfig.sSoundFX = 0x60;
    targetcdvol = 0x60;
}

void CHECK_FOR_SLIDERANDSCROLL(void)
{
    CHECK_FOR_SLIDER();
    CHECK_FOR_MCFILEWINDOW();
}

void FrontEndCheckForReinsert(void)
{
    char dummyPortId;

    if (!SnapShotPadViolation(gViolatingPlayer, &dummyPortId))
        InGame_GOTO_PAUSED();
}

void InGame_GOTO_PAUSED(void)
{
    killallsounds();
    InGame_GOTO_PAUSEDBACK();
}

void InGame_GOTO_DIED2(void)
{
    InGame_GOTO_DIED();
    WaitTime = 600;
}

extern struct WINDOW GameWindows[];
extern struct STRING_TABLE MyStringTable[];
extern struct CURSOR_MEMORY CursorMemory[];
extern int LevelIsMultiPlayer;
extern int Paused;
extern struct FET_MESSAGE FET_INGAME_RESTART[];
extern struct FET_MESSAGE FET_INGAME_RESTART_YESNO[];
extern struct FET_MESSAGE FET_INGAME_RESTARTING[];
extern struct FET_MESSAGE FET_EMPTYDIALOG2[];
extern struct FET_MESSAGE FET_INGAME_OPTIONS[];
extern struct FET_MESSAGE FET_INGAME_PLAYER[];
extern struct FET_MESSAGE FET_INGAME_SOUND[];
extern struct FET_MESSAGE FET_LOADSAVE_WANTTOSAVE2[];
extern struct FET_MESSAGE FET_LOADSAVE_WANTTOSAVE_YESNO2[];

void InGameInitScreen(short screen);
void FrontEndCreateMenu(struct FET_MESSAGE *fetMenu, short fontSize);
void FrontEndAddPausedTitle(char *Title);
void FrontEndLevelCompleteTimeout(void);
void InGame_CHECK_FOR_SAVE(void);
void InGame_GOTO_OPTIONS2(void);
void InGame_GOTO_RESTART(void);
void InGame_GOTO_RESUPPLY_CONFIRM(void);
void InGame_GOTO_QUIT_CONFIRM(void);
void PatchEngineVariables(int IsMultiPlayer);
void CHECK_FOR_SCREEN_SPLIT(void);
int sprintf(char *, const char *, ...);

struct LOADSAVEBITS { unsigned int pad : 24; unsigned int slot : 7; unsigned int top : 1; };

void FrontEndModifyText(struct STRING_TABLE *fetString, char *message)
{
    sprintf(fetString->originalMessage, "%s", fetString->message);
    sprintf(fetString->message, "%s", message);
}

short FrontEndRestoreCursor(void)
{
    if (!CursorMemory[Engine.currentscreen].firsttime)
        return CursorMemory[Engine.currentscreen].cursorpos;
    CursorMemory[Engine.currentscreen].firsttime = 0;
    return Engine.cursorpos;
}

void FrontEndClearCursorMemory(void)
{
    short i;

    for (i = 0; i < 47; i++) {
        CursorMemory[i].cursorpos = 0;
        CursorMemory[i].firsttime = 1;
    }
}

int GetsMusic(void)
{
    return Engine.GameConfig.sMusic;
}

short FrontEndGetMessageLength(char *pMessage)
{
    short count = 0;

    while (*pMessage) {
        if (*pMessage != 'b' && *pMessage != 'd' && *pMessage != 'u' && *pMessage != 'g')
            count++;
        pMessage++;
    }
    return count;
}

void InGame_GOTO_NOWHERE(void)
{
}

void InGame_GOTO_OPTIONS(void)
{
    InGameInitScreen(0x1b);
    FrontEndAddPausedTitle("OPTIONS");
    FrontEndCreateMenu(FET_INGAME_OPTIONS, 0x10);
    Engine.Backfunc = InGame_GOTO_PAUSEDBACK;
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

void InGame_GOTO_RESTART(void)
{
    InGameInitScreen(0x21);
    FrontEndCreateMenu(FET_INGAME_RESTARTING, 0x10);
    FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
    Engine.gFrontEndLevelComplete = 0x15;
    Engine.gFrontEndTimeout = 0xc;
    Engine.Specialfunc = FrontEndLevelCompleteTimeout;
}

void InGame_GOTO_RESTART_CONFIRM(void)
{
    InGameInitScreen(0x20);
    FrontEndCreateMenu(FET_INGAME_RESTART, 0x10);
    FrontEndCreateMenu(FET_INGAME_RESTART_YESNO, 0x10);
}

void InGame_GOTO_RESUPPLY(void)
{
    InGameInitScreen(0x23);
    FrontEndCreateMenu(FET_INGAME_RESTARTING, 0x10);
    FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
    Engine.gFrontEndLevelComplete = 0x14;
    Engine.gFrontEndTimeout = 0xc;
    Engine.Specialfunc = FrontEndLevelCompleteTimeout;
    Engine.GameConfig.cContinues--;
}

void InGame_GOTO_RESUME(void)
{
    Engine.CantUnPause = 0;
    if (LevelIsMultiPlayer) {
        PatchEngineVariables(1);
        CHECK_FOR_SCREEN_SPLIT();
    } else {
        PatchEngineVariables(0);
    }
    Engine.gFrontEndPad[3] = 0;
    Engine.gFrontEndPad[2] = 0;
    Engine.gFrontEndPad[1] = 0;
    Engine.gFrontEndPad[0] = 0;
    Paused = 0;
    targetcdvol = Engine.GameConfig.sMusic * 2;
}

void InGame_GOTO_PLAYER(void)
{
    InGameInitScreen(0x1c);
    FrontEndAddPausedTitle("PLAYER");
    FrontEndCreateMenu(FET_INGAME_PLAYER, 0x10);
    Engine.Backfunc = InGame_GOTO_OPTIONS;
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

void InGame_GOTO_SOUND(void)
{
    InGameInitScreen(0x1d);
    FrontEndAddPausedTitle("SOUND");
    FrontEndCreateMenu(FET_INGAME_SOUND, 0x10);
    Engine.Backfunc = InGame_GOTO_OPTIONS2;
    Engine.Specialfunc = CHECK_FOR_SLIDER;
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

void InGame_GOTO_WANT_TO_SAVE(void)
{
    InGameInitScreen(0x27);
    Engine.CantUnPause = 1;
    Engine.FrontEndCreateMenu(FET_LOADSAVE_WANTTOSAVE2, 0x10);
    Engine.FrontEndCreateMenu(FET_LOADSAVE_WANTTOSAVE_YESNO2, 0x10);
    Engine.Backfunc = 0;
}

void InGame_GOTO_SAVE(void)
{
    InGameInitScreen(0x27);
    Engine.gFrontEndLoadType = 1;
    Engine.Specialfunc = InGame_CHECK_FOR_SAVE;
    ((struct LOADSAVEBITS *)&Engine.gFrontEndPadShot)->slot = 1;
    Engine.MemoryCardSetStatus(1);
}

void INGAME_RESET_CHEATS(void)
{
    Engine.GameConfig.sBlastScale = 0x40;
    Engine.GameConfig.sGravity = 0x40;
    Engine.GameConfig.sGameSpeed = 0x40;
    Engine.GameConfig.sTimeLimit = 10;
    Engine.GameConfig.sFragLimit = 10;
    Engine.GameConfig.bWeaponStay = 0;
    Engine.GameConfig.bInfiniteAmmo = 0;
    Engine.GameConfig.bAllWeapons = 0;
    Engine.GameConfig.bFallingDamage = 1;
    Engine.GameConfig.bOneShotKill = 0;
}

void WAITTIMENOPAD(void)
{
    WaitTime -= *Engine.NewVcounts;
    if (WaitTime < 0) {
        WaitTime = 0;
        if (!GameWindows[0].Input.Padd) {
            MyStringTable[0].selectFunc = InGame_GOTO_RESTART;
            MyStringTable[1].selectFunc = InGame_GOTO_RESUPPLY_CONFIRM;
            MyStringTable[2].selectFunc = InGame_GOTO_QUIT_CONFIRM;
            Engine.Specialfunc = 0;
        }
    }
}

void FrontEndResetPlayerDefaults(void)
{
    int a;

    Engine.GameConfig.bCrossHairs = 1;
    Engine.GameConfig.bAutoCenter = 1;
    Engine.GameConfig.bStatusBar = 0;
    Engine.GameConfig.bAutoSave = 1;
    for (a = 0; a < 4; a++) {
        Engine.GameConfig.MultiPlayerParams[a].controllerSelected = 6;
        Engine.GameConfig.MultiPlayerParams[a].mouseConfigSelected = 0;
        Engine.GameConfig.MultiPlayerParams[a].analogConfigSelected = 4;
        Engine.GameConfig.MultiPlayerParams[a].standardConfigSelected = 6;
        Engine.GameConfig.MultiPlayerParams[a].mouseSensitivity = 0x40;
    }
}

extern short MultiTapExists[];
extern char *PortName[];
extern char *MTapPortName[];
extern short LevelComplete;
extern short Console_Flags;
extern int GameRunningMode;
extern struct FET_MESSAGE FET_INGAME_QUIT[];
extern struct FET_MESSAGE FET_INGAME_QUIT_YESNO[];
extern struct FET_MESSAGE FET_INGAME_SCREENPOSITION[];
void HelpComputer_StartDisplay(int FramesB4Start, int SecondsToStay);
void ReloadAutoSave(void);
void InGame_GOTO_VIDEO(void);
void InGame_CHECK_FOR_SCREENPOSITION(void);

char *ControlGetPortName(char portId)
{
    if (portId < 0x10) {
        if (MultiTapExists[0])
            return MTapPortName[portId];
        else
            return PortName[0];
    } else {
        if (MultiTapExists[1])
            return MTapPortName[portId - 0xc];
        else
            return PortName[1];
    }
}

void InGame_GOTO_MISSION(void)
{
    InGame_GOTO_RESUME();
    HelpComputer_StartDisplay(1, 0xf);
}

void InGame_GOTO_QUIT_CONFIRM(void)
{
    int OldScreen;

    OldScreen = Engine.currentscreen;
    InGameInitScreen(0x24);
    if (OldScreen == 0x29) {
        FET_INGAME_QUIT_YESNO[0].func = InGame_GOTO_DIED;
        Engine.Backfunc = InGame_GOTO_DIED;
    } else {
        FET_INGAME_QUIT_YESNO[0].func = InGame_GOTO_PAUSEDBACK;
        Engine.Backfunc = InGame_GOTO_PAUSED;
    }
    FrontEndCreateMenu(FET_INGAME_QUIT, 0x10);
    FrontEndCreateMenu(FET_INGAME_QUIT_YESNO, 0x10);
}

void InGame_GOTO_SCREENPOSITION(void)
{
    InGameInitScreen(7);
    FrontEndAddPausedTitle("POSITION");
    FrontEndCreateMenu(FET_INGAME_SCREENPOSITION, 0x10);
    FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
    Engine.Backfunc = InGame_GOTO_VIDEO;
    Engine.Specialfunc = InGame_CHECK_FOR_SCREENPOSITION;
    Engine.cursorpos = FrontEndRestoreCursor();
}

/* An RHS call is evaluated before the LHS address (expand_assignment), which is the only
   way found to get sScreenPosX loaded ahead of Engine.Db in the first store. */
static short GetScreenPosX(void)
{
    return Engine.GameConfig.sScreenPosX;
}

void Config_InstallVideo(void)
{
    int y;

    y = Engine.GameConfig.sScreenPosY;
    Engine.Db[0].DispEnv.screen.x = GetScreenPosX();
    Engine.Db[0].DispEnv.screen.y = Engine.GameConfig.sScreenPosY;
    Engine.Db[1].DispEnv.screen.x = Engine.GameConfig.sScreenPosX;
    Engine.Db[1].DispEnv.screen.y = Engine.GameConfig.sScreenPosY;
    if (y < 0) {
        Engine.Db[0].DispEnv.disp.y = -y;
        Engine.Db[1].DispEnv.disp.y = -y;
        Engine.Db[0].DispEnv.disp.h = y + 240;
        Engine.Db[1].DispEnv.disp.h = y + 240;
    } else {
        Engine.Db[0].DispEnv.disp.y = 0;
        Engine.Db[1].DispEnv.disp.y = 0;
        Engine.Db[0].DispEnv.disp.h = 240;
        Engine.Db[1].DispEnv.disp.h = 240;
    }
}

void FrontEndLevelCompleteTimeout(void)
{
    if (Engine.gFrontEndTimeout > 0) {
        Engine.gFrontEndTimeout--;
        return;
    }
    LevelComplete = Engine.gFrontEndLevelComplete;
    if (LevelComplete == 3) {
        ReloadAutoSave();
    } else {
        Engine.Specialfunc = 0;
        PatchEngineVariables(LevelIsMultiPlayer);
        Console_Flags |= 0x10;
        if (LevelComplete == 8)
            GameRunningMode = 3;
        Paused = 0;
    }
}

extern struct FET_MESSAGE FET_INGAME_QUITTING[];
extern struct FET_MESSAGE FET_INMULTI_VIDEO[];
extern struct FET_MESSAGE FET_INGAME_VIDEO[];

void InGame_GOTO_QUIT(void)
{
    InGameInitScreen(0x25);
    FrontEndCreateMenu(FET_INGAME_QUITTING, 0x10);
    FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
    Engine.MyStringTable[0].window = Engine.MyStringTable[1].window = 4;
    Engine.gFrontEndTimeout = 0xc;
    Engine.Specialfunc = FrontEndLevelCompleteTimeout;
    if (LevelIsMultiPlayer)
        Engine.gFrontEndLevelComplete = 0x12;
    else
        Engine.gFrontEndLevelComplete = 0x10;
    *Engine.targetcdvol = Engine.GameConfig.sMusic * 2;
}

void InGame_GOTO_VIDEO(void)
{
    InGameInitScreen(0x1e);
    FrontEndAddPausedTitle("VIDEO");
    if (LevelIsMultiPlayer) {
        FrontEndCreateMenu(FET_INMULTI_VIDEO, 0x10);
        Engine.Backfunc = InGame_GOTO_PAUSEDBACK;
        Engine.Specialfunc = CHECK_FOR_SCREEN_SPLIT;
    } else {
        FrontEndCreateMenu(FET_INGAME_VIDEO, 0x10);
        Engine.Backfunc = InGame_GOTO_OPTIONS;
        Engine.Specialfunc = 0;
    }
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

/* Same code; only the jump table's name differs (splat's jtbl_ label vs gcc's anonymous rodata). */
int MapPadInputToFire(int Player)
{
    struct CTRLER *Input;
    struct MULTIPLAYER *PlayerParams;
    short FireButton;

    Input = &GameWindows[Player].Input;
    PlayerParams = &Engine.GameConfig.MultiPlayerParams[Player];
    switch (PlayerParams->controllerSelected) {
    case 1:
        FireButton = 4;
        break;
    case 0:
    case 2:
        FireButton = 2;
        break;
    case 3:
        FireButton = 0x400;
        break;
    case 4:
    case 5:
        FireButton = 0x800;
        break;
    default:
        FireButton = 0x4000;
        break;
    }
    return (Input->Padd & FireButton) && !(Input->OPadd & FireButton);
}

void FrontEndAddText(struct STRING_TABLE *fetString, char *message, short x, short y, int fontSize, unsigned short window)
{
    sprintf(fetString->message, "%s", message);
    sprintf(fetString->originalMessage, "%s", message);
    fetString->x = x;
    fetString->y = y;
    fetString->fontSize = fontSize;
    fetString->window = window;
    fetString->dontDraw = 0;
}

void FrontEndInitText(void)
{
    short i;
    struct STRING_TABLE *pFetString;

    pFetString = MyStringTable;
    for (i = 0; i < 28; i++) {
        sprintf(pFetString->message, "");
        pFetString->x = 0;
        pFetString->y = 0;
        pFetString->pSliderVar = 0;
        pFetString->pBoolVar = 0;
        pFetString->selectFunc = 0;
        pFetString->cursorItem = 0;
        pFetString->cursorDisabled = 0;
        pFetString->window = 0;
        pFetString->fontSize = 8;
        pFetString->sliderX = 0x118;
        pFetString->dontDraw = 0;
        pFetString++;
    }
}

extern int Pauser;
extern struct FET_MESSAGE FET_INGAME_CONTROLLER[];
extern struct FET_MESSAGE FET_INGAME_RESUPPLY[];
extern struct FET_MESSAGE FET_INGAME_RESUPPLY_YESNO[];
void InGame_CHECK_FOR_CONTROLLER(void);

void InGame_GOTO_CONTROLLER(void)
{
    InGameInitScreen(0x1f);
    FrontEndAddPausedTitle("CONTROLLER");
    FET_INGAME_CONTROLLER[1].pVar = &Engine.GameConfig.MultiPlayerParams[Pauser].vibration;
    FET_INGAME_CONTROLLER[2].pVar = &Engine.GameConfig.MultiPlayerParams[Pauser].invertY;
    FET_INGAME_CONTROLLER[3].pVar = &Engine.GameConfig.MultiPlayerParams[Pauser].useMouse;
    FET_INGAME_CONTROLLER[4].pSlider = &Engine.GameConfig.MultiPlayerParams[Pauser].mouseSensitivity;
    FrontEndCreateMenu(FET_INGAME_CONTROLLER, 0x10);
    if (LevelIsMultiPlayer)
        Engine.Backfunc = InGame_GOTO_PAUSEDBACK;
    else
        Engine.Backfunc = InGame_GOTO_PLAYER;
    Engine.Specialfunc = InGame_CHECK_FOR_CONTROLLER;
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

void InGame_GOTO_RESUPPLY_CONFIRM(void)
{
    char buf[32];

    InGameInitScreen(0x22);
    FrontEndCreateMenu(FET_INGAME_RESUPPLY, 0x10);
    FrontEndCreateMenu(FET_INGAME_RESUPPLY_YESNO, 0x10);
    sprintf(buf, "YOU HAVE %d LEFT", Engine.GameConfig.cContinues);
    FrontEndAddText(&MyStringTable[2], buf, 0x100, 0x6b, 0x10, 0);
}

void InGame_GOTO_OPTIONS2(void)
{
    if (Paused)
        targetcdvol = 0;
    else
        targetcdvol = Engine.GameConfig.sMusic * 2;
    if (LevelIsMultiPlayer)
        InGame_GOTO_PAUSED();
    else
        InGame_GOTO_OPTIONS();
}

extern struct FET_MESSAGE FET_INGAME_YOUDIED[];

void FrontEndResetMultiplayerDefaults(void)
{
    short i;

    INGAME_RESET_CHEATS();
    Engine.GameConfig.sNumPlayers = 2;
    sprintf(Engine.GameConfig.MultiPlayerParams[0].playerName, "%s", "PLAYER 1");
    sprintf(Engine.GameConfig.MultiPlayerParams[1].playerName, "%s", "PLAYER 2");
    sprintf(Engine.GameConfig.MultiPlayerParams[2].playerName, "%s", "PLAYER 3");
    sprintf(Engine.GameConfig.MultiPlayerParams[3].playerName, "%s", "PLAYER 4");
    for (i = 0; i < 4; i++) {
        Engine.GameConfig.MultiPlayerParams[i].playerSelected = i;
        Engine.GameConfig.MultiPlayerParams[i].controllerSelected = 6;
        Engine.GameConfig.MultiPlayerParams[i].mouseConfigSelected = 0;
        Engine.GameConfig.MultiPlayerParams[i].analogConfigSelected = 4;
        Engine.GameConfig.MultiPlayerParams[i].standardConfigSelected = 6;
        Engine.GameConfig.MultiPlayerParams[i].teamSelected = i & 1;
    }
}

void InGame_GOTO_DIED(void)
{
    char buf[32];

    InGameInitScreen(0x29);
    FrontEndCreateMenu(FET_INGAME_YOUDIED, 0x10);
    if (!Engine.GameConfig.cContinues) {
        sprintf(buf, "gRESUPPLY AND RESTART (0 LEFT)");
        FrontEndAddText(&MyStringTable[1], buf, 0x100, 0x78, 0x10, 0);
    } else {
        sprintf(buf, "RESUPPLY AND RESTART (%d LEFT)", Engine.GameConfig.cContinues);
        FrontEndAddText(&MyStringTable[1], buf, 0x100, 0x78, 0x10, 0);
    }
    Engine.CantPause = 1;
    Engine.Backfunc = 0;
    Engine.Specialfunc = WAITTIMENOPAD;
    Engine.cursorpos = Engine.cursorlower;
    WaitTime = 0;
}

extern struct SCRPNT ScreenSize;
extern struct FET_MESSAGE FET_INMULTI_CHEAT_NONE[];
extern struct FET_MESSAGE FET_INMULTI_CHEAT_BRONZE[];
extern struct FET_MESSAGE FET_INMULTI_CHEAT_SILVER[];
extern struct FET_MESSAGE FET_INMULTI_CHEAT_GOLD[];

void FrontEndAddPausedTitle(char *Title)
{
    FrontEndAddText(&MyStringTable[0], Title, 0x100, ((ScreenSize.sy - 188) >> 1) + 10, 0x20, 0);
    MyStringTable[0].cursorItem = 1;
    if (LevelIsMultiPlayer) {
        if (Engine.GameConfig.sMultiMode == 1)
            MyStringTable[0].bannerColour = Engine.GameConfig.MultiPlayerParams[Pauser].teamSelected;
        else
            MyStringTable[0].bannerColour = Engine.GameConfig.MultiPlayerParams[Pauser].playerSelected;
    } else {
        MyStringTable[0].bannerColour = 0;
    }
    Engine.cursorupper = 1;
}

void InGame_GOTO_MPCHEAT(void)
{
    InGameInitScreen(0x2a);
    FrontEndAddText(&MyStringTable[0], "PAUSED", 0x100, ((ScreenSize.sy - 188) >> 1) + 10, 0x20, 0);
    MyStringTable[0].cursorItem = 1;
    Engine.cursorupper = 1;
    if (Engine.GameConfig.cCheats == 3)
        FrontEndCreateMenu(FET_INMULTI_CHEAT_GOLD, 0x10);
    else if (Engine.GameConfig.cCheats == 2)
        FrontEndCreateMenu(FET_INMULTI_CHEAT_SILVER, 0x10);
    else if (Engine.GameConfig.cCheats == 1)
        FrontEndCreateMenu(FET_INMULTI_CHEAT_BRONZE, 0x10);
    else
        FrontEndCreateMenu(FET_INMULTI_CHEAT_NONE, 0x10);
    Engine.Backfunc = InGame_GOTO_PAUSEDBACK;
    Engine.Specialfunc = CHECK_FOR_SLIDERANDSCROLL;
    Engine.cursorpos = Engine.FrontEndRestoreCursor();
}

/* defined in QFr_Sing.Obj, so $gp-relative */
char sOldScreenHorizSplit;

extern int PalNtsc;
extern int PalNtscRate;
extern int Gravity;
extern int MaxWindows;
extern int GlobalKills;
extern int GlobalKillsCount;
extern int GlobalSecretsFound;
extern int GlobalSecretsCount;
extern struct FET_MESSAGE FET_INMULTI_PAUSED[];
extern struct FET_MESSAGE FET_INGAME_PAUSED[];
void LoadSaveGameConfigData(int Write);
void CheckForAllWeap(struct PLAYER *Player);

void InGame_GOTO_PAUSEDBACK(void)
{
    char KillsSecrets[48];

    LoadSaveGameConfigData(1);
    if (LevelIsMultiPlayer) {
        InGameInitScreen(0x2b);
        sOldScreenHorizSplit = Engine.GameConfig.bSplitHoriz;
        FrontEndAddPausedTitle("PAUSED");
        FrontEndCreateMenu(FET_INMULTI_PAUSED, 0x10);
    } else {
        InGameInitScreen(0x1a);
        FrontEndAddPausedTitle("PAUSED");
        FrontEndCreateMenu(FET_INGAME_PAUSED, 0x10);
        sprintf(KillsSecrets, "KILLS %d/%d    %d/%d SECRETS", GlobalKills, GlobalKillsCount, GlobalSecretsFound, GlobalSecretsCount);
        FrontEndAddText(&MyStringTable[6], KillsSecrets, 0x100, 200, 0x10, 0);
        MyStringTable[6].cursorDisabled = 1;
        MyStringTable[6].cursorItem = 1;
        Engine.cursorupper--;
        MyStringTable[6].bannerColour = 0;
    }
    Engine.cursorpos = Engine.cursorlower;
}

void PatchEngineVariables(int IsMultiPlayer)
{
    int a;

    if (IsMultiPlayer) {
        PalNtscRate = (PalNtsc * (Engine.GameConfig.sGameSpeed + 64)) >> 7;
        Console_Flags = 0;
        Gravity = (Engine.GameConfig.sGravity + 64) >> 2;
        if (!Engine.GameConfig.bFallingDamage)
            Console_Flags = 0x40;
        if (Engine.GameConfig.bInfiniteAmmo)
            Console_Flags |= 1;
        if (Engine.GameConfig.bAllWeapons)
            Console_Flags |= 0x20;
        if (Engine.GameConfig.bOneShotKill)
            Console_Flags |= 0x80;
        for (a = 0; a < MaxWindows; a++) {
            if (GameWindows[a].Creature && ((struct CREATURE *)GameWindows[a].Creature)->Player)
                CheckForAllWeap(((struct CREATURE *)GameWindows[a].Creature)->Player);
        }
    } else {
        Gravity = 32;
        Console_Flags = 0;
        PalNtscRate = PalNtsc;
    }
}

void InGame_CHECK_FOR_SCREENPOSITION(void)
{
    short pad;
    static short TimeLeft;
    static short TimeRight;
    static short TimeUp;
    static short TimeDown;

    pad = Engine.gFrontEndPad[Pauser];
    if (pad & 0x80) {
        TimeLeft += TimeLeft < 5;
        if ((TimeLeft == 5 || TimeLeft == 1) && Engine.GameConfig.sScreenPosX >= -19)
            Engine.GameConfig.sScreenPosX--;
    } else {
        TimeLeft = 0;
    }
    if (pad & 0x20) {
        TimeRight += TimeRight < 5;
        if ((TimeRight == 5 || TimeRight == 1) && Engine.GameConfig.sScreenPosX < 20)
            Engine.GameConfig.sScreenPosX++;
    } else {
        TimeRight = 0;
    }
    if (pad & 0x10) {
        TimeUp += TimeUp < 5;
        if ((TimeUp == 5 || TimeUp == 1) && Engine.GameConfig.sScreenPosY >= -31)
            Engine.GameConfig.sScreenPosY--;
    } else {
        TimeUp = 0;
    }
    if (pad & 0x40) {
        TimeDown += TimeDown < 5;
        if ((TimeDown == 5 || TimeDown == 1) && Engine.GameConfig.sScreenPosY < 64)
            Engine.GameConfig.sScreenPosY++;
    } else {
        TimeDown = 0;
    }
    Config_InstallVideo();
}
