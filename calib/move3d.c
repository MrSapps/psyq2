#include "types.h"

extern struct COLLHEADER CollMap;
extern struct COLLHEADER MoveCollMap;
struct COLLHEADER *AsPointMap;
short sDamage;
short *pSpace;

int CreCollideAsPointFunc(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre);

int CreCollideAsFatPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage)
{
    AsPointMap = &MoveCollMap;
    pSpace = &Cre->Box.MoveSpace;
    sDamage = Damage;
    return CreCollideAsPointFunc(Cre, CreIgnore, Bounce, ppHitCre);
}

int CreCollideAsPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage)
{
    AsPointMap = &CollMap;
    pSpace = &Cre->Box.PrimarySpace;
    sDamage = Damage;
    return CreCollideAsPointFunc(Cre, CreIgnore, Bounce, ppHitCre);
}

int GenerateBoxCollFlags(struct COLLBOX3D *Box);
int OutsideExtraCollisionMove(struct VECT *Location, int Flags, int Movement, int CheckSolidBoxs);
int NewTrueMove3dInternal(struct COLLBOX3D *Box, struct SVECT *Rel, struct SHORTPAIR DownPush, struct SHORTPAIR Depth_CheckOther);

int NewTrueMove3dActual(struct COLLBOX3D *Box, struct SVECT *Rel, int Depth, struct SHORTPAIR DownPush)
{
    struct SVECT SVect;
    int Flags;
    int NoProblem;
    struct VECT OldStart;
    struct SVECT OldFloorNorm;
    struct SVECT OldWallNorm;
    short OldSpace;

    Flags = GenerateBoxCollFlags(Box);
    OldStart = Box->Location;
    OldSpace = Box->MoveSpace;
    OldFloorNorm = Box->FloorNorm;
    OldWallNorm = Box->WallNorm;
    NoProblem = NewTrueMove3dInternal(Box, Rel, DownPush, (struct SHORTPAIR){Depth, 0});
    if (OutsideExtraCollisionMove(&Box->Location, Flags, 1, 1)) {
        Box->Location = OldStart;
        Box->MoveSpace = OldSpace;
        Box->FloorNorm = OldFloorNorm;
        Box->WallNorm = OldWallNorm;
        Box->OnFloor = 0;
        Box->OnSolidBox = 0;
        NoProblem = NewTrueMove3dInternal(Box, Rel, DownPush, (struct SHORTPAIR){Depth, 1});
    }
    return NoProblem;
}

int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);

int SimpleMove(struct CREATURE *Cre, struct SVECT SVect)
{
    struct VECT OldLocation;
    short OldSpace;

    if (Cre->Control & 0x8000000) {
        struct VECT Vect;
        struct VECT Old;

        if (Cre->Box.MoveSpace == -1)
            return -1;
        Vect.vx = Cre->Box.Location.vx + SVect.vx;
        Vect.vy = Cre->Box.Location.vy + SVect.vy;
        Vect.vz = Cre->Box.Location.vz + SVect.vz;
        Old = Cre->Box.Location;
        Visable3d(&MoveCollMap, &Cre->Box.Location, &Vect, Cre->Box.MoveSpace);
        Cre->Box.Location = MoveCollMap.AreaIntersectLast->Intersect;
        Cre->Box.MoveSpace = MoveCollMap.ReturnSpaceNum;
        UpdateBoxStuff(&Cre->Box, 1);
        Cre->Draw.Location = Cre->Box.Location;
        Cre->Draw.Location.vy += 0x11E;
        Cre->Draw.Location.vy -= Cre->HeightOffset;
        Cre->ShadowPos.vy = Cre->Box.Location.vy + 0x11E;
        return 1;
    }
    {
        OldLocation = Cre->Box.Location;
        OldSpace = Cre->Box.MoveSpace;
        if (!NewTrueMove3dActual(&Cre->Box, &SVect, 0, (struct SHORTPAIR){0, 0})) {
            Cre->Box.Location = OldLocation;
            Cre->Box.MoveSpace = OldSpace;
            return 0;
        }
        UpdateBoxStuff(&Cre->Box, 1);
        return 1;
    }
}

extern int Paused;
int rand(void);
struct MATRIX *RotMatrixYXZ(struct SVECT *r, struct MATRIX *m);

int Move3d_SpinBounceAbout(struct CREATURE *Cre, struct CREATURE *Thrower, struct CREATURE **pHitCre, struct SHORTPAIR BounceDamage)
{
    int CurVel2;

    if (!Cre->Box.OnFloor && !Cre->Box.OnSolidBox && !Paused) {
        if (!(Cre->Ang.vx & 1)) {
            Cre->Ang.vx += -0xA0 - (rand() & 0x1F) * 2;
            Cre->Ang.vy += -0xA0 - (rand() & 0x1F) * 2;
            Cre->Ang.vz += -0xA0 - (rand() & 0x1F) * 2;
        } else {
            Cre->Ang.vx += 0xA0 + (rand() & 0x1F) * 2;
            Cre->Ang.vy += 0xA0 + (rand() & 0x1F) * 2;
            Cre->Ang.vz += 0xA0 + (rand() & 0x1F) * 2;
        }
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
    } else {
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        return 0;
    }
    if (CreCollideAsPoint(Cre, Thrower, BounceDamage.a, pHitCre, BounceDamage.b)) {
        CurVel2 = Cre->Vel.vx * Cre->Vel.vx + Cre->Vel.vy * Cre->Vel.vy + Cre->Vel.vz * Cre->Vel.vz;
        if (CurVel2 < 800000) {
            Cre->Box.OnFloor = 1;
            return 1;
        }
        Cre->Box.OnFloor = 0;
        Cre->Ang.vx++;
        return 1;
    }
    Cre->Box.OnFloor = 0;
    return 0;
}

extern struct COLLISIONRETURNDATA CollisionReturnData;
int VisableSolidBoxMove(struct VECT *P, struct VECT *Q, struct VECT *Stop, short Flags);
void OnePixel(struct SVECT *A, struct SVECT *B);

static int Shift24(long long b)
{
    long long Result;

    Result = b + 0x800000;
    return Result >> 24;
}

int NewTrueMove3dInternal(struct COLLBOX3D *Box, struct SVECT *Rel, struct SHORTPAIR DownPush, struct SHORTPAIR Depth_CheckOther)
{
    struct VECT Dest;
    struct VECT Dest1;
    struct VECT *DestPntr;
    struct SVECT *N;
    struct SVECT P;
    struct SVECT R;
    struct SVECT LargeMove;
    int VisWorld;
    int VisSolid;
    long long NdotR;
    int Flags;
    int NoProblem;
    struct SVECT MultShiftResult;

    VisSolid = 1;
    NoProblem = 1;
    Flags = GenerateBoxCollFlags(Box);
    LargeMove = *Rel;
    while (1) {
        Dest.vx = Box->Location.vx + LargeMove.vx;
        Dest.vy = Box->Location.vy + LargeMove.vy;
        Dest.vz = Box->Location.vz + LargeMove.vz;
        if (Depth_CheckOther.b) {
            VisSolid = VisableSolidBoxMove(&Box->Location, &Dest, &Dest1, Flags);
            if (VisSolid) {
                DestPntr = &Dest;
            } else {
                DestPntr = &Dest1;
                N = &CollisionReturnData.Normal;
                NoProblem = 0;
                if (N->vy > Box->FloorNorm.vy) {
                    Box->FloorNorm = *N;
                    if (DownPush.a && N->vy > Box->FloorNormMin) {
                        if (CollisionReturnData.IdentType == 2) {
                            Box->OnSolidBox = 1;
                            Box->SolidBoxGroupIdent = CollisionReturnData.GroupIdent;
                        } else {
                            Box->OnFloor = 1;
                        }
                    }
                }
            }
        } else {
            DestPntr = &Dest;
        }
        VisWorld = Visable3d(&MoveCollMap, &Box->Location, DestPntr, Box->MoveSpace);
        if (!VisWorld) {
            N = &MoveCollMap.ReturnPlane->Normal;
            NoProblem = 0;
            if (N->vy > Box->FloorNorm.vy) {
                Box->FloorNorm = *N;
                if (DownPush.a && N->vy > Box->FloorNormMin)
                    Box->OnFloor = 1;
            }
        }
        Box->MoveSpace = MoveCollMap.ReturnSpaceNum;
        Box->Location = MoveCollMap.AreaIntersectLast->Intersect;
        if (VisWorld & VisSolid)
            return NoProblem;
        if (abs(N->vy) < abs(Box->WallNorm.vy))
            Box->WallNorm = *N;
        if (DownPush.b) {
            OnePixel(N, &P);
            Box->Location.vx -= P.vx;
            Box->Location.vy -= P.vy;
            Box->Location.vz -= P.vz;
        }
        if (!Depth_CheckOther.a)
            return NoProblem;
        R.vx = Dest.vx - MoveCollMap.AreaIntersectLast->Intersect.vx;
        R.vy = Dest.vy - MoveCollMap.AreaIntersectLast->Intersect.vy;
        R.vz = Dest.vz - MoveCollMap.AreaIntersectLast->Intersect.vz;
        NdotR = N->vx * R.vx + N->vy * R.vy + N->vz * R.vz;
        MultShiftResult.vx = Shift24(N->vx * NdotR);
        MultShiftResult.vy = Shift24(N->vy * NdotR);
        MultShiftResult.vz = Shift24(N->vz * NdotR);
        LargeMove.vx = R.vx - MultShiftResult.vx;
        LargeMove.vy = R.vy - MultShiftResult.vy;
        LargeMove.vz = R.vz - MultShiftResult.vz;
        NoProblem = 0;
        if (DownPush.b) {
            LargeMove.vx -= P.vx;
            LargeMove.vy -= P.vy;
            LargeMove.vz -= P.vz;
        }
        Depth_CheckOther.a--;
    }
}

int Gravity = 0x20;
extern int NewVcounts;
extern struct SOLIDBOX SolidBoxs[48];
extern struct CREATURE **ThisBumpList;
extern struct CREATURE **ThisBumpListEnd;
extern int MaxWindows;
extern struct WINDOW GameWindows[];
void dprintf(struct WINDOW *Window, char *_format, ...);
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
void DoSolidBoxEventDamage(struct SOLIDBOX *solidbox, short damage, struct VECT *Location);
long SquareRoot0(long a);
void VectorNormal(struct VECT *v0, struct VECT *v1);
struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop);
void CalcCreShadowPos(struct CREATURE *Cre);
void *memset(void *s, int c, unsigned int n);

int CreCollideAsPointFunc(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre)
{
    struct VECT Start;
    struct VECT End;
    struct VECT ISect;
    struct VECT Temp;
    struct SVECT *N;
    struct SVECT P;
    struct SVECT Velo;
    int DP;
    int VelMag;
    int HasHit;
    int a;

    Start = End = Cre->Box.Location;
    HasHit = 0;
    if (!(Cre->Control & 0x2000)) {
        Cre->Vel.vy += Gravity * NewVcounts;
        if (Cre->Vel.vy > 0x2000)
            Cre->Vel.vy = 0x2000;
    }
    memset(&Cre->Box.FloorNorm, 0, sizeof(struct SVECT));
    Cre->Box.WallNorm = *&(struct SVECT){0, 0x1000, 0};
    Cre->Box.OnFloor = 0;
    Velo.vx = Cre->Vel.vx * NewVcounts / 320;
    Velo.vy = Cre->Vel.vy * NewVcounts / 320;
    Velo.vz = Cre->Vel.vz * NewVcounts / 320;
    End.vx += Velo.vx;
    End.vy += Velo.vy;
    End.vz += Velo.vz;
    if (!VisableSolidBoxLine(&Start, &End, &End)) {
        if (CollisionReturnData.IdentType == 2) {
            HasHit |= 1;
            N = &CollisionReturnData.Normal;
            if (sDamage && (SolidBoxs[CollisionReturnData.Ident].Flags & 4)) {
                SolidBoxs[CollisionReturnData.Ident].Flags |= 8;
                DoSolidBoxEventDamage(&SolidBoxs[CollisionReturnData.Ident], sDamage, &End);
                dprintf(GameWindows, "Something hit a solid box\n");
            }
        }
        End.vx -= Velo.vx;
        End.vy -= Velo.vy;
        End.vz -= Velo.vz;
    }
    if (!Visable3d(AsPointMap, &Start, &End, *pSpace)) {
        HasHit |= 1;
        N = &AsPointMap->ReturnPlane->Normal;
    }
    if (HasHit) {
        VelMag = SquareRoot0(Cre->Vel.vx * Cre->Vel.vx + Cre->Vel.vy * Cre->Vel.vy + Cre->Vel.vz * Cre->Vel.vz) * Bounce >> 12;
        OnePixel(N, &P);
        if (N->vy >= Cre->Box.FloorNorm.vy)
            Cre->Box.FloorNorm = *N;
        if (abs(N->vy) < abs(Cre->Box.WallNorm.vy))
            Cre->Box.WallNorm = *N;
        if (Cre->Box.FloorNorm.vy > Cre->Box.FloorNormMin)
            Cre->Box.OnFloor = 1;
        ISect = AsPointMap->AreaIntersectLast->Intersect;
        ISect.vx -= P.vx;
        ISect.vy -= P.vy;
        ISect.vz -= P.vz;
        Temp.vx = End.vx - ISect.vx;
        Temp.vy = End.vy - ISect.vy;
        Temp.vz = End.vz - ISect.vz;
        DP = (-(N->vx * Temp.vx) - N->vy * Temp.vy - N->vz * Temp.vz) * 2;
        Temp.vx = End.vx + Shift24(N->vx * (long long)DP);
        Temp.vy = End.vy + Shift24(N->vy * (long long)DP);
        Temp.vz = End.vz + Shift24(N->vz * (long long)DP);
        Cre->Box.Location = ISect;
        Temp.vx -= ISect.vx;
        Temp.vy -= ISect.vy;
        Temp.vz -= ISect.vz;
        VectorNormal(&Temp, &Temp);
        Cre->Vel.vx = Temp.vx * VelMag >> 12;
        Cre->Vel.vy = Temp.vy * VelMag >> 12;
        Cre->Vel.vz = Temp.vz * VelMag >> 12;
        if (ppHitCre)
            *ppHitCre = 0;
    } else {
        if (ppHitCre) {
            *ppHitCre = LineIntersectCreCylinder(&Start, &End, CreIgnore, &End, ThisBumpList, ThisBumpListEnd);
            if (*ppHitCre && *ppHitCre != Cre)
                HasHit = 1;
        }
        Cre->Box.Location = End;
    }
    *pSpace = AsPointMap->ReturnSpaceNum;
    Cre->Box.Area = AsPointMap->Coll3dSpace[*pSpace].Area;
    for (a = 0; a < MaxWindows; a++)
        Cre->WinData[a].SortArea = Cre->Box.Area;
    if (Cre->Control & 1)
        CalcCreShadowPos(Cre);
    return HasHit;
}

extern int LevelIsMultiPlayer;
int MakeIn3dSpace(struct COLLHEADER *Map, struct VECT *Start, short Indx);
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int CheckVersusBumpList(struct CREATURE *Cre);
void AddCreToNextBumpList(struct CREATURE *Cre);
int Seek();
void CalcCreCameraTarg(struct CREATURE *Cre);
short VectBoxOverlapsArea(struct VECTBOX *VectBox, int Area);

void NewTrueMove3d(struct CREATURE *Cre, int ThirdPersonStuff, int FinalCall, int AlwaysStep)
{
    struct SVECT *N;
    struct SVECT SVect;
    int TempSpace;
    int StepUp;
    struct VECT TempPos;
    struct VECT OldPos;
    struct VECT Dest;
    short OldMoveSpace;
    struct SVECT Temp;
    struct SVECT MoveVel;
    int TempI;
    int StepSize;

    if (Cre->Box.InCrouch || Cre->Box.InLowCrouch)
        StepSize = 0x6C;
    else
        StepSize = 0xD8;
    if (Cre->Box.UseExtraCollisionTypes) {
        int InsideFlags;

        InsideFlags = OutsideExtraCollisionMove(&Cre->Box.Location, 0x3360, 0, 0);
        if (InsideFlags & 0x1000) {
            if (Cre->Box.WallNorm.vy < 0x400) {
                if (Cre->Box.WallNorm.vy > -0x400)
                    Cre->Box.OnLadder = 1;
                else
                    Cre->Box.OnLadder = 0;
            }
        } else {
            Cre->Box.OnLadder = 0;
        }
        if (InsideFlags & 0x200)
            Cre->Box.InAirFlow = 1;
        else
            Cre->Box.InAirFlow = 0;
        if (InsideFlags & 0x2000) {
            if (Cre->Box.OnFloor || Cre->Box.OnSolidBox) {
                Cre->ForceVel.vy -= 0x2400;
                Cre->Control |= 0x4000;
            }
            Cre->Box.InBigAirFlow = 1;
        } else {
            Cre->Box.InBigAirFlow = 0;
        }
    }
    if (FinalCall && !(Cre->Control & 0x80000)) {
        AddCreToNextBumpList(Cre);
        if (CheckVersusBumpList(Cre) & 2) {
            Cre->Box.FloorNorm = *&(struct SVECT){0, 0x1000, 0};
            Cre->Box.OnCreature = 1;
        } else {
            Cre->Box.OnCreature = 0;
        }
    }
    N = &Cre->Box.FloorNorm;
    if (N->vy > Cre->Box.FloorNormMin && (Cre->Box.OnFloor || Cre->Box.OnSolidBox || Cre->Box.OnCreature)
        && !(Cre->Box.OnRope || Cre->Box.InWater || Cre->Box.OnLadder))
        Cre->Vel.vy = -(Cre->Vel.vx * N->vx + Cre->Vel.vz * N->vz) / N->vy;
    Temp = Cre->MoveVel;
    OldMoveSpace = Cre->Box.MoveSpace;
    OldPos = Cre->Box.Location;
    Cre->Box.WallNorm = *&(struct SVECT){0, 0x1000, 0};
    if ((Cre->Box.OnFloor || Cre->Box.OnSolidBox || AlwaysStep) & !Cre->Box.InWater) {
        Cre->Box.OnFloor = 0;
        Cre->Box.OnSolidBox = 0;
        MoveVel = (struct SVECT){0, -StepSize, 0};
        TempPos.vy = Cre->Box.Location.vy;
        NewTrueMove3dActual(&Cre->Box, &MoveVel, 0, (struct SHORTPAIR){0, 0});
        StepUp = TempPos.vy - Cre->Box.Location.vy;
        NewTrueMove3dActual(&Cre->Box, &Cre->MoveVel, 3, (struct SHORTPAIR){0, 1});
        TempPos = Cre->Box.Location;
        TempSpace = Cre->Box.MoveSpace;
        MoveVel = (struct SVECT){0, StepUp + StepSize, 0};
        memset(&Cre->Box.FloorNorm, 0, sizeof(struct SVECT));
        NewTrueMove3dActual(&Cre->Box, &MoveVel, 0, (struct SHORTPAIR){1, 0});
        if (!(Cre->Box.OnFloor || Cre->Box.OnSolidBox)) {
            if (Cre->Box.FloorNorm.vy) {
                Cre->Box.MoveSpace = OldMoveSpace;
                Cre->Box.Location = OldPos;
                NewTrueMove3dActual(&Cre->Box, &Cre->MoveVel, 3, (struct SHORTPAIR){1, 1});
            } else {
                MoveVel = (struct SVECT){0, StepUp, 0};
                Cre->Box.Location = TempPos;
                Cre->Box.MoveSpace = TempSpace;
                NewTrueMove3dActual(&Cre->Box, &MoveVel, 0, (struct SHORTPAIR){1, 0});
            }
        }
    } else {
        NewTrueMove3dActual(&Cre->Box, &Cre->MoveVel, 3, (struct SHORTPAIR){1, 1});
        if (!(Cre->Box.OnFloor || Cre->Box.OnSolidBox) && Cre->Box.FloorNorm.vy > Cre->Box.FloorNormMin) {
            MoveVel = *&(struct SVECT){0, 20, 0};
            memset(&Cre->Box.FloorNorm, 0, sizeof(struct SVECT));
            NewTrueMove3dActual(&Cre->Box, &MoveVel, 0, (struct SHORTPAIR){1, 0});
        }
    }
    if (!OutsideExtraCollisionMove(&Cre->Box.Location, GenerateBoxCollFlags(&Cre->Box), 0, !Cre->Box.OnSolidBox)) {
        TempI = Find3dSpaceIndxLink(&MoveCollMap, &Cre->Box.Location, Cre->Box.MoveSpace, 0);
        if (TempI != -1) {
            Cre->Box.MoveSpace = TempI;
            Cre->Draw.Location = Cre->Box.Location;
            Cre->Draw.Location.vy = Cre->Box.Location.vy - Cre->HeightOffset + 0x11E;
        } else if (MakeIn3dSpace(&MoveCollMap, &Cre->Box.Location, Cre->Box.MoveSpace)) {
            Cre->Draw.Location = Cre->Box.Location;
            Cre->Draw.Location.vy = Cre->Box.Location.vy - Cre->HeightOffset + 0x11E;
        } else {
            Cre->Box.MoveSpace = OldMoveSpace;
            Cre->Box.Location = OldPos;
        }
    } else {
        Cre->Box.MoveSpace = OldMoveSpace;
        Cre->Box.Location = OldPos;
    }
    UpdateBoxStuff(&Cre->Box, 1);
    Cre->Box.AltArea = VectBoxOverlapsArea(&Cre->Box.VectBox, Cre->Box.Area);
    if (!FinalCall)
        return;
    if (ThirdPersonStuff)
        CalcCreCameraTarg(Cre);
    if (Cre->Control & 1)
        CalcCreShadowPos(Cre);
    Cre->MoveVel.vx = Cre->Vel.vx * NewVcounts / 320;
    Cre->MoveVel.vz = Cre->Vel.vz * NewVcounts / 320;
    if ((Cre->Control & 0x1000) || Cre->Box.OnRope || Cre->Box.InWater || (Cre->Box.OnLadder && !(Cre->Control & 0x4000))) {
        Cre->Box.OnFloor = 0;
        Cre->Box.OnSolidBox = 0;
        Cre->MoveVel.vy = Cre->Vel.vy * NewVcounts / 320;
        Cre->Vel.vx += Cre->ForceVel.vx;
        Cre->Vel.vy += Cre->ForceVel.vy;
        Cre->Vel.vz += Cre->ForceVel.vz;
        Cre->ForceVel.vz = 0;
        Cre->ForceVel.vy = 0;
        Cre->ForceVel.vx = 0;
    } else {
        Cre->MoveVel.vy = (Cre->Vel.vy * NewVcounts + Gravity * NewVcounts * NewVcounts / 2) / 320;
        Cre->Vel.vy += Gravity * NewVcounts;
        if (Cre->Box.InBigAirFlow) {
            Seek(&Cre->Vel.vy, -0xC00, NewVcounts << 6);
        } else if (Cre->Box.InAirFlow) {
            if (Cre->Vel.vy >= 0x400)
                Seek(&Cre->Vel.vy, 0x400, NewVcounts << 6);
            else
                Cre->Vel.vy -= NewVcounts * 24;
        } else if (Cre->Vel.vy > 0x2000) {
            Cre->Vel.vy = 0x2000;
        }
        if (Cre->Control & 0x4000) {
            Cre->Box.OnLadder = 0;
            Cre->Box.OnCreature = 0;
            Cre->Box.OnSolidBox = 0;
            Cre->Box.OnFloor = 0;
            memset(&Cre->Box.FloorNorm, 0, sizeof(struct SVECT));
            Cre->Box.WallNorm = *&(struct SVECT){0, 0x1000, 0};
            Cre->Control &= ~0x4000;
            Cre->Vel.vx += Cre->ForceVel.vx;
            Cre->Vel.vy += Cre->ForceVel.vy;
            if (!LevelIsMultiPlayer && Cre->Vel.vy < -0xC00)
                Cre->Vel.vy = -0xC00;
            Cre->Vel.vz += Cre->ForceVel.vz;
            Cre->MoveVel.vx = Cre->Vel.vx * NewVcounts / 320;
            Cre->MoveVel.vy = Cre->Vel.vy * NewVcounts / 320;
            Cre->MoveVel.vz = Cre->Vel.vz * NewVcounts / 320;
            Cre->ForceVel.vz = 0;
            Cre->ForceVel.vy = 0;
            Cre->ForceVel.vx = 0;
        }
    }
}
