#include "types.h"

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

/* defined in this file (misc.obj): $gp-relative */
static struct CROSSDATA *CrossPntr;
static struct CROSSDATA *CrossLast;

extern short rcossin_tbl[][2];

int BytesBetween(void *Pntr0, void *Pntr1)
{
    int a = (char *)Pntr0 - (char *)Pntr1;
    return a < 0 ? -a : a;
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

void ScaleSVect(struct SVECT *Vec, int Scale)
{
    Vec->vx = (Vec->vx * Scale) >> 12;
    Vec->vy = (Vec->vy * Scale) >> 12;
    Vec->vz = (Vec->vz * Scale) >> 12;
}

void Relocate(void *Pntr, void *Base)
{
    int Temp;

    Temp = *(int *)Pntr;
    *(int *)Pntr = Temp + (int)Base;
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

void RotateL(struct MATRIX *m, struct VECT *v1, struct VECT *v2)
{
    v2->vx = v1->vx * m->m[0][0] + v1->vy * m->m[0][1] + v1->vz * m->m[0][2];
    v2->vy = v1->vx * m->m[1][0] + v1->vy * m->m[1][1] + v1->vz * m->m[1][2];
    v2->vz = v1->vx * m->m[2][0] + v1->vy * m->m[2][1] + v1->vz * m->m[2][2];
    v2->vx >>= 12;
    v2->vy >>= 12;
    v2->vz >>= 12;
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

void RotateRelSpeeds(short Ang, short x, short z, struct SVECT *OutPut)
{
    int Cos, Sin;
    Cos = rcossin_tbl[Ang & 0xfff][1];
    Sin = rcossin_tbl[Ang & 0xfff][0];
    OutPut->vx = (Cos * x + Sin * z) >> 12;
    OutPut->vz = (Cos * z + -Sin * x) >> 12;
}

static struct CROSSDATA CrossList[32];
extern short CheatActiveStatus;
extern struct WINDOW GameWindows[5];

void InitCrossEveryFrame(void)
{
    CrossPntr = CrossList;
}

char *PrintBool(int Bool)
{
    static char On[3] = "On";
    static char Off[4] = "Off";

    if (Bool)
        return On;
    return Off;
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

void RemoveMeRoutine(struct SVECTOR *R, struct SVECTOR *P)
{
    gte_ldv3(R, R + 1, R + 2);
    gte_rtpt();
    gte_stsxy3(P, P + 1, P + 2);
    gte_stsz3(&P->vz, &P[1].vz, &P[2].vz);
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

void RotateRelSpeedsLadder(struct MATRIX *M, struct SVECT *Rel, struct SVECT *Vel, struct SVECT *Norm, struct SVECT *OutPut)
{
    struct VECTOR v2;
    struct SVECT Vel2;
    struct SVECT AVel;
    struct SVECT BVel;

    v2.vx = Rel->vx * M->m[0][0] + Rel->vy * M->m[0][1] + Rel->vz * M->m[0][2];
    v2.vy = Rel->vx * M->m[1][0] + Rel->vy * M->m[1][1] + Rel->vz * M->m[1][2];
    v2.vz = Rel->vx * M->m[2][0] + Rel->vy * M->m[2][1] + Rel->vz * M->m[2][2];
    v2.vx >>= 12;
    v2.vy = -(v2.vy >> 12);
    v2.vz >>= 12;
    OutPut->vx = v2.vx + (Norm->vx >> 5);
    OutPut->vy = v2.vy;
    OutPut->vz = v2.vz + (Norm->vz >> 5);
}

extern short ScreenShotOn;
extern short *FreeListPntr;
extern short *FreeListEnd;
extern struct COLLHEADER CollMap;

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
