#include "types.h"

extern int ChrisCount;
extern int NewVcounts;
extern int MaxWindows;
extern struct WINDOW GameWindows[];
extern struct WINDOW *CurrWin;
extern struct POLY_FT4 *NextPolyFt4;
extern struct POLY_FT4 *LastPolyFt4;
extern short tpscreenx[];
extern short tpscreeny[];
extern short palette[];
extern short rcossin_tbl[][2];
int rand(void);
struct SVECT GetSubModXYZ(struct GLOBMODHDR *GlobHdr, struct MODSUBOFF SubOff, int AnimFrame, int AnimJump);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *o2);
int AreaValid(int Win, int Area);
void SetupScreenGivenArea(int Win, int Area);
void SetGeomScreen(long h);
unsigned short GetTPage(int tp, int abr, int x, int y);
void AddPrim_(void *ot, void *p);
void AddPrimInsert_(void *ot, void *p);

/* libgpu primitive header */
struct P_TAG {
    unsigned addr : 24;
    unsigned len : 8;
    unsigned char r0, g0, b0, code;
};

/* GTE macros (inline_c.h style) */
#define gte_mvr(reg, r0) __asm__ volatile("move $" #reg ",%0" : : "r"(r0) : "$" #reg, "memory")
#define gte_op(s) __asm__ volatile(s)
#define gte_st(s) __asm__ volatile(s : : : "memory")
#define gte_SetRotMatrix(r0) __asm__ volatile("move $12,%0\nlw $13,0($12)\nlw $14,4($12)\nctc2 $13,$0\nctc2 $14,$1\nlw $13,8($12)\nlw $14,12($12)\nlw $15,16($12)\nctc2 $13,$2\nctc2 $14,$3\nctc2 $15,$4" : : "r"(r0) : "$12", "$13", "$14", "$15")
#define gte_ldv0_s(r0) { gte_mvr(12, r0); gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); }
#define gte_ldv3c_s(r0) { gte_mvr(12, r0); \
    gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); gte_op("lwc2 $2,8($12)"); \
    gte_op("lwc2 $3,12($12)"); gte_op("lwc2 $4,16($12)"); gte_op("lwc2 $5,20($12)"); }
#define gte_rtv0_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A486012"); }
#define gte_rtpt_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A280030"); }
#define gte_rtps_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A180001"); }
#define gte_stlvnl_s(r0) { gte_mvr(12, r0); \
    gte_st("swc2 $25,0($12)"); gte_st("swc2 $26,4($12)"); gte_st("swc2 $27,8($12)"); }
#define gte_mac2tr_s() __asm__ volatile("mfc2 $12,$25\nmfc2 $13,$26\nmfc2 $14,$27\nctc2 $12,$5\nctc2 $13,$6\nctc2 $14,$7" : : : "$12", "$13", "$14")
#define gte_stsxy3_s(r0, r1, r2) { gte_mvr(12, r0); gte_mvr(13, r1); gte_mvr(14, r2); \
    gte_st("swc2 $12,0($12)"); gte_st("swc2 $13,0($13)"); gte_st("swc2 $14,0($14)"); }
#define gte_stsxy_s(r0) { gte_mvr(12, r0); gte_st("swc2 $14,0($12)"); }
#define gte_stsz_s(r0) { gte_mvr(12, r0); gte_st("swc2 $19,0($12)"); }

void MuzzleFlash(struct CREATURE *Cre, struct MODSUBOFF ModSubOff, int Scale)
{
    struct SVECT Rel;
    struct SVECT Temp[4];
    struct SVECTOR RelList[4];
    struct VECT Pos;
    struct VECT Tran;
    int a;
    int Cos;
    int Sin;
    int Win;
    int Angle;
    struct WINDOW *CWin;
    static int MuzzleAngle;
    static int LastFrame;
    struct POWER Power;
    struct MODWIN *WinData;
    int SortZVal;

    SortZVal = 0;
    if (ChrisCount != LastFrame) {
        LastFrame = ChrisCount;
        MuzzleAngle += NewVcounts * 2;
    }
    if (ChrisCount & 1)
        Angle = MuzzleAngle & 0xfff;
    else
        Angle = (MuzzleAngle - 0x80) & 0xfff;
    Rel = GetSubModXYZ(Cre->ModelHdr, ModSubOff, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Pos.vx = Cre->Draw.Location.vx + Rel.vx;
    Pos.vy = Cre->Draw.Location.vy + Rel.vy;
    Pos.vz = Cre->Draw.Location.vz + Rel.vz;
    Power.Full = rand() * 200 / 32768 + 850;
    Power.Fall = rand() * 400 / 32768 + 1200;
    AddLight(&Pos, *&(struct COLOUR){200, 100, 100, 0}, Power, *&(struct SHORTPAIR){0, 0});
    Cos = rcossin_tbl[Angle][1] * Scale / 4096;
    Sin = rcossin_tbl[Angle][0] * Scale / 4096;
    Temp[0].vx = Rel.vx - Cos;
    Temp[0].vy = Rel.vy - Sin;
    Temp[0].vz = Rel.vz;
    Temp[1].vx = Rel.vx + Sin;
    Temp[1].vy = Rel.vy - Cos;
    Temp[1].vz = Rel.vz;
    Temp[2].vx = Rel.vx - Sin;
    Temp[2].vy = Rel.vy + Cos;
    Temp[2].vz = Rel.vz;
    Temp[3].vx = Rel.vx + Cos;
    Temp[3].vy = Rel.vy + Sin;
    Temp[3].vz = Rel.vz;
    for (a = 0; a < 4; a++)
        RotateS(&Cre->Matrix, &Temp[a], (struct SVECT *)&RelList[a]);
    WinData = Cre->WinData;
    for (Win = 0; Win < MaxWindows; Win++, WinData++) {
        if (AreaValid(Win, Cre->Box.Area) && NextPolyFt4 < LastPolyFt4) {
            CurrWin = CWin = &GameWindows[Win];
            SetupScreenGivenArea(Win, Cre->Box.Area);
            SetGeomScreen(CWin->Focal);
            gte_SetRotMatrix(&CWin->CamMat);
            Rel.vx = Cre->Draw.Location.vx - CWin->CamPos.vx;
            Rel.vy = Cre->Draw.Location.vy - CWin->CamPos.vy;
            Rel.vz = Cre->Draw.Location.vz - CWin->CamPos.vz;
            gte_ldv0_s(&Rel);
            gte_rtv0_s();
            gte_stlvnl_s(&Tran);
            if (Tran.vz > 256) {
                gte_mac2tr_s();
                ((struct P_TAG *)NextPolyFt4)->len = 9;
                ((struct P_TAG *)NextPolyFt4)->code = 0x2c;
                ((struct P_TAG *)NextPolyFt4)->code |= 2;
                NextPolyFt4->r0 = 0xff;
                NextPolyFt4->g0 = 0xff;
                NextPolyFt4->b0 = 0xff;
                NextPolyFt4->u0 = 0x40;
                NextPolyFt4->v0 = 0xd0;
                NextPolyFt4->u1 = 0x5f;
                NextPolyFt4->v1 = 0xd0;
                NextPolyFt4->u2 = 0x40;
                NextPolyFt4->v2 = 0xef;
                NextPolyFt4->u3 = 0x5f;
                NextPolyFt4->v3 = 0xef;
                NextPolyFt4->tpage = (GetTPage(0, 0, tpscreenx[15], tpscreeny[15]) & 0x1f) | 0x20;
                NextPolyFt4->clut = palette[64];
                gte_ldv3c_s(RelList);
                gte_rtpt_s();
                gte_stsxy3_s(&NextPolyFt4->x0, &NextPolyFt4->x1, &NextPolyFt4->x2);
                gte_stsz_s(&SortZVal);
                gte_ldv0_s(&RelList[3]);
                gte_rtps_s();
                gte_stsxy_s(&NextPolyFt4->x3);
                if (WinData->Sort.ScreenZ < SortZVal)
                    AddPrim_(&WinData->Sort.FirstLast[0], NextPolyFt4++);
                else
                    AddPrimInsert_(&WinData->Sort.FirstLast[1], NextPolyFt4++);
            }
        }
    }
}
