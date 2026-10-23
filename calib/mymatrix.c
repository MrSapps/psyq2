#include "types.h"
extern short rcossin_tbl[][2];
int SquareRoot0(int a);

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
