#include "types.h"

extern short rcossin_tbl[][2];

void PackedEulerToCamQuat(struct PACKEDEULER Ang, struct QUAT *Res)
{
    int cr, cp, cy, sr, sp, sy, cpcy, spsy, Temp;
    struct SVECT Ang2;

    Ang2.vx = Ang.z;
    Ang2.vy = Ang.x;
    Ang2.vz = Ang.y * 2;
    sp = rcossin_tbl[Ang2.vx][0];
    cp = rcossin_tbl[Ang2.vx][1];
    sy = rcossin_tbl[Ang2.vy][0];
    cy = rcossin_tbl[Ang2.vy][1];
    sr = rcossin_tbl[Ang2.vz][0];
    cr = rcossin_tbl[Ang2.vz][1];
    cpcy = cp * cy >> 12;
    spsy = sp * sy >> 12;
    Res->w = (cr * cpcy >> 12) + (sr * spsy >> 12);
    Res->vy = (sr * cpcy >> 12) + (cr * spsy >> 12);
    Temp = ((cr * sp >> 12) * cy >> 12) - ((sr * cp >> 12) * sy >> 12);
    Res->vz = Temp;
    Temp = ((cr * cp >> 12) * sy >> 12) - ((sr * sp >> 12) * cy >> 12);
    Res->vx = Temp;
}

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

extern short ArcCos[];

static int MyAcos(int c)
{
    {
        int a;

        a = -c + 0x1000;
        a >>= 1;
        if (a == 0x800)
            a = 0x7ff;
        a += 0x800;
        a &= 0xfff;
        return ArcCos[a];
    }
}

int QuatSlerp(struct QUAT *From, struct QUAT *To, int T, struct QUAT *Res)
{
    int to1[4];
    int omega;
    int cosom;
    int sinom;
    int scale0;
    int scale1;

    cosom = From->vx * To->vx + From->vy * To->vy + From->vz * To->vz + From->w * To->w;
    if (cosom < 0) {
        cosom = -cosom;
        to1[0] = -To->vx;
        to1[1] = -To->vy;
        to1[2] = -To->vz;
        to1[3] = -To->w;
    } else {
        to1[0] = To->vx;
        to1[1] = To->vy;
        to1[2] = To->vz;
        to1[3] = To->w;
    }
    cosom >>= 12;
    cosom = 0x1000 - cosom;
    if ((From->vx == To->vx && From->vy == To->vy && From->vz == To->vz && From->w == To->w) || T == 0x1000 || T == 0) {
        if (T == 0) {
            Res->vx = From->vx;
            Res->vy = From->vy;
            Res->vz = From->vz;
            Res->w = From->w;
            return MyAcos(cosom);
        }
        Res->vx = To->vx;
        Res->vy = To->vy;
        Res->vz = To->vz;
        Res->w = To->w;
        if (T == 0x1000)
            return MyAcos(cosom);
        return 0;
    }
    if (cosom > 0x80) {
        omega = MyAcos(cosom);
        sinom = rcossin_tbl[omega][0];
        scale0 = (rcossin_tbl[((0x1000 - T) * omega >> 12) & 0xfff][0] << 12) / sinom;
        scale1 = (rcossin_tbl[(T * omega >> 12) & 0xfff][0] << 12) / sinom;
    } else {
        scale0 = 0x1000 - T;
        scale1 = T;
        omega = scale0;
    }
    Res->vx = (scale0 * From->vx >> 12) + (scale1 * to1[0] >> 12);
    Res->vy = (scale0 * From->vy >> 12) + (scale1 * to1[1] >> 12);
    Res->vz = (scale0 * From->vz >> 12) + (scale1 * to1[2] >> 12);
    Res->w = (scale0 * From->w >> 12) + (scale1 * to1[3] >> 12);
    return omega;
}
