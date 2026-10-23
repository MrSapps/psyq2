#include "types.h"

int DebrisModelsActive;
struct THICKLINE *ThickLinePntr;
struct THICKLINE *LastThickLine;
struct SVECTOR *ShieldEffPoints;
char *ShieldEffPolys;
struct GLOBMODHDR *DebrisModelList[32];
struct THICKLINE ThickLines[32];

int AddThickLine(struct VECT *Start, struct VECT *Stop, short Width, short Space, struct SIMPOL *ModelData, struct SIMPOL *EndModelDataA, struct SIMPOL *EndModelDataB)
{
    if (ThickLinePntr < LastThickLine) {
        ThickLinePntr->Start = *Start;
        ThickLinePntr->Stop = *Stop;
        ThickLinePntr->Width = Width;
        ThickLinePntr->Model = ModelData;
        ThickLinePntr->EndModelA = EndModelDataA;
        ThickLinePntr->EndModelB = EndModelDataB;
        ThickLinePntr->Space = Space;
        ThickLinePntr++;
        return 1;
    }
    return 0;
}

void InitThickLines(void)
{
    ThickLinePntr = ThickLines;
    LastThickLine = ThickLines + 32;
}

void ProcessThickLines(void)
{
    struct THICKLINE *ThickLine;

    for (ThickLine = ThickLines; ThickLine < ThickLinePntr; ThickLine++)
        Draw3dLine(ThickLine);
    ThickLinePntr = ThickLines;
    LastThickLine = ThickLines + 32;
}

void PreInitDebris(struct GLOBMODHDR *Hdr)
{
    if (Hdr && DebrisModelsActive < 32)
        DebrisModelList[DebrisModelsActive++] = Hdr;
}

struct MATRIX *RotMatrixYXZ(struct SVECT *r, struct MATRIX *m);
void ElipseMatrix(struct MATRIX *Source, struct MATRIX *Dest, struct SVECT *R);
void MulMatrix0(struct MATRIX *m0, struct MATRIX *m1, struct MATRIX *m2);

void GenShieldMatrix(struct CREATURE *Cre, struct SVECT *ShieldAng, struct SVECT *Radius, struct MATRIX *S)
{
    struct MATRIX P;
    struct MATRIX R;

    RotMatrixYXZ(ShieldAng, &P);
    ElipseMatrix(&Cre->Matrix, &R, Radius);
    MulMatrix0(&R, &P, S);
}

void CopyFromGlobalTexture(struct POLY_GT4 *Poly0, struct POLY_GT4 *Poly1, struct GLOBTEXTDATA *GlobText)
{
    *(long *)&Poly0->r0 = *(long *)&Poly1->r0 = *(long *)&GlobText->Colour;
    *(long *)&Poly0->r1 = *(long *)&Poly1->r1 = *(long *)&GlobText->Colour;
    *(long *)&Poly0->r2 = *(long *)&Poly1->r2 = *(long *)&GlobText->Colour;
    *(long *)&Poly0->r3 = *(long *)&Poly1->r3 = *(long *)&GlobText->Colour;
    *(long *)&Poly0->u0 = *(long *)&Poly1->u0 = *(long *)&GlobText->Uv0;
    *(long *)&Poly0->u1 = *(long *)&Poly1->u1 = *(long *)&GlobText->Uv1;
    *(short *)&Poly0->u2 = *(short *)&Poly1->u2 = *(short *)&GlobText->Uv2;
    *(short *)&Poly0->u3 = *(short *)&Poly1->u3 = *(short *)&GlobText->Uv3;
}

void LoadShieldData(void *Addr)
{
    ShieldEffPolys = Addr;
    if (Addr)
        ShieldEffPoints = (struct SVECTOR *)((char *)Addr + 0x360);
}

extern struct POLY_G4 *NextGeneralG4;
extern struct AREA AreaData[96];
char *GetNextGlobalMemory(int Size);
void OrderedSort(struct SORT *Self, struct SORT **SortLink);
struct SORT *GetNextSortData(void);
void FreeLastSortData(void);
void AddPrim_(void *ot, void *p);

void GeneralG4Sort(struct POLY_G4 *OldPoly, struct VECT *Centre, struct SHORTPAIR AreaWin, int ScreenZ)
{
    struct SORT *Sort;
    struct POLY_G4 *Poly;
    struct VECT *Pos;

    Pos = (struct VECT *)GetNextGlobalMemory(sizeof(struct VECT));
    /* do{}while(0): weights Pos's refs so that, inlined into Draw3dLine, Pos
       outranks the caller's OldPoly in global allocation (Pos s2, OldPoly s3) */
    do {
        if (!Pos)
            return;
    } while (0);
    Sort = GetNextSortData();
    if (!Sort)
        return;
    if (OldPoly != NextGeneralG4) {
        for (Poly = OldPoly; Poly < NextGeneralG4; Poly++)
            AddPrim_(&Sort->FirstLast[0], Poly);
        Sort->FirstLast[1] = (struct POLY_GT4 *)OldPoly;
        *Pos = *Centre;
        Sort->Flags = 1;
        Sort->VectBox = (struct VECTBOX *)Pos;
        Sort->ScreenZ = ScreenZ;
        OrderedSort(Sort, &AreaData[AreaWin.a].Data[AreaWin.b].QuickSortLink);
    } else
        FreeLastSortData();
}

extern int MaxWindows;
extern struct WINDOW GameWindows[];
extern struct WINDOW *CurrWin;
extern struct VECT *CurrCamPos;
extern struct MATRIX *CurrCamMat;
extern int CurrWinIndx;
extern struct SCRPNT CurrScreenSize;
extern struct SCRPNT CurrScreenMin;
extern unsigned long *CurrOt;
extern struct DB *currdb;
extern struct POLY_G4 *LastGeneralG4;
extern struct COLLHEADER CollMap;
void *memset(void *s, int c, unsigned int n);
void SetGeomOffset(long ofx, long ofy);
void SetGeomScreen(long h);
void SetRotMatrix(struct MATRIX *m);
int Visable3d(struct COLLHEADER *Map, struct VECT *Old, struct VECT *New, int Space);
int VectorNormal(struct VECT *v0, struct VECT *v1);
void GenerateCrossSection(struct SVECTOR *HxPnts, struct VECT *Rel, struct SHORTPAIR WidthSect);
int AreaValid(int Win, int Area);
void SetupScreenGivenArea(int Win, int Area);
int SquareRoot0(int a);
struct POLY_G4 *HH_SimpleModel_(struct SIMPOL *Model, struct SIMPOL *ModelEnd, struct SCRPNT *ScrPnts, struct POLY_G4 *Poly);

#define gte_mvr_d(reg, r0) __asm__ volatile("move $" #reg ",%0" : : "r"(r0) : "$" #reg)
#define gte_op_d(s) __asm__ volatile(s)
#define gte_st_d(s) __asm__ volatile(s : : : "memory")
#define gte_ldv0_d(r0) { gte_mvr_d(12, r0); gte_op_d("lwc2 $0,0($12)"); gte_op_d("lwc2 $1,4($12)"); }
#define gte_rtv0_d() { gte_op_d("nop"); gte_op_d("nop"); gte_op_d(".word 0x4A486012"); }
#define gte_mvlvtr_d() __asm__ volatile("mfc2 $12,$25\nmfc2 $13,$26\nmfc2 $14,$27\nctc2 $12,$5\nctc2 $13,$6\nctc2 $14,$7" : : : "$12", "$13", "$14", "$15")
#define gte_ldv3c_d(r0) { gte_mvr_d(12, r0); \
    gte_op_d("lwc2 $0,0($12)"); gte_op_d("lwc2 $1,4($12)"); gte_op_d("lwc2 $2,8($12)"); \
    gte_op_d("lwc2 $3,12($12)"); gte_op_d("lwc2 $4,16($12)"); gte_op_d("lwc2 $5,20($12)"); }
#define gte_rtpt_d() { gte_op_d("nop"); gte_op_d("nop"); gte_op_d(".word 0x4A280030"); }
#define gte_stsxy3c_d(r0) { gte_mvr_d(12, r0); \
    gte_st_d("swc2 $12,0($12)"); gte_st_d("swc2 $13,4($12)"); gte_st_d("swc2 $14,8($12)"); }
#define gte_stsz_d(r0) { gte_mvr_d(12, r0); gte_st_d("swc2 $19,0($12)"); }

void Draw3dLine(struct THICKLINE *ThickLine)
{
    struct AREAINTERSECT *AreaIntersect;
    struct SVECTOR HxPnts[6];
    struct SCRPNT ScrPnts[12];
    struct POLY_G4 *OldPoly;
    struct VECT Rel;
    struct SVECT Base;
    struct VECT Direction;
    int AreasDone;
    struct SHORTPAIR AreaWin;
    int i;

    memset(&AreaWin, 0, sizeof(AreaWin));
    for (i = 0; i < MaxWindows; i++) {
        AreaWin.b = i;
        CurrWin = &GameWindows[AreaWin.b];
        CurrCamPos = &CurrWin->CamPos;
        CurrCamMat = &CurrWin->CamMat;
        CurrWinIndx = AreaWin.b;
        CurrOt = currdb->Ot[AreaWin.b];
        CurrScreenSize = CurrWin->Size;
        memset(&CurrScreenMin, 0, sizeof(CurrScreenMin));
        SetGeomOffset(CurrWin->Geom.sx, CurrWin->Geom.sy);
        SetGeomScreen(CurrWin->Focal);
        SetRotMatrix(CurrCamMat);
        Visable3d(&CollMap, &ThickLine->Start, &ThickLine->Stop, ThickLine->Space);
        Rel.vx = ThickLine->Stop.vx - ThickLine->Start.vx;
        Rel.vy = ThickLine->Stop.vy - ThickLine->Start.vy;
        Rel.vz = ThickLine->Stop.vz - ThickLine->Start.vz;
        VectorNormal(&Rel, &Direction);
        Direction.vx = Direction.vx * 640 >> 12;
        Direction.vy = Direction.vy * 640 >> 12;
        Direction.vz = Direction.vz * 640 >> 12;
        GenerateCrossSection(HxPnts, &Rel, (struct SHORTPAIR){ThickLine->Width, 6});
        {
        struct VECT Centre;
        int Za;
        int Zb;
        int DrawArea;
        int LastDrawArea;

        Za = 0;
        DrawArea = 0;
        for (AreasDone = 0, AreaIntersect = CollMap.AreaIntersects; AreaIntersect < CollMap.AreaIntersectLast; AreaIntersect++, AreasDone++) {
            LastDrawArea = DrawArea;
            AreaWin.a = AreaIntersect->Area;
            DrawArea = AreaValid(AreaWin.b, AreaWin.a);
            if (!DrawArea)
                continue;
            SetupScreenGivenArea(AreaWin.b, AreaWin.a);
            if (!LastDrawArea) {
                Base.vx = AreaIntersect->Intersect.vx - CurrCamPos->vx;
                Base.vy = AreaIntersect->Intersect.vy - CurrCamPos->vy;
                Base.vz = AreaIntersect->Intersect.vz - CurrCamPos->vz;
                gte_ldv0_d(&Base);
                gte_rtv0_d();
                gte_mvlvtr_d();
            }
            gte_ldv3c_d(HxPnts);
            gte_rtpt_d();
            gte_stsxy3c_d(ScrPnts);
            gte_ldv3c_d(&HxPnts[3]);
            gte_rtpt_d();
            gte_stsxy3c_d(&ScrPnts[3]);
            gte_stsz_d(&Za);
            {
                struct VECT Temp;
                int NumSections;
                int a;
                int s;

                if (!AreasDone) {
                    if (NextGeneralG4 > LastGeneralG4 - 2)
                        return;
                    OldPoly = NextGeneralG4;
                    NextGeneralG4 = HH_SimpleModel_(ThickLine->EndModelB, ThickLine->EndModelB + 1, ScrPnts, OldPoly);
                    GeneralG4Sort(OldPoly, &AreaIntersect->Intersect, AreaWin, Za);
                }
                Temp.vx = AreaIntersect->Intersect.vx - AreaIntersect[1].Intersect.vx;
                Temp.vy = AreaIntersect->Intersect.vy - AreaIntersect[1].Intersect.vy;
                Temp.vz = AreaIntersect->Intersect.vz - AreaIntersect[1].Intersect.vz;
                NumSections = (SquareRoot0(Temp.vx * Temp.vx + Temp.vy * Temp.vy + Temp.vz * Temp.vz) + 639) / 640;
                Centre.vx = AreaIntersect->Intersect.vx - Direction.vx / 2;
                Centre.vy = AreaIntersect->Intersect.vy - Direction.vy / 2;
                Centre.vz = AreaIntersect->Intersect.vz - Direction.vz / 2;
                for (s = NumSections - 1; s >= 0; s--) {
                    if (NextGeneralG4 > LastGeneralG4 - 6)
                        return;
                    for (a = 0; a < 6; a++)
                        *(long *)&ScrPnts[a + 6] = *(long *)&ScrPnts[a];
                    Zb = Za;
                    if (s) {
                        Base.vx += Direction.vx;
                        Base.vy += Direction.vy;
                        Base.vz += Direction.vz;
                        Centre.vx += Direction.vx;
                        Centre.vy += Direction.vy;
                        Centre.vz += Direction.vz;
                    } else {
                        Temp.vx = AreaIntersect[1].Intersect.vx - CurrCamPos->vx;
                        Temp.vy = AreaIntersect[1].Intersect.vy - CurrCamPos->vy;
                        Temp.vz = AreaIntersect[1].Intersect.vz - CurrCamPos->vz;
                        Centre.vx += (Direction.vx + Temp.vx - Base.vx) / 2;
                        Centre.vy += (Direction.vy + Temp.vy - Base.vy) / 2;
                        Centre.vz += (Direction.vz + Temp.vz - Base.vz) / 2;
                        Base = (struct SVECT){Temp.vx, Temp.vy, Temp.vz};
                    }
                    gte_ldv0_d(&Base);
                    gte_rtv0_d();
                    gte_mvlvtr_d();
                    gte_ldv3c_d(HxPnts);
                    gte_rtpt_d();
                    gte_stsxy3c_d(ScrPnts);
                    gte_ldv3c_d(&HxPnts[3]);
                    gte_rtpt_d();
                    gte_stsxy3c_d(&ScrPnts[3]);
                    gte_stsz_d(&Za);
                    OldPoly = NextGeneralG4;
                    NextGeneralG4 = HH_SimpleModel_(ThickLine->Model, ThickLine->Model + 5, ScrPnts, OldPoly);
                    GeneralG4Sort(OldPoly, &Centre, AreaWin, Za + Zb / 2);
                }
            }
        }
        if (DrawArea) {
            if (NextGeneralG4 > LastGeneralG4 - 2)
                return;
            OldPoly = NextGeneralG4;
            NextGeneralG4 = HH_SimpleModel_(ThickLine->EndModelA, ThickLine->EndModelA + 1, ScrPnts, OldPoly);
            GeneralG4Sort(OldPoly, &Centre, AreaWin, Za);
        }
        }
    }
}

extern short rcossin_tbl[][2];
int VectorNormal(struct VECT *v0, struct VECT *v1);
void GenTangentialAxis(struct VECT *R, struct VECT *A, struct VECT *B);

void GenerateCrossSection(struct SVECTOR *HxPnts, struct VECT *Rel, struct SHORTPAIR WidthSect)
{
    int a;
    int Angle;
    int Sin;
    int Cos;
    struct VECT A;
    struct VECT B;
    struct VECT R;

    if (VectorNormal(Rel, &R)) {
        GenTangentialAxis(&R, &A, &B);
        for (a = 0; a < 3; a++) {
            Angle = (a << 12) / 6;
            Sin = -(rcossin_tbl[Angle][0] * WidthSect.a) >> 12;
            Cos = (rcossin_tbl[Angle][1] * WidthSect.a) >> 12;
            HxPnts[a].vx = (A.vx * Sin + B.vx * Cos) >> 12;
            HxPnts[a].vy = (A.vy * Sin + B.vy * Cos) >> 12;
            HxPnts[a].vz = (A.vz * Sin + B.vz * Cos) >> 12;
            HxPnts[a + 3].vx = -HxPnts[a].vx;
            HxPnts[a + 3].vy = -HxPnts[a].vy;
            HxPnts[a + 3].vz = -HxPnts[a].vz;
        }
    }
}

extern struct COLLHEADER CollMap;
struct CREATURE *GetNextCre(int UseExpendable);
int GetFloorOffset(struct CREATURE *Cre);
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ);
void GetLimitedPolyData(struct SCENE2 *Scene2, int PolyNum, struct GLOBTEXTDATA *GlobTextData);
void PrimaryMovingFragment(struct CREATURE *Cre);
int rand(void);

void GenerateSceneFrag(struct SCENE2 *Scene2, struct VECT *Location, struct SVECT *Vel, struct GLOBMODHDR *Hdr)
{
    struct CREATURE *Cre;

    Cre = GetNextCre(0);
    if (Cre) {
        Cre->Primary = PrimaryMovingFragment;
        Cre->ModelHdr = Hdr;
        Cre->HeightOffset = GetFloorOffset(Cre);
        Cre->Control = 0x1800000;
        Cre->Draw.Location = *Location;
        Cre->Box.Location = *Location;
        Cre->DefaultAmbience = *(struct COLOUR *)"@@@";
        Cre->Ambience = *(struct COLOUR *)"@@@";
        Cre->Vel = *Vel;
        Cre->Ang = (struct SVECT){rand(), rand(), rand()};
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        Cre->Life = 0x834;
        Cre->Box.MoveSpace = -1;
        Cre->Box.PrimarySpace = Find3dSpaceIndxLink(&CollMap, Location, -1, 1);
        Cre->Box.Area = CollMap.Coll3dSpace[Cre->Box.PrimarySpace].Area;
        SetupCollBoxMinMax(&Cre->Box, *&(struct SVECT){20, 20, 20});
        GetLimitedPolyData(Scene2, 0, (struct GLOBTEXTDATA *)Cre->Params);
    }
}

extern struct SCENE2 *Scene2Data;

void SceneObjFragments(int ObjIndx, int NumBits, struct VECT *Loc)
{
    struct VECT Location;
    struct VECT Size;
    struct VECT Min;
    struct VECT Max;
    struct SVECT Vel;
    struct SCENE2 *Scene2;
    int a;

    if (!DebrisModelsActive)
        return;
    Scene2 = &Scene2Data[ObjIndx];
    Min = Scene2->VectBox.Min;
    Max = Scene2->VectBox.Max;
    Size.vx = Max.vx - Min.vx;
    Size.vy = Max.vy - Min.vy;
    Size.vz = Max.vz - Min.vz;
    for (a = 0; a < NumBits; a++) {
        if (Loc)
            Location = *Loc;
        else {
            Location.vx = Min.vx + ((rand() * Size.vx) >> 15);
            Location.vy = Min.vy + ((rand() * Size.vy) >> 15);
            Location.vz = Min.vz + ((rand() * Size.vz) >> 15);
        }
        Vel.vx = ((rand() - 0x4000) * 0x600) >> 14;
        Vel.vy = (((rand() - 0x4000) * 0x600) >> 14) - 0x600;
        Vel.vz = ((rand() - 0x4000) * 0x600) >> 14;
        GenerateSceneFrag(Scene2, &Location, &Vel, DebrisModelList[rand() % DebrisModelsActive]);
    }
}

extern int MaxWindows;
extern int Paused;
extern int NewVcounts;
void ForEachPolyInModelParams(struct CREATURE *Cre, void *VoidRoutine, int Win, int ParamA, int ParamB);
int CreCollideAsPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage);
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);
void KillModel(struct CREATURE *Cre);

void PrimaryMovingFragment(struct CREATURE *Cre)
{
    {
        {
            struct GLOBTEXTDATA GlobTextData;
            int Win;
            int Flag;

            if (Cre->ModelHdr && (Cre->Control & 0xf00)) {
                Flag = 0x100;
                GlobTextData = *(struct GLOBTEXTDATA *)Cre->Params;
                for (Win = 0; Win < MaxWindows; Win++, Flag <<= 1) {
                    if (Cre->Control & Flag) {
                        ForEachPolyInModelParams(Cre, CopyFromGlobalTexture, Win, (int)&GlobTextData, 0);
                        Cre->Control &= ~Flag;
                    }
                }
            }
        }
    }
    if (CreCollideAsPoint(Cre, 0, 0x800, 0, 0)) {
        Cre->Ang.vx++;
        Cre->Life -= 300;
    }
    if (!Paused) {
        if (Cre->Ang.vx & 1) {
            Cre->Ang.vx += ((rand() & 0x7f) + 0x7f) * 2;
            Cre->Ang.vy -= ((rand() & 0x7f) + 0x7f) * 2;
            Cre->Ang.vz += ((rand() & 0x7f) + 0x7f) * 2;
        } else {
            Cre->Ang.vx -= ((rand() & 0x7f) + 0x7f) * 2;
            Cre->Ang.vy += ((rand() & 0x7f) + 0x7f) * 2;
            Cre->Ang.vz -= ((rand() & 0x7f) + 0x7f) * 2;
        }
    }
    RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
    Cre->Draw.Location = Cre->Box.Location;
    Cre->Life -= NewVcounts;
    if (Cre->Life <= 0)
        KillModel(Cre);
    UpdateBoxStuff(&Cre->Box, 2);
}

extern struct SHIELDGLINT ReactorShieldGlints[];
extern struct WINDOW *CurrWin;
extern struct MATRIX *CurrCamMat;
void MulMatrix2(struct MATRIX *m0, struct MATRIX *m1);
void SetRotMatrix(struct MATRIX *m);
void DrawGlint(struct CREATURE *Cre, struct MATRIX *Matrix, struct COLOUR Colour, int Trigger, int Light, int Range);

/* PSY-Q inline_c.h style GTE macros */
#define gte_ldv0(r0) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)"); }
#define gte_rtv0() __asm__ volatile("nop\nnop\n.word 0x4A486012")
#define gte_mvlvtr() __asm__ volatile("mfc2 $12,$25\nmfc2 $13,$26\nmfc2 $14,$27\nctc2 $12,$5\nctc2 $13,$6\nctc2 $14,$7" : : : "$12", "$13", "$14")

void NewShieldEffect(struct CREATURE *Cre)
{
    struct COLOUR Colour;
    struct SVECT Radius;
    struct MATRIX S;
    struct VECT Relative;
    struct SVECTOR Rel;
    int a;

    if (!ShieldEffPolys)
        return;
    Relative.vx = Cre->Draw.Location.vx - CurrWin->CamPos.vx;
    Relative.vy = Cre->Draw.Location.vy - CurrWin->CamPos.vy;
    Relative.vz = Cre->Draw.Location.vz - CurrWin->CamPos.vz;
    Rel.vx = Relative.vx;
    Rel.vy = Relative.vy;
    Rel.vz = Relative.vz;
    SetRotMatrix(CurrCamMat);
    gte_ldv0(&Rel);
    gte_rtv0();
    gte_mvlvtr();
    Radius = *&(struct SVECT){0x300, 0x300, 0x300};
    if (!Cre->ShieldColour.Pad) {
        GenShieldMatrix(Cre, &Cre->ShieldAng, &Radius, &S);
        MulMatrix2(CurrCamMat, &S);
        DrawGlint(Cre, &S, Cre->ShieldColour, Cre->ShieldTimer--, 1, 0x2000);
    } else {
        for (a = 0; a < Cre->ShieldColour.Pad; a++) {
            GenShieldMatrix(Cre, &ReactorShieldGlints[a].Ang, &Radius, &S);
            MulMatrix2(CurrCamMat, &S);
            DrawGlint(Cre, &S, ReactorShieldGlints[a].Colour, ReactorShieldGlints[a].Timer--, 1, 0x1000);
        }
    }
}

extern long *DummyMemory;
extern struct POLY_G4 *LastGeneralG4;
int abs(int);
#define setlen(p, _len) (*(((unsigned char *)(p)) + 3) = (unsigned char)(_len))
#define setcode(p, _code) (*(((unsigned char *)(p)) + 7) = (unsigned char)(_code))

int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
void AddPrimInsert_(void *ot, void *p);

#define gte_mvr(reg, r0) __asm__ volatile("move $" #reg ",%0" : : "r"(r0) : "$" #reg, "memory")
#define gte_op(s) __asm__ volatile(s)
#define gte_st(s) __asm__ volatile(s : : : "memory")
#define gte_ldv3c_s(r0) { gte_mvr(12, r0); \
    gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); gte_op("lwc2 $2,8($12)"); \
    gte_op("lwc2 $3,12($12)"); gte_op("lwc2 $4,16($12)"); gte_op("lwc2 $5,20($12)"); }
/* rtpt without the nops, storing SXY0-2 with an 8-byte stride (TEMPTYPE) */
#define gte_rtpt_sxy8(r0) { gte_mvr(13, r0); gte_op(".word 0x4A280030"); \
    gte_st("swc2 $12,0($13)"); gte_st("swc2 $13,8($13)"); gte_st("swc2 $14,16($13)"); }
#define gte_ldsxy3_s(r0, r1, r2) { gte_mvr(12, r0); gte_mvr(13, r1); gte_mvr(14, r2); \
    gte_op("mtc2 $12,$12"); gte_op("mtc2 $14,$14"); gte_op("mtc2 $13,$13"); }
#define gte_nclip_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4B400006"); }
#define gte_stopz_s(r0) { gte_mvr(12, r0); gte_st("swc2 $24,0($12)"); }

/* B1: the original copies Trigger into B1 (same $s2) and uses it for Bright and the brightness scale;
   the depth tested against Bright is SVECTOR.pad (lh 6) */
void DrawGlint(struct CREATURE *Cre, struct MATRIX *Matrix, struct COLOUR Colour, int Trigger, int Light, int Range)
{
    int Bright;
    int R;
    int G;
    int B;
    int NClip;
    int Br;
    int B1;
    int a;
    int g;
    int h;
    int i;
    int j;
    int p0;
    struct TEMPTYPE *TTrr;
    struct TEMPTYPE *TTr;
    struct SVECTOR *S;
    unsigned char *P;

    TTrr = (struct TEMPTYPE *)DummyMemory;
    R = (Colour.r << 13) / Range;
    G = (Colour.g << 13) / Range;
    B = (Colour.b << 13) / Range;
    if (Light)
        AddLight(&Cre->Draw.Location, Colour, *&(struct POWER){300, 800}, *&(struct SHORTPAIR){1, 3});
    B1 = Trigger;
    Bright = Range / 4 * (4 - B1) + 1024;
    SetRotMatrix(Matrix);
    TTr = TTrr;
    S = ShieldEffPoints;
    for (a = 0; a < 94; a += 3, S += 3, TTr += 3) {
        gte_ldv3c_s(S);
        gte_rtpt_sxy8(TTr);
        for (p0 = 0; p0 < 3; p0++) {
            Br = abs(S[p0].pad - Bright);
            if (Br < Range)
                Br = Range - Br;
            else
                Br = 0;
            Br = Br * B1;
            TTr[p0].r = R * Br >> 15;
            TTr[p0].g = G * Br >> 15;
            TTr[p0].b = B * Br >> 15;
        }
    }
    P = (unsigned char *)ShieldEffPolys;
    for (a = 0; a < 79; a++, P += 4) {
        p0 = *(int *)&TTrr[P[0]].r;
        p0 |= *(int *)&TTrr[P[1]].r;
        p0 |= *(int *)&TTrr[P[2]].r;
        p0 |= *(int *)&TTrr[P[3]].r;
        if (p0 & 0xFFFFFF) {
            if (NextGeneralG4 >= LastGeneralG4)
                return;
            g = *(int *)&TTrr[P[0]];
            h = *(int *)&TTrr[P[1]];
            i = *(int *)&TTrr[P[3]];
            j = *(int *)&TTrr[P[2]];
            gte_ldsxy3_s(g, h, i);
            *(int *)&NextGeneralG4->x0 = g;
            *(int *)&NextGeneralG4->x1 = h;
            *(int *)&NextGeneralG4->x2 = i;
            *(int *)&NextGeneralG4->x3 = j;
            *(int *)&NextGeneralG4->r0 = *(int *)&TTrr[P[0]].r;
            *(int *)&NextGeneralG4->r1 = *(int *)&TTrr[P[1]].r;
            *(int *)&NextGeneralG4->r2 = *(int *)&TTrr[P[3]].r;
            *(int *)&NextGeneralG4->r3 = *(int *)&TTrr[P[2]].r;
            setlen(NextGeneralG4, 8), setcode(NextGeneralG4, 0x38);
            setcode(NextGeneralG4, ((unsigned char *)NextGeneralG4)[7] | 2);
            gte_nclip_s();
            gte_stopz_s(&NClip);
            if (NClip >= 0)
                AddPrimInsert_(&Cre->WinData[0].Sort.FirstLast[1], NextGeneralG4++);
            else
                AddPrim_(&Cre->WinData[0].Sort.FirstLast[0], NextGeneralG4++);
        }
    }
}
