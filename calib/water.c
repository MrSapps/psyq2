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
