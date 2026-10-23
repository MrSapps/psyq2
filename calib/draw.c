#include "types.h"
extern int MaxWindows;
extern struct WINDOW GameWindows[];
extern unsigned short tptranstable[];
extern unsigned short allpals[];
extern unsigned short tppalx[20];
extern unsigned short tppaly[];
extern int MYSFXInit(void);

extern int *AllIffPics;
extern int drawbgflag;
int drawbgflag2;
extern short movedispflag2;
extern short dispx;
extern short dispy;
short focal;
extern unsigned char DbFrame[];
extern unsigned char TbFrame[];
extern struct DB *currdb;
extern struct SCRPNT ScreenSize;
extern struct DB db[];
void SetVideoMode(long mode);
void InitGeom(void);
void SetGeomOffset(long ofx, long ofy);
void SetGeomScreen(long h);
void SetupDrawDisplay3D(void);
void SetUpSemiTrans(void);
void SetDispMask(int mask);
int VSync(int mode);
struct DRAWENV *PutDrawEnv(struct DRAWENV *env);
struct DISPENV *PutDispEnv(struct DISPENV *env);
int loadtp16b_ConCat(char *name, int tpnum, int skipheight);
void InitLevPals(void);

void InitialiseDrawNewGame(void)
{
    SetVideoMode(0);
    ScreenSize.sx = 512;
    ScreenSize.sy = 240;
    movedispflag2 = 0;
    dispx = 0;
    dispy = 0;
    InitGeom();
    SetGeomOffset(ScreenSize.sx / 2, ScreenSize.sy / 2);
    SetGeomScreen(focal);
    DbFrame[0] = 0;
    DbFrame[1] = 1;
    TbFrame[0] = 0;
    TbFrame[1] = 1;
    TbFrame[2] = 2;
    SetupDrawDisplay3D();
    SetUpSemiTrans();
    SetDispMask(1);
    currdb = &db[DbFrame[0]];
    VSync(0);
    PutDrawEnv(&currdb->DrawEnv[0]);
    PutDispEnv(&currdb->DispEnv);
}

void DownloadTextures(void)
{
    if (AllIffPics) {
        drawbgflag = 0;
        drawbgflag2 = 0;
        loadtp16b_ConCat("tp0.opt", 0, 0);
        loadtp16b_ConCat("tp1.opt", 1, 0);
        loadtp16b_ConCat("tp2.opt", 2, 0);
        loadtp16b_ConCat("tp3.opt", 3, 0);
        loadtp16b_ConCat("tp4.opt", 4, 0);
        loadtp16b_ConCat("tp5.opt", 5, 0);
        loadtp16b_ConCat("tp6.opt", 6, 0);
        loadtp16b_ConCat("tp7.opt", 7, 0);
        loadtp16b_ConCat("tp8.opt", 8, 0);
        loadtp16b_ConCat("tp9.opt", 9, 0);
        loadtp16b_ConCat("tp10.opt", 10, 0);
        loadtp16b_ConCat("tp11.opt", 11, 0);
        loadtp16b_ConCat("tp12.opt", 12, 0);
        InitLevPals();
    }
}

extern int ObjSortIndx;
extern short SceneValid;
extern int ChrisCount;
extern void (*VBlankSpecific)();
int ClearingNecessary;
extern struct MATRIX *CurrCamMat;
extern struct WINDOW *CurrWin;
extern struct VECT *CurrCamPos;
extern unsigned long *CurrOt;
extern int CurrWinSizeX;
int TempTime;
int DrawModTime;
long GetRCnt();
void DrawModels(int Win);
void DrawHeavens(void);
void InitialiseScreenIndxStuff(void);
void ResetSceneValidates(void);
void DrawWinScene(int Win);
void InitCrossEveryFrame(void);
void PreClearScreen(void);

void DrawAllModels(void)
{
    int Win;

    for (Win = 0; Win < MaxWindows; Win++) {
        CurrWin = &GameWindows[Win];
        CurrCamMat = &CurrWin->CamMat;
        CurrCamPos = &CurrWin->CamPos;
        CurrOt = currdb->Ot[Win];
        CurrWinSizeX = CurrWin->Size.sx;
        SetGeomOffset(CurrWin->Geom.sx, CurrWin->Geom.sy);
        SetGeomScreen(CurrWin->Focal);
        TempTime = GetRCnt(0xf2000001);
        if (CurrWin->ControlBits & 2)
            DrawModels(Win);
        DrawModTime += GetRCnt(0xf2000001) - TempTime;
    }
    DrawHeavens();
}

static void ClearSkyFlags(void)
{
    int a;

    for (a = 0; a < MaxWindows; a++)
        GameWindows[a].SkyClear = 0;
}

void DrawAllScenes(void)
{
    int a;

    ObjSortIndx = 0;
    InitialiseScreenIndxStuff();
    if (SceneValid && !(ChrisCount & 0x7fff))
        ResetSceneValidates();
    if (VBlankSpecific)
        ClearSkyFlags();
    else if (ClearingNecessary) {
        PreClearScreen();
        ClearSkyFlags();
    }
    for (a = 0; a < MaxWindows; a++)
        DrawWinScene(a);
    InitCrossEveryFrame();
}

void SetUpSemiTrans(void)
{
    int a;

    for (a = 0; a < 4; a++)
        tptranstable[a + 1] = a << 5;
    tptranstable[0] = 32;
}

void InitCluts(void)
{
    int a;

    for (a = 0; a < sizeof(tppalx) / sizeof(tppalx[0]); a++)
        allpals[a] = (tppaly[a] << 6) | ((tppalx[a] >> 4) & 0x3f);
}

void InitHeaps(void)
{
    MYSFXInit();
}

void ProcessAllPanels(void)
{
    int a;

    for (a = 0; a < MaxWindows; a++)
        if (GameWindows[a].PanelRoutine)
            GameWindows[a].PanelRoutine(&GameWindows[a]);
}

void SetWinFade(int Win, struct COLOUR Colour, short FadeType, int Time, int InitTime)
{
    struct WINDOW *Window;

    Window = &GameWindows[Win];
    Window->FadeTime = Time;
    Window->FadeInitTime = InitTime;
    Window->FadeColour = Colour;
    Window->FadeType = FadeType;
}

void AlterFocalSubDiv(int Win, int Focal, int SubDivDist)
{
    GameWindows[Win].Focal = Focal;
    GameWindows[Win].SubDivDist = SubDivDist;
}

extern short alltpages[];
extern short tpscreenx[20];
extern short tpscreeny[];
extern unsigned char DbFrame[];
extern struct DB *currdb;
extern struct SCRPNT ScreenSize;
struct SCRPNT ScreenDisplay[2];
struct COLOUR SkyColour;
extern struct DB db[];
unsigned short GetTPage(int tp, int abr, int x, int y);
struct DRAWENV *SetDefDrawEnv(struct DRAWENV *env, int x, int y, int w, int h);
void SetDrawEnv(struct DR_ENV *dr_env, struct DRAWENV *env);
void AddPrim_(void *ot, void *p);
struct DISPENV *SetDefDispEnv(struct DISPENV *env, int x, int y, int w, int h);
void Config_InstallVideo(void);
int ClearImage(struct RECT *rect, unsigned char r, unsigned char g, unsigned char b);
int DrawSync(int mode);
int VSync(int mode);
struct DISPENV *PutDispEnv(struct DISPENV *env);

void PreClearScreen(void)
{
    SetDefDrawEnv(&currdb->PreClearEnv, ScreenDisplay[DbFrame[0]].sx, ScreenDisplay[DbFrame[0]].sy, ScreenSize.sx, ScreenSize.sy);
    currdb->PreClearEnv.isbg = 1;
    currdb->PreClearEnv.r0 = SkyColour.r;
    currdb->PreClearEnv.g0 = SkyColour.g;
    currdb->PreClearEnv.b0 = SkyColour.b;
    SetDrawEnv(&currdb->PreClearEnv.dr_env, &currdb->PreClearEnv);
    AddPrim_(&currdb->InitOt[1], &currdb->PreClearEnv.dr_env);
}

void SetupDrawDisplay3D(void)
{
    SetDefDispEnv(&db[0].DispEnv, ScreenSize.sx, 0, ScreenSize.sx, ScreenSize.sy);
    SetDefDispEnv(&db[1].DispEnv, 0, 0, ScreenSize.sx, ScreenSize.sy);
    Config_InstallVideo();
    ClearImage(&db[0].DispEnv.disp, 0, 0, 0);
    ClearImage(&db[1].DispEnv.disp, 0, 0, 0);
    DrawSync(0);
    VSync(0);
    PutDispEnv(&db[0].DispEnv);
    db[0].DrawEnv[0].isbg = 1;
    db[1].DrawEnv[0].isbg = 1;
    db[0].DrawEnv[0].r0 = 0;
    db[0].DrawEnv[0].g0 = 0;
    db[0].DrawEnv[0].b0 = 0;
    db[1].DrawEnv[0].r0 = 0;
    db[1].DrawEnv[0].g0 = 0;
    db[1].DrawEnv[0].b0 = 0;
}

void InitTpages(void)
{
    int x;

    for (x = 0; x < sizeof(tpscreenx) / sizeof(tpscreenx[0]); x++)
        alltpages[x] = GetTPage(0, 0, tpscreenx[x], tpscreeny[x]);
}

void *memset();
void SetGeomOffset(long ofx, long ofy);
void SetGeomScreen(long h);
void SetRotMatrix(struct MATRIX *m);

/* inline_c.h */
#define gte_SetTransMatrix(r0) __asm__ volatile (	\
	"move	$12,%0\n"			\
	"lw	$13,20($12)\n"			\
	"lw	$14,24($12)\n"			\
	"ctc2	$13,$5\n"			\
	"lw	$15,28($12)\n"			\
	"ctc2	$14,$6\n"			\
	"ctc2	$15,$7"				\
	:					\
	: "r"( r0 )				\
	: "$12", "$13", "$14", "$15")

void SetGeomStuffWin(int Win)
{
    struct SVECTOR NullVec;

    memset(&NullVec, 0, sizeof(NullVec));
    SetGeomOffset(GameWindows[Win].Geom.sx, GameWindows[Win].Geom.sy);
    SetGeomScreen(GameWindows[Win].Focal);
    SetRotMatrix(&GameWindows[Win].CamMat);
    gte_SetTransMatrix((struct MATRIX *)((char *)&NullVec - 20));
}

void SetupScreenHeavens(void)
{
    GameWindows[4].Size = ScreenSize;
    GameWindows[4].StandardSize = GameWindows[4].Size;
    GameWindows[4].ModelFt4Dist = 0x2000;
    GameWindows[4].Geom = (struct SCRPNT){GameWindows[4].Size.sx / 2, GameWindows[4].Size.sy / 2};
    GameWindows[4].ConeScale = (GameWindows[4].Size.sx << 9) / ScreenSize.sy;
    GameWindows[4].Focal = 320;
    memset(&GameWindows[4].Pos, 0, sizeof(struct SCRPNT));
    GameWindows[4].SkyColour = SkyColour;
    GameWindows[4].SkyClear = 0;
}

