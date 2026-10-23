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
