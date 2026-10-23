#include "types.h"
struct DR_MOVE *WaterMoves[2];
int MaxWaterMoves;
struct DR_MOVE *LastWaterMove;
struct DR_MOVE *WaterMove;
extern unsigned char DbFrame[];
extern int LevelIsMultiPlayer;
void *MY_malloc_name(int s, char *name);
void MyFreeRep(void **Addr, char *Name);

void InitWaterStuffFrame(void)
{
    WaterMove = WaterMoves[DbFrame[0]];
    LastWaterMove = WaterMove + MaxWaterMoves;
}

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

extern struct WINDOW *CurrWin;
extern struct VECT *CurrCamPos;
int SquareRoot0(int a);
void ProcessPortalWibble(struct PORTALDATA *PortalData, struct SCREENCHANGE *SChange);

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

void SetDrawMove(struct DR_MOVE *p, struct RECT *rect, int x, int y);
void AddPrim(void *ot, void *p);
extern unsigned long *CurrOt;

/* The effect was switched off with an early return (line 368): the original
   keeps ~165 lines of dead code after it, and is not inlined into
   PortalTestWibble because of them (here it simply comes after its caller).
   Frame 0x18 = Size + outgoing args of the dead calls. A loop in the dead code
   is what makes gcc put the epilogue's stack restore in the delay slot of the
   `j $31` (with no loop it stays in front of the jump). */
void ProcessPortalWibble(struct PORTALDATA *PortalData, struct SCREENCHANGE *SChange)
{
    struct SCRPNT Size;
    static int LastFrame;
    int y;

    return;
    Size.sx = SChange->MinMax.Max.sx - SChange->MinMax.Min.sx;
    Size.sy = SChange->MinMax.Max.sy - SChange->MinMax.Min.sy;
    for (y = 0; y < Size.sy; y++) {
        SetDrawMove(WaterMove, (struct RECT *)&Size,
                    SChange->MinMax.Min.sx + ((PortalData->WibbleScale * y) >> 12),
                    SChange->MinMax.Min.sy + y);
        AddPrim(CurrOt, WaterMove++);
    }
    LastFrame = y;
}
