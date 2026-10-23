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
