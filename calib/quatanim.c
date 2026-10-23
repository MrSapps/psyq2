#include "types.h"

void QuatToMat(struct QUAT *Quat, struct MATRIX *Mat)
{
    int wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    x2 = Quat->vx + Quat->vx;
    y2 = Quat->vy + Quat->vy;
    z2 = Quat->vz + Quat->vz;
    xx = Quat->vx * x2 >> 12;
    xy = Quat->vx * y2 >> 12;
    xz = Quat->vx * z2 >> 12;
    yy = Quat->vy * y2 >> 12;
    yz = Quat->vy * z2 >> 12;
    zz = Quat->vz * z2 >> 12;
    wx = Quat->w * x2 >> 12;
    wy = Quat->w * y2 >> 12;
    wz = Quat->w * z2 >> 12;
    Mat->m[0][0] = 4096 - (yy + zz);
    Mat->m[0][1] = xy - wz;
    Mat->m[0][2] = xz + wy;
    Mat->m[1][0] = xy + wz;
    Mat->m[1][1] = 4096 - (xx + zz);
    Mat->m[1][2] = yz - wx;
    Mat->m[2][0] = xz - wy;
    Mat->m[2][1] = yz + wx;
    Mat->m[2][2] = 4096 - (xx + yy);
}
