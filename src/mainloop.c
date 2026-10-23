#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void VBlankMainLoop(void);
void InitLoadingAnim(void);
void SpriteAnim(int Mode);

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


extern int NewVcounts;
extern int PalNtscRate;
extern short palette[];
extern unsigned char DbFrame[];
extern struct SCRPNT ScreenSize;
void SetSprt(struct SPRT *p);
void AddPrim_(void *ot, void *p);
unsigned short GetTPage(int tp, int abr, int x, int y);
void SetDrawTPage(struct DR_TPAGE *p, int dfe, int dtd, int tpage);

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

void SpriteAnim(int Mode)
{
    static struct DR_MOVE DrMoves[2];
    static struct DR_MOVE *DrMove;
    static struct SPRT Sprites[2];
    static struct SPRT *Sprt;
    static struct DR_TPAGE TPages[2];
    static struct DR_TPAGE *SetTPage;
    int TPage;
    static short Frame = 0;
    static short DemoFrame = 0;
    static struct COLOUR Colour = {0x80, 0x80, 0x80, 0};
    struct SCRPNT ScreenPos;
    struct SCRPNT Size;
    struct CHARPAIR UVData;
    short BaseX;
    short BaseY;
    short Pal;

    if (Mode == 0) {
        if (Frame < 5) {
            UVData.x = 0xe0;
            UVData.y = 0;
        } else if (Frame == 5) {
            UVData.x = 0xc0;
            UVData.y = 0x50;
        } else if (Frame == 6) {
            UVData.x = 0xe0;
            UVData.y = 0x78;
        } else if (Frame == 7) {
            UVData.x = 0;
            UVData.y = 0xdc;
        } else
            UVData = (struct CHARPAIR){(Frame & 7) * 32, ((Frame & 0x18) >> 3) * 20 + 124};
        TPage = 13;
        ScreenPos = (struct SCRPNT){0x1cc, 0x28};
        Size = (struct SCRPNT){0x20, 0x14};
        Pal = palette[78];
    } else if (Mode == 1) {
        DemoFrame += NewVcounts / PalNtscRate;
        while (DemoFrame >= 0x60)
            DemoFrame -= 0x60;
        {
            int Temp;

            TPage = 12;
            Temp = DemoFrame;
            if (Temp >= 0x30) {
                TPage = 11;
                Temp -= 0x30;
            }
            Pal = palette[79];
            UVData = (struct CHARPAIR){(DemoFrame & 3) * 64, (Temp >> 2) * 20};
        }
        ScreenPos = (struct SCRPNT){0xe0, 0x14};
        Size = (struct SCRPNT){0x40, 0x14};
    }
    Sprt = &Sprites[DbFrame[0]];
    SetTPage = &TPages[DbFrame[0]];
    if (Mode == 0 && RetryMessageTime) {
        char Bright;

        if (RetryMessageTime > 16)
            Bright = 0x80;
        else
            Bright = RetryMessageTime * 8;
        Sprt->r0 = Sprt->g0 = Sprt->b0 = Bright;
        Sprt->x0 = ScreenPos.sx;
        Sprt->y0 = ScreenPos.sy;
        Sprt->u0 = 0xe0;
        Sprt->v0 = 0xb8;
        Sprt->clut = Pal;
        *(struct SCRPNT *)&Sprt->w = (struct SCRPNT){0x20, 0x14};
        SetSprt(Sprt);
        AddPrim_(&currdb->Ot[0][49], Sprt++);
        RetryMessageTime--;
    }
    *(struct COLOUR *)&Sprt->r0 = Colour;
    *(struct SCRPNT *)&Sprt->x0 = ScreenPos;
    *(struct CHARPAIR *)&Sprt->u0 = UVData;
    Sprt->clut = Pal;
    *(struct SCRPNT *)&Sprt->w = Size;
    SetSprt(Sprt);
    AddPrim_(&currdb->Ot[0][49], Sprt);
    SetDrawTPage(SetTPage, 1, 0, GetTPage(0, 1, tpscreenx[TPage], tpscreeny[TPage]));
    AddPrim_(&currdb->Ot[0][49], SetTPage);
    if (Mode == 0) {
        DrMove = &DrMoves[DbFrame[0]];
        DrMove->tag = 0x5000000;
        DrMove->code[0] = 0x1000000;
        DrMove->code[1] = 0x80000000;
        DrMove->code[4] = 0x10001;
        BaseX = GameWindows[4].Pos.sx;
        BaseY = GameWindows[4].Pos.sy;
        if (DbFrame[1])
            BaseX += ScreenSize.sx;
        *(struct SCRPNT *)&DrMove->code[2] = (struct SCRPNT){tpscreenx[13] + 0x10, tpscreeny[13] + 0x50};
        *(struct SCRPNT *)&DrMove->code[3] = (struct SCRPNT){BaseX + ScreenPos.sx, BaseY + ScreenPos.sy};
        *(struct SCRPNT *)&DrMove->code[4] = (struct SCRPNT){0x20, 0x14};
        AddPrim_(&currdb->Ot[0][49], DrMove);
    }
    if (++Frame == 31)
        Frame = 0;
}
