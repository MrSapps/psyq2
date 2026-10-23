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
