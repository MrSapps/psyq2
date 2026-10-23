#include "types.h"

/* defined in this file (subdiv.obj) */
short *SubPackFreePntr;
struct SUBPACK *SubPackBase;
struct SUBPACK *SubPack;
int MaxSubPols;

extern struct SCRPNTZ *CurrentCoordBuffer;
extern struct MODELREF *GlobalSubModelRef;
extern struct DB db[];
extern unsigned char DbFrame[];

void UpdateSubdivisionLighting(void)
{
    struct SUBPACK *SubP;
    int Frame;
    struct POLY_GT4 *Poly;
    struct POLY_GT4 *Dest;

    Frame = DbFrame[0];
    for (SubP = SubPack; SubP < &SubPack[MaxSubPols]; SubP++) {
        if (SubP->Source[0]) {
            Poly = SubP->Source[Frame];
            Dest = SubP->Polys[Frame];
            if (*(int *)&Poly->r0 != *(int *)&Dest[0].r0
                || *(int *)&Poly->r1 != *(int *)&Dest[3].r1
                || *(int *)&Poly->r2 != *(int *)&Dest[12].r2
                || *(int *)&Poly->r3 != *(int *)&Dest[15].r3)
                Resubdiv_(Dest, 0, 0, Poly);
        }
    }
}

int PolyHasZeroZ(struct GLOBSMODGT4 *Mod)
{
    int a;
    unsigned char *Pnts;

    Pnts = &Mod->pnt0;
    for (a = 0; a < 4; a++, Pnts++) {
        if (!CurrentCoordBuffer[*Pnts].sz)
            return 1;
    }
    return 0;
}

void FreeSubDivFromPoly(struct POLY_GT4 *Poly)
{
    char *temp;

    temp = (char *)SubPackBase;
    ((struct SUBPACK *)(temp + Poly->pad3))->Source[0] = 0;
    *--SubPackFreePntr = Poly->pad3;
}

struct SUBPACK *NewSubGt4(struct GLOBSMODGT4 *Mod, struct POLY_GT4 *Poly, unsigned long *ot)
{
    char *temp;
    struct POLY_GT4 *PolyA;
    struct POLY_GT4 *PolyB;

    if (Poly >= db[1].modpolsg4) {
        PolyB = Poly;
        PolyA = (struct POLY_GT4 *)((char *)Poly + (int)db[0].modpolsg4 - (int)db[1].modpolsg4);
    } else {
        PolyA = Poly;
        PolyB = (struct POLY_GT4 *)((char *)Poly + (int)db[1].modpolsg4 - (int)db[0].modpolsg4);
    }
    PolyB->pad3 = PolyA->pad3 = *SubPackFreePntr++;
    temp = (char *)SubPackBase;
    temp += Poly->pad3;
    ((struct SUBPACK *)temp)->Source[0] = PolyA;
    ((struct SUBPACK *)temp)->Source[1] = PolyB;
    GlobSub3DPointsGt4_(Mod, temp, GlobalSubModelRef, Poly);
    Resubdiv_(((struct SUBPACK *)temp)->Polys[0], 0, 0, Poly);
    Resubdiv_(((struct SUBPACK *)temp)->Polys[1], 0, 0, Poly);
    return (struct SUBPACK *)temp;
}
