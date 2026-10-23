#include "types.h"
extern unsigned char DbFrame[];
extern short SubDivFails;
extern int ProtectedSubdivideDistance;
extern short *SubPackFreePntr;
extern struct SUBPACK *SubPackBase;
extern short *LastUnprotectedSubPackFree;
extern short *LastSubPackFree;
extern short *LastSemiProtectedSubPackFree;
int PolyHasZeroZ(struct GLOBSMODGT4 *Mod);
struct SUBPACK *NewSubGt4(struct GLOBSMODGT4 *Mod, struct POLY_GT4 *Poly, unsigned long *ot);
void GlobDrawSubGt4_(struct POLY_GT4 *Dest, struct SUBPACK *Sub, unsigned long *ot, struct POLY_GT4 *Poly);

int T2GlobSubdivideGt4(struct GLOBSMODGT4 *Mod, struct POLY_GT4 *Poly, unsigned long *ot, int ScreenZ)
{
    struct SUBPACK *Sub;
    char *temp;
    int Allocate;

    if (Poly->pad3)
    {
        temp = (char *)SubPackBase;
        Sub = (struct SUBPACK *)(temp + Poly->pad3);
        GlobDrawSubGt4_(Sub->Polys[DbFrame[0]], Sub, ot, Poly);
        Sub->ScreenZ = ScreenZ;
        return 1;
    }
    else
    {
        Allocate = 0;
        if (SubPackFreePntr < LastUnprotectedSubPackFree
            || (SubPackFreePntr < LastSemiProtectedSubPackFree && ProtectedSubdivideDistance >= ScreenZ))
            Allocate = 1;
        if (!Allocate && PolyHasZeroZ(Mod) && !(Allocate = SubPackFreePntr < LastSubPackFree))
        {
            SubDivFails++;
            return 1;
        }
        if (Allocate)
        {
            Sub = NewSubGt4(Mod, Poly, ot);
            GlobDrawSubGt4_(Sub->Polys[DbFrame[0]], Sub, ot, Poly);
            Sub->ScreenZ = ScreenZ;
            return 1;
        }
        else
        {
            if (ProtectedSubdivideDistance >= ScreenZ)
                SubDivFails++;
            return 0;
        }
    }
}
