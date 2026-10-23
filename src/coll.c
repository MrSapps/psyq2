#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);
void GenVectBox(struct VECTBOX *MinMax, struct VECT *A, struct VECT *B);
int GetPrimarySpaceFromSecondary(int Indx, struct VECT *Location);
int FindNewSpaceFromOld(struct COLLHEADER *Map, struct VECT *New, struct VECT *Old, int Space);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);
int InVectBox3D(struct VECTBOX *VectBox, struct VECT *Vect);
int InVectBox(struct VECTBOX *VectBox, struct VECT *Vect);
void InitialiseBumpListFrame(void);
void InitialiseBumpListStart(void);
void AddCreToNextBumpList(struct CREATURE *Cre);
int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB);
int FindBoxMoveSpace(struct COLLBOX3D *Box, int Main);
void ReCalcCollBox(struct CREATURE *Cre);
int VertexAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, short VertexNum, enum DAMAGETYPES DamType);
void ProcessSolidBoxs(void);
int GenerateBoxCollFlags(struct COLLBOX3D *Box);
void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ);
void InitSolidBoxs(void);
struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
void ExpandVectBox(struct VECTBOX *PQBox, struct VECTBOX *PQExp, short Width, struct SHORTPAIR MinMaxY);
void FreeSolidBoxs(struct SOLIDBOX *SolidBox);
int PointHit(struct CREATURE *Aggressor, int PrimarySpace, struct CREATURE *Ignore, struct VECT *Centre, struct SHORTPAIR DamRad, enum DAMAGETYPES DamType, int IgnoreAggressor);
int PointAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, struct MODSUBOFF ModSubOff, enum DAMAGETYPES DamType);
int OutsideExtraCollisionMove(struct VECT *Location, int Flags, int Movement, int CheckSolidBoxs);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
int rand(void); int CheckVersusBumpList(struct CREATURE *Cre);
int ReadCollisionData(struct FILEHDR *File, struct COLLHEADER *CollHdr, struct IDENTITYNAME Name, struct IDENTITYNAME RemapName, int ZeroIfMissing);
int CreSeesCre(struct CREATURE *CreA, struct CREATURE *CreB);
int CheckSolidBoxOkToMove(struct SOLIDBOX *SolidBox, struct SVECT Movement);
int MoveSolidBoxs(struct SOLIDBOX *SolidBox, struct SVECT Movement);
int LineCollideMinMaxBox(struct VECT *P, struct SVECT *PQ, struct VECTBOX *PQExp, struct VECTBOX *MinMax);
int CollideSolidBox(struct VECT *P, struct SVECT *PQ, struct VECTBOX *PQExp, struct VECTBOX *VectBox, struct VECTBOX *CVectBox, struct SVECT *Movement, short Width, struct SHORTPAIR MinMaxY);
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
int VisableSolidBoxMove(struct VECT *P, struct VECT *Q, struct VECT *Stop, short Flags);
void LineIntersectCrePlane(struct VECT *P, struct VECT *Q, struct CREATURE *Target, struct VECT *I);
struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop);
void DrawDebugBoxs(void);
int GenMissileHitPlane(struct VECT *PlaneOrigin, struct COLLPLANE *Plane, struct VECT *Point, struct SVECT *Vel);

struct CREATURE **ThisBumpList;
struct CREATURE **NextBumpList;
struct CREATURE **ThisBumpListEnd;
struct CREATURE **NextBumpListEnd;
struct CREATURE **NextBumpListPntr;
unsigned char SBoxGroupIdentNum;
struct CREATURE *BumpList[2][32];
struct SOLIDBOX SolidBoxs[48];

extern struct COLLHEADER MoveCollMap;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int PointAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, struct MODSUBOFF ModSubOff, enum DAMAGETYPES DamType);
struct MODSUBOFF GetModSubOffFromVertex(struct GLOBMODHDR *Hdr, short VertexNum);

extern struct COLLHEADER CollMap;

extern short DrawMonstersOn;
extern short DrawCollisionOn;
extern struct SCRPNT CurrScreenSize;
extern struct WINDOW GameWindows[];
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
void DrawCylinder(struct VECT *Location, short Radius, short MinY, short MaxY);

int CheckSolidBoxOkToMove(struct SOLIDBOX *SolidBox, struct SVECT Movement);

struct SVECT GetSubModXYZ(struct GLOBMODHDR *GlobHdr, struct MODSUBOFF SubOff, int AnimFrame, int AnimJump);
void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *o2);
void AddCross(struct VECT *Pos, int Radius, struct COLOUR Colour);
int PointHit(struct CREATURE *Aggressor, int PrimarySpace, struct CREATURE *Ignore, struct VECT *Centre, struct SHORTPAIR DamRad, enum DAMAGETYPES DamType, int IgnoreAggressor);

int SquareRoot0(int a);

struct FILEDATA GetFileData(struct FILEHDR *BaseHdr, struct IDENTITYNAME Name);
char *PrintIdentityName(struct IDENTITYNAME Name);
char *PrintBool(int Bool);

int SquareRootNR(int a);
void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType, struct VECT *Centre);

extern struct COLLISIONRETURNDATA CollisionReturnData;
int VisableSolidBoxMove(struct VECT *P, struct VECT *Q, struct VECT *Stop, short Flags);

extern struct COLLHEADER TrigBoundCollMap;

static struct TEMPBACKUPPOS *BackupBase;
extern long *DummyMemory;
extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
int SimpleMove(struct CREATURE *Cre, struct SVECT SVect);

static int AlreadyBackedUp(struct TEMPBACKUPPOS *BackupPntr, struct CREATURE *Cre)
{
    while (BackupPntr-- != BackupBase) {
        if (BackupPntr->Cre == Cre)
            return 1;
    }
    return 0;
}

static struct TEMPBACKUPPOS *BackupCre(struct CREATURE *Cre, struct TEMPBACKUPPOS *Backup)
{
    if (AlreadyBackedUp(Backup, Cre))
        return Backup;
    Backup->Cre = Cre;
    Backup->Location = Cre->Box.Location;
    Backup->MoveSpace = Cre->Box.MoveSpace;
    Backup->Squashed = 0;
    return Backup + 1;
}

static void MarkSquashed(struct CREATURE *Cre, struct TEMPBACKUPPOS *BackupPntr)
{
    while (BackupPntr-- != BackupBase) {
        if (BackupPntr->Cre == Cre) {
            BackupPntr->Squashed = 1;
            return;
        }
    }
}

void FindMaxMin(struct VECT *Start, struct VECT *Stop, struct VECTBOX *VectBox);
struct PAIR QuadraticSolutionsR12(int a, int b, int c);

static int CylinderOverlap(struct COLLBOX3D *Box, struct VECTBOX *VectBox)
{
    if (VectBox->Max.vx < Box->Location.vx - Box->HitRadius)
        return 0;
    if (Box->Location.vx + Box->HitRadius < VectBox->Min.vx)
        return 0;
    if (VectBox->Max.vz < Box->Location.vz - Box->HitRadius)
        return 0;
    if (Box->Location.vz + Box->HitRadius < VectBox->Min.vz)
        return 0;
    if (VectBox->Max.vy < Box->Location.vy - Box->HitHeight + 0x11e)
        return 0;
    return Box->Location.vy + 0x11e >= VectBox->Min.vy;
}

static int CheckBoxFace(struct BOXCHECKDATA *Data)
{
    if (Data->TravelC)
        Data->A = Data->TravelA * Data->TravelledC / Data->TravelC;
    else
        Data->A = 0;
    if (Data->A < Data->MinA || Data->A > Data->MaxA)
        return 0;
    if (Data->TravelC)
        Data->B = Data->TravelB * Data->TravelledC / Data->TravelC;
    else
        Data->B = 0;
    if (Data->B < Data->MinB)
        return 0;
    return Data->B <= Data->MaxB;
}

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

int FindNewSpaceFromOld(struct COLLHEADER *Map, struct VECT *New, struct VECT *Old, int Space)
{
    Visable3d(Map, Old, New, Space);
    return Map->ReturnSpaceNum;
}

int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace)
{
    return FindNewSpaceFromOld(&CollMap, Location, MoveLoc, GetPrimarySpaceFromSecondary(MoveSpace, MoveLoc));
}

int PointHit(struct CREATURE *Aggressor, int PrimarySpace, struct CREATURE *Ignore, struct VECT *Centre, struct SHORTPAIR DamRad, enum DAMAGETYPES DamType, int IgnoreAggressor)
{
    struct CREATURE **Bump;
    struct VECT Max;
    struct VECT Min;
    int Dx;
    int Dy;
    int Dz;
    int Dist2;
    int Size;
    int Size2;
    int Points;
    int bHitSomething;

    bHitSomething = 0;
    Min.vx = Centre->vx - DamRad.b;
    Min.vy = Centre->vy - DamRad.b;
    Min.vz = Centre->vz - DamRad.b;
    Max.vx = Centre->vx + DamRad.b;
    Max.vy = Centre->vy + DamRad.b;
    Max.vz = Centre->vz + DamRad.b;
    for (Bump = ThisBumpList; Bump < ThisBumpListEnd; Bump++) {
        if (*Bump == Aggressor && IgnoreAggressor)
            continue;
        if (*Bump == Ignore)
            continue;
        if (Max.vx <= (*Bump)->Box.VectBox.Min.vx)
            continue;
        if (Min.vx >= (*Bump)->Box.VectBox.Max.vx)
            continue;
        if (Max.vz <= (*Bump)->Box.VectBox.Min.vz)
            continue;
        if (Min.vz >= (*Bump)->Box.VectBox.Max.vz)
            continue;
        if (Max.vy <= (*Bump)->Box.VectBox.Min.vy)
            continue;
        if (Min.vy >= (*Bump)->Box.VectBox.Max.vy)
            continue;
        Dx = Centre->vx - (*Bump)->Box.Location.vx;
        Dy = Centre->vy - (*Bump)->Box.Location.vy;
        Dz = Centre->vz - (*Bump)->Box.Location.vz;
        Dist2 = Dx * Dx + Dy * Dy + Dz * Dz;
        Size = (*Bump)->Box.HitRadius + DamRad.b;
        Size2 = Size * Size;
        if (Size2 < Dist2)
            continue;
        Points = DamRad.a - ((SquareRootNR(Dist2) * 170) >> 12);
        if (*Bump == Aggressor)
            Points >>= 1;
        if (Points > 0 && Visable3d(&CollMap, Centre, &(*Bump)->Box.Location, PrimarySpace)
            && VisableSolidBoxLine(Centre, &(*Bump)->Box.Location, 0)) {
            DoDamage(Aggressor, *Bump, Points, DamType, Centre);
            bHitSomething = 1;
        }
    }
    return bHitSomething;
}

int PointAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, struct MODSUBOFF ModSubOff, enum DAMAGETYPES DamType)
{
    struct SVECT Rel;
    struct SVECT Offset;
    struct VECT Pos;

    /* the original copies ModSubOff member by member into a temporary */
    Rel = GetSubModXYZ(Cre->ModelHdr, (struct MODSUBOFF){ModSubOff.Offset, ModSubOff.SubModel}, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    RotateS(&Cre->Matrix, &Rel, &Offset);
    Pos.vx = Cre->Draw.Location.vx + Offset.vx;
    Pos.vy = Cre->Draw.Location.vy + Offset.vy;
    Pos.vz = Cre->Draw.Location.vz + Offset.vz;
    if (DrawCollisionOn)
        AddCross(&Pos, DamRad.b, *&(struct COLOUR){0xff, 0xff, 0xff, 0});
    return PointHit(Cre, GetPrimarySpaceFromSecondary(Cre->Box.MoveSpace, &Cre->Box.Location), Cre, &Pos, DamRad, DamType, 1);
}

int VertexAttack(struct CREATURE *Cre, struct SHORTPAIR DamRad, short VertexNum, enum DAMAGETYPES DamType)
{
    return PointAttack(Cre, DamRad, GetModSubOffFromVertex(Cre->ModelHdr, VertexNum), DamType);
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

int OutsideExtraCollisionMove(struct VECT *Location, int Flags, int Movement, int CheckSolidBoxs)
{
    struct SOLIDBOX *SolidBox;
    struct VECTBOX VectBox;
    struct COLL3DSPACE *Trigger;

    VectBox.Min.vx = Location->vx - 0x11e;
    VectBox.Min.vy = Location->vy - 0x11d;
    VectBox.Min.vz = Location->vz - 0x11e;
    VectBox.Max.vx = Location->vx + 0x11e;
    VectBox.Max.vy = Location->vy + 0x11d;
    VectBox.Max.vz = Location->vz + 0x11e;
    if (CheckSolidBoxs) {
        for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
            if (SolidBox->Flags) {
                if (Movement) {
                    if (VectBoxOverlap(&VectBox, &SolidBox->CMinMax))
                        return 1;
                } else {
                    if (VectBoxOverlap(&VectBox, &SolidBox->MinMax))
                        return 1;
                }
            }
        }
    }
    if (Flags) {
        for (Trigger = TrigBoundCollMap.Coll3dSpace; Trigger < TrigBoundCollMap.Coll3dSpace + TrigBoundCollMap.TotalNumSpaces; Trigger++) {
            if ((Trigger->u.trigflags & Flags) && VectBoxOverlap(&VectBox, &Trigger->MinMax))
                return Trigger->u.trigflags;
        }
    }
    return 0;
}

int GenerateBoxCollFlags(struct COLLBOX3D *Box)
{
    int Flags;

    Flags = 0;
    if (Box->UseCreatureBlockers)
        Flags = 0x810;
    return Flags;
}

void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ)
{
    Box->Min.vx = -RadiusXYZ.vx;
    Box->Min.vy = -RadiusXYZ.vy;
    Box->Min.vz = -RadiusXYZ.vz;
    Box->Max = RadiusXYZ;
}

int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize)
{
    struct VECT Temp;
    int Flags;
    struct SVECT *N;

    Flags = GenerateBoxCollFlags(Box);
    Box->Min.vx = -CollSize.sx;
    Box->Min.vy = -CollSize.sy + 0x11e;
    Box->Min.vz = -CollSize.sx;
    Box->Max.vx = CollSize.sx;
    Box->Max.vy = 0x11e;
    Box->Max.vz = CollSize.sx;
    Box->Radius = CollSize.sx;
    Box->HitRadius = HitSize.a;
    Box->HitHeight = HitSize.b;
    Box->Location = *Location;
    Box->Location.vy -= 0x11e;
    Temp = Box->Location;
    Temp.vy += MainSpaceDrop.b;
    Box->Location.vy -= 0x1e;
    Box->MoveSpace = Find3dSpaceIndxLink(&MoveCollMap, &Box->Location, MainSpaceDrop.a, 1);
    if (!VisableSolidBoxMove(&Box->Location, &Temp, &Temp, Flags)) {
        Temp.vy -= 1;
        if (CollisionReturnData.IdentType == 2) {
            Box->SolidBoxGroupIdent = CollisionReturnData.GroupIdent;
            Box->OnSolidBox = 1;
        }
    }
    if (!Visable3d(&MoveCollMap, &Box->Location, &Temp, Box->MoveSpace)) {
        N = &MoveCollMap.ReturnPlane->Normal;
        if (Box->FloorNorm.vy < N->vy) {
            Box->FloorNorm = *N;
            if (N->vy > Box->FloorNormMin)
                Box->OnFloor = 1;
        }
    }
    Box->Location.vy = MoveCollMap.AreaIntersectLast->Intersect.vy;
    Box->Area = MoveCollMap.AreaIntersectLast->Area;
    Box->MoveSpace = MoveCollMap.ReturnSpaceNum;
    UpdateBoxStuff(Box, 0);
    return Box->MoveSpace != -1;
}

int FindBoxMoveSpace(struct COLLBOX3D *Box, int Main)
{
    Box->MoveSpace = Find3dSpaceIndxLink(&MoveCollMap, &Box->Location, Main, 1);
    Box->Area = MoveCollMap.Coll3dSpace[Box->MoveSpace].Area;
    return Box->MoveSpace != -1;
}

void ReCalcCollBox(struct CREATURE *Cre)
{
    Cre->Box.MoveSpace = -1;
}

void InitialiseBumpListStart(void)
{
    ThisBumpList = BumpList[0];
    ThisBumpListEnd = BumpList[1];
    NextBumpList = BumpList[1];
    NextBumpListEnd = BumpList[1] + 32;
    NextBumpListPntr = BumpList[1];
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

void AddCreToNextBumpList(struct CREATURE *Cre)
{
    if (NextBumpListPntr != NextBumpListEnd)
        *NextBumpListPntr++ = Cre;
}

int rand(void);

int CheckVersusBumpList(struct CREATURE *Cre)
{
    struct CREATURE **Bump;
    struct VECT Max;
    struct VECT Min;
    struct VECT Centre;
    int Dx;
    int Dz;
    int Dist2;
    int Size;
    int Size2;
    int Root;
    int ToMove;
    int RetVal;
    struct VECTBOX VectBox;

    RetVal = 0;
    if (Cre->Control & 0x8000)
        return RetVal;
    Centre.vx = Cre->Box.Location.vx + Cre->MoveVel.vx;
    Centre.vy = Cre->Box.Location.vy + Cre->MoveVel.vy;
    Centre.vz = Cre->Box.Location.vz + Cre->MoveVel.vz;
    VectBox.Min.vx = Centre.vx + Cre->Box.Min.vx;
    VectBox.Min.vy = Centre.vy + Cre->Box.Min.vy;
    VectBox.Min.vz = Centre.vz + Cre->Box.Min.vz;
    VectBox.Max.vx = Centre.vx + Cre->Box.Max.vx;
    VectBox.Max.vy = Centre.vy + Cre->Box.Max.vy;
    VectBox.Max.vz = Centre.vz + Cre->Box.Max.vz;
    for (Bump = ThisBumpList; Bump < ThisBumpListEnd; Bump++) {
        if (VectBoxOverlap(&VectBox, &(*Bump)->Box.VectBox) && *Bump != Cre && !((*Bump)->Control & 0x8000)) {
            Dx = Centre.vx - (*Bump)->Box.Location.vx;
            Dz = Centre.vz - (*Bump)->Box.Location.vz;
            Dist2 = Dx * Dx + Dz * Dz;
            Size = (*Bump)->Box.Radius + Cre->Box.Radius;
            Size2 = Size * Size;
            if (Size2 >= Dist2) {
                /* Max is never set: the original reads it uninitialised */
                if (Max.vy >= (*Bump)->Box.VectBox.Min.vy && Max.vy - Cre->MoveVel.vy <= (*Bump)->Box.VectBox.Min.vy) {
                    RetVal |= 2;
                    Cre->MoveVel.vy -= Max.vy - (*Bump)->Box.VectBox.Min.vy;
                    Cre->Control |= 0x50000;
                } else {
                    RetVal |= 1;
                    Root = SquareRoot0(Dist2);
                    Cre->Control |= 0x40000;
                    (*Bump)->Control |= 0x40000;
                    ToMove = Size - Root;
                    if (Root && (Dx | Dz)) {
                        Cre->MoveVel.vx += Dx * ToMove / Root;
                        Cre->MoveVel.vz += Dz * ToMove / Root;
                    } else {
                        Cre->MoveVel.vx += (rand() - 0x4000) >> 10;
                        Cre->MoveVel.vz += (rand() - 0x4000) >> 10;
                    }
                }
            }
        }
    }
    return RetVal;
}

int ReadCollisionData(struct FILEHDR *File, struct COLLHEADER *CollHdr, struct IDENTITYNAME Name, struct IDENTITYNAME RemapName, int ZeroIfMissing)
{
    CollHdr->CollMap3d = GetFileData(File, Name).Addr;
    if (CollHdr->CollMap3d) {
        CollHdr->TotalNumSpaces = CollHdr->CollMap3d[0];
        CollHdr->TotalNumPlanes = CollHdr->CollMap3d[1];
        CollHdr->Coll3dSpace = (struct COLL3DSPACE *)(CollHdr->CollMap3d + 2);
        CollHdr->Coll3dPlane = (struct COLLPLANE *)&CollHdr->Coll3dSpace[CollHdr->TotalNumSpaces + 1];
        CollHdr->Coll3dJoin = (struct COLL3DCONX *)(CollHdr->Coll3dPlane + CollHdr->TotalNumPlanes);
        CollHdr->RemapTable = GetFileData(File, RemapName).Addr;
        PrintIdentityName(Name);
        PrintBool((int)CollHdr->RemapTable);
        return 1;
    }
    if (ZeroIfMissing) {
        CollHdr->TotalNumSpaces = 0;
        CollHdr->TotalNumPlanes = 0;
    }
    return 0;
}

int CreSeesCre(struct CREATURE *CreA, struct CREATURE *CreB)
{
    int SpaceA;

    SpaceA = GetPrimarySpaceFromSecondary(CreA->Box.MoveSpace, &CreA->Box.Location);
    if (!Visable3d(&CollMap, &CreA->Box.Location, &CreB->Box.Location, SpaceA))
        return 0;
    return VisableSolidBoxLine(&CreA->Box.Location, &CreB->Box.Location, 0) != 0;
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

int CheckSolidBoxOkToMove(struct SOLIDBOX *SolidBox, struct SVECT Movement)
{
    struct VECTBOX VectBox;
    struct VECTBOX TempVectBox;
    struct CREATURE **Bump;
    struct CREATURE *Cre;
    struct SOLIDBOX *SBox;
    struct SVECT SVect;
    unsigned char MyGroupIdent;
    int Return;
    struct TEMPBACKUPPOS *BackupPntr;
    struct SVECT ThisMovement;

    MyGroupIdent = SolidBox->GroupIdent;
    BackupBase = (struct TEMPBACKUPPOS *)DummyMemory;
    BackupPntr = BackupBase;
    Return = 1;
    for (SBox = SolidBox; SBox; SBox = SBox->LinkedData)
        SBox->Flags &= ~1;
    for (Cre = Creatures; Cre < (struct CREATURE *)CreFreeList; Cre++) {
        if (Cre->Life > 0 && Cre->Box.OnSolidBox && Cre->Box.SolidBoxGroupIdent == MyGroupIdent) {
            BackupPntr = BackupCre(Cre, BackupPntr);
            if (!SimpleMove(Cre, Movement)) {
                Return = 0;
                goto Done; /* the SYM lists a label Done */
            }
        }
    }
    for (SBox = SolidBox; SBox; SBox = SBox->LinkedData) {
        VectBox.Min.vx = SBox->MinMax.Min.vx + Movement.vx;
        VectBox.Min.vy = SBox->MinMax.Min.vy + Movement.vy;
        VectBox.Min.vz = SBox->MinMax.Min.vz + Movement.vz;
        VectBox.Max.vx = SBox->MinMax.Max.vx + Movement.vx;
        VectBox.Max.vy = SBox->MinMax.Max.vy + Movement.vy;
        VectBox.Max.vz = SBox->MinMax.Max.vz + Movement.vz;
        for (Bump = ThisBumpList; Bump < ThisBumpListEnd; Bump++) {
            if ((*Bump)->Box.OnSolidBox && (*Bump)->Box.SolidBoxGroupIdent == MyGroupIdent)
                continue;
            if (VectBoxOverlap(&VectBox, &(*Bump)->Box.VectBox)) {
                ThisMovement = Movement;
                if ((*Bump)->Box.OnFloor || (*Bump)->Box.OnSolidBox) {
                    if (ThisMovement.vx || ThisMovement.vz)
                        ThisMovement.vy -= 20;
                }
                BackupPntr = BackupCre(*Bump, BackupPntr);
                if (!SimpleMove(*Bump, ThisMovement)) {
                    MarkSquashed(*Bump, BackupPntr);
                    Return = 0;
                }
            }
        }
    }
Done:
    for (SBox = SolidBox; SBox; SBox = SBox->LinkedData)
        SBox->Flags |= 1;
    if (!Return) {
        while (BackupBase != BackupPntr--) {
            BackupPntr->Cre->Box.Location = BackupPntr->Location;
            BackupPntr->Cre->Box.MoveSpace = BackupPntr->MoveSpace;
            if (BackupPntr->Squashed)
                DoDamage(0, BackupPntr->Cre, 30, 20, 0);
        }
    }
    return Return;
}

int MoveSolidBoxs(struct SOLIDBOX *SolidBox, struct SVECT Movement)
{
    struct SOLIDBOX *SBox;

    if (!CheckSolidBoxOkToMove(SolidBox, Movement))
        return 0;
    for (SBox = SolidBox; SBox; SBox = SBox->LinkedData) {
        SBox->MinMax.Min.vx += Movement.vx;
        SBox->MinMax.Min.vy += Movement.vy;
        SBox->MinMax.Min.vz += Movement.vz;
        SBox->MinMax.Max.vx += Movement.vx;
        SBox->MinMax.Max.vy += Movement.vy;
        SBox->MinMax.Max.vz += Movement.vz;
        SBox->Moved.vx += Movement.vx;
        SBox->Moved.vy += Movement.vy;
        SBox->Moved.vz += Movement.vz;
        if (Movement.vx < 0)
            SBox->CMinMax.Min.vx += Movement.vx;
        else
            SBox->CMinMax.Max.vx += Movement.vx;
        if (Movement.vy < 0)
            SBox->CMinMax.Min.vy += Movement.vy;
        else
            SBox->CMinMax.Max.vy += Movement.vy;
        if (Movement.vz < 0)
            SBox->CMinMax.Min.vz += Movement.vz;
        else
            SBox->CMinMax.Max.vz += Movement.vz;
    }
    return 1;
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

int LineCollideMinMaxBox(struct VECT *P, struct SVECT *PQ, struct VECTBOX *PQExp, struct VECTBOX *MinMax)
{
    struct BOXCHECKDATA Data;

    if (PQExp->Min.vx > MinMax->Max.vx)
        return 0;
    if (PQExp->Max.vx < MinMax->Min.vx)
        return 0;
    if (PQExp->Min.vz > MinMax->Max.vz)
        return 0;
    if (PQExp->Max.vz < MinMax->Min.vz)
        return 0;
    if (PQExp->Min.vy > MinMax->Max.vy)
        return 0;
    if (PQExp->Max.vy < MinMax->Min.vy)
        return 0;

    Data.MinA = MinMax->Min.vx - P->vx;
    Data.MaxA = MinMax->Max.vx - P->vx;
    Data.TravelA = PQ->vx;
    Data.MinB = MinMax->Min.vz - P->vz;
    Data.MaxB = MinMax->Max.vz - P->vz;
    Data.TravelB = PQ->vz;
    Data.TravelC = PQ->vy;
    if (P->vy <= MinMax->Min.vy) {
        Data.TravelledC = MinMax->Min.vy - P->vy;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, MinMax->Min.vy, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0x1000, 0};
            return 1;
        }
    } else if (P->vy >= MinMax->Max.vy) {
        Data.TravelledC = MinMax->Max.vy - P->vy;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, MinMax->Max.vy, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0, -0x1000, 0};
            return 1;
        }
    }

    Data.MinA = MinMax->Min.vx - P->vx;
    Data.MaxA = MinMax->Max.vx - P->vx;
    Data.TravelA = PQ->vx;
    Data.MinB = MinMax->Min.vy - P->vy;
    Data.MaxB = MinMax->Max.vy - P->vy;
    Data.TravelB = PQ->vy;
    Data.TravelC = PQ->vz;
    if (P->vz <= MinMax->Min.vz) {
        Data.TravelledC = MinMax->Min.vz - P->vz;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, P->vy + Data.B, MinMax->Min.vz};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0, 0x1000};
            return 1;
        }
    } else if (P->vz >= MinMax->Max.vz) {
        Data.TravelledC = MinMax->Max.vz - P->vz;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, P->vy + Data.B, MinMax->Max.vz};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0, -0x1000};
            return 1;
        }
    }

    Data.MinA = MinMax->Min.vy - P->vy;
    Data.MaxA = MinMax->Max.vy - P->vy;
    Data.TravelA = PQ->vy;
    Data.MinB = MinMax->Min.vz - P->vz;
    Data.MaxB = MinMax->Max.vz - P->vz;
    Data.TravelB = PQ->vz;
    Data.TravelC = PQ->vx;
    if (P->vx <= MinMax->Min.vx) {
        Data.TravelledC = MinMax->Min.vx - P->vx;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){MinMax->Min.vx, P->vy + Data.A, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0x1000, 0, 0};
            return 1;
        }
    } else if (P->vx >= MinMax->Max.vx) {
        Data.TravelledC = MinMax->Max.vx - P->vx;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){MinMax->Max.vx, P->vy + Data.A, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){-0x1000, 0, 0};
            return 1;
        }
    }
    return 0;
}

/* Width is int in the SYM; as a short param it is loaded at entry, as in the original */
int CollideSolidBox(struct VECT *P, struct SVECT *PQ, struct VECTBOX *PQExp, struct VECTBOX *VectBox, struct VECTBOX *CVectBox, struct SVECT *Movement, short Width, struct SHORTPAIR MinMaxY)
{
    struct VECTBOX BoxExp;
    struct BOXCHECKDATA Data;
    int Temp;
    struct SHORTPAIR MinMaxSwapY;

    MinMaxSwapY = (struct SHORTPAIR){-MinMaxY.b, -MinMaxY.a};
    if (PQExp->Min.vx > CVectBox->Max.vx)
        return 0;
    if (PQExp->Max.vx < CVectBox->Min.vx)
        return 0;
    if (PQExp->Min.vz > CVectBox->Max.vz)
        return 0;
    if (PQExp->Max.vz < CVectBox->Min.vz)
        return 0;
    if (PQExp->Min.vy > CVectBox->Max.vy)
        return 0;
    if (PQExp->Max.vy < CVectBox->Min.vy)
        return 0;
    ExpandVectBox(VectBox, &BoxExp, Width, MinMaxSwapY);

    Data.MinA = BoxExp.Min.vx - P->vx;
    Data.MaxA = BoxExp.Max.vx - P->vx;
    Data.TravelA = PQ->vx;
    Data.MinB = BoxExp.Min.vz - P->vz;
    Data.MaxB = BoxExp.Max.vz - P->vz;
    Data.TravelB = PQ->vz;
    Data.TravelC = PQ->vy - Movement->vy;
    Temp = P->vy + Movement->vy;
    if (BoxExp.Min.vy >= Temp && Data.TravelC >= 0) {
        Data.TravelledC = BoxExp.Min.vy - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, BoxExp.Min.vy, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0x1000, 0};
            return 1;
        }
    } else if (BoxExp.Max.vy <= Temp && Data.TravelC <= 0) {
        Data.TravelledC = BoxExp.Max.vy - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, BoxExp.Max.vy, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0, -0x1000, 0};
            return 1;
        }
    }

    Data.MinA = BoxExp.Min.vx - P->vx;
    Data.MaxA = BoxExp.Max.vx - P->vx;
    Data.TravelA = PQ->vx;
    Data.MinB = BoxExp.Min.vy - P->vy;
    Data.MaxB = BoxExp.Max.vy - P->vy;
    Data.TravelB = PQ->vy;
    Data.TravelC = PQ->vz - Movement->vz;
    Temp = P->vz + Movement->vz;
    if (BoxExp.Min.vz >= Temp && Data.TravelC >= 0) {
        Data.TravelledC = BoxExp.Min.vz - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, P->vy + Data.B, BoxExp.Min.vz - 1};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0, 0x1000};
            return 1;
        }
    } else if (BoxExp.Max.vz <= Temp && Data.TravelC <= 0) {
        Data.TravelledC = BoxExp.Max.vz - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){P->vx + Data.A, P->vy + Data.B, BoxExp.Max.vz + 1};
            CollisionReturnData.Normal = *&(struct SVECT){0, 0, -0x1000};
            return 1;
        }
    }

    Data.MinA = BoxExp.Min.vy - P->vy;
    Data.MaxA = BoxExp.Max.vy - P->vy;
    Data.TravelA = PQ->vy;
    Data.MinB = BoxExp.Min.vz - P->vz;
    Data.MaxB = BoxExp.Max.vz - P->vz;
    Data.TravelB = PQ->vz;
    Data.TravelC = PQ->vx - Movement->vx;
    Temp = P->vx + Movement->vx;
    if (BoxExp.Min.vx >= Temp && Data.TravelC >= 0) {
        Data.TravelledC = BoxExp.Min.vx - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){BoxExp.Min.vx - 1, P->vy + Data.A, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){0x1000, 0, 0};
            return 1;
        }
    } else if (BoxExp.Max.vx <= Temp && Data.TravelC <= 0) {
        Data.TravelledC = BoxExp.Max.vx - Temp;
        if (CheckBoxFace(&Data)) {
            CollisionReturnData.IdentType = -1;
            CollisionReturnData.Location = (struct VECT){BoxExp.Max.vx + 1, P->vy + Data.A, P->vz + Data.B};
            CollisionReturnData.Normal = *&(struct SVECT){-0x1000, 0, 0};
            return 1;
        }
    }
    return 0;
}

int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop)
{
    struct VECTBOX PQBox;
    struct SOLIDBOX *SolidBox;
    struct SVECT PQ;
    int Vis;

    PQ.vx = Q->vx - P->vx;
    PQ.vy = Q->vy - P->vy;
    PQ.vz = Q->vz - P->vz;
    Vis = 1;
    GenVectBox(&PQBox, P, Q);
    if (Stop)
        *Stop = *Q;
    for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
        if (SolidBox->Flags && LineCollideMinMaxBox(P, &PQ, &PQBox, &SolidBox->MinMax)) {
            CollisionReturnData.IdentType = 2;
            CollisionReturnData.GroupIdent = SolidBox->GroupIdent;
            CollisionReturnData.Ident = SolidBox - SolidBoxs;
            if (Stop) {
                *Stop = CollisionReturnData.Location;
                PQ.vx = Stop->vx - P->vx;
                PQ.vy = Stop->vy - P->vy;
                PQ.vz = Stop->vz - P->vz;
                Vis = 0;
                GenVectBox(&PQBox, P, Stop);
            } else
                return 0;
        }
    }
    return Vis;
}

int VisableSolidBoxMove(struct VECT *P, struct VECT *Q, struct VECT *Stop, short Flags)
{
    struct VECTBOX PQBox;
    struct VECTBOX PQExp;
    struct SOLIDBOX *SolidBox;
    struct SVECT PQ;
    int Visible;
    struct COLL3DSPACE *Trigger;

    PQ.vx = Q->vx - P->vx;
    PQ.vy = Q->vy - P->vy;
    PQ.vz = Q->vz - P->vz;
    Visible = 1;
    if (Stop)
        *Stop = *Q;
    GenVectBox(&PQBox, P, Q);
    ExpandVectBox(&PQBox, &PQExp, 0x11e, (struct SHORTPAIR){-0x11e, 0x11e});
    for (SolidBox = SolidBoxs; SolidBox < SolidBoxs + 48; SolidBox++) {
        if (SolidBox->Flags && CollideSolidBox(P, &PQ, &PQExp, &SolidBox->MinMax, &SolidBox->CMinMax, &SolidBox->Moved, 0x11e, (struct SHORTPAIR){-0x11e, 0x11e})) {
            CollisionReturnData.IdentType = 2;
            CollisionReturnData.GroupIdent = SolidBox->GroupIdent;
            CollisionReturnData.Ident = SolidBox - SolidBoxs;
            if (Stop) {
                *Stop = CollisionReturnData.Location;
                PQ.vx = Stop->vx - P->vx;
                PQ.vy = Stop->vy - P->vy;
                PQ.vz = Stop->vz - P->vz;
                Visible = 0;
                GenVectBox(&PQBox, P, Stop);
                ExpandVectBox(&PQBox, &PQExp, 0x11e, (struct SHORTPAIR){-0x11e, 0x11e});
            } else
                return 0;
        }
    }
    if (Flags) {
        for (Trigger = TrigBoundCollMap.Coll3dSpace; Trigger < TrigBoundCollMap.Coll3dSpace + TrigBoundCollMap.TotalNumSpaces; Trigger++) {
            if ((Trigger->u.trigflags & Flags) && CollideSolidBox(P, &PQ, &PQExp, &Trigger->MinMax, &Trigger->MinMax, &*&(struct SVECT){0, 0, 0}, 0x11e, (struct SHORTPAIR){-0x11e, 0x11e})) {
                CollisionReturnData.IdentType = 3;
                CollisionReturnData.Ident = Trigger - TrigBoundCollMap.Coll3dSpace;
                if (Stop) {
                    *Stop = CollisionReturnData.Location;
                    PQ.vx = Stop->vx - P->vx;
                    PQ.vy = Stop->vy - P->vy;
                    PQ.vz = Stop->vz - P->vz;
                    Visible = 0;
                    GenVectBox(&PQBox, P, Stop);
                    ExpandVectBox(&PQBox, &PQExp, 0x11e, (struct SHORTPAIR){-0x11e, 0x11e});
                } else
                    return 0;
            }
        }
    }
    return Visible;
}

void LineIntersectCrePlane(struct VECT *P, struct VECT *Q, struct CREATURE *Target, struct VECT *I)
{
    struct VECT AQ;
    struct VECT PQ;
    struct VECT CP;
    struct VECT AP;
    int Dist2;
    int D2;

    PQ.vx = Q->vx - P->vx;
    PQ.vy = Q->vy - P->vy;
    PQ.vz = Q->vz - P->vz;
    AQ.vx = Q->vx - Target->Box.Location.vx;
    AQ.vy = Q->vy - Target->Box.Location.vy;
    AQ.vz = Q->vz - Target->Box.Location.vz;
    AP.vx = P->vx - Target->Box.Location.vx;
    AP.vy = P->vy - Target->Box.Location.vy;
    AP.vz = P->vz - Target->Box.Location.vz;
    CP.vx = (AP.vy * AQ.vz - AP.vz * AQ.vy) / 4096;
    CP.vy = (AP.vz * AQ.vx - AP.vx * AQ.vz) / 4096;
    CP.vz = (AP.vx * AQ.vy - AP.vy * AQ.vx) / 4096;
    Dist2 = CP.vx * CP.vx + CP.vy * CP.vy + CP.vz * CP.vz;
    D2 = AP.vx * AP.vx + AP.vy * AP.vy + AP.vz * AP.vz;
    Dist2 = SquareRoot0(D2 - Dist2);
    I->vx = P->vx + PQ.vx * Dist2 / 4096;
    I->vy = P->vy + PQ.vy * Dist2 / 4096;
    I->vz = P->vz + PQ.vz * Dist2 / 4096;
}

struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop)
{
    struct CREATURE **Bump;
    struct VECTBOX VectBox;
    struct VECT PQ;
    struct VECT A;
    struct VECT AQ;
    struct VECT AP;
    struct VECT Q2;
    struct CREATURE *HitCre;
    long Dist;
    long PQLengthXZ;
    int a;
    int b; /* not in the SYM, but the code computes it before the a != 0 test */
    int c;
    int EntryXZ;
    int ExitXZ;
    int EntryY;
    int ExitY;
    int EntryK;
    int ExitK;
    struct PAIR Solutions12;
    int MinY;
    int MaxY;

    HitCre = 0;
    PQ.vx = Q->vx - P->vx;
    PQ.vy = Q->vy - P->vy;
    PQ.vz = Q->vz - P->vz;
    PQLengthXZ = SquareRootNR(PQ.vx * PQ.vx + PQ.vy * PQ.vy + PQ.vz * PQ.vz);
    Q2.vx = P->vx + PQ.vx;
    Q2.vy = P->vy + PQ.vy;
    Q2.vz = P->vz + PQ.vz;
    FindMaxMin(P, Q, &VectBox);
    for (Bump = BumpStart; Bump < BumpStop; Bump++) {
        if (CreIgnore == *Bump)
            continue;
        if (!CylinderOverlap(&(*Bump)->Box, &VectBox))
            continue;
        A = (*Bump)->Box.Location;
        AP.vx = P->vx - A.vx;
        AP.vy = P->vy - A.vy;
        AP.vz = P->vz - A.vz;
        if (PQ.vx * AP.vx + PQ.vy * AP.vy + PQ.vz * AP.vz >= 0)
            continue;
        AQ.vx = Q2.vx - A.vx;
        AQ.vy = Q2.vy - A.vy;
        AQ.vz = Q2.vz - A.vz;
        Dist = abs(AP.vz * AQ.vx - AP.vx * AQ.vz);
        if (Dist >= (*Bump)->Box.HitRadius * PQLengthXZ)
            continue;
        a = PQ.vx * PQ.vx + PQ.vz * PQ.vz;
        b = (PQ.vx * AP.vx + PQ.vz * AP.vz) * 2;
        c = AP.vx * AP.vx + AP.vz * AP.vz - (*Bump)->Box.HitRadius * (*Bump)->Box.HitRadius;
        if (a) {
            Solutions12 = QuadraticSolutionsR12(a, b, c);
            if ((Solutions12.a & Solutions12.b) == 0x7fffffff)
                continue;
            EntryXZ = Solutions12.a;
            ExitXZ = Solutions12.b;
        } else {
            EntryXZ = 0;
            ExitXZ = 0x1000;
        }
        if (EntryXZ < 0 && ExitXZ >= 0)
            EntryXZ = 0;
        if (EntryXZ < 0 || EntryXZ >= 0x1000)
            continue;
        MaxY = (*Bump)->Box.Location.vy + 0x11e;
        MinY = MaxY - (*Bump)->Box.HitHeight;
        if (PQ.vy > 0) {
            EntryY = ((MinY - P->vy) << 12) / PQ.vy;
            ExitY = ((MaxY - P->vy) << 12) / PQ.vy;
        } else if (PQ.vy < 0) {
            EntryY = ((MaxY - P->vy) << 12) / PQ.vy;
            ExitY = ((MinY - P->vy) << 12) / PQ.vy;
        } else {
            if (P->vy < MinY || P->vy > MaxY)
                continue;
            EntryY = 0;
            ExitY = 0x1000;
        }
        EntryK = EntryY;
        if (EntryK < EntryXZ)
            EntryK = EntryXZ;
        ExitK = ExitY;
        if (ExitXZ < ExitK)
            ExitK = ExitXZ;
        if (ExitK < EntryK)
            continue;
        if (EntryK > 0x1000)
            continue;
        HitCre = *Bump;
        if (!I)
            return HitCre;
        Q2.vx = P->vx + PQ.vx * EntryK / 4096;
        Q2.vy = P->vy + PQ.vy * EntryK / 4096;
        Q2.vz = P->vz + PQ.vz * EntryK / 4096;
        PQ.vx = Q2.vx - P->vx;
        PQ.vy = Q2.vy - P->vy;
        PQ.vz = Q2.vz - P->vz;
        PQLengthXZ = SquareRootNR(PQ.vx * PQ.vx + PQ.vy * PQ.vy + PQ.vz * PQ.vz);
        FindMaxMin(P, &Q2, &VectBox);
    }
    *I = Q2;
    return HitCre;
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

void FreeSolidBoxs(struct SOLIDBOX *SolidBox)
{
    while (SolidBox) {
        SolidBox->Flags = 0;
        SolidBox = SolidBox->LinkedData;
    }
}

int GenMissileHitPlane(struct VECT *PlaneOrigin, struct COLLPLANE *Plane, struct VECT *Point, struct SVECT *Vel)
{
    int Speed;
    int Dist;
    struct VECT Rel;
    struct VECT ToMove;

    Speed = Plane->Normal.vx * Vel->vx + Plane->Normal.vy * Vel->vy + Plane->Normal.vz * Vel->vz;
    if (Speed >= 0)
        return 0;
    Rel.vx = PlaneOrigin->vx + Plane->Point.vx - Point->vx;
    Rel.vy = PlaneOrigin->vy + Plane->Point.vy - Point->vy;
    Rel.vz = PlaneOrigin->vz + Plane->Point.vz - Point->vz;
    Dist = Plane->Normal.vx * Rel.vx + Plane->Normal.vy * Rel.vy + Plane->Normal.vz * Rel.vz;
    ToMove.vx = Vel->vx * Dist / Speed;
    ToMove.vy = Vel->vy * Dist / Speed;
    ToMove.vz = Vel->vz * Dist / Speed;
    Point->vx += ToMove.vx;
    Point->vy += ToMove.vy;
    Point->vz += ToMove.vz;
    return 1;
}
