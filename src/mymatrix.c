#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
struct MATRIX *GenRotMatrixY(int yang, struct MATRIX *mat);
int SquareRootNR(int a);
void ReverseMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn);
void ScaleCameraMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, short x, short y);
void MyScaleMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, int Scale);
void MYRotMatrixANIM(struct SVECT *ang, struct MATRIX *m);
void MYRotMatrixYXZ2(struct SVECT *ang, struct MATRIX *m);
void ElipseMatrix(struct MATRIX *Source, struct MATRIX *Dest, struct SVECT *R);
extern short rcossin_tbl[][2];
int SquareRoot0(int a);

void MYRotMatrixANIM(struct SVECT *ang, struct MATRIX *m)
{
    int sinx, cosx, siny, cosy, sinz, cosz;

    sinx = rcossin_tbl[(ang->vx >> 2) & 0xfff][0];
    cosx = rcossin_tbl[(ang->vx >> 2) & 0xfff][1];
    siny = rcossin_tbl[(ang->vy >> 2) & 0xfff][0];
    cosy = rcossin_tbl[(ang->vy >> 2) & 0xfff][1];
    sinz = rcossin_tbl[(ang->vz >> 2) & 0xfff][0];
    cosz = rcossin_tbl[(ang->vz >> 2) & 0xfff][1];
    m->m[0][0] = cosy * cosz / 4096;
    m->m[1][0] = cosy * sinz / 4096;
    m->m[2][0] = -siny;
    m->m[0][1] = (sinx * siny / 4096) * cosz / 4096 - cosx * sinz / 4096;
    m->m[1][1] = (sinx * siny / 4096) * sinz / 4096 + cosx * cosz / 4096;
    m->m[2][1] = sinx * cosy / 4096;
    m->m[0][2] = (cosx * siny / 4096) * cosz / 4096 + sinx * sinz / 4096;
    m->m[1][2] = (cosx * siny / 4096) * sinz / 4096 - sinx * cosz / 4096;
    m->m[2][2] = cosx * cosy / 4096;
}

struct MATRIX *GenRotMatrixY(int yang, struct MATRIX *mat)
{
    yang &= 0xfff;
    mat->m[0][0] = rcossin_tbl[yang][1];
    mat->m[0][1] = 0;
    mat->m[0][2] = rcossin_tbl[yang][0];
    mat->m[1][0] = 0;
    mat->m[1][1] = 4096;
    mat->m[1][2] = 0;
    mat->m[2][0] = -rcossin_tbl[yang][0];
    mat->m[2][1] = 0;
    mat->m[2][2] = rcossin_tbl[yang][1];
    return mat;
}

void MYRotMatrixYXZ2(struct SVECT *ang, struct MATRIX *m)
{
    int sinx, cosx, siny, cosy, sinz, cosz;

    sinx = rcossin_tbl[ang->vx & 0xfff][0];
    cosx = rcossin_tbl[ang->vx & 0xfff][1];
    siny = rcossin_tbl[ang->vy & 0xfff][0];
    cosy = rcossin_tbl[ang->vy & 0xfff][1];
    sinz = rcossin_tbl[ang->vz & 0xfff][0];
    cosz = rcossin_tbl[ang->vz & 0xfff][1];
    m->m[0][0] = cosz * cosy / 4096 - (sinz * sinx / 4096) * siny / 4096;
    m->m[0][1] = -sinz * cosx / 4096;
    m->m[0][2] = cosz * siny / 4096 + (sinz * sinx / 4096) * cosy / 4096;
    m->m[1][0] = sinz * cosy / 4096 + (cosz * sinx / 4096) * siny / 4096;
    m->m[1][1] = cosz * cosx / 4096;
    m->m[1][2] = sinz * siny / 4096 - (cosz * sinx / 4096) * cosy / 4096;
    m->m[2][0] = -cosx * siny / 4096;
    m->m[2][1] = sinx;
    m->m[2][2] = cosx * cosy / 4096;
}

void ScaleCameraMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, short x, short y)
{
    MatrixOut->m[0][0] = MatrixIn->m[0][0] * x / 320;
    MatrixOut->m[0][1] = MatrixIn->m[0][1] * x / 320;
    MatrixOut->m[0][2] = MatrixIn->m[0][2] * x / 320;
    MatrixOut->m[1][0] = MatrixIn->m[1][0] * y / 240;
    MatrixOut->m[1][1] = MatrixIn->m[1][1] * y / 240;
    MatrixOut->m[1][2] = MatrixIn->m[1][2] * y / 240;
    MatrixOut->m[2][0] = MatrixIn->m[2][0];
    MatrixOut->m[2][1] = MatrixIn->m[2][1];
    MatrixOut->m[2][2] = MatrixIn->m[2][2];
}

void MyScaleMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, int Scale)
{
    MatrixOut->m[0][0] = MatrixIn->m[0][0] * Scale / 4096;
    MatrixOut->m[0][1] = MatrixIn->m[0][1] * Scale / 4096;
    MatrixOut->m[0][2] = MatrixIn->m[0][2] * Scale / 4096;
    MatrixOut->m[1][0] = MatrixIn->m[1][0] * Scale / 4096;
    MatrixOut->m[1][1] = MatrixIn->m[1][1] * Scale / 4096;
    MatrixOut->m[1][2] = MatrixIn->m[1][2] * Scale / 4096;
    MatrixOut->m[2][0] = MatrixIn->m[2][0] * Scale / 4096;
    MatrixOut->m[2][1] = MatrixIn->m[2][1] * Scale / 4096;
    MatrixOut->m[2][2] = MatrixIn->m[2][2] * Scale / 4096;
}

void ReverseMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn)
{
    MatrixOut->m[0][0] = MatrixIn->m[0][0];
    MatrixOut->m[0][1] = MatrixIn->m[1][0];
    MatrixOut->m[0][2] = MatrixIn->m[2][0];
    MatrixOut->m[1][0] = MatrixIn->m[0][1];
    MatrixOut->m[1][1] = MatrixIn->m[1][1];
    MatrixOut->m[1][2] = MatrixIn->m[2][1];
    MatrixOut->m[2][0] = MatrixIn->m[0][2];
    MatrixOut->m[2][1] = MatrixIn->m[1][2];
    MatrixOut->m[2][2] = MatrixIn->m[2][2];
}

int SquareRootNR(int a)
{
    int result;
    if (a != 0) {
        result = SquareRoot0(a);
        result -= (result * result - a) / (result << 1);
        result -= (result * result - a) / (result << 1);
        return result;
    }
    return 0;
}

void ElipseMatrix(struct MATRIX *Source, struct MATRIX *Dest, struct SVECT *R)
{
    Dest->m[0][0] = Source->m[0][0] * R->vx / 256;
    Dest->m[0][1] = Source->m[0][1] * R->vy / 256;
    Dest->m[0][2] = Source->m[0][2] * R->vz / 256;
    Dest->m[1][0] = Source->m[1][0] * R->vx / 256;
    Dest->m[1][1] = Source->m[1][1] * R->vy / 256;
    Dest->m[1][2] = Source->m[1][2] * R->vz / 256;
    Dest->m[2][0] = Source->m[2][0] * R->vx / 256;
    Dest->m[2][1] = Source->m[2][1] * R->vy / 256;
    Dest->m[2][2] = Source->m[2][2] * R->vz / 256;
}
