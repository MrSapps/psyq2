#include "types.h"

extern int TitleLoad;
extern struct WINDOW GameWindows[];
extern void (*LevelSpecific)();
extern volatile int DebugWatch;
extern struct DB *currdb;
extern short tpscreenx[];
extern short tpscreeny[];
/* defined in mainloop.obj, so $gp-relative */
static int RetryMessageTime;

void ChrisInitEveryFrame(void);
void MoveModels(void);
void SpriteAnim(int);
void ProcessAllCameras(void);
void DrawAllScenes(void);
void DrawAllModels(void);
void DrawAllSortedObjs(void);
void DrawSortedObjectsHeavens(void);
void FreeSubDivs(void);
void ReallyFreePolyPackets(void);
int DrawSync(int mode);
int MoveImage(struct RECT *rect, int x, int y);

void VBlankMainLoop(void)
{
    if (!TitleLoad) {
        GameWindows[0].SkyClear = 0;
        GameWindows[1].SkyClear = 0;
        GameWindows[2].SkyClear = 0;
        GameWindows[3].SkyClear = 0;
        ChrisInitEveryFrame();
        MoveModels();
        SpriteAnim(0);
        if (LevelSpecific)
            LevelSpecific();
        ProcessAllCameras();
        DrawAllScenes();
        DrawAllModels();
        DrawAllSortedObjs();
        DebugWatch = 27;
        DrawSortedObjectsHeavens();
        DebugWatch = 28;
        DebugWatch = 29;
        FreeSubDivs();
        DebugWatch = 30;
        ReallyFreePolyPackets();
        DebugWatch = 34;
    }
}

void InitLoadingAnim(void)
{
    struct RECT Rect;

    DrawSync(0);
    Rect.x = currdb->DispEnv.disp.x + 460;
    Rect.y = currdb->DispEnv.disp.y + 40;
    Rect.w = 32;
    Rect.h = 20;
    MoveImage(&Rect, tpscreenx[13] + 16, tpscreeny[13] + 80);
    RetryMessageTime = 0;
}
