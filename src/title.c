#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void InitialiseVBlankLoading(void); void TestIt(char *Name);
void VBlankLoading(void);
void InitialiseVBlankLoading(void);

extern struct DB *olddb;
extern struct DB *currdb;
extern struct DB db[];
extern unsigned char DbFrame[];
extern char FinishedCutScene;
extern char WaitingCutScene;
extern char InitialisedCutScene;
extern int UseCdData;
extern short SceneValid;
extern struct GLOBMODHDR *CastList;

extern int VDbFrame;
extern struct COLOUR SkyColour;
extern char SkyClear;
extern int MaxWindows;
extern void (*LevelSpecific)();
extern void (*VBlankSpecific)();
extern struct WINDOW GameWindows[];
void QuakeLoadingSpecific(void);
void QuakeCameraLoading(int Win);
void VBlankLoading(void);
struct DRAWENV *SetDefDrawEnv(struct DRAWENV *env, int x, int y, int w, int h);
struct DISPENV *SetDefDispEnv(struct DISPENV *env, int x, int y, int w, int h);

void VBlankLoading(void)
{
    DoubleTrebleBuffer();
    PutDispEnv(&olddb->DispEnv);
    DrawOTag(olddb->Ot[0]);
    ClearOTag(currdb->Ot[0], 50);
    VBlankMainLoop();
}

void InitialiseVBlankLoading(void)
{
    int a;

    VDbFrame = 1;
    ClearOTag(db[0].Ot[0], 3);
    ClearOTag(db[1].Ot[0], 3);
    SetDefDrawEnv(&db[0].DrawEnv[0], 0, 0, 512, 256);
    SetDefDrawEnv(&db[1].DrawEnv[0], 512, 0, 512, 256);
    SetDefDispEnv(&db[0].DispEnv, 512, 0, 512, 256);
    SetDefDispEnv(&db[1].DispEnv, 0, 0, 512, 256);
    db[0].DispEnv.screen.y = 0;
    db[1].DispEnv.screen.y = 0;
    db[0].DrawEnv[0].isbg = 0;
    db[1].DrawEnv[0].isbg = 0;
    db[0].DrawEnv[0].isbg = 1;
    db[1].DrawEnv[0].isbg = 1;
    db[0].DrawEnv[0].r0 = 255;
    db[0].DrawEnv[0].g0 = 8;
    db[0].DrawEnv[0].b0 = 32;
    db[1].DrawEnv[0].r0 = 8;
    db[1].DrawEnv[0].g0 = 255;
    db[1].DrawEnv[0].b0 = 32;
    InitEveryLevel();
    InitCreFreeList();
    SkyColour = (struct COLOUR){0};
    SkyClear = 0;
    SetupScreenLoading();
    AquireGt4s(512, 32);
    LevelSpecific = QuakeLoadingSpecific;
    for (a = 0; a < MaxWindows; a++)
        GameWindows[a].CameraRoutine = QuakeCameraLoading;
    VSync(0);
    VBlankSpecific = VBlankLoading;
}

void InitialiseVBlankLoading(void);


void TestIt(char *Name)
{
    char Path[64];

    FinishedCutScene = 0;
    WaitingCutScene = 0;
    InitialisedCutScene = 0;
    DrawSync(0);
    ClearOTag(db[0].Ot[0], 51);
    ClearOTag(db[1].Ot[0], 51);
    DbFrame[0] = 1;
    currdb = &db[1];
    VSync(0);
    VSync(0);
    PutDispEnv(&currdb->DispEnv);
    SceneValid = 0;
    if (UseCdData)
        strcpy(Path, "\\Q2DATA\\");
    else
        strcpy(Path, "c:\\PsxData\\Q2Data\\");
    strcat(Path, "LEVELS\\TITLE\\");
    CastList = 0;
    SetDispMask(1);
    InitLoadingAnim();
    InitialiseVBlankLoading();
}
