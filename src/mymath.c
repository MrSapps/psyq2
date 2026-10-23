#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void GenTangentialAxis(struct VECT *R, struct VECT *A, struct VECT *B);
void FractionDist(struct SVECT *R, struct SVECT *T, struct FRACTION *F);
int abs(int); struct PAIR QuadraticSolutionsR12(int a, int b, int c);
void OnePixel(struct SVECT *A, struct SVECT *B);
void LongIntMult_(struct LONGINT *L, int a, int b);
int LongIntAccDiv24_(struct LONGINT *L, int d);

int SquareRootNR(int a);
int AccMultDiv_(int a, int b, int c);

void GenTangentialAxis(struct VECT *R, struct VECT *A, struct VECT *B)
{
    if (abs(R->vy) >= 0xb51) {
        A->vx = 0;
        A->vy = -R->vz;
        A->vz = R->vy;
        B->vx = R->vx * R->vx / 4096 - 4096;
        B->vy = R->vy * R->vx / 4096;
        B->vz = R->vz * R->vx / 4096;
    } else {
        A->vx = R->vz;
        A->vy = 0;
        A->vz = -R->vx;
        B->vx = R->vx * R->vy / 4096;
        B->vy = R->vy * R->vy / 4096 - 4096;
        B->vz = R->vz * R->vy / 4096;
    }
}

void FractionDist(struct SVECT *R, struct SVECT *T, struct FRACTION *F)
{
    struct LONGINT L;

    LongIntMult_(&L, T->vx, F->Numerator);
    R->vx = LongIntAccDiv24_(&L, F->Denominator);
    LongIntMult_(&L, T->vy, F->Numerator);
    R->vy = LongIntAccDiv24_(&L, F->Denominator);
    LongIntMult_(&L, T->vz, F->Numerator);
    R->vz = LongIntAccDiv24_(&L, F->Denominator);
}

int abs(int);

struct PAIR QuadraticSolutionsR12(int a, int b, int c)
{
    unsigned int Root;
    long long b2_4ac;
    long long ac4;
    int Sa;
    int Sb;

    b2_4ac = b;
    ac4 = a;
    b2_4ac = b2_4ac * b2_4ac - ac4 * c * 4;
    if (b2_4ac < 0)
        return (struct PAIR){0x7fffffff, 0x7fffffff};
    {
        long long BigNum = b2_4ac;
        if (BigNum < 0)
            Root = 0x7fffffff;
        else {
            int ShiftVal = 0;
            while (BigNum > 0x7fffffff) {
                BigNum >>= 2;
                ShiftVal++;
            }
            Root = SquareRootNR(BigNum) << ShiftVal;
        }
    }
    Sa = -b - Root;
    if ((Sa ^ a) < 0)
        Sa = AccMultDiv_(-0x800, abs(Sa), abs(a));
    else
        Sa = AccMultDiv_(0x800, abs(Sa), abs(a));
    Sb = Root - b;
    if ((Sb ^ a) < 0)
        Sb = AccMultDiv_(-0x800, abs(Sb), abs(a));
    else
        Sb = AccMultDiv_(0x800, abs(Sb), abs(a));
    if (a > 0)
        return (struct PAIR){Sa, Sb};
    else
        return (struct PAIR){Sb, Sa};
}

void OnePixel(struct SVECT *A, struct SVECT *B)
{
    int a;
    int b;

    a = A->vx;
    b = 0;
    if (a < -A->vx) {
        a = -A->vx;
        b = 1;
    }
    if (a < A->vy) {
        a = A->vy;
        b = 2;
    }
    if (a < -A->vy) {
        a = -A->vy;
        b = 3;
    }
    if (a < A->vz) {
        a = A->vz;
        b = 4;
    }
    if (a < -A->vz) {
        a = -A->vz;
        b = 5;
    }
    switch (b) {
    case 0:
        *B = (struct SVECT){1, 0, 0};
        break;
    case 1:
        *B = (struct SVECT){-1, 0, 0};
        break;
    case 2:
        *B = (struct SVECT){0, 1, 0};
        break;
    case 3:
        *B = (struct SVECT){0, -1, 0};
        break;
    case 4:
        *B = (struct SVECT){0, 0, 1};
        break;
    case 5:
        *B = (struct SVECT){0, 0, -1};
        break;
    }
}
