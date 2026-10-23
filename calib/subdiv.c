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

short *SubToFreeList;
short *SubToFreeListPntr;
short *LastSubPackFree;
short SubDivFails;
extern struct WINDOW GameWindows[];
void dprintf(struct WINDOW *Window, char *_format, ...);

void FreeSubDivs(void)
{
    {
        char *temp;
        unsigned short *ap;

        for (ap = SubToFreeList; ap < SubToFreeListPntr; ap++) {
            temp = (char *)SubPackBase;
            temp += *ap;
            *--SubPackFreePntr = *ap;
            if (((struct SUBPACK *)temp)->Source[0]) {
                ((struct SUBPACK *)temp)->Source[0]->pad3 = 0;
                ((struct SUBPACK *)temp)->Source[1]->pad3 = 0;
                ((struct SUBPACK *)temp)->Source[0] = 0;
            }
        }
    }
    {
        short NeedFree;
        short Count;
        int MaxZ;
        char *Temp;
        struct SUBPACK *SubP;
        struct SUBPACK *ToFree;
        int Used;

        SubToFreeListPntr = SubToFreeList;
        Used = LastSubPackFree - SubPackFreePntr;
        NeedFree = 6;
        if (MaxSubPols < 6)
            NeedFree = MaxSubPols;
        NeedFree -= Used;
        for (Count = 0; Count < NeedFree; Count++) {
            MaxZ = 0;
            ToFree = 0;
            for (SubP = SubPack; SubP < &SubPack[MaxSubPols]; SubP++) {
                if (SubP->Source[0] && SubP->ScreenZ > MaxZ) {
                    ToFree = SubP;
                    MaxZ = SubP->ScreenZ;
                }
            }
            if (ToFree) {
                Temp = (char *)SubPackBase;
                *SubToFreeListPntr++ = (char *)ToFree - Temp;
                ToFree->Source[0]->pad3 = 0;
                ToFree->Source[1]->pad3 = 0;
                ToFree->Source[0] = 0;
            } else {
                dprintf(GameWindows, "Subdiv Confusion\n");
            }
        }
    }
    SubDivFails = 0;
}

short *SubPackFree;
short *LastUnprotectedSubPackFree;
short *LastSemiProtectedSubPackFree;
void SetSemiTrans(void *p, int abe);
void SetShadeTex(void *p, int tge);
#define setlen(p, _len) (*(((unsigned char *)(p)) + 3) = (unsigned char)(_len))
#define setcode(p, _code) (*(((unsigned char *)(p)) + 7) = (unsigned char)(_code))
#define setPolyGT4(p) setlen(p, 12), setcode(p, 0x3c)

void InitSubdivision(void)
{
    int a;
    int c;
    struct POLY_GT4 *Gt40;
    struct POLY_GT4 *Gt41;

    Gt40 = db[0].subpolsg4;
    Gt41 = db[1].subpolsg4;
    for (c = 0; c < MaxSubPols * 16; Gt40++, Gt41++, c++) {
        setPolyGT4(Gt40);
        SetSemiTrans(Gt40, 0);
        SetShadeTex(Gt40, 0);
        setPolyGT4(Gt41);
        SetSemiTrans(Gt41, 0);
        SetShadeTex(Gt41, 0);
    }
    SubPackBase = SubPack - 1;
    for (a = 0; a < MaxSubPols; a++) {
        SubPackFree[a] = (a + 1) * sizeof(struct SUBPACK);
        SubPack[a].Polys[0] = db[0].subpolsg4 + a * 16;
        SubPack[a].Polys[1] = db[1].subpolsg4 + a * 16;
        SubPack[a].Source[0] = 0;
    }
    SubToFreeListPntr = SubToFreeList;
    SubPackFreePntr = SubPackFree;
    LastSubPackFree = SubPackFree + MaxSubPols;
    if (MaxSubPols > 6) {
        LastUnprotectedSubPackFree = LastSubPackFree - 6;
        LastSemiProtectedSubPackFree = LastSubPackFree - 3;
    } else {
        LastUnprotectedSubPackFree = LastSubPackFree;
        LastSemiProtectedSubPackFree = LastSubPackFree;
    }
}
