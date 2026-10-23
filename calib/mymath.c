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
