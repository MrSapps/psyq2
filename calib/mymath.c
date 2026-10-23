#include "types.h"
void LongIntMult_(struct LONGINT *L, int a, int b);
int LongIntAccDiv24_(struct LONGINT *L, int d);

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
