#include "types.h"

#define setlen(p, _len) (*(((unsigned char *)(p)) + 3) = (unsigned char)(_len))
#define setcode(p, _code) (*(((unsigned char *)(p)) + 7) = (unsigned char)(_code))
#define setRGB0(p, _r0, _g0, _b0) (p)->r0 = _r0, (p)->g0 = _g0, (p)->b0 = _b0
#define setXY4(p, _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x1), (p)->y1 = (_y1), \
    (p)->x2 = (_x2), (p)->y2 = (_y2), (p)->x3 = (_x3), (p)->y3 = (_y3)
#define setPolyG4(p) setlen(p, 8), setcode(p, 0x3a)
#define setRGB1(p, _r1, _g1, _b1) (p)->r1 = _r1, (p)->g1 = _g1, (p)->b1 = _b1
#define setRGB2(p, _r2, _g2, _b2) (p)->r2 = _r2, (p)->g2 = _g2, (p)->b2 = _b2
#define setRGB3(p, _r3, _g3, _b3) (p)->r3 = _r3, (p)->g3 = _g3, (p)->b3 = _b3
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define setPolyFT4(p) setlen(p, 9), setcode(p, 0x2c)
#define setXYWH(p, _x0, _y0, _w, _h) \
    (p)->x0 = (_x0), (p)->y0 = (_y0), \
    (p)->x1 = (_x0) + (_w), (p)->y1 = (_y0), \
    (p)->x2 = (_x0), (p)->y2 = (_y0) + (_h), \
    (p)->x3 = (_x0) + (_w), (p)->y3 = (_y0) + (_h)
#define setUVWH(p, _u0, _v0, _w, _h) \
    (p)->u0 = (_u0), (p)->v0 = (_v0), \
    (p)->u1 = (_u0) + (_w), (p)->v1 = (_v0), \
    (p)->u2 = (_u0), (p)->v2 = (_v0) + (_h), \
    (p)->u3 = (_u0) + (_w), (p)->v3 = (_v0) + (_h)
#define setLineF2(p) setlen(p, 3), setcode(p, 0x40)
#define setPolyF4(p) setlen(p, 5), setcode(p, 0x28)
#define setXY2(p, _x0, _y0, _x1, _y1) (p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x1), (p)->y1 = (_y1)

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
void FrontEndAddText(struct STRING_TABLE *fetString, char *message, short x, short y, short fontSize, unsigned short window);

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

extern struct DB *currdb;
extern int Paused;
extern int MaxWindows;
extern struct SCRPNT ScreenSize;
void AddPrim_(void *ot, void *p);
short FrontEndGetMessageLength(char *pMessage);

extern struct RECT gDarkBoxRect;
extern struct RECT gDarkBoxRect;  /* splat labels .w (gDarkBoxRect+4) D_800B2C4C */
int strcmp(const char *a, const char *b);

void FrontEndDrawCursor(struct STRING_TABLE *fetString, struct POLY_FT4 **poly)
{
    struct POLY_FT4 *pActualPoly;
    struct POLY_G4 *pCastPolyG4;
    short BannerWidth;
    short BannerXPos;
    short y;
    char NumWins;
    long *OtBanner;
    short fontSizeY;

    pActualPoly = *poly;
    NumWins = 1;
    if (!Paused)
        NumWins = MaxWindows;
    if (!strcmp(fetString->message, ""))
        return;
    if (fetString->cursorItem != 1)
        return;
    if (fetString->bannerColour == 4)
        return;
    switch (fetString->fontSize) {
    case 32:
        fontSizeY = 20;
        break;
    case 16:
        fontSizeY = 11;
        break;
    case 8:
        fontSizeY = 8;
        break;
    }
    FrontEndGetMessageLength(fetString->message);
    y = fetString->y - (fontSizeY >> 1);
    if (NumWins >= 2) {
        BannerXPos = 0;
        if (fetString->window == 4) {
            OtBanner = (long *)&currdb->BigOt[5];
            BannerWidth = ScreenSize.sx;
        } else {
            OtBanner = (long *)&currdb->Ot[fetString->window][49];
            BannerWidth = GameWindows[fetString->window].Size.sx;
        }
    } else {
        if (fetString->fontSize != 16)
            OtBanner = (long *)&currdb->BigOt[5];
        else
            OtBanner = (long *)&currdb->BigOt[7];
        if (Paused) {
            BannerWidth = gDarkBoxRect.w;
            BannerXPos = gDarkBoxRect.x;
        } else {
            BannerWidth = 0x200;
            BannerXPos = 0;
        }
    }
    setPolyG4(pActualPoly);
    setXY4(pActualPoly, BannerXPos, y - 2, BannerXPos + (BannerWidth >> 1), y - 2, BannerXPos, y + 1 + fontSizeY, BannerXPos + (BannerWidth >> 1), y + 1 + fontSizeY);
    pCastPolyG4 = (struct POLY_G4 *)pActualPoly;
    switch (fetString->bannerColour) {
    case 1:
        setRGB0((struct POLY_G4 *)pActualPoly, 0x20, 0, 0);
        setRGB1((struct POLY_G4 *)pActualPoly, 0xff, 0, 0);
        setRGB2((struct POLY_G4 *)pActualPoly, 0x20, 0, 0);
        setRGB3((struct POLY_G4 *)pActualPoly, 0xff, 0, 0);
        break;
    case 0:
        setRGB0((struct POLY_G4 *)pActualPoly, 0, 0, 0x20);
        setRGB1((struct POLY_G4 *)pActualPoly, 0, 0, 0xff);
        setRGB2((struct POLY_G4 *)pActualPoly, 0, 0, 0x20);
        setRGB3((struct POLY_G4 *)pActualPoly, 0, 0, 0xff);
        break;
    case 2:
        setRGB0((struct POLY_G4 *)pActualPoly, 0x10, 0, 0x20);
        setRGB1((struct POLY_G4 *)pActualPoly, 0x7f, 0, 0xff);
        setRGB2((struct POLY_G4 *)pActualPoly, 0x10, 0, 0x20);
        setRGB3((struct POLY_G4 *)pActualPoly, 0x7f, 0, 0xff);
        break;
    case 3:
        setRGB0((struct POLY_G4 *)pActualPoly, 0, 0x20, 0);
        setRGB1((struct POLY_G4 *)pActualPoly, 0, 0xff, 0);
        setRGB2((struct POLY_G4 *)pActualPoly, 0, 0x20, 0);
        setRGB3((struct POLY_G4 *)pActualPoly, 0, 0xff, 0);
        break;
    }
    AddPrim_(OtBanner, pCastPolyG4);
    pActualPoly++;
    *poly += 1;
    setPolyG4(pActualPoly);
    setXY4(pActualPoly, BannerXPos + (BannerWidth >> 1), y - 2, BannerXPos + (BannerWidth >> 1) + (BannerWidth >> 1), y - 2, BannerXPos + (BannerWidth >> 1), y + 1 + fontSizeY, BannerXPos + (BannerWidth >> 1) + (BannerWidth >> 1), y + 1 + fontSizeY);
    pCastPolyG4 = (struct POLY_G4 *)pActualPoly;
    switch (fetString->bannerColour) {
    case 1:
        setRGB0((struct POLY_G4 *)pActualPoly, 0xff, 0, 0);
        setRGB1((struct POLY_G4 *)pActualPoly, 0x20, 0, 0);
        setRGB2((struct POLY_G4 *)pActualPoly, 0xff, 0, 0);
        setRGB3((struct POLY_G4 *)pActualPoly, 0x20, 0, 0);
        break;
    case 0:
        setRGB0((struct POLY_G4 *)pActualPoly, 0, 0, 0xff);
        setRGB1((struct POLY_G4 *)pActualPoly, 0, 0, 0x20);
        setRGB2((struct POLY_G4 *)pActualPoly, 0, 0, 0xff);
        setRGB3((struct POLY_G4 *)pActualPoly, 0, 0, 0x20);
        break;
    case 2:
        setRGB0((struct POLY_G4 *)pActualPoly, 0x7f, 0, 0xff);
        setRGB1((struct POLY_G4 *)pActualPoly, 0x10, 0, 0x20);
        setRGB2((struct POLY_G4 *)pActualPoly, 0x7f, 0, 0xff);
        setRGB3((struct POLY_G4 *)pActualPoly, 0x10, 0, 0x20);
        break;
    case 3:
        setRGB0((struct POLY_G4 *)pActualPoly, 0, 0xff, 0);
        setRGB1((struct POLY_G4 *)pActualPoly, 0, 0x20, 0);
        setRGB2((struct POLY_G4 *)pActualPoly, 0, 0xff, 0);
        setRGB3((struct POLY_G4 *)pActualPoly, 0, 0x20, 0);
        break;
    }
    AddPrim_(OtBanner, pCastPolyG4);
    pActualPoly++;
    *poly += 1;
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
int abs(int);

#define DPAD_FWD(fwd, back) \
    if (Input->Padd & (fwd)) { \
        OutControls->ForwardSpeed = 127; \
        OutControls->AutoCentreAdd = 1; \
    } else if (Input->Padd & (back)) { \
        OutControls->ForwardSpeed = -127; \
        OutControls->AutoCentreAdd = 1; \
    } else { \
        OutControls->ForwardSpeed = 0; \
    }
#define DPAD_AXIS(field, neg, pos) \
    if (Input->Padd & (neg)) \
        OutControls->field = -127; \
    else if (Input->Padd & (pos)) \
        OutControls->field = 127; \
    else \
        OutControls->field = 0;
#define DPAD_LOOK(up, down) \
    if (Input->Padd & (up)) { \
        OutControls->LookUpSpeed = -127; \
        OutControls->DigLookUpPressed = 1; \
    } \
    if (Input->Padd & (down)) { \
        OutControls->LookUpSpeed = 127; \
        OutControls->DigLookDownPressed = 1; \
    } else if (!(Input->Padd & (up))) { \
        OutControls->LookUpSpeed = 0; \
    }

void MapPadInputToCommands(int Player, struct PLAYERCONTROLS *OutControls)
{
    struct CTRLER *Input;
    struct MULTIPLAYER *PlayerParams;
    short FireButton;
    short JumpButton;
    short NextWeapButton;
    short PrevWeapButton;

    Input = &GameWindows[Player].Input;
    PlayerParams = &Engine.GameConfig.MultiPlayerParams[Player];
    OutControls->PauseTapped = (Input->Padd & 8) && !(Input->OPadd & 8);
    OutControls->AutoCentreAdd = 0;
    OutControls->DigLookDownPressed = 0;
    OutControls->DigLookUpPressed = 0;
    switch (PlayerParams->controllerSelected) {
    case 0:
        OutControls->TurnSpeed = ((signed char)Input->APaddLX * (PlayerParams->mouseSensitivity + 32)) >> 4;
        OutControls->LookUpSpeed = ((PlayerParams->invertY ? -(signed char)Input->APaddLY : (signed char)Input->APaddLY) * (PlayerParams->mouseSensitivity + 32)) >> 4;
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(SideSpeed, 0x80, 0x20)
        FireButton = 2;
        JumpButton = 4;
        NextWeapButton = 0x400;
        PrevWeapButton = 0x100;
        break;
    case 1:
        OutControls->TurnSpeed = ((signed char)Input->APaddLX * (PlayerParams->mouseSensitivity + 32)) >> 4;
        OutControls->LookUpSpeed = ((PlayerParams->invertY ? -(signed char)Input->APaddLY : (signed char)Input->APaddLY) * (PlayerParams->mouseSensitivity + 32)) >> 4;
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(SideSpeed, 0x80, 0x20)
        FireButton = 4;
        JumpButton = 2;
        NextWeapButton = 0x400;
        PrevWeapButton = 0x100;
        break;
    case 2:
        OutControls->TurnSpeed = ((signed char)Input->APaddLX * (PlayerParams->mouseSensitivity + 32)) >> 4;
        OutControls->LookUpSpeed = ((PlayerParams->invertY ? -(signed char)Input->APaddLY : (signed char)Input->APaddLY) * (PlayerParams->mouseSensitivity + 32)) >> 4;
        DPAD_FWD(0x4, 0x40)
        DPAD_AXIS(SideSpeed, 0x80, 0x20)
        FireButton = 2;
        JumpButton = 0x10;
        NextWeapButton = 0x400;
        PrevWeapButton = 0x100;
        break;
    case 3:
        OutControls->TurnSpeed = (signed char)Input->APaddRX;
        OutControls->LookUpSpeed = PlayerParams->invertY ? (signed char)Input->APaddRY : -(signed char)Input->APaddRY;
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(SideSpeed, 0x80, 0x20)
        FireButton = 0x400;
        JumpButton = 0x100;
        NextWeapButton = 0x800;
        PrevWeapButton = 0x200;
        break;
    case 4:
        OutControls->TurnSpeed = (signed char)Input->APaddLX;
        OutControls->LookUpSpeed = PlayerParams->invertY ? (signed char)Input->APaddLY : -(signed char)Input->APaddLY;
        DPAD_FWD(0x1000, 0x4000)
        DPAD_AXIS(SideSpeed, 0x8000, 0x2000)
        FireButton = 0x800;
        JumpButton = 0x200;
        NextWeapButton = 0x400;
        PrevWeapButton = 0x100;
        break;
    case 5:
        OutControls->TurnSpeed = (signed char)Input->APaddLX;
        OutControls->LookUpSpeed = PlayerParams->invertY ? -(signed char)Input->APaddRY : (signed char)Input->APaddRY;
        OutControls->SideSpeed = (signed char)Input->APaddRX;
        OutControls->ForwardSpeed = -(signed char)Input->APaddLY;
        if (abs(OutControls->ForwardSpeed) > 100)
            OutControls->AutoCentreAdd = 1;
        if ((Input->Padd & 4) && !(Input->OPadd & 4)) {
            OutControls->DigLookUpPressed = 1;
            OutControls->DigLookDownPressed = 1;
        } else {
            OutControls->DigLookUpPressed = 0;
            OutControls->DigLookDownPressed = 0;
        }
        FireButton = 0x800;
        JumpButton = 0x200;
        NextWeapButton = 0x400;
        PrevWeapButton = 0x100;
        break;
    case 6:
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(TurnSpeed, 0x80, 0x20)
        DPAD_AXIS(SideSpeed, 0x100, 0x200)
        DPAD_LOOK(0x800, 0x400)
        FireButton = 0x4000;
        JumpButton = 0x8000;
        NextWeapButton = 0x1000;
        PrevWeapButton = 0x2000;
        break;
    case 7:
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(TurnSpeed, 0x80, 0x20)
        DPAD_AXIS(SideSpeed, 0x100, 0x200)
        DPAD_LOOK(0x1000, 0x2000)
        FireButton = 0x4000;
        JumpButton = 0x8000;
        NextWeapButton = 0x800;
        PrevWeapButton = 0x400;
        break;
    case 8:
        DPAD_FWD(0x10, 0x40)
        DPAD_AXIS(TurnSpeed, 0x100, 0x200)
        DPAD_AXIS(SideSpeed, 0x80, 0x20)
        DPAD_LOOK(0x800, 0x400)
        FireButton = 0x4000;
        JumpButton = 0x8000;
        NextWeapButton = 0x1000;
        PrevWeapButton = 0x2000;
        break;
    }
    OutControls->FireTapped = (Input->Padd & FireButton) && !(Input->OPadd & FireButton);
    OutControls->FireDepressed = (Input->Padd & FireButton) != 0;
    OutControls->FireHeld = (Input->OPadd & FireButton) && OutControls->FireDepressed;
    OutControls->JumpTapped = (Input->Padd & JumpButton) && !(Input->OPadd & JumpButton);
    OutControls->JumpDepressed = (Input->Padd & JumpButton) != 0;
    OutControls->ChangeWeapUpTapped = (Input->Padd & NextWeapButton) && !(Input->OPadd & NextWeapButton);
    OutControls->ChangeWeapDownTapped = (Input->Padd & PrevWeapButton) && !(Input->OPadd & PrevWeapButton);
}

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

void FrontEndAddText(struct STRING_TABLE *fetString, char *message, short x, short y, short fontSize, unsigned short window)
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

extern struct _MEMCARD_DATA memcard_data;
long MemoryCardGetStatus(void);
long MemoryCardGetNumFreeBlocks(void);

void InGame_CHECK_FOR_SAVE(void)
{
    short fileIndex;

    Engine.MemoryCardUpdate();
    if ((!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) || MemoryCardGetStatus() == 13 || MemoryCardGetStatus() == 3) {
        switch (Engine.MemoryCardGetStatus()) {
        case 14:
        case 16:
            if (!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) {
                Engine.CantUnPause = 0;
                if (LevelIsMultiPlayer) {
                    PatchEngineVariables(1);
                    CHECK_FOR_SCREEN_SPLIT();
                } else {
                    PatchEngineVariables(0);
                }
                Engine.gFrontEndPad[0] = Engine.gFrontEndPad[1] = Engine.gFrontEndPad[2] = Engine.gFrontEndPad[3] = 0;
                Paused = 0;
                targetcdvol = Engine.GameConfig.sMusic * 2;
            }
            break;
        case 3:
            CHECK_FOR_MCFILEWINDOW();
            if (!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) {
                Engine.MemoryCardSetActiveCard(Engine.cursorpos - Engine.cursorlower);
                Engine.MemoryCardSetStatus(4);
            }
            break;
        case 13: {
            int Cheats;
            int Difficulty;
            char buf[32];
            char cheatText[4][8] = {"NONE", "BRONZE", "SILVER", "GOLD"};
            char difficultyText[3][8] = {"EASY", "MEDIUM", "HARD"};

            for (fileIndex = Engine.cursorlower; fileIndex < Engine.cursorupper; fileIndex++)
                FrontEndModifyText(&Engine.MyStringTable[fileIndex], Engine.MyStringTable[fileIndex].originalMessage + 3);
            fileIndex = Engine.cursorpos - Engine.cursorlower;
            Engine.PDHCheats = (memcard_data.fileName[fileIndex].name[13] - '0') & 3;
            Engine.PDHContinues = (memcard_data.fileName[fileIndex].name[14] - '0') & 3;
            Engine.PDHDifficulty = (memcard_data.fileName[fileIndex].name[15] - '0') & 3;
            if ((fileIndex + 1 == Engine.cursorupper - Engine.cursorlower && MemoryCardGetNumFreeBlocks())
                || (Engine.MyStringTable[fileIndex + Engine.cursorlower].originalMessage[3] == 'M'
                    && *(long *)&Engine.MyStringTable[fileIndex + Engine.cursorlower].originalMessage[4] == 0x49544c55)) {
                if (fileIndex + 1 == Engine.cursorupper - Engine.cursorlower && MemoryCardGetNumFreeBlocks()) {
                    Engine.MyStringTable[2].dontDraw = Engine.MyStringTable[3].dontDraw = Engine.MyStringTable[4].dontDraw = 1;
                } else {
                    Engine.MyStringTable[2].dontDraw = Engine.MyStringTable[4].dontDraw = 1;
                    Cheats = Engine.PDHCheats;
                    if (Cheats) {
                        sprintf(buf, "CHEATS %s", cheatText[Cheats]);
                        FrontEndModifyText(&Engine.MyStringTable[3], buf);
                        Engine.MyStringTable[3].dontDraw = 0;
                    } else {
                        Engine.MyStringTable[3].dontDraw = 1;
                    }
                }
            } else {
                Cheats = Engine.PDHCheats;
                Difficulty = Engine.PDHDifficulty;
                sprintf(buf, "RESUPPLIES %d", Engine.PDHContinues);
                FrontEndModifyText(&Engine.MyStringTable[2], buf);
                sprintf(buf, "DIFFICULTY %s", difficultyText[Difficulty]);
                FrontEndModifyText(&Engine.MyStringTable[3], buf);
                Engine.MyStringTable[2].dontDraw = Engine.MyStringTable[3].dontDraw = 0;
                if (Cheats) {
                    sprintf(buf, "CHEATS %s", cheatText[Cheats]);
                    FrontEndModifyText(&Engine.MyStringTable[4], buf);
                    Engine.MyStringTable[4].dontDraw = 0;
                } else {
                    Engine.MyStringTable[4].dontDraw = 1;
                }
            }
            CHECK_FOR_MCFILEWINDOW();
            if (!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) {
                fileIndex = Engine.cursorpos - Engine.cursorlower;
                Engine.MemoryCardSetActiveFile(fileIndex);
                if (!Engine.gFrontEndLoadType)
                    Engine.MemoryCardSetStatus(21);
                if (Engine.gFrontEndLoadType == 1) {
                    if (fileIndex + 1 == Engine.cursorupper - Engine.cursorlower && MemoryCardGetNumFreeBlocks())
                        Engine.MemoryCardSetStatus(22);
                    else
                        Engine.MemoryCardSetStatus(19);
                }
            }
            break;
        }
        case 19:
            if (!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) {
                if (Engine.cursorpos == Engine.cursorlower + 1) {
                    Engine.MemoryCardSetStatus(20);
                } else if (Engine.cursorpos == Engine.cursorlower) {
                    Engine.MemoryCardSetStatus(13);
                    CHECK_FOR_MCFILEWINDOW();
                }
            }
            break;
        case 5:
            if (!(Engine.gFrontEndPad[0] & 0x4000) && (Engine.gFrontEndOPad[0] & 0x4000)) {
                if (Engine.cursorpos == Engine.cursorlower + 1)
                    Engine.MemoryCardSetStatus(6);
                else if (Engine.cursorpos == Engine.cursorlower)
                    Engine.MemoryCardSetStatus(1);
            }
            break;
        case 1:
            break;
        }
    }
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

void FrontEndGetTextCoords(char letter, short *u, short *v, short fontSize)
{
    short index;
    short numLettersPerRow;

    *v = 0;
    if (fontSize == 32) {
        numLettersPerRow = 7;
        switch (letter) {
        case '2':
            *u = 5;
            *v = 3;
            return;
        case '3':
            *u = 6;
            *v = 3;
            return;
        case '4':
            *u = 0;
            *v = 4;
            return;
        case '?':
            *u = 1;
            *v = 4;
            return;
        }
    } else {
        numLettersPerRow = 15;
    }
    if ((unsigned char)(letter - 'A') < 26) {
        index = letter - 'A';
        while (index > numLettersPerRow - 1) {
            index -= numLettersPerRow;
            (*v)++;
        }
        *u = index;
        return;
    }
    if ((unsigned char)(letter - '0') < 10) {
        *u = letter - '0';
        *v = 3;
        return;
    }
    *v = 2;
    switch (letter) {
    case ':': *u = 1; break;
    case '/': *u = 2; break;
    case '.': *u = 3; break;
    case '?': *u = 4; break;
    case 'i': *u = 5; break;
    case '\'': *u = 6; break;
    case '!': *u = 9; break;
    case ',': *u = 10; break;
    case '&': *u = 11; break;
    case '(': *u = 12; break;
    case ')': *u = 13; break;
    case '-':
    case 'u':
        *u = 0;
        break;
    }
}

extern struct QENGINELEVINTERFACE Engine;
extern struct STRING_TABLE MyStringTable[];
void FrontEndDrawText(struct STRING_TABLE *fetString, struct POLY_FT4 **poly);
void FrontEndDrawSlider(struct STRING_TABLE *fetString, struct POLY_FT4 **poly, short isCursor);
void FrontEndDrawCursor(struct STRING_TABLE *fetString, struct POLY_FT4 **poly);
int sprintf(char *buf, const char *fmt, ...);

short dbidx;
void CHECK_FOR_BOOLEAN(void);
void FrontEndDrawMenu(struct POLY_FT4 **poly);
void DrawBottomPoly(struct POLY_FT4 **poly);
int GetSoundNum(struct IDENTITYNAME Name);
void SoundAtLocation(int Sound, struct VECT *Location);
extern struct CREATURE Creatures[];

void DoFrontEndStuff(void)
{
    short i;
    short oldcursorpos;
    signed char canup;
    signed char candown;
    signed char nextup;
    signed char nextdown;
    signed char firstup;
    signed char firstdown;
    void (*func)();
    struct POLY_FT4 *pActualPoly;
    void (*func_ppPolyFT4)();
    short disabledSelection;

    Engine.gFrontEndOPad[0] = Engine.gFrontEndPad[0];
    Engine.gFrontEndOPad[1] = Engine.gFrontEndPad[1];
    Engine.gFrontEndOPad[2] = Engine.gFrontEndPad[2];
    Engine.gFrontEndOPad[3] = Engine.gFrontEndPad[3];
    Engine.gFrontEndPad[0] = GameWindows[0].Input.Padd;
    Engine.gFrontEndPad[1] = GameWindows[1].Input.Padd;
    Engine.gFrontEndPad[2] = GameWindows[2].Input.Padd;
    Engine.gFrontEndPad[3] = GameWindows[3].Input.Padd;
    if (Paused)
        Engine.gFrontEndPadShot = Engine.gFrontEndPad[Pauser] & (Engine.gFrontEndPad[Pauser] ^ Engine.gFrontEndOPad[Pauser]);
    else
        Engine.gFrontEndPadShot = Engine.gFrontEndPad[0] & (Engine.gFrontEndPad[0] ^ Engine.gFrontEndOPad[0]);
    if (MaxWindows == 1 || Paused) {
        canup = 0;
        for (i = Engine.cursorpos + 1; i <= Engine.cursorupper - 1; i++) {
            disabledSelection = Engine.MyStringTable[i].cursorDisabled || Engine.MyStringTable[i].message[0] == 'g' || Engine.MyStringTable[i].originalMessage[0] == 'g';
            if (!disabledSelection) {
                nextup = i;
                canup = 1;
                break;
            }
        }
        candown = 0;
        for (i = Engine.cursorpos - 1; i >= Engine.cursorlower; i--) {
            disabledSelection = Engine.MyStringTable[i].cursorDisabled || Engine.MyStringTable[i].message[0] == 'g' || Engine.MyStringTable[i].originalMessage[0] == 'g';
            if (!disabledSelection) {
                nextdown = i;
                candown = 1;
                break;
            }
        }
        firstdown = -1;
        for (i = Engine.cursorlower; i < Engine.cursorupper; i++) {
            disabledSelection = Engine.MyStringTable[i].cursorDisabled || Engine.MyStringTable[i].message[0] == 'g' || Engine.MyStringTable[i].originalMessage[0] == 'g';
            if (!disabledSelection) {
                if (firstdown == -1)
                    firstdown = i;
                firstup = i;
            }
        }
        if (!canup && candown) {
            if (!Engine.MyStringTable[Engine.cursorupper - 1].dontWrap) {
                nextup = firstdown;
                canup = 1;
            }
        }
        if (!candown && canup) {
            if (!Engine.MyStringTable[Engine.cursorlower].dontWrap) {
                nextdown = firstup;
                candown = 1;
            }
        }
        oldcursorpos = Engine.cursorpos;
        if (Engine.cursorupper != Engine.cursorlower) {
            if ((Engine.gFrontEndPadShot & 0x10) && candown)
                Engine.cursorpos = nextdown;
            if ((Engine.gFrontEndPadShot & 0x40) && canup)
                Engine.cursorpos = nextup;
        }
        if (oldcursorpos != Engine.cursorpos)
            SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_menu2"}), &Creatures[0].Draw.Location);
        if (Engine.gFrontEndPadShot & 0x1000) {
            func = Engine.Backfunc;
            if (func) {
                SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_menu3"}), &Creatures[0].Draw.Location);
                func();
            }
        } else {
            if (MyStringTable[Engine.cursorpos].selectFunc) {
                if (MyStringTable[Engine.cursorpos].onRelease) {
                    if ((Engine.gFrontEndOPad[Pauser] & 0x4000) && !(Engine.gFrontEndPad[Pauser] & 0x4000)) {
                        func = MyStringTable[Engine.cursorpos].selectFunc;
                        SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_menu1"}), &Creatures[0].Draw.Location);
                        func();
                    }
                } else if (Engine.gFrontEndPadShot & 0x4000) {
                    func = MyStringTable[Engine.cursorpos].selectFunc;
                    SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_menu1"}), &Creatures[0].Draw.Location);
                    func();
                }
            }
        }
    }
    if (Engine.Controllerfunc)
        ((void (*)())Engine.Controllerfunc)();
    if (Engine.Lightingfunc)
        ((void (*)())Engine.Lightingfunc)();
    dbidx = 1 - dbidx;
    if (dbidx)
        pActualPoly = Engine.PolyStack0;
    else
        pActualPoly = Engine.PolyStack1;
    CHECK_FOR_BOOLEAN();
    if (Engine.Specialfunc)
        ((void (*)())Engine.Specialfunc)();
    FrontEndDrawMenu(&pActualPoly);
    if (Paused) {
        Engine.MoveBottomPoly(0, MyStringTable[Engine.cursorpos].selectFunc ? 0xd8 : 0x104);
        Engine.MoveBottomPoly(1, Engine.Backfunc ? 0xd8 : 0x104);
    } else {
        Engine.MoveBottomPoly(0, MyStringTable[Engine.cursorpos].selectFunc ? 0xdc : 0x104);
        Engine.MoveBottomPoly(1, Engine.Backfunc ? 0xdc : 0x104);
    }
    DrawBottomPoly(&pActualPoly);
    func_ppPolyFT4 = Engine.Draw256func;
    if (func_ppPolyFT4)
        func_ppPolyFT4(&pActualPoly);
}

void FrontEndDrawMenu(struct POLY_FT4 **poly)
{
    short i;
    short isCursor;
    char message[32];

    for (i = 0; i < Engine.maxcursor; i++) {
        if (Engine.currentscreen == 11)
            isCursor = i % 7 == Engine.GameConfig.MultiPlayerParams[(short)(i / 7)].cursorpos;
        else if (i < Engine.cursorlower)
            isCursor = 0;
        else
            isCursor = i == Engine.cursorpos;
        if (!MyStringTable[i].dontDraw) {
            if (isCursor) {
                MyStringTable[i].cursorItem = 1;
                FrontEndDrawText(&MyStringTable[i], poly);
                FrontEndDrawSlider(&MyStringTable[i], poly, 1);
                FrontEndDrawCursor(&MyStringTable[i], poly);
                MyStringTable[i].cursorItem = 0;
            } else {
                FrontEndDrawText(&MyStringTable[i], poly);
                FrontEndDrawCursor(&MyStringTable[i], poly);
                FrontEndDrawSlider(&MyStringTable[i], poly, 0);
            }
        }
        sprintf(MyStringTable[i].message, "%s", MyStringTable[i].originalMessage);
    }
}

void SetupScreen1by2(void (*func)());
void SetupScreen2by1(void (*func)());
void ProcessQuakePanel1by2();
void ProcessQuakePanel2by1();
void MoveQuakeCamera();
void ResetAllCameras(void);

void CHECK_FOR_SCREEN_SPLIT(void)
{
    void *WinCres[4];
    short WinAreas[4];
    short CamSpaces[4];
    short CamMoveSpaces[4];
    struct VECT CamPoss[4];
    struct SVECT CamAngs[4];
    int a;

    if (sOldScreenHorizSplit != Engine.GameConfig.bSplitHoriz && Engine.GameConfig.sNumPlayers == 2) {
        for (a = 0; a < MaxWindows; a++) {
            WinCres[a] = GameWindows[a].Creature;
            WinAreas[a] = GameWindows[a].Area;
            CamSpaces[a] = GameWindows[a].CamSpace;
            CamMoveSpaces[a] = GameWindows[a].CamMoveSpace;
            CamPoss[a] = GameWindows[a].CamPos;
            CamAngs[a] = GameWindows[a].CamAng;
        }
        sOldScreenHorizSplit = Engine.GameConfig.bSplitHoriz;
        if (Engine.GameConfig.bSplitHoriz)
            SetupScreen1by2(ProcessQuakePanel1by2);
        else
            SetupScreen2by1(ProcessQuakePanel2by1);
        for (a = 0; a < MaxWindows; a++)
            GameWindows[a].CameraRoutine = MoveQuakeCamera;
        ResetAllCameras();
        for (a = 0; a < MaxWindows; a++) {
            GameWindows[a].Creature = WinCres[a];
            GameWindows[a].Area = WinAreas[a];
            GameWindows[a].CamSpace = CamSpaces[a];
            GameWindows[a].CamMoveSpace = CamMoveSpaces[a];
            GameWindows[a].CamPos = CamPoss[a];
            GameWindows[a].CamAng = CamAngs[a];
        }
    }
}

extern struct FET_MESSAGE FET_REINSERTCONTROLLER[];
char gViolatingType;
char gViolatingPort;

void InGame_GOTO_REINSERT_CONTROLLER(int ViolatingType, int ViolatingPort, int ViolatingPlayer)
{
    char buf[32];

    gViolatingType = ViolatingType;
    gViolatingPort = ViolatingPort;
    gViolatingPlayer = ViolatingPlayer;
    InGameInitScreen(0x26);
    FrontEndCreateMenu(FET_REINSERTCONTROLLER, 0x10);
    if (gViolatingType == 3)
        Engine.sprintf(buf, "MULTI TAP INTO");
    else
        Engine.sprintf(buf, "%s INTO", gViolatingType == 1 ? "CONTROLLER" : "MOUSE");
    FrontEndAddText(&Engine.MyStringTable[Engine.cursorupper - 2], buf, 0x100, 0x78, 0x10, MaxWindows != 1 ? 4 : 0);
    Engine.sprintf(buf, "CONTROLLER PORT %s", Engine.ControlGetPortName(gViolatingPort));
    FrontEndAddText(&Engine.MyStringTable[Engine.cursorupper - 1], buf, 0x100, 0x92, 0x10, MaxWindows != 1 ? 4 : 0);
    FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
    Engine.Specialfunc = FrontEndCheckForReinsert;
}

extern struct DB *currdb;
extern short alltpages[];
extern short palette[];
void AddPrim_(void *ot, void *p);


void DrawBottomPoly(struct POLY_FT4 **poly)
{
    long *Ot;
    short i;
    struct POLY_FT4 *p;

    Ot = (long *)&currdb->BigOt[10];
    p = *poly;
    for (i = 0; i < 3; i++) {
        if (bottomPoly[i].idealY != bottomPoly[i].y) {
            if (bottomPoly[i].y < bottomPoly[i].idealY)
                bottomPoly[i].y += MIN(bottomPoly[i].idealY - bottomPoly[i].y, 3);
            else
                bottomPoly[i].y += MAX(bottomPoly[i].idealY - bottomPoly[i].y, -3);
        }
        setPolyFT4(p);
        if (Engine.currentscreen == 11 && i == 1) {
            struct BOTTOMPOLY *bp;

            /* bp is only set after x0: the original hoists 230 before &bottomPoly[1] */
            p->x0 = 0xe6;
            bp = &bottomPoly[1];
            p->y0 = 0x6e;
            p->x1 = 0xe6 + bp->w;
            p->y1 = 0x6e;
            p->x2 = 0xe6;
            p->y2 = 0x6e + bp->h;
            p->x3 = 0xe6 + bp->w;
            p->y3 = 0x6e + bp->h;
            setUVWH(p, bottomPoly[1].u, bp->v, bp->w, bp->h);
        } else {
            setXYWH(p, bottomPoly[i].x, bottomPoly[i].y, bottomPoly[i].w, bottomPoly[i].h);
            setUVWH(p, bottomPoly[i].u, bottomPoly[i].v, bottomPoly[i].w, bottomPoly[i].h);
        }
        p->r0 = 0x60, p->g0 = 0x60, p->b0 = 0x60;
        p->tpage = alltpages[13];
        p->clut = palette[69];
        AddPrim_(Ot, p++);
        *poly += 1;
    }
}

extern struct CREATURE Creatures[];
int GetSoundNum(struct IDENTITYNAME Name);
void SoundAtLocation(int Sound, struct VECT *Location);

extern char *ConfigNames[];
int IsPlayerPadShockable(int Player);

void InGame_CHECK_FOR_CONTROLLER(void)
{
    char buf[32];
    short padShot;
    int UsingMouse;
    int UsingAnalog;
    char *configSelected;
    char minConfig;
    char maxConfig;

    UsingMouse = 0;
    UsingAnalog = 1;
    padShot = Engine.gFrontEndPadShot;
    switch (GameWindows[Pauser].Input.Method) {
    case 9:
    case 10:
        minConfig = 0;
        maxConfig = 3;
        UsingMouse = 1;
        configSelected = &Engine.GameConfig.MultiPlayerParams[Pauser].mouseConfigSelected;
        break;
    case 5:
    case 7:
        minConfig = 3;
        maxConfig = 6;
        configSelected = &Engine.GameConfig.MultiPlayerParams[Pauser].analogConfigSelected;
        break;
    case 4:
    case 6:
    case 8:
    default:
        minConfig = 6;
        maxConfig = 9;
        UsingAnalog = 0;
        configSelected = &Engine.GameConfig.MultiPlayerParams[Pauser].standardConfigSelected;
        break;
    }
    if (Engine.cursorpos == Engine.cursorlower) {
        if (padShot & 0x80)
            (*configSelected)--;
        if (padShot & 0x20)
            (*configSelected)++;
    }
    if (*(signed char *)configSelected < minConfig)
        *configSelected = maxConfig - 1;
    if (*(signed char *)configSelected >= maxConfig)
        *configSelected = minConfig;
    Engine.GameConfig.MultiPlayerParams[Pauser].controllerSelected = *(signed char *)configSelected;
    if (!IsPlayerPadShockable(Pauser)) {
        if (Engine.MyStringTable[2].originalMessage[0] != 'g') {
            sprintf(buf, "g%s", Engine.MyStringTable[2].originalMessage);
            sprintf(Engine.MyStringTable[2].originalMessage, "%s", buf);
            Engine.MyStringTable[2].cursorDisabled = 1;
            if (Engine.cursorpos == 2)
                Engine.cursorpos--;
        }
    } else if (Engine.MyStringTable[2].originalMessage[0] == 'g') {
        sprintf(buf, "%s", Engine.MyStringTable[2].originalMessage + 1);
        sprintf(Engine.MyStringTable[2].originalMessage, "%s", buf);
        Engine.MyStringTable[2].cursorDisabled = 0;
    }
    if (!UsingAnalog) {
        if (Engine.MyStringTable[3].originalMessage[0] != 'g') {
            sprintf(buf, "g%s", Engine.MyStringTable[3].originalMessage);
            sprintf(Engine.MyStringTable[3].originalMessage, "%s", buf);
            Engine.MyStringTable[3].cursorDisabled = 1;
            if (Engine.cursorpos == 3)
                Engine.cursorpos--;
        }
    } else if (Engine.MyStringTable[3].originalMessage[0] == 'g') {
        sprintf(buf, "%s", Engine.MyStringTable[3].originalMessage + 1);
        sprintf(Engine.MyStringTable[3].originalMessage, "%s", buf);
        Engine.MyStringTable[3].cursorDisabled = 0;
    }
    if (!UsingMouse && !Engine.gFreeMice) {
        if (Engine.MyStringTable[4].originalMessage[0] != 'g') {
            Engine.sprintf(buf, "g%s", Engine.MyStringTable[4].originalMessage);
            Engine.sprintf(Engine.MyStringTable[4].originalMessage, "%s", buf);
            Engine.MyStringTable[4].cursorDisabled = 1;
            if (Engine.cursorpos == 4)
                Engine.cursorpos--;
        }
    } else if (Engine.MyStringTable[4].originalMessage[0] == 'g') {
        Engine.sprintf(buf, "%s", Engine.MyStringTable[4].originalMessage + 1);
        Engine.sprintf(Engine.MyStringTable[4].originalMessage, "%s", buf);
        Engine.MyStringTable[4].cursorDisabled = 0;
    }
    if (!UsingMouse) {
        sprintf(buf, "%c%s", 'g', Engine.MyStringTable[5].message);
        FrontEndModifyText(&Engine.MyStringTable[5], buf);
        MyStringTable[5].cursorDisabled = 1;
        if (Engine.cursorpos == 5) {
            Engine.cursorpos--;
            if (!Engine.gFreeMice)
                Engine.cursorpos--;
        }
    } else {
        Engine.MyStringTable[5].cursorDisabled = 0;
    }
    Engine.FrontEndModifyText(&Engine.MyStringTable[Engine.cursorlower], ConfigNames[*(signed char *)configSelected]);
    CHECK_FOR_SLIDER();
}

void CHECK_FOR_SLIDER(void)
{
    short *pSlider;
    short i;
    short musicsliderfound;
    short pad;
    static int SoundTimer = -1;

    musicsliderfound = -1;
    if (!Paused)
        pad = Engine.gFrontEndPad[0];
    else
        pad = Engine.gFrontEndPad[Pauser];
    for (i = 0; i < Engine.maxcursor; i++) {
        if (MyStringTable[i].pSliderVar) {
            pSlider = MyStringTable[i].pSliderVar;
            if (i == Engine.cursorpos) {
                if (pSlider != &Engine.GameConfig.sSoundFX)
                    SoundTimer = 0;
                if (pad & 0x80) {
                    *pSlider -= 2;
                    if (SoundTimer < 0) {
                        SoundTimer = 8;
                        SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_comp_up"}), &Creatures[0].Draw.Location);
                    }
                }
                if (pad & 0x20) {
                    *pSlider += 2;
                    if (SoundTimer < 0) {
                        SoundTimer = 8;
                        SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_comp_up"}), &Creatures[0].Draw.Location);
                    }
                }
                if (*pSlider < 0)
                    *pSlider = 0;
                if (*pSlider >= 0x80)
                    *pSlider = 0x7f;
            }
            if (pSlider == &Engine.GameConfig.sMusic)
                musicsliderfound = i;
        }
    }
    if (musicsliderfound >= 0) {
        if (musicsliderfound == Engine.cursorpos)
            targetcdvol = Engine.GameConfig.sMusic * 2;
        else
            targetcdvol = 0;
    }
    SoundTimer--;
}



static void FrontEndDrawBox(struct POLY_FT4 **poly, short x, short y, short w, short h, short r, short g, short b, short window)
{
    int NumWins;
    long *Ot;
    struct POLY_FT4 *pActualPoly;
    struct LINE_F2 *l;

    pActualPoly = *poly;
    NumWins = 1;
    if (!Paused)
        NumWins = MaxWindows;
    if (NumWins >= 2)
        Ot = (long *)&currdb->Ot[window][50];
    else
        Ot = (long *)&currdb->BigOt[7];
    l = (struct LINE_F2 *)pActualPoly;
    setLineF2(l);
    setXY2(l, x, y, x + w - 1, y);
    setRGB0(l, r, g, b);
    AddPrim_(Ot, l);
    pActualPoly++;
    *poly += 1;
    l = (struct LINE_F2 *)pActualPoly;
    setLineF2(l);
    setXY2(l, x, y + h - 1, x + w - 1, y + h - 1);
    setRGB0(l, r, g, b);
    AddPrim_(Ot, l);
    pActualPoly++;
    *poly += 1;
    l = (struct LINE_F2 *)pActualPoly;
    setLineF2(l);
    setXY2(l, x, y, x, y + h - 1);
    setRGB0(l, r, g, b);
    AddPrim_(Ot, l);
    pActualPoly++;
    *poly += 1;
    l = (struct LINE_F2 *)pActualPoly;
    setLineF2(l);
    setXY2(l, x + w - 1, y, x + w - 1, y + h - 1);
    setRGB0(l, r, g, b);
    AddPrim_(Ot, l);
    pActualPoly++;
    *poly += 1;
}

void FrontEndDrawSlider(struct STRING_TABLE *fetString, struct POLY_FT4 **poly, short isCursor)
{
    short x;
    short y;
    long *Ot;
    int NumWins;
    short r;
    short g;
    short b;
    struct POLY_F4 *f4;
    int greyed;

    NumWins = 1;
    if (!Paused)
        NumWins = MaxWindows;
    if (!fetString->pSliderVar)
        return;
    greyed = fetString->message[0] == 'g';
    if (greyed) {
        b = g = r = 0x20;
    } else if (isCursor) {
        b = g = r = 0xff;
    } else {
        r = 0x4a;
        g = 0x8f;
        b = 0xaa;
    }
    if (NumWins >= 2)
        Ot = (long *)&currdb->Ot[fetString->window][50];
    else
        Ot = (long *)&currdb->BigOt[7];
    x = fetString->sliderX;
    y = fetString->y - 4;
    FrontEndDrawBox(poly, x, y, 0x85, 8, r, g, b, fetString->window);
    f4 = (struct POLY_F4 *)*poly;
    setPolyF4(f4);
    setXY4(f4, x + 3, y + 2, x + 3 + *fetString->pSliderVar, y + 2, x + 3, y + 6, x + 3 + *fetString->pSliderVar, y + 6);
    setRGB0(f4, r, g, b);
    AddPrim_(Ot, f4);
    *poly += 1;
}

void CHECK_FOR_BOOLEAN(void)
{
    short on;
    short i;
    short padShot;
    char buf[32];

    padShot = Engine.gFrontEndPadShot;
    for (i = 0; i < Engine.maxcursor; i++) {
        if (MyStringTable[i].pBoolVar) {
            if (i == Engine.cursorpos) {
                if (padShot & 0x80) {
                    if (*MyStringTable[i].pBoolVar != 1)
                        SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"itm_pkup"}), &Creatures[0].Draw.Location);
                    *MyStringTable[i].pBoolVar = 1;
                }
                if (padShot & 0x20) {
                    if (*MyStringTable[i].pBoolVar)
                        SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"itm_pkup"}), &Creatures[0].Draw.Location);
                    *MyStringTable[i].pBoolVar = 0;
                }
                on = *MyStringTable[i].pBoolVar;
                sprintf(buf, "b%s %sON %sOFF", MyStringTable[i].message, *MyStringTable[i].pBoolVar ? "b" : "d", on ? "d" : "b");
                FrontEndModifyText(&MyStringTable[i], buf);
            } else {
                sprintf(buf, "%s %s", MyStringTable[i].message, *MyStringTable[i].pBoolVar ? "ON" : "OFF");
                FrontEndModifyText(&MyStringTable[i], buf);
            }
        }
    }
}

int gScrollPixelIndex;
struct DRAWENV LoadDoClipDrawEnvs[2];
struct DRAWENV LoadUnClipDrawEnvs[2];
extern unsigned char DbFrame[];
struct DRAWENV *SetDefDrawEnv(struct DRAWENV *env, int x, int y, int w, int h);
void SetDrawEnv(struct DR_ENV *dr_env, struct DRAWENV *env);
void DrawBorder(struct RECT Rect, long *Ot, int OffsetY);

void CHECK_FOR_MCFILEWINDOW(void)
{
    int DestPixelIndex;
    int i;
    int topFileIndex;
    int TotalFiles;
    int MaxWidth; /* never initialised in the original either */
    int TempWidth;
    int Gap;
    int OffsetY;
    char *pChar;
    static short TopFileId;
    static short ScrollPos;
    struct SCRPNT Pos;
    struct DRAWENV *ThisDrawEnv;
    struct DRAWENV *ThisDrawEnv2;

    OffsetY = 0;
    if (Engine.currentscreen == 6)
        OffsetY = 0x45;
    topFileIndex = Engine.cursorpos - Engine.cursorlower;
    TotalFiles = Engine.cursorupper - Engine.cursorlower;
    if (TotalFiles >= 4) {
        if (topFileIndex > TotalFiles - 3)
            topFileIndex = TotalFiles - 3;
        DestPixelIndex = 0x69 - topFileIndex * 16;
        Gap = 16;
    } else if (TotalFiles == 3) {
        DestPixelIndex = 0x69;
        Gap = 16;
    } else {
        DestPixelIndex = 0x31 - TotalFiles * 16;
        DestPixelIndex /= TotalFiles + 1;
        Gap = DestPixelIndex + 16;
        DestPixelIndex += 0x68;
    }
    for (i = 0; i < TotalFiles; i++) {
        TempWidth = 0;
        pChar = MyStringTable[i + Engine.cursorlower].originalMessage;
        while (*pChar++)
            TempWidth++;
        if (MyStringTable[i + Engine.cursorlower].pBoolVar)
            TempWidth += 7;
        if (MyStringTable[i + Engine.cursorlower].pSliderVar)
            TempWidth += 11;
        if (TempWidth > MaxWidth)
            MaxWidth = TempWidth;
    }
    MaxWidth += 3;
    MaxWidth *= 16;
    if (gScrollPixelIndex < DestPixelIndex)
        gScrollPixelIndex += 4;
    if (gScrollPixelIndex > DestPixelIndex)
        gScrollPixelIndex -= 4;
    for (i = 0; i < TotalFiles; i++) {
        MyStringTable[i + Engine.cursorlower].y = gScrollPixelIndex + i * Gap;
        if (MyStringTable[i + Engine.cursorlower].y + (Gap >> 1) < 0x60 || MyStringTable[i + Engine.cursorlower].y - (Gap >> 1) > 0x90)
            MyStringTable[i + Engine.cursorlower].dontDraw = 1;
        else
            MyStringTable[i + Engine.cursorlower].dontDraw = 0;
    }
    if (DbFrame[0]) {
        ThisDrawEnv = &LoadDoClipDrawEnvs[0];
        ThisDrawEnv2 = &LoadUnClipDrawEnvs[0];
        Pos = (struct SCRPNT){ScreenSize.sx + GameWindows[4].Pos.sx, GameWindows[4].Pos.sy};
    } else {
        ThisDrawEnv = &LoadDoClipDrawEnvs[1];
        Pos = (struct SCRPNT){GameWindows[4].Pos.sx, GameWindows[4].Pos.sy};
        ThisDrawEnv2 = &LoadUnClipDrawEnvs[1];
    }
    SetDefDrawEnv(ThisDrawEnv, Pos.sx + ((0x200 - MaxWidth) >> 1), Pos.sy + OffsetY + 0x60, MaxWidth, 0x31);
    SetDefDrawEnv(ThisDrawEnv2, Pos.sx, Pos.sy, 0x200, 0xf0);
    ThisDrawEnv->ofs[0] -= ((0x200 - MaxWidth) >> 1);
    ThisDrawEnv->ofs[1] -= 0x60;
    ThisDrawEnv->isbg = 0;
    ThisDrawEnv2->isbg = 0;
    SetDrawEnv(&ThisDrawEnv->dr_env, ThisDrawEnv);
    SetDrawEnv(&ThisDrawEnv2->dr_env, ThisDrawEnv2);
    AddPrim_(&currdb->BigOt[6], &ThisDrawEnv->dr_env);
    AddPrim_(&currdb->BigOt[8], &ThisDrawEnv2->dr_env);
    if (!Paused && Engine.currentscreen != 6)
        DrawBorder((struct RECT){((0x200 - MaxWidth) >> 1), 0x60, MaxWidth, 0x31}, (long *)&currdb->BigOt[10], OffsetY);
}

extern struct FET_MESSAGE FET_LOADSAVE_CHOOSEFILE[];
char *MemoryCardGetFilename(long index);
long MemoryCardGetNumFreeBlocks(void);

extern struct FET_MESSAGE FET_LOADSAVE[];
extern struct FET_MESSAGE FET_LOADSAVE2[];
extern struct FET_MESSAGE FET_LOADSAVE_CHOOSECARD[];
extern struct FET_MESSAGE FET_LOADSAVE_NOTFORMATTED[];
extern struct FET_MESSAGE FET_LOADSAVE_NOTFORMATTED_YESNO[];
extern struct FET_MESSAGE FET_LOADSAVE_DONOTINSERT[];
extern struct FET_MESSAGE FET_SAVEDCONTINUE[];
extern struct FET_MESSAGE FET_SAVE_OVERWRITE[];
extern struct FET_MESSAGE FET_SAVE_OVERWRITE_YESNO[];
void *gOldMemcardBackFunc;
void FrontEnd_DoFileWindow(void);

void InGame_PrintMemCardMessage(long status)
{
    short i;
    char buf[24];

    if (status != 3 && status != 13) {
        Engine.MoveBottomPoly(0, 0x104);
        Engine.MoveBottomPoly(1, 0x104);
    } else if (Paused) {
        Engine.MoveBottomPoly(0, 0xd8);
        Engine.MoveBottomPoly(1, 0xd8);
    } else {
        Engine.MoveBottomPoly(0, 0xdc);
        Engine.MoveBottomPoly(1, 0xdc);
    }
    Engine.FrontEndInitText();
    Engine.cursorupper = 0;
    Engine.cursorlower = 0;
    switch (status) {
    case 19:
        Engine.FrontEndCreateMenu(FET_SAVE_OVERWRITE, 0x10);
        Engine.FrontEndCreateMenu(FET_SAVE_OVERWRITE_YESNO, 0x10);
        Engine.MoveBottomPoly(0, 0xdc);
        Engine.MoveBottomPoly(1, 0xdc);
        break;
    case 1:
        if (Engine.gFrontEndLoadType == 1 && Engine.gFrontEndLoadSubType == 1)
            Engine.Backfunc = InGame_GOTO_WANT_TO_SAVE;
        gOldMemcardBackFunc = Engine.Backfunc;
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "CHECKING FOR");
        Engine.sprintf(Engine.MyStringTable[1].message, "MEMORY CARDS");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        break;
    case 3:
        Engine.FrontEndCreateMenu(FET_LOADSAVE_CHOOSECARD, 0x10);
        i = 0;
        Engine.MyStringTable[0].window = Engine.MyStringTable[1].window = 4;
        Engine.cursorlower = Engine.cursorupper;
        while (i < Engine.MemoryCardGetNumCards()) {
            Engine.MemoryCardGetCardname(i, buf);
            Engine.FrontEndAddText(&Engine.MyStringTable[i + Engine.cursorlower], buf, 0x100, (short)(i * 16 + 100), 0x10, 0);
            Engine.MyStringTable[i + Engine.cursorlower].dontDraw = 0;
            Engine.MyStringTable[i + Engine.cursorlower].selectFunc = InGame_GOTO_NOWHERE;
            Engine.MyStringTable[i + Engine.cursorlower].bannerColour = 0;
            Engine.MyStringTable[i + Engine.cursorlower].dontWrap = 1;
            i++;
            Engine.cursorupper++;
            Engine.maxcursor++;
        }
        break;
    case 4:
    case 10:
    case 11:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "CHECKING FOR");
        Engine.sprintf(Engine.MyStringTable[1].message, "QUAKE 2 FILES");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        break;
    case 2:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "NO MEMORY");
        Engine.sprintf(Engine.MyStringTable[1].message, "CARDS FOUND");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        break;
    case 9:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "MEMORY CARD");
        Engine.sprintf(Engine.MyStringTable[1].message, "FORMAT FAILED");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        break;
    case 17:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "MEMORY CARD");
        Engine.sprintf(Engine.MyStringTable[1].message, "%s FAILED", !Engine.gFrontEndLoadType ? "LOAD" : "SAVE");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        Engine.Backfunc = gOldMemcardBackFunc;
        break;
    case 16:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.Backfunc = 0;
        if (!Engine.gFrontEndLoadType) {
            if (!Engine.gFrontEndLoadSubType) {
                Engine.FrontEndCreateMenu(FET_SAVEDCONTINUE, 0x10);
                Engine.sprintf(Engine.MyStringTable[0].message, "MULTIPLAYER");
                Engine.sprintf(Engine.MyStringTable[1].message, "CONFIG LOADED");
            } else {
                Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
                Engine.sprintf(Engine.MyStringTable[0].message, "STARTING");
                Engine.sprintf(Engine.MyStringTable[1].message, "GAME");
            }
        } else {
            if (!Engine.gFrontEndLoadSubType) {
                Engine.FrontEndCreateMenu(FET_SAVEDCONTINUE, 0x10);
                Engine.sprintf(Engine.MyStringTable[0].message, "MULTIPLAYER");
                Engine.sprintf(Engine.MyStringTable[1].message, "DATA SAVED");
            } else {
                Engine.FrontEndCreateMenu(FET_SAVEDCONTINUE, 0x10);
                Engine.sprintf(Engine.MyStringTable[0].message, "GAME");
                Engine.sprintf(Engine.MyStringTable[1].message, "SAVED");
            }
            Engine.MoveBottomPoly(0, 0xdc);
        }
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        Engine.CantUnPause = 0;
        break;
    case 5:
        Engine.FrontEndCreateMenu(FET_LOADSAVE_NOTFORMATTED, 0x10);
        Engine.FrontEndCreateMenu(FET_LOADSAVE_NOTFORMATTED_YESNO, 0x10);
        break;
    case 6:
        Engine.FrontEndCreateMenu(FET_LOADSAVE_DONOTINSERT, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "FORMATTING");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        break;
    case 8:
        Engine.FrontEndCreateMenu(FET_LOADSAVE, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "MEMORY CARD");
        Engine.sprintf(Engine.MyStringTable[1].message, "FORMAT COMPLETE");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        Engine.MoveBottomPoly(0, 0xdc);
        break;
    case 12:
        Engine.FrontEndCreateMenu(FET_LOADSAVE2, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        if (Engine.gFrontEndLoadSubType == 1) {
            Engine.sprintf(Engine.MyStringTable[0].message, "NO QUAKE 2");
            Engine.sprintf(Engine.MyStringTable[1].message, "SINGLE PLAYER");
            Engine.sprintf(Engine.MyStringTable[2].message, "FILES FOUND");
        } else {
            Engine.sprintf(Engine.MyStringTable[0].message, "NO QUAKE 2");
            Engine.sprintf(Engine.MyStringTable[1].message, "MULTIPLAYER");
            Engine.sprintf(Engine.MyStringTable[2].message, "FILES FOUND");
        }
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[2], Engine.MyStringTable[2].message);
        break;
    case 18:
        Engine.FrontEndCreateMenu(FET_LOADSAVE2, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, "INSUFFICIENT FREE BLOCKS");
        Engine.sprintf(Engine.MyStringTable[1].message, "PLEASE INSERT ANOTHER");
        Engine.sprintf(Engine.MyStringTable[2].message, "MEMORY CARD");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
        Engine.FrontEndModifyText(&Engine.MyStringTable[2], Engine.MyStringTable[2].message);
        break;
    case 13:
        FrontEnd_DoFileWindow();
        break;
    case 14:
    case 15:
    case 20:
    case 21:
    case 22:
        Engine.FrontEndCreateMenu(FET_LOADSAVE_DONOTINSERT, 0x10);
        Engine.FrontEndCreateMenu(FET_EMPTYDIALOG2, 0x10);
        Engine.sprintf(Engine.MyStringTable[0].message, !Engine.gFrontEndLoadType ? "LOADING FROM" : "SAVING TO");
        Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
        Engine.Backfunc = 0;
        for (i = 0; i < 4; i++)
            Engine.MyStringTable[i].window = 4;
        break;
    case 7:
        break;
    }
    Engine.cursorpos = Engine.cursorlower;
}

void FrontEnd_DoFileWindow(void)
{
    int i;
    int NumAdded;
    char buf[16];

    gScrollPixelIndex = 0x69;
    Engine.FrontEndCreateMenu(FET_LOADSAVE_CHOOSEFILE, 0x20);
    NumAdded = 0;
    if (!Engine.gFrontEndLoadType) {
        if (!Engine.gFrontEndLoadSubType)
            sprintf(Engine.MyStringTable[0].message, "LOAD MP SETUP");
        else
            sprintf(Engine.MyStringTable[0].message, "LOAD SINGLE");
    } else {
        sprintf(Engine.MyStringTable[0].message, "SAVE FILE");
    }
    Engine.FrontEndModifyText(&Engine.MyStringTable[0], Engine.MyStringTable[0].message);
    sprintf(Engine.MyStringTable[1].message, "");
    Engine.FrontEndModifyText(&Engine.MyStringTable[1], Engine.MyStringTable[1].message);
    Engine.MyStringTable[1].bannerColour = 4;
    sprintf(Engine.MyStringTable[2].message, "RESUPPLIES");
    Engine.FrontEndModifyText(&Engine.MyStringTable[2], Engine.MyStringTable[2].message);
    sprintf(Engine.MyStringTable[3].message, "DIFFICULTY");
    Engine.FrontEndModifyText(&Engine.MyStringTable[3], Engine.MyStringTable[3].message);
    sprintf(Engine.MyStringTable[4].message, "CHEATS");
    Engine.FrontEndModifyText(&Engine.MyStringTable[4], Engine.MyStringTable[4].message);
    Engine.MyStringTable[2].window = Engine.MyStringTable[3].window = Engine.MyStringTable[4].window = 4;
    Engine.MyStringTable[2].fontSize = Engine.MyStringTable[3].fontSize = Engine.MyStringTable[4].fontSize = 16;
    Engine.MyStringTable[4].dontDraw = 1;
    Engine.MyStringTable[0].cursorItem = 1;
    Engine.MyStringTable[2].cursorItem = 1;
    Engine.MyStringTable[3].cursorItem = 1;
    Engine.MyStringTable[4].cursorItem = 1;
    Engine.MyStringTable[2].bannerColour = 4;
    Engine.MyStringTable[3].bannerColour = 4;
    Engine.MyStringTable[4].bannerColour = 4;
    Engine.cursorlower = Engine.cursorupper;
    for (i = 0; i < Engine.MemoryCardGetNumFiles(); i++) {
        MemoryCardGetFilename(i);
        Engine.FrontEndAddText(&Engine.MyStringTable[NumAdded + Engine.cursorlower], Engine.MemoryCardGetFilename(i), 0x100, (short)(NumAdded * 16 + 100), 0x10, 0);
        Engine.MyStringTable[NumAdded + Engine.cursorlower].selectFunc = InGame_GOTO_NOWHERE;
        Engine.MyStringTable[NumAdded + Engine.cursorlower].bannerColour = 0;
        NumAdded++;
        Engine.cursorupper++;
        Engine.maxcursor++;
    }
    if (Engine.gFrontEndLoadType == 1 && MemoryCardGetNumFreeBlocks() > 0) {
        Engine.MyStringTable[NumAdded + Engine.cursorlower].dontWrap = 1;
        Engine.FrontEndAddText(&Engine.MyStringTable[NumAdded + Engine.cursorlower], "aaaNEW FILE", 0x100, (short)(NumAdded * 16 + 100), 0x10, 0);
        Engine.MyStringTable[NumAdded + Engine.cursorlower].selectFunc = InGame_GOTO_NOWHERE;
        NumAdded++;
        Engine.cursorupper++;
        Engine.maxcursor++;
    }
    if (NumAdded >= 4) {
        for (i = 0; i < NumAdded; i++)
            Engine.MyStringTable[i + Engine.cursorlower].dontWrap = 1;
    }
    Engine.MoveBottomPoly(0, 0xdc);
}
