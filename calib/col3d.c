#include "types.h"
extern struct COLLHEADER CollMap;
struct COLLRET Next3dSpaceIndxG(struct COLLHEADER *Map, struct VECT *Location, int Space, int PMask);
int TimesectLine(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *V, short Scale);
int ProcessCol3dMove(struct COLLHEADER *Map, struct COL3DMOVE *Move3d, struct VECT *Location, int NumFrames)
{
    struct VECT Intersect;

    Move3d->SpaceTime -= NumFrames << 6;
    while (Move3d->SpaceTime <= 0) {
        Intersect.vx = Location->vx + Move3d->Vel.vx * Move3d->SpaceTime / 64 / Move3d->Scale;
        Intersect.vy = Location->vy + Move3d->Vel.vy * Move3d->SpaceTime / 64 / Move3d->Scale;
        Intersect.vz = Location->vz + Move3d->Vel.vz * Move3d->SpaceTime / 64 / Move3d->Scale;
        Move3d->SpacePlane = Next3dSpaceIndxG(&CollMap, &Intersect, Move3d->SpacePlane & 0x7ff, Move3d->SpacePlane & 0xf800).a;
        if (Move3d->SpacePlane < 0) {
            *Location = Intersect;
            return 0;
        }
        Move3d->SpaceTime += TimesectLine(&CollMap, Move3d->SpacePlane, &Intersect, &Move3d->Vel, Move3d->Scale);
        Move3d->Area = Map->Coll3dSpace[Move3d->SpacePlane].Area;
        Move3d->SpacePlane |= CollMap.ReturnPlaneNum << 11;
    }
    return 1;
}

int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int TimesectLine(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *V, short Scale);

int InitCol3dMove(struct COLLHEADER *Map, struct COL3DMOVE *Move3d, struct VECT *Location, short SpaceNum)
{
    Move3d->SpacePlane = Find3dSpaceIndxLink(Map, Location, SpaceNum, 1);
    if (Move3d->SpacePlane >= 0) {
        Move3d->SpaceTime = TimesectLine(Map, Move3d->SpacePlane, Location, &Move3d->Vel, Move3d->Scale);
        Move3d->Area = Map->Coll3dSpace[Move3d->SpacePlane].Area;
        Move3d->SpacePlane |= Map->ReturnPlaneNum << 11;
        return 1;
    }
    return 0;
}

int In3dSpaceIndx(struct COLLHEADER *Map, struct VECT *Loc, int Indx);

int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll)
{
    int Conx;
    int SpaceIndx;

    if (Main >= 0) {
        if (In3dSpaceIndx(Map, Loc, Main))
            return Main;
        for (Conx = Map->Coll3dSpace[Main].StartJoin; Conx < Map->Coll3dSpace[Main + 1].StartJoin; Conx++) {
            SpaceIndx = Map->Coll3dJoin[Conx].Conx & 0x7ff;
            if (In3dSpaceIndx(Map, Loc, SpaceIndx))
                return SpaceIndx;
        }
    }
    if (CheckAll) {
        for (SpaceIndx = 0; SpaceIndx < Map->TotalNumSpaces; SpaceIndx++)
            if (In3dSpaceIndx(Map, Loc, SpaceIndx))
                return SpaceIndx;
    }
    return -1;
}

struct COLLPLANE *LittlePlane;
short LittleOut;
struct FRACTION IntersectLinePlane(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *T);
void FractionDist(struct SVECT *R, struct SVECT *T, struct FRACTION *F);

/* Find3dSpaceIndxLink (defined above) is inlined here, as in the original */
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0)
{
    struct VECT Temp;
    struct SVECT Rel;
    struct FRACTION Intersect;
    struct COLLRET CollRet;
    short Area;

    if (S0 == -1)
        S0 = Find3dSpaceIndxLink(Map, Start, -1, 1);
    Area = -1;
    Map->ReturnSpaceNum = S0;
    Map->AreaIntersectLast = Map->AreaIntersects;
    Temp = *Start;
    CollRet.b = -1;
    CollRet.Plane = 0;
    if (!((Start->vx - End->vx) | (Start->vy - End->vy) | (Start->vz - End->vz))) {
        Temp = *End;
        goto Done;
    }
    while (S0 >= 0) {
        if (Area != Map->Coll3dSpace[S0].Area && Map->AreaIntersectLast < &Map->AreaIntersects[8]) {
            Area = Map->Coll3dSpace[S0].Area;
            Map->AreaIntersectLast->Area = Area;
            (Map->AreaIntersectLast++)->Intersect = Temp;
        }
        Map->ReturnSpaceNum = S0;
        Rel.vx = End->vx - Temp.vx;
        Rel.vy = End->vy - Temp.vy;
        Rel.vz = End->vz - Temp.vz;
        Intersect = IntersectLinePlane(Map, S0, &Temp, &Rel);
        if (Intersect.Numerator == Intersect.Denominator) {
            LittleOut = -1;
            Temp = *End;
            goto Done;
        }
        FractionDist(&Rel, &Rel, &Intersect);
        Temp.vx += Rel.vx;
        Temp.vy += Rel.vy;
        Temp.vz += Rel.vz;
        CollRet = Next3dSpaceIndxG(Map, &Temp, S0, Map->ReturnPlaneNum << 11);
        if (CollRet.a == S0)
            S0 = -1;
        else
            S0 = CollRet.a;
    }
    LittleOut = CollRet.b;
    LittlePlane = CollRet.Plane;
Done:
    Map->AreaIntersectLast->Intersect = Temp;
    if (Map->ReturnSpaceNum >= 0)
        Map->AreaIntersectLast->Area = Map->Coll3dSpace[Map->ReturnSpaceNum].Area;
    else
        Map->AreaIntersectLast->Area = 0;
    return S0 != -1;
}

int In3dSpaceIndx(struct COLLHEADER *Map, struct VECT *Loc, int Indx)
{
    int DotProduct;
    int a;
    struct COLL3DSPACE *Space;
    struct COLLPLANE *Plane;
    struct SVECT Rel;
    struct SVECT RelA;

    if (Indx >= 0)
        Space = &Map->Coll3dSpace[Indx];
    else if (Indx == -1)
        return 0;
    else
        Space = (struct COLL3DSPACE *)Indx;
    if (Loc->vx > Space->MinMax.Max.vx || Loc->vx < Space->MinMax.Min.vx
        || Loc->vy > Space->MinMax.Max.vy || Loc->vy < Space->MinMax.Min.vy
        || Loc->vz > Space->MinMax.Max.vz || Loc->vz < Space->MinMax.Min.vz)
        return 0;
    Rel.vx = Loc->vx - Space->MinMax.Min.vx;
    Rel.vy = Loc->vy - Space->MinMax.Min.vy;
    Rel.vz = Loc->vz - Space->MinMax.Min.vz;
    if ((a = Space->StartPlane) >= 0) {
        for (Plane = &Map->Coll3dPlane[a & 0x7fff]; Plane < &Map->Coll3dPlane[Space[1].StartPlane & 0x7fff]; Plane++) {
            RelA.vx = Rel.vx - Plane->Point.vx;
            RelA.vy = Rel.vy - Plane->Point.vy;
            RelA.vz = Rel.vz - Plane->Point.vz;
            DotProduct = RelA.vx * Plane->Normal.vx + RelA.vy * Plane->Normal.vy + RelA.vz * Plane->Normal.vz;
            if (DotProduct > 0)
                return 0;
        }
        return 1;
    }
    return 0;
}

struct COLLRET In3dSpaceIndxGen(struct COLLHEADER *Map, struct VECT *Loc, int Indx, int Ignore);
struct COLLRET Next3dSpaceIndxG(struct COLLHEADER *Map, struct VECT *Location, int Space, int PMask)
{
    int a;
    short AlmostIn;
    struct COLLRET CollRet;
    struct COLLPLANE *Plane;

    Plane = 0;
    AlmostIn = -1;
    for (a = Map->Coll3dSpace[Space].StartJoin; a < Map->Coll3dSpace[Space + 1].StartJoin; a++) {
        if ((Map->Coll3dJoin[a].Conx & 0xf800) == PMask) {
            CollRet = In3dSpaceIndxGen(Map, Location, Map->Coll3dJoin[a].Conx & 0x7ff, Map->Coll3dJoin[a].Ignore);
            if (CollRet.a)
                return (struct COLLRET){Map->Coll3dJoin[a].Conx & 0x7ff, -1, 0};
            if (CollRet.b) {
                AlmostIn = Map->Coll3dJoin[a].Conx & 0x7ff;
                Plane = CollRet.Plane;
            }
        }
    }
    return (struct COLLRET){-1, AlmostIn, Plane};
}

struct COLLRET In3dSpaceIndxGen(struct COLLHEADER *Map, struct VECT *Loc, int Indx, int Ignore)
{
    int DotProduct;
    int PNum;
    int Ret;
    struct COLL3DSPACE *Space;
    struct COLLPLANE *Plane;
    struct COLLPLANE *OutPlane;
    struct SVECT Rel;

    Space = &Map->Coll3dSpace[Indx];
    if (Space->StartPlane < 0)
        return (struct COLLRET){0};
    Ret = 1;
    OutPlane = 0;
    Rel.vx = Loc->vx - Space->MinMax.Min.vx;
    Rel.vy = Loc->vy - Space->MinMax.Min.vy;
    Rel.vz = Loc->vz - Space->MinMax.Min.vz;
    PNum = 0;
    for (Plane = &Map->Coll3dPlane[Space->StartPlane]; Plane < &Map->Coll3dPlane[Space[1].StartPlane & 0x7fff]; Plane++, PNum++) {
        if (PNum != Ignore) {
            DotProduct = (Rel.vx - Plane->Point.vx) * Plane->Normal.vx + (Rel.vy - Plane->Point.vy) * Plane->Normal.vy + (Rel.vz - Plane->Point.vz) * Plane->Normal.vz;
            if (DotProduct > 0) {
                if (DotProduct >= 0x100)
                    return (struct COLLRET){0};
                Ret = 0;
                OutPlane = Plane;
            }
        }
    }
    return (struct COLLRET){Ret, !Ret, OutPlane};
}

int ComparePosFractions_(struct FRACTION a, struct FRACTION b);
struct FRACTION IntersectLinePlane(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *T)
{
    struct COLLPLANE *Plane;
    struct SVECT R;
    struct COLL3DSPACE *Space;
    struct FRACTION Intersect;
    struct FRACTION Fraction;
    int Speed;
    int Dist;
    int plane;

    Space = &Map->Coll3dSpace[SpaceIndx];
    R.vx = Loc->vx - Space->MinMax.Min.vx;
    R.vy = Loc->vy - Space->MinMax.Min.vy;
    R.vz = Loc->vz - Space->MinMax.Min.vz;
    Intersect = (struct FRACTION){1, 1};
    Map->ReturnPlaneNum = -1;
    plane = 0;
    for (Plane = &Map->Coll3dPlane[Space->StartPlane & 0x7fff]; Plane < &Map->Coll3dPlane[Space[1].StartPlane & 0x7fff]; Plane++, plane++) {
        Speed = Plane->Normal.vx * T->vx + Plane->Normal.vy * T->vy + Plane->Normal.vz * T->vz;
        if (Speed > 0) {
            Dist = Plane->Normal.vx * (Plane->Point.vx - R.vx) + Plane->Normal.vy * (Plane->Point.vy - R.vy) + Plane->Normal.vz * (Plane->Point.vz - R.vz);
            if (Dist >= 0) {
                Fraction.Numerator = Dist;
                Fraction.Denominator = Speed;
                if (ComparePosFractions_(Fraction, Intersect)) {
                    Intersect = Fraction;
                    Map->ReturnPlaneNum = plane;
                    Map->ReturnPlane = Plane;
                    Map->ReturnSpace = Space;
                }
            }
        }
    }
    return Intersect;
}

int TimesectLine(struct COLLHEADER *Map, int SpaceIndx, struct VECT *Loc, struct SVECT *V, short Scale)
{
    struct COLLPLANE *Plane;
    struct SVECT R;
    struct COLL3DSPACE *Space;
    int Intersect;
    int plane;
    int a;
    int b;

    Space = &Map->Coll3dSpace[SpaceIndx];
    R.vx = Loc->vx - Space->MinMax.Min.vx;
    R.vy = Loc->vy - Space->MinMax.Min.vy;
    R.vz = Loc->vz - Space->MinMax.Min.vz;
    Intersect = 0xfffffff;
    plane = 0;
    for (Plane = &Map->Coll3dPlane[Space->StartPlane & 0x7fff]; Plane < &Map->Coll3dPlane[Space[1].StartPlane & 0x7fff]; Plane++, plane++) {
        a = Plane->Normal.vx * V->vx / Scale + Plane->Normal.vy * V->vy / Scale + Plane->Normal.vz * V->vz / Scale;
        if (a > 0) {
            b = (Plane->Normal.vx * (Plane->Point.vx - R.vx) + Plane->Normal.vy * (Plane->Point.vy - R.vy) + Plane->Normal.vz * (Plane->Point.vz - R.vz)) * 16 / a;
            if (b >= 0 && b < Intersect) {
                Map->ReturnPlaneNum = plane;
                Intersect = b;
            }
        }
    }
    return Intersect << 2;
}

extern struct WINDOW GameWindows[];
void dprintf(struct WINDOW *Window, char *_format, ...);
int MaxSizeSVect(struct SVECT V);
void *memset(void *s, int c, unsigned int n);

int MakeIn3dSpace(struct COLLHEADER *Map, struct VECT *Start, short Indx)
{
    struct SVECT R;
    struct SVECT Rel;
    struct SVECT RelPlane;
    struct SVECT ToCentre;
    int DotProduct;
    int Step;
    int StepTotal;
    int NumPlanes;
    int Safe;
    struct COLLPLANE *Plane;
    struct COLLPLANE *FirstPlane;
    struct COLLPLANE *LastPlane;
    struct COLL3DSPACE *Space;
    struct VECT Centre;

    if (Indx < 0)
        return 1;
    Space = &Map->Coll3dSpace[Indx];
    Rel.vx = Start->vx - Space->MinMax.Min.vx;
    Rel.vy = Start->vy - Space->MinMax.Min.vy;
    Rel.vz = Start->vz - Space->MinMax.Min.vz;
    Step = 0;
    StepTotal = 0;
    R = Rel;
    NumPlanes = (Space[1].StartPlane & 0x7fff) - (Space->StartPlane & 0x7fff);
    FirstPlane = &Map->Coll3dPlane[Space->StartPlane & 0x7fff];
    LastPlane = FirstPlane + NumPlanes;
    do {
        for (Plane = FirstPlane; Plane < LastPlane; Plane++) {
            RelPlane.vx = R.vx - Plane->Point.vx;
            RelPlane.vy = R.vy - Plane->Point.vy;
            RelPlane.vz = R.vz - Plane->Point.vz;
            DotProduct = RelPlane.vx * Plane->Normal.vx + RelPlane.vy * Plane->Normal.vy + RelPlane.vz * Plane->Normal.vz;
            if (DotProduct > 0)
                break;
        }
        Safe = Plane == LastPlane;
        if (!Safe) {
            if (!Step) {
                memset(&Centre, 0, sizeof(Centre));
                for (Plane = FirstPlane; Plane < LastPlane; Plane++) {
                    Centre.vx += Plane->Point.vx;
                    Centre.vy += Plane->Point.vy;
                    Centre.vz += Plane->Point.vz;
                }
                Centre.vx /= NumPlanes;
                Centre.vy /= NumPlanes;
                Centre.vz /= NumPlanes;
                ToCentre.vx = Centre.vx - R.vx;
                ToCentre.vy = Centre.vy - R.vy;
                ToCentre.vz = Centre.vz - R.vz;
                Step = 0x1000 / MaxSizeSVect(ToCentre);
                if (!Step)
                    Step = 1;
            }
            if (StepTotal == 0x1000) {
                dprintf(GameWindows, "Help %d\n", Indx);
                return 0;
            }
            StepTotal += Step;
            if (StepTotal >= 0x1000) {
                StepTotal = 0x1000;
                R.vx = Centre.vx;
                R.vy = Centre.vy;
                R.vz = Centre.vz;
            } else {
                R.vx = Rel.vx + ToCentre.vx * StepTotal / 4096;
                R.vy = Rel.vy + ToCentre.vy * StepTotal / 4096;
                R.vz = Rel.vz + ToCentre.vz * StepTotal / 4096;
            }
        }
    } while (!Safe);
    Start->vx = Space->MinMax.Min.vx + R.vx;
    Start->vy = Space->MinMax.Min.vy + R.vy;
    Start->vz = Space->MinMax.Min.vz + R.vz;
    return 1;
}
