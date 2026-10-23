#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void ProcessPortalWibble(struct PORTALDATA *PortalData, struct SCREENCHANGE *SChange);
void PortalTestWibble(struct SCREENCHANGE *SChange, struct SCENE2 *Scene2);
void InitWaterStuffFrame(void);
void FreeWaterMoves(void);
void InitWaterStuffZone(void);
void ProcessWaterEffect(struct WINDOW *Window);
struct DR_MOVE *WaterMoves[2];
int MaxWaterMoves;
struct DR_MOVE *LastWaterMove;
struct DR_MOVE *WaterMove;
extern unsigned char DbFrame[];
extern int LevelIsMultiPlayer;
void *MY_malloc_name(int s, char *name);
void MyFreeRep(void **Addr, char *Name);

extern struct WINDOW GameWindows[];
extern struct DB *currdb;
extern int NewVcounts;
extern struct SCRPNT ScreenSize;
extern short rcossin_tbl[][2];
void *memset(void *s, int c, unsigned int n);
void AddPrim_(void *ot, void *p);
void SetTile(struct TILE *p);

extern struct WINDOW *CurrWin;
extern struct VECT *CurrCamPos;
int SquareRoot0(int a);
void ProcessPortalWibble(struct PORTALDATA *PortalData, struct SCREENCHANGE *SChange);

void SetDrawMove(struct DR_MOVE *p, struct RECT *rect, int x, int y);
void AddPrim(void *ot, void *p);
extern unsigned long *CurrOt;

void FreeWaterMoves(void)
{
    MyFreeRep((void **)WaterMoves, "Water Moves");
}

void InitWaterStuffZone(void)
{
    int a;
    int b;

    if (LevelIsMultiPlayer)
        MaxWaterMoves = 48;
    else
        MaxWaterMoves = 24;
    WaterMoves[0] = MY_malloc_name(MaxWaterMoves * 2 * sizeof(struct DR_MOVE), "Water Moves");
    WaterMoves[1] = WaterMoves[0] + MaxWaterMoves;
    for (b = 0; b < 2; b++) {
        for (a = 0; a < MaxWaterMoves; a++) {
            WaterMoves[b][a].tag = 0x5000000;
            WaterMoves[b][a].code[0] = 0x1000000;
            WaterMoves[b][a].code[1] = 0x80000000;
            WaterMoves[b][a].code[4] = 0x10001;
        }
    }
}

void InitWaterStuffFrame(void)
{
    WaterMove = WaterMoves[DbFrame[0]];
    LastWaterMove = WaterMove + MaxWaterMoves;
}

void ProcessWaterEffect(struct WINDOW *Window)
{
    int BaseX;
    int BaseY;
    int SizeX;
    int SizeY;
    int AngleX;
    int AngleY;
    int a;
    int Offset;
    int LastOffset;
    int ChunkStart;
    int ScaleX;
    int ScaleY;
    int OffX;
    int OffY;
    struct SCRPNT ThisSize;
    unsigned long *Ot;
    struct TILE *Tile;
    /* not in SYM: constant pseudos, spilled (REG_EQUIV) and rematerialised by reload,
       so they get no debug info. Their (later deleted) set insns are what makes sched1
       order the angle update like the original. */
    int StepX;
    int StepY;

    Ot = &currdb->Ot[Window - GameWindows][47];
    if (!Window->Creature)
        return;
    if (((struct CREATURE *)Window->Creature)->Box.InWater) {
        Window->WaterScale += NewVcounts * 24;
        if (Window->WaterScale > 0x1000)
            Window->WaterScale = 0x1000;
    } else {
        Window->WaterScale -= NewVcounts * 24;
        if (Window->WaterScale < 0)
            Window->WaterScale = 0;
    }
    if (!Window->WaterScale) {
        Window->WaterAngleX = 0;
        Window->WaterAngleY = 0;
        memset(&Window->WaterOffset, 0, sizeof(Window->WaterOffset));
        return;
    }
    ScaleX = Window->WaterScale * 4095 * 4 / 4096;
    ScaleY = Window->WaterScale * 4095 * 2 / 4096;
    OffX = ScaleX / 4096;
    OffY = ScaleY / 4096;
    Window->WaterOffset.sx = OffX;
    Window->WaterOffset.sy = OffY;
    BaseX = Window->Pos.sx;
    BaseY = Window->Pos.sy;
    if (DbFrame[0])
        BaseX += ScreenSize.sx;
    SizeX = Window->Size.sx;
    SizeY = Window->Size.sy;
    Window->WaterAngleX += NewVcounts * 5;
    Window->WaterAngleY += NewVcounts * 7;
    AngleX = Window->WaterAngleX & 4095;
    AngleY = Window->WaterAngleY & 4095;
    StepX = 13;
    StepY = 20;
    LastOffset = 0x7fffffff;
    for (a = 0; a < Window->Size.sx + 1; a++) {
        Offset = rcossin_tbl[AngleY][1] * ScaleY / 0x1000000 + OffY;
        if (Offset != LastOffset || a == Window->Size.sx) {
            if (a) {
                ThisSize.sx = a - ChunkStart;
                ThisSize.sy = SizeY - LastOffset;
                if (ThisSize.sx && ThisSize.sy) {
                    *(struct SCRPNT *)&WaterMove->code[2] = (struct SCRPNT){BaseX + ChunkStart, BaseY + LastOffset};
                    *(struct SCRPNT *)&WaterMove->code[3] = (struct SCRPNT){BaseX + ChunkStart, BaseY};
                    *(struct SCRPNT *)&WaterMove->code[4] = ThisSize;
                    AddPrim_(Ot, WaterMove);
                    WaterMove++;
                    if (WaterMove >= LastWaterMove)
                        goto Done;
                }
            }
            ChunkStart = a;
            LastOffset = Offset;
        }
        AngleY += StepY; AngleY &= 0xfff;
    }
    LastOffset = 0x7fffffff;
    for (a = 0; a < Window->Size.sy + 1; a++) {
        Offset = rcossin_tbl[AngleX][1] * ScaleX / 0x1000000 + OffX;
        if (Offset != LastOffset || a == Window->Size.sy) {
            if (a) {
                ThisSize.sy = a - ChunkStart;
                ThisSize.sx = SizeX - LastOffset;
                if (ThisSize.sx && ThisSize.sy) {
                    *(struct SCRPNT *)&WaterMove->code[2] = (struct SCRPNT){BaseX + LastOffset, BaseY + ChunkStart};
                    *(struct SCRPNT *)&WaterMove->code[3] = (struct SCRPNT){BaseX, BaseY + ChunkStart};
                    *(struct SCRPNT *)&WaterMove->code[4] = ThisSize;
                    AddPrim_(Ot, WaterMove);
                    WaterMove++;
                    if (WaterMove >= LastWaterMove)
                        goto Done;
                }
            }
            ChunkStart = a;
            LastOffset = Offset;
        }
        AngleX += StepX; AngleX &= 0xfff;
    }
Done:
    Tile = &currdb->WaterTiles[Window - GameWindows];
    SetTile(Tile);
    Tile->code |= 2;
    Tile->r0 = Window->WaterScale * 15 / 4096;
    Tile->g0 = 0;
    Tile->b0 = Window->WaterScale * 55 / 4096;
    Tile->x0 = 0;
    Tile->y0 = 0;
    Tile->w = Window->Size.sx;
    Tile->h = Window->Size.sy;
    AddPrim_(Ot, Tile);
}

/* The effect was switched off with an early return (line 368): the original
   keeps ~165 lines of dead code after it (lines 369-531, lost). They still count
   towards gcc's inline-candidate limit (insns are counted before jump optimisation
   deletes them), so the function is emitted in place and PortalTestWibble calls it
   rather than inlining an empty body. The dead code below is a stand-in of about
   the right weight (a row wibble like ProcessWaterEffect's), not a recovery.
   Frame 0x18 = Size + outgoing args of the dead calls. A loop in the dead code
   is what makes gcc put the epilogue's stack restore in the delay slot of the
   `j $31` (with no loop it stays in front of the jump). */
void ProcessPortalWibble(struct PORTALDATA *PortalData, struct SCREENCHANGE *SChange)
{
    struct SCRPNT Size;
    static int LastFrame;
    int y;

    return;
    PortalData->WibbleAngleX += NewVcounts * 5;
    PortalData->WibbleAngleY += NewVcounts * 7;
    Size.sx = SChange->MinMax.Max.sx - SChange->MinMax.Min.sx;
    Size.sy = SChange->MinMax.Max.sy - SChange->MinMax.Min.sy;
    for (y = 0; y < Size.sy; y++) {
        if (WaterMove >= LastWaterMove)
            break;
        SetDrawMove(WaterMove, (struct RECT *)&Size,
                    SChange->MinMax.Min.sx
                        + ((rcossin_tbl[(PortalData->WibbleAngleY + y * 20) & 4095][1] * PortalData->WibbleScale) >> 24),
                    SChange->MinMax.Min.sy + y);
        AddPrim(CurrOt, WaterMove++);
    }
    LastFrame = y;
}

void PortalTestWibble(struct SCREENCHANGE *SChange, struct SCENE2 *Scene2)
{
    static struct PORTALDATA PortalData;
    int Dist;
    struct VECT Centre;
    struct VECT Rel;

    if (CurrWin->WaterScale == 0) {
        Centre.vx = (Scene2->VectBox.Min.vx + Scene2->VectBox.Max.vx) / 2;
        Centre.vy = (Scene2->VectBox.Min.vy + Scene2->VectBox.Max.vy) / 2;
        Centre.vz = (Scene2->VectBox.Min.vz + Scene2->VectBox.Max.vz) / 2;
        Rel.vx = CurrCamPos->vx - Centre.vx;
        Rel.vy = CurrCamPos->vy - Centre.vy;
        Rel.vz = CurrCamPos->vz - Centre.vz;
        Dist = SquareRoot0(Rel.vx * Rel.vx + Rel.vy * Rel.vy + Rel.vz * Rel.vz);
        if (Dist < 0x200)
            Dist = 0x200;
        else if (Dist > 0x2200)
            Dist = 0x2200;
        PortalData.WibbleScale = (0x2200 - Dist) / 2;
        if (PortalData.WibbleScale)
            ProcessPortalWibble(&PortalData, SChange);
    }
}
