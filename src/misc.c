#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
int MaxSizeSVect(struct SVECT SVect);
void InitCrossEveryFrame(void);
int BytesBetween(void *Pntr0, void *Pntr1);
int SeekAng(int Start, int Stop, int Speed);
void Rotate(struct MATRIX *m, struct SVECT *v1, struct VECT *v2);
void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *v2);
void RotateL(struct MATRIX *m, struct VECT *v1, struct VECT *v2);
int Seek(short *a, short b, short c);
void SeekL(long *a, short b, short c);
void RotateRelSpeeds(short Ang, short x, short z, struct SVECT *OutPut);
void CalcVectorL(struct MATRIX *m, struct VECT *v1, struct VECT *v2);
void FindMaxMin(struct VECT *Start, struct VECT *Stop, struct VECTBOX *VectBox);
void AddCross(struct VECT *Pos, int Radius, struct COLOUR Colour);
void Relocate(void *Pntr, void *Base);
char *PrintBool(int Bool);
int mystrlen(char *String);
void RemoveMeRoutine(struct SVECTOR *R, struct SVECTOR *P);
void DisplayBuildInfo(int Flags);
void ScaleSVect(struct SVECT *Vec, int Scale);
void DrawCrossesCurrWin(void);
void RotateRelSpeedsFly(struct MATRIX *M, struct SVECT *Rel, struct SVECT *Vel);
void PrintDebugInfoScene(int *DebugScreen);
void DrawCylinder(struct VECT *Location, short Radius, short MinY, short MaxY);
void DrawVectBox(struct VECTBOX *VectBox);
char *GetStringFromStringtable(struct IDENTITYNAME StringName);

/* defined in this file (misc.obj): $gp-relative */
static struct CROSSDATA *CrossPntr;
static struct CROSSDATA *CrossLast;

extern short rcossin_tbl[][2];

static struct CROSSDATA CrossList[32];
extern short CheatActiveStatus;
extern struct WINDOW GameWindows[5];

/* PSY-Q inline_c.h style GTE macros (one asm statement per register move, as the original) */
#define gte_mv3(r0, r1, r2) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("move $13,%0" : : "r"(r1) : "$13"); \
    __asm__ volatile("move $14,%0" : : "r"(r2) : "$14"); }
#define gte_ldv3(r0, r1, r2) { gte_mv3(r0, r1, r2); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)\nlwc2 $2,0($13)\nlwc2 $3,4($13)\nlwc2 $4,0($14)\nlwc2 $5,4($14)"); }
#define gte_rtpt() __asm__ volatile("nop\nnop\n.word 0x4A280030")
#define gte_stsxy3(r0, r1, r2) { gte_mv3(r0, r1, r2); \
    __asm__ volatile("swc2 $12,0($12)\nswc2 $13,0($13)\nswc2 $14,0($14)" : : : "memory"); }
#define gte_stsz3(r0, r1, r2) { gte_mv3(r0, r1, r2); \
    __asm__ volatile("swc2 $17,0($12)\nswc2 $18,0($13)\nswc2 $19,0($14)" : : : "memory"); }

extern short ScreenShotOn;
extern short *FreeListPntr;
extern short *FreeListEnd;
extern struct COLLHEADER CollMap;

extern int *StringTable;
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);

static char *FindStringInTable(struct IDENTITYNAME Name)
{
    char *String;
    int Offset;
    int Finished;
    int *Labels;

    Finished = 0;
    Labels = StringTable;
    if (Labels) {
        while (!Finished) {
            Offset = Labels[3];
            String = (char *)StringTable + Offset;
            switch (ModNameCmp(&Name, (struct IDENTITYNAME *)Labels)) {
            case 0:
                break;
            case 1:
                return String;
            case 2:
                Finished = 1;
                break;
            }
            Labels += 4;
        }
    }
    return 0;
}

/* GTE macros in the PSY-Q inline_c.h style: one asm statement per instruction, so that each
   counts as one insn for gcc's register allocation (DrawVectBox needs this to match) */
#define gte_mvr(reg, r0) __asm__ volatile("move $" #reg ",%0" : : "r"(r0) : "$" #reg, "memory")
#define gte_op(s) __asm__ volatile(s)
#define gte_st(s) __asm__ volatile(s : : : "memory")
#define gte_ldv0_s(r0) { gte_mvr(12, r0); gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); }
#define gte_ldv3_s(r0, r1, r2) { gte_mvr(12, r0); gte_mvr(13, r1); gte_mvr(14, r2); \
    gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); gte_op("lwc2 $2,0($13)"); \
    gte_op("lwc2 $3,4($13)"); gte_op("lwc2 $4,0($14)"); gte_op("lwc2 $5,4($14)"); }
#define gte_ldv3c_s(r0) { gte_mvr(12, r0); \
    gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); gte_op("lwc2 $2,8($12)"); \
    gte_op("lwc2 $3,12($12)"); gte_op("lwc2 $4,16($12)"); gte_op("lwc2 $5,20($12)"); }
#define gte_rtv0_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A486012"); }
#define gte_rtpt_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A280030"); }
#define gte_stsxy3_s(r0, r1, r2) { gte_mvr(12, r0); gte_mvr(13, r1); gte_mvr(14, r2); \
    gte_st("swc2 $12,0($12)"); gte_st("swc2 $13,0($13)"); gte_st("swc2 $14,0($14)"); }
#define gte_stsxy3c_s(r0) { gte_mvr(12, r0); \
    gte_st("swc2 $12,0($12)"); gte_st("swc2 $13,4($12)"); gte_st("swc2 $14,8($12)"); }
#define gte_stlvnl_s(r0) { gte_mvr(12, r0); \
    gte_st("swc2 $25,0($12)"); gte_st("swc2 $26,4($12)"); gte_st("swc2 $27,8($12)"); }
/* translation vector = last rtv0 result (MAC1-3 -> TRX/TRY/TRZ) */
#define gte_mac2tr_s() __asm__ volatile("mfc2 $12,$25\nmfc2 $13,$26\nmfc2 $14,$27\nctc2 $12,$5\nctc2 $13,$6\nctc2 $14,$7" : : : "$12", "$13", "$14")
#define gte_SetTransMatrix_s(r0) __asm__ volatile("move $12,%0\nlw $13,20($12)\nlw $14,24($12)\nctc2 $13,$5\nlw $15,28($12)\nctc2 $14,$6\nctc2 $15,$7" : : "r"(r0) : "$12", "$13", "$14", "$15")

extern struct POLY_G4 *LastGeneralG4;
extern struct POLY_G4 *NextGeneralG4;
extern struct LINE_F2 *NextLineF2;
extern struct LINE_F2 *LastLineF2;
extern struct VECT *CurrCamPos;
extern struct MATRIX *CurrCamMat;
extern unsigned long *CurrOt;
extern struct DB *currdb;
static struct CROSSDATA CrossList[32];
void AddPrim_(void *ot, void *p);
void dprintf();
void *memset();
void SetRotMatrix(struct MATRIX *m);
struct POLY_G4 *HH_SimpleModel_(struct SIMPOL *Model, struct SIMPOL *ModelEnd, struct SCRPNT *ScrPnts, struct POLY_G4 *Poly);

int BytesBetween(void *Pntr0, void *Pntr1)
{
    int a = (char *)Pntr0 - (char *)Pntr1;
    return a < 0 ? -a : a;
}

void AddCross(struct VECT *Pos, int Radius, struct COLOUR Colour)
{
    struct CROSSDATA *Cross;
#if 3 == 1
    Cross = CrossPntr;
    if (CrossPntr != CrossLast) {
        CrossPntr++;
        Cross->Pos = *Pos;
        Cross->Radius = Radius;
        Cross->Colour = Colour;
    }
#elif 3 == 2
    if (CrossPntr != CrossLast) {
        Cross = CrossPntr;
        Cross->Pos = *Pos;
        Cross->Radius = Radius;
        Cross->Colour = Colour;
        CrossPntr++;
    }
#else
    if (CrossPntr != CrossLast) {
        CrossPntr->Pos = *Pos;
        CrossPntr->Radius = Radius;
        CrossPntr->Colour = Colour;
        CrossPntr++;
    }
#endif
}

void DrawCrossesCurrWin(void)
{
    struct CROSSDATA *Cross;
    struct VECT Tran;
    struct VECT Temp;
    struct SVECTOR Points[6];
    int Rad;
    struct SVECTOR Rel;

    SetRotMatrix(CurrCamMat);
    for (Cross = CrossList; Cross < CrossPntr; Cross++) {
        if (NextLineF2 + 3 > LastLineF2)
            continue;
        Temp.vx = Cross->Pos.vx - CurrCamPos->vx;
        Temp.vy = Cross->Pos.vy - CurrCamPos->vy;
        Temp.vz = Cross->Pos.vz - CurrCamPos->vz;
        if (Temp.vx < -32000 || Temp.vx > 32000)
            continue;
        if (Temp.vy < -32000 || Temp.vy > 32000)
            continue;
        if (Temp.vz < -32000 || Temp.vz > 32000)
            continue;
        Rel = (struct SVECTOR){Temp.vx, Temp.vy, Temp.vz};
        gte_ldv0_s(&Rel);
        gte_rtv0_s();
        gte_stlvnl_s(&Tran);
        if (Tran.vz < 0x80)
            continue;
        gte_SetTransMatrix_s((struct MATRIX *)((char *)&Tran - 20));
        Rad = Cross->Radius;
        Points[0] = (struct SVECTOR){Rad, 0, 0};
        Points[1] = (struct SVECTOR){-Rad, 0, 0};
        Points[2] = (struct SVECTOR){0, Rad, 0};
        Points[3] = (struct SVECTOR){0, -Rad, 0};
        Points[4] = (struct SVECTOR){0, 0, Rad};
        Points[5] = (struct SVECTOR){0, 0, -Rad};
        gte_ldv3_s(&Points[0], &Points[1], &Points[2]);
        gte_rtpt_s();
        gte_stsxy3_s(&NextLineF2[0].x0, &NextLineF2[0].x1, &NextLineF2[1].x0);
        gte_ldv3_s(&Points[3], &Points[4], &Points[5]);
        gte_rtpt_s();
        gte_stsxy3_s(&NextLineF2[1].x1, &NextLineF2[2].x0, &NextLineF2[2].x1);
        *(struct RGB3 *)&NextLineF2[0].r0 = *(struct RGB3 *)&Cross->Colour;
        *(struct RGB3 *)&NextLineF2[1].r0 = *(struct RGB3 *)&Cross->Colour;
        *(struct RGB3 *)&NextLineF2[2].r0 = *(struct RGB3 *)&Cross->Colour;
        ((unsigned char *)&NextLineF2[0].tag)[3] = 3;
        NextLineF2[0].code = 0x40;
        ((unsigned char *)&NextLineF2[1].tag)[3] = 3;
        NextLineF2[1].code = 0x40;
        ((unsigned char *)&NextLineF2[2].tag)[3] = 3;
        NextLineF2[2].code = 0x40;
        AddPrim_(&CurrOt[50], &NextLineF2[0]);
        AddPrim_(&CurrOt[50], &NextLineF2[1]);
        AddPrim_(&CurrOt[50], &NextLineF2[2]);
        NextLineF2 += 3;
    }
}

char *PrintBool(int Bool)
{
    static char On[3] = "On";
    static char Off[4] = "Off";

    if (Bool)
        return On;
    return Off;
}

void InitCrossEveryFrame(void)
{
    CrossPntr = CrossList;
}

void Relocate(void *Pntr, void *Base)
{
    int Temp;

    Temp = *(int *)Pntr;
    *(int *)Pntr = Temp + (int)Base;
}

void FindMaxMin(struct VECT *Start, struct VECT *Stop, struct VECTBOX *VectBox)
{
    if (Start->vx > Stop->vx) {
        VectBox->Max.vx = Start->vx;
        VectBox->Min.vx = Stop->vx;
    } else {
        VectBox->Max.vx = Stop->vx;
        VectBox->Min.vx = Start->vx;
    }
    if (Start->vy > Stop->vy) {
        VectBox->Max.vy = Start->vy;
        VectBox->Min.vy = Stop->vy;
    } else {
        VectBox->Max.vy = Stop->vy;
        VectBox->Min.vy = Start->vy;
    }
    if (Start->vz > Stop->vz) {
        VectBox->Max.vz = Start->vz;
        VectBox->Min.vz = Stop->vz;
    } else {
        VectBox->Max.vz = Stop->vz;
        VectBox->Min.vz = Start->vz;
    }
}

int SeekAng(int Start, int Stop, int Speed)
{
    int a, b;

    a = (Stop - Start) & 0xfff;
    if ((Stop - Start) & 0x800) {
        a = 0x1000 - a;
        b = (a * Speed + 0x7ff) >> 11;
        return (Start - b) & 0xfff;
    }
    b = (a * Speed + 0x7ff) >> 11;
    return (Start + b) & 0xfff;
}

int Seek(short *a, short b, short c)
{
    if (*a < b) {
        if (*a + c >= b)
            *a = b;
        else
            *a += c;
    } else {
        if (*a - c <= b)
            *a = b;
        else
            *a -= c;
    }
    return *a == b;
}

void SeekL(long *a, short b, short c)
{
    if (*a < b) {
        if (*a + c >= b)
            *a = b;
        else
            *a += c;
    } else {
        if (*a - c <= b)
            *a = b;
        else
            *a -= c;
    }
}

void CalcVectorL(struct MATRIX *m, struct VECT *v1, struct VECT *v2)
{
    v2->vx = v1->vx * m->m[0][0] + v1->vy * m->m[1][0] + v1->vz * m->m[2][0];
    v2->vy = v1->vx * m->m[0][1] + v1->vy * m->m[1][1] + v1->vz * m->m[2][1];
    v2->vz = v1->vx * m->m[0][2] + v1->vy * m->m[1][2] + v1->vz * m->m[2][2];
    v2->vx >>= 12;
    v2->vy >>= 12;
    v2->vz >>= 12;
}

void Rotate(struct MATRIX *m, struct SVECT *v1, struct VECT *v2)
{
    v2->vx = v1->vx * m->m[0][0] + v1->vy * m->m[0][1] + v1->vz * m->m[0][2];
    v2->vy = v1->vx * m->m[1][0] + v1->vy * m->m[1][1] + v1->vz * m->m[1][2];
    v2->vz = v1->vx * m->m[2][0] + v1->vy * m->m[2][1] + v1->vz * m->m[2][2];
    v2->vx >>= 12;
    v2->vy >>= 12;
    v2->vz >>= 12;
}

void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *v2)
{
    struct VECT v;

    v.vx = v1->vx * m->m[0][0] + v1->vy * m->m[0][1] + v1->vz * m->m[0][2];
    v.vy = v1->vx * m->m[1][0] + v1->vy * m->m[1][1] + v1->vz * m->m[1][2];
    v.vz = v1->vx * m->m[2][0] + v1->vy * m->m[2][1] + v1->vz * m->m[2][2];
    v.vx >>= 12;
    v.vy >>= 12;
    v.vz >>= 12;
    v2->vx = v.vx;
    v2->vy = v.vy;
    v2->vz = v.vz;
}

void RotateRelSpeedsLadder(struct MATRIX *M, struct SVECT *Rel, struct SVECT *Vel, struct SVECT *Norm, struct SVECT *OutPut)
{
    struct VECTOR v2;
    struct SVECT Vel2;
    struct SVECT AVel;
    struct SVECT BVel;
    int Dot;  /* not in SYM: dead, so its pseudo never gets a home */

    v2.vx = Rel->vx * M->m[0][0] + Rel->vy * M->m[0][1] + Rel->vz * M->m[0][2];
    v2.vy = Rel->vx * M->m[1][0] + Rel->vy * M->m[1][1] + Rel->vz * M->m[1][2];
    v2.vz = Rel->vx * M->m[2][0] + Rel->vy * M->m[2][1] + Rel->vz * M->m[2][2];
    v2.vx >>= 12;
    v2.vy = -(v2.vy >> 12);
    v2.vz >>= 12;
    /* Dead (result unused, the code that used it is gone): the line table has a statement
       here whose only surviving code is the Norm->vx load that CSE reuses for OutPut->vx.
       Its pre-optimisation insns are what make this function too big to be an inline
       candidate, so it is emitted in place. */
    Dot = Norm->vx * Vel->vx + Norm->vy * Vel->vy + Norm->vz * Vel->vz;
    OutPut->vx = v2.vx + (Norm->vx >> 5);
    OutPut->vy = v2.vy;
    OutPut->vz = v2.vz + (Norm->vz >> 5);
}

void RotateRelSpeedsFly(struct MATRIX *M, struct SVECT *Rel, struct SVECT *Vel)
{
    struct VECTOR v2;

    v2.vx = Rel->vx * M->m[0][0] + Rel->vy * M->m[0][1] + Rel->vz * M->m[0][2];
    v2.vy = Rel->vx * M->m[1][0] + Rel->vy * M->m[1][1] + Rel->vz * M->m[1][2];
    v2.vz = Rel->vx * M->m[2][0] + Rel->vy * M->m[2][1] + Rel->vz * M->m[2][2];
    v2.vx >>= 12;
    v2.vy >>= 12;
    v2.vz >>= 12;
    Vel->vx = v2.vx;
    Vel->vy = -v2.vy;
    Vel->vz = v2.vz;
}

void RotateRelSpeeds(short Ang, short x, short z, struct SVECT *OutPut)
{
    int Cos, Sin;
    Cos = rcossin_tbl[Ang & 0xfff][1];
    Sin = rcossin_tbl[Ang & 0xfff][0];
    OutPut->vx = (Cos * x + Sin * z) >> 12;
    OutPut->vz = (Cos * z + -Sin * x) >> 12;
}

void RotateL(struct MATRIX *m, struct VECT *v1, struct VECT *v2)
{
    v2->vx = v1->vx * m->m[0][0] + v1->vy * m->m[0][1] + v1->vz * m->m[0][2];
    v2->vy = v1->vx * m->m[1][0] + v1->vy * m->m[1][1] + v1->vz * m->m[1][2];
    v2->vz = v1->vx * m->m[2][0] + v1->vy * m->m[2][1] + v1->vz * m->m[2][2];
    v2->vx >>= 12;
    v2->vy >>= 12;
    v2->vz >>= 12;
}

void ScaleSVect(struct SVECT *Vec, int Scale)
{
    Vec->vx = (Vec->vx * Scale) >> 12;
    Vec->vy = (Vec->vy * Scale) >> 12;
    Vec->vz = (Vec->vz * Scale) >> 12;
}

int MaxSizeSVect(struct SVECT SVect)
{
    int Max;

    if (SVect.vx >= 0)
        Max = SVect.vx;
    else
        Max = -SVect.vx;
    if (SVect.vy < 0) {
        if (Max < -SVect.vy)
            Max = -SVect.vy;
    } else {
        if (Max < SVect.vy)
            Max = SVect.vy;
    }
    if (SVect.vz < 0) {
        if (Max < -SVect.vz)
            Max = -SVect.vz;
    } else {
        if (Max < SVect.vz)
            Max = SVect.vz;
    }
    return Max;
}

void DisplayBuildInfo(int Flags)
{
    char Version[10] = "9 8";

    if (CheatActiveStatus == 6) {
        if (Flags)
            SpriText(GameWindows, "^7E7E30@0C00FVer %s  %s  %s", Version, "Sep 23 1999", "15:36:14");
        else
            SpriText(GameWindows, "^7E7E30@0B000Ver %s  %s  %s", Version, "Sep 23 1999", "15:36:14");
        if (Flags)
            SpriText(GameWindows, "^6E6E30@190 0F (%s)", "Anonymous");
        else
            SpriText(GameWindows, "^6E6E30@180 00 (%s)", "Anonymous");
    }
}

void PrintDebugInfoScene(int *DebugScreen)
{
    char AString[128];
    struct WINDOW *Win;

    if (ScreenShotOn == 2) {
        if (CollMap.RemapTable) {
            Win = GameWindows;
            if (Win->CamSpace >= 0)
                sprintf(AString, "|1@1a010%d   %d", CollMap.RemapTable[Win->CamSpace], FreeListEnd - FreeListPntr);
            else
                sprintf(AString, "|1@1a010Error  %d", FreeListEnd - FreeListPntr);
            SpriText(Win, AString);
        }
        DisplayBuildInfo(0);
    }
}

void DrawCylinder(struct VECT *Location, short Radius, short MinY, short MaxY)
{
    static struct SIMPOL CylinderModel[10] = {
    {{0, 1, 6, 7}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{1, 2, 7, 8}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{2, 3, 8, 9}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{3, 4, 9, 10}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{4, 5, 10, 11}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{5, 0, 11, 6}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{1, 0, 2, 3}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{4, 3, 5, 0}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{6, 7, 9, 8}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    {{9, 10, 6, 11}, {{0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}, {0x7f, 0x0, 0x0, 0x3a}}},
    };
    struct SCRPNT ScrPnts[12];
    struct SVECTOR Points[12];
    struct SVECT Rel;
    struct VECT Tran;
    struct POLY_G4 *OldPoly;
    struct POLY_G4 *Poly;

    if (NextGeneralG4 <= LastGeneralG4 - 10) {
        Rel.vx = Location->vx - GameWindows[0].CamPos.vx;
        Rel.vy = Location->vy - GameWindows[0].CamPos.vy;
        Rel.vz = Location->vz - GameWindows[0].CamPos.vz;
        {
            int a;
            int y;
            int ValA;
            int ValB;

            y = MinY;
            ValA = Radius / 2;
            ValB = Radius * 3547 / 4096;
            for (a = 0; a < 12; a += 6) {
                Points[a + 0] = (struct SVECTOR){ValA, y, -ValB};
                Points[a + 1] = (struct SVECTOR){Radius, y, 0};
                Points[a + 2] = (struct SVECTOR){ValA, y, ValB};
                Points[a + 3] = (struct SVECTOR){-ValA, y, ValB};
                Points[a + 4] = (struct SVECTOR){-Radius, y, 0};
                Points[a + 5] = (struct SVECTOR){-ValA, y, -ValB};
                y = MaxY;
            }
        }
        gte_ldv0_s(&Rel);
        gte_rtv0_s();
        gte_mac2tr_s();
        {
            int a;

            for (a = 0; a < 12; a += 3) {
                gte_ldv3c_s(&Points[a]);
                gte_rtpt_s();
                gte_stsxy3c_s(&ScrPnts[a]);
            }
        }
        OldPoly = NextGeneralG4;
        NextGeneralG4 = HH_SimpleModel_(CylinderModel, &CylinderModel[9], ScrPnts, OldPoly);
        for (Poly = OldPoly; Poly < NextGeneralG4; Poly++)
            AddPrim_(&currdb->Ot[0][48], Poly);
    } else
        dprintf(GameWindows, "Failed to draw Cylinder \n");
}

void DrawVectBox(struct VECTBOX *VectBox)
{
    static struct SIMPOL ModelData[8] = {
    {{0, 1, 2, 3}, {{0xff, 0x0, 0x0, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0xff, 0x0, 0x0, 0x3a}}},
    {{0, 4, 1, 5}, {{0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}}},
    {{1, 5, 3, 7}, {{0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}}},
    {{3, 7, 2, 6}, {{0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}}},
    {{2, 6, 0, 4}, {{0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0xff, 0x0, 0x0, 0x3a}, {0x0, 0x0, 0xff, 0x3a}}},
    {{5, 4, 7, 6}, {{0x0, 0x0, 0xff, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0x0, 0x0, 0xff, 0x3a}, {0x0, 0x0, 0xff, 0x3a}}},
    {{0, 0, 0, 0}, {{0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {{0, 0, 0, 0}, {{0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    };
    struct SCRPNT ScrPnts[9];
    struct SVECTOR Points[9];
    struct SVECT Rel;
    struct VECT Tran;
    int X;
    int Y;
    int Z;
    struct POLY_G4 *OldPoly;
    struct POLY_G4 *Poly;

    if (NextGeneralG4 <= LastGeneralG4 - 6) {
        X = VectBox->Max.vx - VectBox->Min.vx;
        Y = VectBox->Max.vy - VectBox->Min.vy;
        Z = VectBox->Max.vz - VectBox->Min.vz;
        Rel.vx = VectBox->Min.vx - GameWindows[0].CamPos.vx;
        Rel.vy = VectBox->Min.vy - GameWindows[0].CamPos.vy;
        Rel.vz = VectBox->Min.vz - GameWindows[0].CamPos.vz;
        Points[0] = (struct SVECTOR){0, 0, 0};
        Points[1] = (struct SVECTOR){X, 0, 0};
        Points[2] = (struct SVECTOR){0, 0, Z};
        Points[3] = (struct SVECTOR){X, 0, Z};
        Points[4] = (struct SVECTOR){0, Y, 0};
        Points[5] = (struct SVECTOR){X, Y, 0};
        Points[6] = (struct SVECTOR){0, Y, Z};
        Points[7] = (struct SVECTOR){X, Y, Z};
        gte_ldv0_s(&Rel);
        gte_rtv0_s();
        gte_mac2tr_s();
        gte_ldv3_s(&Points[0], &Points[1], &Points[2]);
        gte_rtpt_s();
        gte_stsxy3_s(&ScrPnts[0], &ScrPnts[1], &ScrPnts[2]);
        gte_ldv3_s(&Points[3], &Points[4], &Points[5]);
        gte_rtpt_s();
        gte_stsxy3_s(&ScrPnts[3], &ScrPnts[4], &ScrPnts[5]);
        gte_ldv3_s(&Points[6], &Points[7], &Points[8]);
        gte_rtpt_s();
        gte_stsxy3_s(&ScrPnts[6], &ScrPnts[7], &ScrPnts[8]);
        OldPoly = NextGeneralG4;
        NextGeneralG4 = HH_SimpleModel_(ModelData, &ModelData[5], ScrPnts, OldPoly);
        for (Poly = OldPoly; Poly < NextGeneralG4; Poly++)
            AddPrim_(&currdb->Ot[0][48], Poly);
    } else
        dprintf(GameWindows, "Failed to draw a VectBox\n");
}

char *GetStringFromStringtable(struct IDENTITYNAME StringName)
{
    int a;
    struct IDENTITYNAME UsName;
    char *ReturnString;

    for (a = 0; a < 12; a++) {
        if (!StringName.Name[a])
            break;
        UsName.Name[a] = StringName.Name[a];
    }
    if (a != 12) {
        UsName.Name[a++] = 'U';
        if (a != 12) {
            UsName.Name[a++] = 'S';
            for (; a < 12; a++)
                UsName.Name[a] = 0;
        }
    }
    ReturnString = FindStringInTable(UsName);
    if (!ReturnString)
        ReturnString = FindStringInTable(StringName);
    return ReturnString;
}

int mystrlen(char *String)
{
    char *tc;

    if (String == 0)
        return 0;
    tc = String;
    while (*tc++)
        ;
    return tc - String - 1;
}

void RemoveMeRoutine(struct SVECTOR *R, struct SVECTOR *P)
{
    gte_ldv3(R, R + 1, R + 2);
    gte_rtpt();
    gte_stsxy3(P, P + 1, P + 2);
    gte_stsz3(&P->vz, &P[1].vz, &P[2].vz);
}
