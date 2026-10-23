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
    if (!Pos)
        return;
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
