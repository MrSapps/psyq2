#include "types.h"

struct CREATURE **ThisBumpList;
struct CREATURE **NextBumpList;
struct CREATURE **ThisBumpListEnd;
struct CREATURE **NextBumpListEnd;
struct CREATURE **NextBumpListPntr;
unsigned char SBoxGroupIdentNum;
struct CREATURE *BumpList[2][32];
struct SOLIDBOX SolidBoxs[48];

int FindNewSpaceFromOld(struct COLLHEADER *Map, struct VECT *New, struct VECT *Old, int Space)
{
    Visable3d(Map, Old, New, Space);
    return Map->ReturnSpaceNum;
}

void InitialiseBumpListFrame(void)
{
    struct CREATURE **TempBumpList;

    TempBumpList = ThisBumpList;
    ThisBumpList = NextBumpList;
    NextBumpList = TempBumpList;
    ThisBumpListEnd = NextBumpListPntr;
    NextBumpListEnd = TempBumpList + 32;
    NextBumpListPntr = TempBumpList;
}

void InitialiseBumpListStart(void)
{
    ThisBumpList = BumpList[0];
    ThisBumpListEnd = BumpList[1];
    NextBumpList = BumpList[1];
    NextBumpListEnd = BumpList[1] + 32;
    NextBumpListPntr = BumpList[1];
}

void AddCreToNextBumpList(struct CREATURE *Cre)
{
    if (NextBumpListPntr != NextBumpListEnd)
        *NextBumpListPntr++ = Cre;
}

void ReCalcCollBox(struct CREATURE *Cre)
{
    Cre->Box.MoveSpace = -1;
}

int GenerateBoxCollFlags(struct COLLBOX3D *Box)
{
    int Flags;

    Flags = 0;
    if (Box->UseCreatureBlockers)
        Flags = 0x810;
    return Flags;
}

void InitSolidBoxs(void)
{
    int Indx;
    struct SOLIDBOX *SolidBox;

    SolidBox = SolidBoxs;
    SBoxGroupIdentNum = 0;
    for (Indx = 0; Indx < 48; Indx++) {
        SolidBox->Flags = 0;
        SolidBox++;
    }
}

void FreeSolidBoxs(struct SOLIDBOX *SolidBox)
{
    while (SolidBox) {
        SolidBox->Flags = 0;
        SolidBox = SolidBox->LinkedData;
    }
}

extern struct COLLHEADER MoveCollMap;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int PointAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, struct MODSUBOFF ModSubOff, enum DAMAGETYPES DamType);
struct MODSUBOFF GetModSubOffFromVertex(struct GLOBMODHDR *Hdr, short VertexNum);

void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ)
{
    Box->Min.vx = -RadiusXYZ.vx;
    Box->Min.vy = -RadiusXYZ.vy;
    Box->Min.vz = -RadiusXYZ.vz;
    Box->Max = RadiusXYZ;
}

int InVectBox(struct VECTBOX *VectBox, struct VECT *Vect)
{
    if (Vect->vx < VectBox->Min.vx)
        return 0;
    if (Vect->vx > VectBox->Max.vx)
        return 0;
    if (Vect->vz < VectBox->Min.vz)
        return 0;
    return Vect->vz <= VectBox->Max.vz;
}

void ExpandVectBox(struct VECTBOX *PQBox, struct VECTBOX *PQExp, short Width, struct SHORTPAIR MinMaxY)
{
    PQExp->Min.vx = PQBox->Min.vx - Width;
    PQExp->Max.vx = PQBox->Max.vx + Width;
    PQExp->Min.vz = PQBox->Min.vz - Width;
    PQExp->Max.vz = PQBox->Max.vz + Width;
    PQExp->Min.vy = PQBox->Min.vy + MinMaxY.a;
    PQExp->Max.vy = PQBox->Max.vy + MinMaxY.b;
}

int FindBoxMoveSpace(struct COLLBOX3D *Box, int Main)
{
    Box->MoveSpace = Find3dSpaceIndxLink(&MoveCollMap, &Box->Location, Main, 1);
    Box->Area = MoveCollMap.Coll3dSpace[Box->MoveSpace].Area;
    return Box->MoveSpace != -1;
}

int VertexAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, short VertexNum, enum DAMAGETYPES DamType)
{
    return PointAttack(Cre, DamRad, GetModSubOffFromVertex(Cre->ModelHdr, VertexNum), DamType);
}

int InVectBox3D(struct VECTBOX *VectBox, struct VECT *Vect)
{
    if (Vect->vx < VectBox->Min.vx)
        return 0;
    if (Vect->vx > VectBox->Max.vx)
        return 0;
    if (Vect->vy < VectBox->Min.vy)
        return 0;
    if (Vect->vy > VectBox->Max.vy)
        return 0;
    if (Vect->vz < VectBox->Min.vz)
        return 0;
    return Vect->vz <= VectBox->Max.vz;
}

int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB)
{
    if (VBoxA->Min.vx > VBoxB->Max.vx)
        return 0;
    if (VBoxA->Max.vx < VBoxB->Min.vx)
        return 0;
    if (VBoxA->Min.vz > VBoxB->Max.vz)
        return 0;
    if (VBoxA->Max.vz < VBoxB->Min.vz)
        return 0;
    if (VBoxA->Min.vy > VBoxB->Max.vy)
        return 0;
    return VBoxA->Max.vy >= VBoxB->Min.vy;
}

void GenVectBox(struct VECTBOX *MinMax, struct VECT *A, struct VECT *B)
{
    if (A->vx > B->vx) {
        MinMax->Min.vx = B->vx;
        MinMax->Max.vx = A->vx;
    } else {
        MinMax->Min.vx = A->vx;
        MinMax->Max.vx = B->vx;
    }
    if (A->vy > B->vy) {
        MinMax->Min.vy = B->vy;
        MinMax->Max.vy = A->vy;
    } else {
        MinMax->Min.vy = A->vy;
        MinMax->Max.vy = B->vy;
    }
    if (A->vz > B->vz) {
        MinMax->Min.vz = B->vz;
        MinMax->Max.vz = A->vz;
    } else {
        MinMax->Min.vz = A->vz;
        MinMax->Max.vz = B->vz;
    }
}

void ProcessSolidBoxs(void)
{
    struct SOLIDBOX *SolidBox;

    for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
        if (SolidBox->Flags) {
            SolidBox->CMinMax = SolidBox->MinMax;
            memset(&SolidBox->Moved, 0, 6);
            if (SolidBox->Primary)
                SolidBox->Primary(SolidBox);
        }
    }
}

extern struct COLLHEADER CollMap;

void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource)
{
    Box->VectBox.Min.vx = Box->Location.vx + Box->Min.vx;
    Box->VectBox.Min.vy = Box->Location.vy + Box->Min.vy;
    Box->VectBox.Min.vz = Box->Location.vz + Box->Min.vz;
    Box->VectBox.Max.vx = Box->Location.vx + Box->Max.vx;
    Box->VectBox.Max.vy = Box->Location.vy + Box->Max.vy;
    Box->VectBox.Max.vz = Box->Location.vz + Box->Max.vz;
    if (AreaSource == 1)
        Box->Area = MoveCollMap.Coll3dSpace[Box->MoveSpace].Area;
    else if (AreaSource == 2)
        Box->Area = CollMap.Coll3dSpace[Box->PrimarySpace].Area;
}

int GetPrimarySpaceFromSecondary(int Indx, struct VECT *Location)
{
    int LinkData;
    int Point;
    struct VECT Vect;
    struct COLL3DSPACE *Space;
    struct COLLPLANE *Plane;

    if (Indx == -1)
        return Find3dSpaceIndxLink(&CollMap, Location, -1, 1);
    Space = &MoveCollMap.Coll3dSpace[Indx];
    LinkData = Space->SortIndx;
    Point = ((LinkData & 0xf800) >> 11) + Space->StartPlane;
    Plane = &MoveCollMap.Coll3dPlane[Point];
    Vect.vx = Space->MinMax.Min.vx + Plane->Point.vx;
    Vect.vy = Space->MinMax.Min.vy + Plane->Point.vy;
    Vect.vz = Space->MinMax.Min.vz + Plane->Point.vz;
    Visable3d(&CollMap, &Vect, Location, LinkData & 0x7ff);
    return CollMap.ReturnSpaceNum;
}

int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace)
{
    return FindNewSpaceFromOld(&CollMap, Location, MoveLoc, GetPrimarySpaceFromSecondary(MoveSpace, MoveLoc));
}

struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup)
{
    struct SOLIDBOX *SolidBox;

    for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
        if (!SolidBox->Flags) {
            if (NewGroup)
                SBoxGroupIdentNum++;
            SolidBox->CMinMax = *MinMax;
            SolidBox->MinMax = SolidBox->CMinMax;
            SolidBox->Flags = Flags | 1;
            SolidBox->LinkedData = 0;
            memset(&SolidBox->Moved, 0, 6);
            SolidBox->Primary = 0;
            SolidBox->GroupIdent = SBoxGroupIdentNum;
            return SolidBox;
        }
    }
    /* no return here in the original either */
}

extern short DrawMonstersOn;
extern short DrawCollisionOn;
extern struct SCRPNT CurrScreenSize;
extern struct WINDOW GameWindows[];
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
void DrawCylinder(struct VECT *Location, short Radius, short MinY, short MaxY);

int CreSeesCre(struct CREATURE *CreA, struct CREATURE *CreB)
{
    int SpaceA;

    SpaceA = GetPrimarySpaceFromSecondary(CreA->Box.MoveSpace, &CreA->Box.Location);
    if (!Visable3d(&CollMap, &CreA->Box.Location, &CreB->Box.Location, SpaceA))
        return 0;
    return VisableSolidBoxLine(&CreA->Box.Location, &CreB->Box.Location, 0) != 0;
}

void DrawDebugBoxs(void)
{
    struct SOLIDBOX *SolidBox;
    struct SOLIDBOX TheBox;
    struct CREATURE **Bump;

    SetRotMatrix(&GameWindows[0].CamMat);
    SetGeomOffset(GameWindows[0].Geom.sx, GameWindows[0].Geom.sy);
    SetGeomScreen(GameWindows[0].Focal);
    CurrScreenSize = GameWindows[0].Size;
    if (DrawCollisionOn) {
        for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
            if (SolidBox->Flags)
                DrawVectBox(&SolidBox->MinMax);
        }
    }
    if (DrawMonstersOn) {
        for (Bump = ThisBumpList; Bump < ThisBumpListEnd; Bump++) {
            if (*Bump != GameWindows[0].Creature)
                DrawCylinder(&(*Bump)->Box.Location, (*Bump)->Box.HitRadius, -(*Bump)->Box.HitHeight + 0x11e, 0x11e);
        }
    }
}
