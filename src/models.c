#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void LinkCastLists(void);
void UnLinkCastLists(void);
int GetSceneNum(struct IDENTITYNAME Name);
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
void InitCreFreeList(void);
int GetFloorOffset(struct CREATURE *Cre);
void InitialiseModelFile(struct GLOBMODHDR *Hdr);
void FreeModelPackets(struct CREATURE *Cre);
void KillModel(struct CREATURE *Cre);
char *PrintIdentityName(struct IDENTITYNAME Name);
void PrintAnimData(struct GLOBMODHDR *Hdr);
struct ANIMHDR *FindAnimHdr(struct GLOBMODHDR *Hdr, struct IDENTITYNAME Name);
void AlterSubModAng(struct GLOBMODHDR *GlobHdr, short FrameNum, short SubModelNum, short AngX, short AngY, short AngZ);
void FreeAllModelPolys(void);
struct MODSUBOFF GetModSubOffFromVertex(struct GLOBMODHDR *Hdr, short VertexNum);
int GetNumberVertices(struct GLOBMODHDR *Hdr);
void FreePolyPackets(short *Source);
void ReallyFreePolyPackets(void);
void ForEachPolyInModelParams(struct CREATURE *Cre, void *VoidRoutine, int Win, int ParamA, int ParamB);
void ForEachPolyInModel(struct CREATURE *Cre, void *VoidRoutine, int Win);
void ForEachPolyInModelHdr(struct GLOBMODHDR *Hdr, void *VoidRoutine, int a, int b, int c);
int FindAnimSequenceNumFromFrame(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int FrameNum);
int LastFrameFromSequenceNum(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int SequenceNum);
void *FindIdentNameInList(struct IDENTITYNAME *Name, void *VPntr, int NumBytes);
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
struct SHORTPAIR GetCreHitSize(struct CREATURE *Cre);
int DrawAnimGlobal(struct CREATURE *Cre, struct MODWIN *WinData, int Win);
void PolyOpaqueSetup(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD);
void PolyOpaqueSetupSecondary(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD);
void ResetPolyTpageCode(struct GLOBMODHDR *Hdr, struct GLOBPOL *GlobPol, struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB);
int GetCeilingOffset(struct CREATURE *Cre);
int PolysAvailable(int NumPolys);
void MallocPolyPackets(short *Dest, int Count);
void ProcessPostDraw(void);

extern short rcossin_tbl[][2];

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

extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
extern struct CREATURE **CurrFreeCre;
extern struct GLOBMODHDR **CastListLinkAddr;
extern struct GLOBMODHDR *CastList;

extern unsigned char DbFrame[];
extern struct CREATURE *MissColList[2][32];
extern struct CREATURE *CreColList[2][32];
/* QLevIfce.obj per SYM, but $gp-relative here, so (tentatively) defined in this file too */
struct CREATURE **CreColListMaxA;
struct CREATURE **MissColListMaxA;
struct CREATURE **CreColListStartA;
struct CREATURE **MissColListStartA;
struct CREATURE **CreColListStartB;
struct CREATURE **MissColListStartB;
struct CREATURE **MissColListStopA;
struct CREATURE **CreColListStopA;
struct CREATURE **MissColListStopB;
struct CREATURE **CreColListStopB;


extern struct MAPNAME *MapNames;

short FailedGetCre;
short SucceedGetCre;
short CrIdent;
void KillModel(struct CREATURE *Cre);
void *memset();

/* GetNextCre's loop bound &Creatures[48] is CreFreeList in splat's naming (cmpfunc compares the
   address); naming CreFreeList in C makes gcc CSE it with the count check. */
static int KillExpendableCre(void)
{
    struct CREATURE *Cre;

    for (Cre = Creatures; Cre < &Creatures[48]; Cre++) {
        if (Cre->Life && (Cre->Control & 0x1000000)) {
            KillModel(Cre);
            return 1;
        }
    }
    return 0;
}


extern void SoundStopOnCreDeath(int, int);
extern void GameFreeCreatureStuff(struct CREATURE *Cre);
void FreeCrePolyPackets(struct CREATURE *Cre);

extern struct WINDOW *CurrWin;

extern short TotalModelsLoaded;
extern int MaxWindows;
static short ShortTerminator[1];
extern void Relocate(void *Pntr, void *Base);
void BodgeAnimData(struct GLOBMODHDR *Hdr);
void FreePolyPackets(short *Source);

extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
extern struct AREA AreaData[];
extern struct SORT HeavenlySortLink;
extern short palette[];
extern short alltpages[];
extern unsigned short tptranstable[];
extern unsigned char semitranstable[];
int DrawAnimGlobalSetup(struct CREATURE *Cre, struct MODWIN *WinData, int *Params, int Win);
extern void ModelGlobal_(int *Params, void *Scratch);
extern void OrderedSort(struct SORT *Self, struct SORT **SortLink);

char *PolsToFreeListPntr;
char PolsToFreeList[256];
extern struct CREATURE *HeavenlyDrawList[];
extern struct CREATURE **HeavenlyDrawListPntr;
extern short *FreeListEnd;
extern short *FreeListPntr;

struct CREATURE *LastCre;
struct MODWIN *LastWinData;
extern struct WINDOW GameWindows[];
int AreaValid(int Win, int Area);
void dprintf(struct WINDOW *Window, char *_format, ...);
void ForEachPolyInModelOpaque(struct CREATURE *Cre, void *VoidRoutine, int Win);
void ForEachPolyInModel(struct CREATURE *Cre, void *VoidRoutine, int Win);
void FilterInOpaquePolys(struct MODWIN *WinData, int Opaque);
void PolyOpaqueSetup(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD);
void PolyOpaqueSetupSecondary(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD);
void ResetPolyTpageCode(struct GLOBMODHDR *Hdr, struct GLOBPOL *GlobPol, struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB);
void DrawCreShadow(struct CREATURE *Cre, struct MODWIN *WinData);
void NewShieldEffect(struct CREATURE *Cre);
int InitModelGlobal(struct GLOBMODHDR *Hdr, short *Pols);
int ClipGlobalModel(struct CREATURE *Cre, struct MODWIN *WinData, int Win);
int DrawAnimGlobal(struct CREATURE *Cre, struct MODWIN *WinData, int Win);

int InitModelGlobal(struct GLOBMODHDR *Hdr, short *Pols);
int ClipGlobalModel(struct CREATURE *Cre, struct MODWIN *WinData, int Win);
int DrawAnimGlobal(struct CREATURE *Cre, struct MODWIN *WinData, int Win);

#define FT4(p) ((struct POLY_FT4 *)(p))
#define UVS(x) (*(unsigned short *)&(x))

extern struct DB *currdb;
extern struct DB *olddb;

extern struct DB *currdb;
extern struct DB *olddb;

extern struct DB *currdb;
void FilterInOpaquePolys_(struct POLY_GT4 **FirstLast, struct POLY_GT4 *Last, struct POLY_GT4 *ShadowTable[2][16], int Colour);

extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
extern struct CREATURE **CurrFreeCre;
extern short *FreeListEnd;
extern short *FreeListPntr;
struct GLOBMODHDR **CastListLinkAddr;
struct GLOBMODHDR *CastList;
struct GLOBMODHDR *CommonCastList;
struct GLOBMODHDR *ZoneCastList;

extern void Relocate(void *Pntr, void *Base);

struct SVECT GetSubModXYZ(struct GLOBMODHDR *GlobHdr, struct MODSUBOFF SubOff, short AnimFrame, short AnimJump);
void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *o2);

int CurrWinSizeX = 0;
extern struct VECT *CurrCamPos;
extern struct MATRIX *CurrCamMat;
extern struct SCRPNT CurrScreenMin;
extern struct SCRPNT CurrScreenSize;
extern struct SCRPNT CurrScreenHalfSize;
extern int ChrisCount;
void *memset();
int ChooseSortArea(int Win, int Area, int AltArea);
void SetupScreenGivenArea(int Win, int AreaIndx);
void CalcSortList(struct CREATURE *Cre, struct VECT *In, short *Seed, short *Offset);

/* inline_c.h */
#define gte_SetTransMatrix(r0) __asm__ volatile("move $12,%0\nlw $13,20($12)\nlw $14,24($12)\nctc2 $13,$5\nlw $15,28($12)\nctc2 $14,$6\nctc2 $15,$7" : : "r"(r0) : "$12", "$13", "$14", "$15")
#define gte_ldv0(r0) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)"); }
#define gte_rtps() __asm__ volatile("nop\nnop\n.word 0x4A180001")
#define gte_stsxy(r0) __asm__ volatile("move $12,%0\nswc2 $14,0($12)" : : "r"(r0) : "$12", "memory")
#define gte_stsz(r0) __asm__ volatile("move $12,%0\nswc2 $19,0($12)" : : "r"(r0) : "$12", "memory")

void CalcVectorL(struct MATRIX *m, struct VECT *v1, struct VECT *v2);

static int FindSortRanges(struct GLOBMODHDR *lpModelHeader, int iSortDir, int iFrame, struct RANGESHDR **lpRangesHdrOut)
{
    struct EXTRASORTDIRTABLE *lpExtraSortDirTable;

    lpExtraSortDirTable = (struct EXTRASORTDIRTABLE *)((char *)lpModelHeader + (int)lpModelHeader->ExtraSortData);
    if (lpExtraSortDirTable->wOffset[iSortDir]) {
        int i;
        struct RANGESHDR *lpRangesHdr;

        lpRangesHdr = (struct RANGESHDR *)((char *)lpExtraSortDirTable + lpExtraSortDirTable->wOffset[iSortDir]);
        while (1) {
            struct RANGE *lpRange;

            lpRange = (struct RANGE *)(lpRangesHdr + 1);
            for (i = 0; i < lpRangesHdr->wRanges; i++, lpRange++) {
                if (iFrame >= lpRange->wStart && iFrame <= lpRange->wStop) {
                    *lpRangesHdrOut = lpRangesHdr;
                    return 1;
                }
            }
            if (!lpRangesHdr->wOffset)
                break;
            lpRangesHdr = (struct RANGESHDR *)((char *)lpExtraSortDirTable + lpRangesHdr->wOffset);
        }

        return 0;
    }
    return 0;
}

extern unsigned short *GamePals;
extern short NumMapPals;
extern struct DB db[];

int NumLights = 0;
short lightorder[4];
struct BESTLIGHT bestlights[4];
int CalcLightBright(struct LIGHTS *Light, struct SVECT *Rel, short OverBright);
int abs(int);

extern struct LIGHTS LightList[];
extern struct LIGHTS *LightListCurrent;
extern struct LIGHTS HeavenlyLightList[];
extern struct LIGHTS *HeavenlyLightListCurrent;
extern struct LIGHTS tempgloblight;
extern struct COLLHEADER MoveCollMap;
extern short *FixedLightList;
extern struct LIGHTS *FixedLightData;
extern struct MATRIX rgbmatrix;
void CalcLightModel(struct CREATURE *Cre, struct LIGHTS *Light);
void VectorNormalSS(struct SVECTOR *v0, struct SVECTOR *v1);
void ScaleSVect(struct SVECTOR *Vec, int Scale);
/* SetBackColor (libgte) is called without its prototype in scope: implicit int. Its int
   return value matters to CreLights: with a void prototype its last argument is computed in
   $v0 instead of $v1. */
int SetBackColor();

static struct QUATANIMFRAME GlobalTweenedQuatFrames[32];
void PackedEulerToCamQuat(struct PACKEDEULER Ang, struct QUAT *Res);
int QuatSlerp(struct QUAT *From, struct QUAT *To, int T, struct QUAT *Res);
void dprintf(struct WINDOW *Window, char *_format, ...);
extern struct WINDOW GameWindows[];

static inline void TweenQuatFrame(struct QUATANIMFRAME *BaseFrame, struct ANIMFRAME *KeyFrameA, struct ANIMFRAME *KeyFrameB, int ToDo, int Count)
{
    int Scale;
    int Scale2;

    if (ToDo == Count) {
        PackedEulerToCamQuat(KeyFrameA[1].Ang, &BaseFrame->Orient);
        BaseFrame->Tran.vx = KeyFrameA[1].Tran.x << 1;
        BaseFrame->Tran.vy = KeyFrameA[1].Tran.y << 2;
        BaseFrame->Tran.vz = KeyFrameA[1].Tran.z << 1;
    } else if (ToDo <= Count) {
        struct QUAT Orient1;
        struct QUAT Orient2;

        Scale = ((ToDo << 12) + (Count >> 1)) / Count;
        if (Scale < 0)
            dprintf(GameWindows, "ToDo>Count or Scale<0!");
        PackedEulerToCamQuat(KeyFrameA->Ang, &Orient1);
        PackedEulerToCamQuat(KeyFrameB->Ang, &Orient2);
        QuatSlerp(&Orient1, &Orient2, Scale, &BaseFrame->Orient);
        Scale2 = KeyFrameA->Tran.x << 1;
        BaseFrame->Tran.vx = ((((KeyFrameB->Tran.x << 1) - Scale2) * Scale) >> 12) + Scale2;
        Scale2 = KeyFrameA->Tran.y << 2;
        BaseFrame->Tran.vy = ((((KeyFrameB->Tran.y << 2) - Scale2) * Scale) >> 12) + Scale2;
        Scale2 = KeyFrameA->Tran.z << 1;
        BaseFrame->Tran.vz = ((((KeyFrameB->Tran.z << 1) - Scale2) * Scale) >> 12) + Scale2;
    }
}

int PrintOutStuff;

static inline int TweenLerp(int A, int B, int Scale)
{
    return (((B - A) * Scale) >> 12) + A;
}

static inline void TweenQuatFrameScaled(struct QUATANIMFRAME *BaseFrame, struct ANIMFRAME *KeyFrameA, struct ANIMFRAME *KeyFrameB, int ToDo, int Count)
{
    {
        struct QUAT Orient1;
        struct QUAT Orient2;
        int Scale;

        Scale = ((ToDo << 12) + (Count >> 1)) / Count;
        if (ToDo > Count || Scale < 0)
            dprintf(GameWindows, "ToDo>Count or Scale<0!");
        PackedEulerToCamQuat(KeyFrameA->Ang, &Orient1);
        PackedEulerToCamQuat(KeyFrameB->Ang, &Orient2);
        QuatSlerp(&Orient1, &Orient2, Scale, &BaseFrame->Orient);
        BaseFrame->Tran.vx = TweenLerp(KeyFrameA->Tran.x << 1, KeyFrameB->Tran.x << 1, Scale);
        BaseFrame->Tran.vy = TweenLerp(KeyFrameA->Tran.y << 2, KeyFrameB->Tran.y << 2, Scale);
        BaseFrame->Tran.vz = TweenLerp(KeyFrameA->Tran.z << 1, KeyFrameB->Tran.z << 1, Scale);
    }
}

void QuatToMat(struct QUAT *Quat, struct MATRIX *Mat);
void MYRotMatrixANIM(struct SVECT *ang, struct MATRIX *m);

struct VECTOR *ObjTranVect;
extern struct DUMMYGPU *LastDummyGpu;
extern struct DUMMYGPU *NextDummyGpu;
extern long *primlistarray[];
void GetAnimFramesForAnim(struct ANIMFRAMEHDRHDR *FrameHdrHdr, struct ANIMFRAMEHDRHDR *NextKeyFrameHdrHdr, int ToDo300FPSUnClipped, int NumSubMods);
void MulMatrix0(struct MATRIX *m0, struct MATRIX *m1, struct MATRIX *m2);
void SetColorMatrix(struct MATRIX *m);
void SetRotMatrix(struct MATRIX *m);

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

void FreeModelPackets(struct CREATURE *Cre)
{
    int Win;

    for (Win = 0; Win < MaxWindows; Win++) {
        if (Cre->WinData[Win].GotPackets) {
            Cre->WinData[Win].GotPackets = 0;
            FreePolyPackets(Cre->WinData[Win].Pols);
        }
    }
}

int GetFloorOffset(struct CREATURE *Cre)
{
    if (Cre->ModelHdr)
        return Cre->ModelHdr->FloorOffset;
    return 0;
}

int GetCeilingOffset(struct CREATURE *Cre)
{
    if (Cre->ModelHdr)
        return Cre->ModelHdr->CeilingOffset;
    return 0;
}

void CalcSortList(struct CREATURE *Cre, struct VECT *In, short *Seed, short *Offset)
{
    struct VECT Out;
    int Sort;
    struct MATRIX Matrix;
    struct MATRIX StaticMatrix;
    struct RANGESHDR *lpRangesHdr;
    struct RANGE *lpRange;

    CalcVectorL(&Cre->Matrix, In, &Out);
    Sort = Out.vx > 0;
    if (Out.vy > 0)
        Sort += 2;
    if (Out.vz > 0)
        Sort += 4;
    if (FindSortRanges(Cre->ModelHdr, Sort, Cre->AnimFrame, &lpRangesHdr)) {
        *Seed = lpRangesHdr->wSeed;
        lpRange = (struct RANGE *)(lpRangesHdr + 1);
        lpRange += lpRangesHdr->wRanges;
        *Offset = (int)lpRange - (int)Cre->ModelHdr->SortData;
    } else {
        *Seed = Cre->ModelHdr->SortData[Sort].Seed;
        *Offset = Cre->ModelHdr->SortData[Sort].Offset;
    }
}

int PolysAvailable(int NumPolys)
{
    return (FreeListEnd - FreeListPntr) * 32 >= NumPolys;
}

void MallocPolyPackets(short *Dest, int Count)
{
    Count += 31;
    Count /= 32;
    *Dest++ = Count;
    for (; Count > 0; Count--)
        *Dest++ = *FreeListPntr++;
}

void FreePolyPackets(short *Source)
{
    int Count;

    Count = *Source;
    *Source++ = 0;
    for (; Count > 0; Count--)
        *PolsToFreeListPntr++ = *Source++ / 32;
}

void ReallyFreePolyPackets(void)
{
    char *Data;

    for (Data = PolsToFreeList; Data < PolsToFreeListPntr; Data++)
        *--FreeListPntr = *Data * 32;
    PolsToFreeListPntr = PolsToFreeList;
}

void FreeCrePolyPackets(struct CREATURE *Cre)
{
    int Win;

    for (Win = 0; Win < MaxWindows; Win++) {
        if (Cre->WinData[Win].GotPackets) {
            FreePolyPackets(Cre->WinData[Win].Pols);
            Cre->WinData[Win].GotPackets = 0;
        }
        if (Cre->WinData[Win].GotOpaquePackets) {
            FreePolyPackets(Cre->WinData[Win].OpaquePols);
            Cre->WinData[Win].GotOpaquePackets = 0;
        }
    }
}

void FreeAllModelPolys(void)
{
    int a;
    struct CREATURE *Cre;

    Cre = Creatures;
    for (a = 0; a < 48; a++, Cre++)
        if (Cre->Life)
            FreeCrePolyPackets(Cre);
    CurrWin->ModOverLoad = 10;
}

int InitModelGlobal(struct GLOBMODHDR *Hdr, short *Pols)
{
    short *fp;
    struct POLY_GT4 *pol1g4;
    struct POLY_GT4 *pol2g4;
    int a;
    int f;
    struct GLOBPOL *GlobPoly;

    if (!PolysAvailable(Hdr->TotalPolys))
        return 0;
    fp = Pols + 1;
    MallocPolyPackets(Pols, Hdr->TotalPolys);
    GlobPoly = Hdr->GlobPolys;
    for (a = Hdr->TotalPolys, f = 0; a > 0; pol1g4++, pol2g4++, a--, GlobPoly++, f++) {
        if (!(f & 31)) {
            pol1g4 = &db[0].modpolsg4[*fp];
            pol2g4 = &db[1].modpolsg4[*fp];
            fp++;
        }
        UVS(pol1g4->u0) = UVS(GlobPoly->Uv[0]);
        UVS(pol1g4->u1) = UVS(GlobPoly->Uv[1]);
        UVS(pol1g4->u2) = UVS(GlobPoly->Uv[3]);
        UVS(pol1g4->u3) = UVS(GlobPoly->Uv[2]);
        pol1g4->clut = GamePals[GlobPoly->Clut + NumMapPals];
        pol1g4->tpage = alltpages[GlobPoly->Tp & 0x1f] | tptranstable[GlobPoly->Tp >> 5];
        pol1g4->code = semitranstable[GlobPoly->Tp >> 5] | 0x3c;
        pol1g4->tag = 0xc000000;

        *pol2g4 = *pol1g4;
    }
    return 1;
}

void ProcessPostDraw(void)
{
    struct CREATURE *Cre;

    for (Cre = Creatures; Cre < (struct CREATURE *)CreFreeList; Cre++)
        if (Cre->Life && Cre->PostDraw)
            Cre->PostDraw(Cre);
}

void MoveModels(void)
{
    struct CREATURE *Cre;

    MissColListStartB = MissColListStartA;
    MissColListStopB = MissColListStopA;
    MissColListStartA = MissColList[DbFrame[0]];
    MissColListStopA = MissColList[DbFrame[0]];
    MissColListMaxA = &MissColList[DbFrame[0]][32];
    CreColListStartB = CreColListStartA;
    CreColListStopB = CreColListStopA;
    CreColListStartA = CreColList[DbFrame[0]];
    CreColListStopA = CreColList[DbFrame[0]];
    CreColListMaxA = &CreColList[DbFrame[0]][32];
    for (Cre = Creatures; Cre < (struct CREATURE *)CreFreeList; Cre++) {
        if (Cre->Life) {
            Cre->ModPre = 0;
            if (Cre->Primary)
                Cre->Primary(Cre);
        }
    }
}

void FilterInOpaquePolys(struct MODWIN *WinData, int Opaque)
{
    struct POLY_GT4 *ShadowTable[2][16];
    int Green;
    int Blue;
    int Colour;
    int a;

    Green = Opaque * 129 / 4096;
    Blue = Opaque * 133 / 4096;
    /* multiplies, not shifts: the extra pre-optimisation insns keep this function out of the
       inline candidates, so it is emitted here (not at the end) and not inlined into DrawModels */
    Colour = 0x2e000000 + Blue * 0x10000 + Green * 0x100 + Blue;
    for (a = 0; a < WinData->Pols[0]; a++) {
        ShadowTable[0][a] = currdb->modpolsg4 + WinData->Pols[a + 1];
        ShadowTable[1][a] = currdb->modpolsg4 + WinData->OpaquePols[a + 1];
    }
    FilterInOpaquePolys_(WinData->Sort.FirstLast, WinData->Sort.FirstLast[1], ShadowTable, Colour);
}

void DrawModels(int Win)
{
    struct CREATURE *Cre;
    struct MODWIN *WinData;

    for (Cre = Creatures; Cre < &Creatures[48]; Cre++) {
        if (Cre->Life > 0 && Cre->ModelHdr) {
            WinData = &Cre->WinData[Win];
            LastCre = Cre;
            LastWinData = WinData;
            if (!WinData->NoDraw && Cre->ModelOpaque && AreaValid(Win, Cre->Box.Area) && !ClipGlobalModel(Cre, WinData, Win)) {
                if (!WinData->GotPackets) {
                    if (CurrWin->ModOverLoad && InitModelGlobal(Cre->ModelHdr, WinData->Pols)) {
                        Cre->Control |= 0x100 << Win;
                        Cre->WinData[Win].GotPackets = 3;
                        CurrWin->ModOverLoad--;
                    }
                }
                if (WinData->GotPackets) {
                    if (DrawAnimGlobal(Cre, WinData, Win)) {
                        Cre->WinData[Win].GotPackets = 3;
                        if (Cre->ModelOpaque != 0x1000) {
                            if (!WinData->GotOpaquePackets) {
                                if (PolysAvailable(Cre->ModelHdr->TotalPolys)) {
                                    MallocPolyPackets(WinData->OpaquePols, Cre->ModelHdr->TotalPolys);
                                    WinData->GotOpaquePackets = 1;
                                    ForEachPolyInModelOpaque(Cre, PolyOpaqueSetup, Win);
                                } else {
                                    dprintf(&GameWindows[Win], "Opaque Poly Error\n");
                                }
                            } else if (WinData->GotOpaquePackets == 1) {
                                WinData->GotOpaquePackets = 2;
                                ForEachPolyInModelOpaque(Cre, PolyOpaqueSetupSecondary, Win);
                            }
                            if (WinData->GotOpaquePackets)
                                FilterInOpaquePolys(WinData, Cre->ModelOpaque);
                        } else if (WinData->GotOpaquePackets) {
                            FreePolyPackets(WinData->OpaquePols);
                            WinData->GotOpaquePackets = 0;
                            ForEachPolyInModel(Cre, ResetPolyTpageCode, Win);
                        }
                        if (Cre->Control & 1)
                            DrawCreShadow(Cre, WinData);
                        if ((Cre->Control & 0x4000000) && Cre->ShieldTimer)
                            NewShieldEffect(Cre);
                    }
                }
            }
            if (WinData->GotPackets) {
                if (--WinData->GotPackets == 0) {
                    FreePolyPackets(WinData->Pols);
                    FreePolyPackets(WinData->OpaquePols);
                    WinData->GotOpaquePackets = 0;
                }
            }
        }
    }
    CurrWin->ModOverLoad = 10;
}

void DrawHeavenlyModels(void)
{
    struct CREATURE *Cre;
    struct CREATURE **Heavenly;
    struct MODWIN *WinData;

    for (Heavenly = HeavenlyDrawList; Heavenly < HeavenlyDrawListPntr; Heavenly++) {
        Cre = *Heavenly;
        if (Cre->Life && Cre->ModelHdr) {
            WinData = &Cre->WinData[0];
            if (!WinData->NoDraw && !ClipGlobalModel(Cre, WinData, 0)) {
                if (!WinData->GotPackets) {
                    if (InitModelGlobal(Cre->ModelHdr, Cre->WinData[0].Pols)) {
                        Cre->Control |= 0x100;
                        WinData->GotPackets = 3;
                        CurrWin->ModOverLoad--;
                    }
                }
                if (WinData->GotPackets) {
                    if (DrawAnimGlobal(Cre, WinData, 0))
                        WinData->GotPackets = 3;
                }
            }
            if (WinData->GotPackets) {
                if (--WinData->GotPackets == 0)
                    FreePolyPackets(WinData->Pols);
            }
        }
    }
}

void CalcLightModel(struct CREATURE *Cre, struct LIGHTS *Light)
{
    int b;
    int Bright;
    struct BESTLIGHT *BestLight;

    BestLight = &bestlights[lightorder[3]];
    BestLight->dir.vx = Light->Location.vx - Cre->Draw.Location.vx;
    BestLight->dir.vy = Light->Location.vy - Cre->Draw.Location.vy;
    BestLight->dir.vz = Light->Location.vz - Cre->Draw.Location.vz;
    if (abs(BestLight->dir.vx) < Light->Radius && abs(BestLight->dir.vz) < Light->Radius
        && abs(BestLight->dir.vy) < Light->Radius) {
        Bright = CalcLightBright(Light, &BestLight->dir, 64);
        if (Bright) {
            if (Bright > 0x1000)
                Bright = 0x1000;
            BestLight->r = Light->Colour.r * Bright / 256;
            BestLight->g = Light->Colour.g * Bright / 256;
            BestLight->b = Light->Colour.b * Bright / 256;
            if (BestLight->r > 0xfff)
                BestLight->r = 0x1000;
            if (BestLight->g > 0xfff)
                BestLight->g = 0x1000;
            if (BestLight->b > 0xfff)
                BestLight->b = 0x1000;
            BestLight->bright = BestLight->r + BestLight->g + BestLight->b;
            if (bestlights[lightorder[0]].bright < BestLight->bright) {
                b = lightorder[3];
                lightorder[3] = lightorder[2];
                lightorder[2] = lightorder[1];
                lightorder[1] = lightorder[0];
                lightorder[0] = b;
                NumLights++;
            } else if (bestlights[lightorder[1]].bright < BestLight->bright) {
                b = lightorder[3];
                lightorder[3] = lightorder[2];
                lightorder[2] = lightorder[1];
                lightorder[1] = b;
                NumLights++;
            } else if (bestlights[lightorder[2]].bright < BestLight->bright) {
                b = lightorder[3];
                lightorder[3] = lightorder[2];
                lightorder[2] = b;
                NumLights++;
            }
        }
    }
}

void CreLights(struct CREATURE *Cre, struct SVECTOR *LightVec)
{
    struct LIGHTS *Lighting;
    short *fp;
    short *LightStop;

    lightorder[1] = 1;
    lightorder[2] = 2;
    lightorder[3] = 3;
    bestlights[0].bright = 0;
    lightorder[0] = 0;
    bestlights[1].bright = 0;
    bestlights[2].bright = 0;
    NumLights = 0;
    if (Cre->Life < 0) {
        for (Lighting = HeavenlyLightList; Lighting < HeavenlyLightListCurrent; Lighting++)
            CalcLightModel(Cre, Lighting);
    } else {
        for (Lighting = LightList; Lighting < LightListCurrent; Lighting++)
            CalcLightModel(Cre, Lighting);
        if (Cre->Box.MoveSpace >= 0) {
            fp = FixedLightList + MoveCollMap.Coll3dSpace[Cre->Box.MoveSpace].StartLight;
            LightStop = FixedLightList + MoveCollMap.Coll3dSpace[Cre->Box.MoveSpace + 1].StartLight;
            for (; fp < LightStop; fp++)
                CalcLightModel(Cre, &FixedLightData[*fp]);
        } else {
            tempgloblight.Location.vx = Cre->Draw.Location.vx + 500;
            tempgloblight.Location.vy = Cre->Draw.Location.vy - 2000;
            tempgloblight.Location.vz = Cre->Draw.Location.vz - 1000;
            CalcLightModel(Cre, &tempgloblight);
        }
    }
    if (NumLights > 3)
        NumLights = 3;
    switch (NumLights) {
    case 3:
        rgbmatrix.m[0][0] = bestlights[lightorder[0]].r;
        rgbmatrix.m[1][0] = bestlights[lightorder[0]].g;
        rgbmatrix.m[2][0] = bestlights[lightorder[0]].b;
        rgbmatrix.m[0][1] = bestlights[lightorder[1]].r;
        rgbmatrix.m[1][1] = bestlights[lightorder[1]].g;
        rgbmatrix.m[2][1] = bestlights[lightorder[1]].b;
        rgbmatrix.m[0][2] = bestlights[lightorder[2]].r;
        rgbmatrix.m[1][2] = bestlights[lightorder[2]].g;
        rgbmatrix.m[2][2] = bestlights[lightorder[2]].b;
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[0]].dir, &LightVec[0]);
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[1]].dir, &LightVec[1]);
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[2]].dir, &LightVec[2]);
        ScaleSVect(&LightVec[0], Cre->ModelOpaque * Cre->ModelBright / 2048);
        ScaleSVect(&LightVec[1], Cre->ModelOpaque * Cre->ModelBright / 2048);
        ScaleSVect(&LightVec[2], Cre->ModelOpaque * Cre->ModelBright / 2048);
        break;
    case 2:
        rgbmatrix.m[0][0] = bestlights[lightorder[0]].r;
        rgbmatrix.m[1][0] = bestlights[lightorder[0]].g;
        rgbmatrix.m[2][0] = bestlights[lightorder[0]].b;
        rgbmatrix.m[0][1] = bestlights[lightorder[1]].r;
        rgbmatrix.m[1][1] = bestlights[lightorder[1]].g;
        rgbmatrix.m[2][1] = bestlights[lightorder[1]].b;
        rgbmatrix.m[0][2] = 0;
        rgbmatrix.m[1][2] = 0;
        rgbmatrix.m[2][2] = 0;
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[0]].dir, &LightVec[0]);
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[1]].dir, &LightVec[1]);
        ScaleSVect(&LightVec[0], Cre->ModelOpaque * Cre->ModelBright / 2048);
        ScaleSVect(&LightVec[1], Cre->ModelOpaque * Cre->ModelBright / 2048);
        break;
    case 1:
        rgbmatrix.m[0][0] = bestlights[lightorder[0]].r;
        rgbmatrix.m[1][0] = bestlights[lightorder[0]].g;
        rgbmatrix.m[2][0] = bestlights[lightorder[0]].b;
        rgbmatrix.m[0][1] = 0;
        rgbmatrix.m[1][1] = 0;
        rgbmatrix.m[2][1] = 0;
        rgbmatrix.m[0][2] = 0;
        rgbmatrix.m[1][2] = 0;
        rgbmatrix.m[2][2] = 0;
        VectorNormalSS((struct SVECTOR *)&bestlights[lightorder[0]].dir, &LightVec[0]);
        ScaleSVect(&LightVec[0], Cre->ModelOpaque * Cre->ModelBright / 2048);
        break;
    }
    {
        int Bright = Cre->ModelBright * Cre->ModelOpaque;
        SetBackColor(Cre->Ambience.r * Bright >> 24, Cre->Ambience.g * Bright >> 24, Cre->Ambience.b * Bright >> 24);
    }
}

int FindAnimSequenceNumFromFrame(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int FrameNum)
{
    int SequenceNum;

    SequenceNum = 0;
    while (FrameNum >= FrameHdrHdr->NumFrames) {
        FrameNum -= FrameHdrHdr->NumFrames;
        Relocate(&FrameHdrHdr, (void *)FrameHdrHdr->Offset);
        SequenceNum++;
    }
    return SequenceNum;
}

int LastFrameFromSequenceNum(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int SequenceNum)
{
    int FrameNum;

    FrameNum = -1;
    while (SequenceNum >= 0) {
        SequenceNum--;
        FrameNum += FrameHdrHdr->NumFrames;
        Relocate(&FrameHdrHdr, (void *)FrameHdrHdr->Offset);
    }
    return FrameNum;
}

void GetAnimFramesForAnim(struct ANIMFRAMEHDRHDR *FrameHdrHdr, struct ANIMFRAMEHDRHDR *NextKeyFrameHdrHdr, int ToDo300FPSUnClipped, int NumSubMods)
{
    struct QUATANIMFRAME *BaseFrame;
    unsigned char *Temp;
    struct ANIMFRAME *AnimFrame;
    struct ANIMFRAME *AnimFrameB;
    struct TWEENHDR *TweenHdr;
    struct TWEENHDR *TweenHdr2;
    int SubIndx;
    int ToDo300FPS;

    BaseFrame = GlobalTweenedQuatFrames;
    TweenHdr = (struct TWEENHDR *)(FrameHdrHdr + 1);
    TweenHdr2 = 0;
    ToDo300FPS = ToDo300FPSUnClipped;
    if (ToDo300FPS > FrameHdrHdr->NumFrames - 10)
        ToDo300FPS = FrameHdrHdr->NumFrames - 10;
    if (!ToDo300FPS) {
        for (SubIndx = 0; SubIndx < NumSubMods; SubIndx++) {
            AnimFrame = (struct ANIMFRAME *)((char *)TweenHdr + TweenHdr->AnimFrameOffset);
            PackedEulerToCamQuat(AnimFrame->Ang, &BaseFrame->Orient);
            BaseFrame->Tran.vx = AnimFrame->Tran.x << 1;
            BaseFrame->Tran.vy = AnimFrame->Tran.y << 2;
            BaseFrame->Tran.vz = AnimFrame->Tran.z << 1;
            BaseFrame++;
            TweenHdr++;
        }
        return;
    }
    {
        int ToDo;
        int ThisToDo;
        int Count;
        int Remainder;
        int Nibbles8;
        unsigned long *CountPntr;
        unsigned long CountData;

        if (NextKeyFrameHdrHdr)
            TweenHdr2 = (struct TWEENHDR *)(NextKeyFrameHdrHdr + 1);
        ToDo = ToDo300FPS / 10;
        Remainder = ToDo300FPS - ToDo * 10;
        for (SubIndx = 0; SubIndx < NumSubMods; SubIndx++, TweenHdr++) {
            PrintOutStuff = 0;
            if (!SubIndx)
                PrintOutStuff = 1;
            Nibbles8 = 0;
            ThisToDo = ToDo;
            Temp = (unsigned char *)TweenHdr + TweenHdr->CountOffset;
            CountData = *(unsigned long *)Temp;
            CountPntr = (unsigned long *)Temp;
            Count = CountData & 0xf;
            CountData >>= 4;
            while (ThisToDo * 10 + Remainder > Count * 10) {
                Nibbles8++;
                ThisToDo -= Count;
                if (!(Nibbles8 & 7))
                    CountData = *++CountPntr;
                Count = CountData & 0xf;
                CountData >>= 4;
            }
            AnimFrame = (struct ANIMFRAME *)((char *)TweenHdr + TweenHdr->AnimFrameOffset);
            AnimFrame += Nibbles8;
            AnimFrameB = AnimFrame + 1;
            Count *= 10;
            ThisToDo = ThisToDo * 10 + Remainder;
            if (TweenHdr2) {
                AnimFrame = AnimFrameB;
                AnimFrameB = (struct ANIMFRAME *)((char *)TweenHdr2 + TweenHdr2->AnimFrameOffset);
                ThisToDo = ToDo300FPSUnClipped - ToDo300FPS;
                Count = 10;
                TweenHdr2++;
            }
            if (ThisToDo == Count) {
                if (!TweenHdr2) {
                    PackedEulerToCamQuat(AnimFrame[1].Ang, &BaseFrame->Orient);
                    BaseFrame->Tran.vx = AnimFrame[1].Tran.x << 1;
                    BaseFrame->Tran.vy = AnimFrame[1].Tran.y << 2;
                    BaseFrame->Tran.vz = AnimFrame[1].Tran.z << 1;
                } else {
                    PackedEulerToCamQuat(AnimFrameB->Ang, &BaseFrame->Orient);
                    BaseFrame->Tran.vx = AnimFrameB->Tran.x << 1;
                    BaseFrame->Tran.vy = AnimFrameB->Tran.y << 2;
                    BaseFrame->Tran.vz = AnimFrameB->Tran.z << 1;
                }
                BaseFrame++;
            } else {
                if (ThisToDo > Count)
                    for (;;)
                        ;
                TweenQuatFrameScaled(BaseFrame++, AnimFrame, AnimFrameB, ThisToDo, Count);
            }
        }
    }
}

int DrawAnimGlobalSetup(struct CREATURE *Cre, struct MODWIN *WinData, int *Params, int Win)
{
    struct GLOBMODHDR *GlobHdr;
    struct ANIMFRAMEHDRHDR *FrameHdrHdrA;
    struct ANIMFRAMEHDRHDR *FrameHdrHdrB;
    struct ANIMFRAMEHDR *FrameHdr;
    struct QUATANIMFRAME *BaseQFrame;
    struct ANIMFRAME *BaseFrame;
    struct MATRIX *MainMat;
    struct MATRIX *CreMat;
    char *Pntr;
    int FrameNumA;
    int FrameNumB;

    FrameNumA = Cre->AnimFrame;
    GlobHdr = Cre->ModelHdr;
    FrameHdrHdrA = GlobHdr->AnimData;
    while (FrameNumA >= FrameHdrHdrA->NumFrames) {
        FrameNumA -= FrameHdrHdrA->NumFrames;
        Relocate(&FrameHdrHdrA, (void *)FrameHdrHdrA->Offset);
    }
    if (!Cre->Draw.AnimJump) {
        FrameHdrHdrB = 0;
    } else {
        FrameNumB = Cre->Draw.TargetAnimFrame;
        FrameHdrHdrB = GlobHdr->AnimData;
        while (FrameNumB >= FrameHdrHdrB->NumFrames) {
            FrameNumB -= FrameHdrHdrB->NumFrames;
            Relocate(&FrameHdrHdrB, (void *)FrameHdrHdrB->Offset);
        }
    }
    if (FrameHdrHdrA->Flags & 1) {
        if (!FrameHdrHdrB)
            GetAnimFramesForAnim(FrameHdrHdrA, 0, FrameNumA, GlobHdr->NumSubModels);
        else if (FrameHdrHdrB->Flags & 1)
            GetAnimFramesForAnim(FrameHdrHdrA, FrameHdrHdrB, FrameNumA, GlobHdr->NumSubModels);
    } else {
        BaseQFrame = GlobalTweenedQuatFrames;
        FrameHdr = (struct ANIMFRAMEHDR *)(FrameHdrHdrA + 1);
        Pntr = (char *)(FrameHdr + FrameNumA / 10);
        BaseFrame = (struct ANIMFRAME *)(Pntr + ((struct ANIMFRAMEHDR *)Pntr)->Offset);
        for (Pntr = (char *)BaseFrame; BaseFrame < (struct ANIMFRAME *)Pntr + GlobHdr->NumSubModels; BaseFrame++) {
            PackedEulerToCamQuat(BaseFrame->Ang, &BaseQFrame->Orient);
            BaseQFrame->Tran.vx = BaseFrame->Tran.x << 1;
            BaseQFrame->Tran.vy = BaseFrame->Tran.y << 2;
            BaseQFrame->Tran.vz = BaseFrame->Tran.z << 1;
            BaseQFrame++;
        }
    }
    MainMat = (struct MATRIX *)&Params[35];
    CreMat = (struct MATRIX *)&Params[24];
    CreMat->m[0][0] = Cre->Matrix.m[0][0];
    CreMat->m[0][1] = Cre->Matrix.m[0][1];
    CreMat->m[0][2] = Cre->Matrix.m[0][2];
    CreMat->m[1][0] = Cre->Matrix.m[1][0];
    CreMat->m[1][1] = Cre->Matrix.m[1][1];
    CreMat->m[1][2] = Cre->Matrix.m[1][2];
    CreMat->m[2][0] = Cre->Matrix.m[2][0];
    CreMat->m[2][1] = Cre->Matrix.m[2][1];
    CreMat->m[2][2] = Cre->Matrix.m[2][2];
    MulMatrix0(CurrCamMat, &Cre->Matrix, MainMat);
    rgbmatrix.m[0][0] = 0; rgbmatrix.m[1][0] = 0; rgbmatrix.m[2][0] = 0;
    rgbmatrix.m[0][1] = 0; rgbmatrix.m[1][1] = 0; rgbmatrix.m[2][1] = 0;
    rgbmatrix.m[0][2] = 0; rgbmatrix.m[1][2] = 0; rgbmatrix.m[2][2] = 0;
    CreLights(Cre, (struct SVECTOR *)&Params[29]);
    SetColorMatrix(&rgbmatrix);
    SetRotMatrix(MainMat);
    Params[4] = (int)GlobHdr->GlobPoints;
    Params[0] = (int)GlobHdr;
    Params[3] = (int)GlobHdr->GlobPolys;
    Params[5] = (int)GlobHdr->SubModels;
    Params[1] = (int)&WinData->Pols[1];
    Params[2] = (int)currdb->modpolsg4;
    Params[13] = (int)((struct POLY_GT4 *)Params[2] + WinData->Pols[1]);
    Params[9] = (int)primlistarray;
    Params[12] = (int)((struct POLY_GT4 *)Params[13] + 32);
    Params[10] = (int)ObjTranVect;
    Params[15] = (int)rcossin_tbl;
    Params[7] = (int)GlobalTweenedQuatFrames;
    Params[8] = (int)(GlobalTweenedQuatFrames + GlobHdr->NumSubModels);
    Params[16] = (WinData->Seed << 16) | WinData->Offset;
    if (NextDummyGpu != LastDummyGpu) {
        Params[17] = (int)&WinData->Sort.FirstLast[0];
        NextDummyGpu->Tag = 0;
        WinData->Sort.VectBox = &Cre->Box.VectBox;
        WinData->Sort.FirstLast[0] = (struct POLY_GT4 *)NextDummyGpu;
        WinData->Sort.FirstLast[1] = (struct POLY_GT4 *)NextDummyGpu;
        NextDummyGpu++;
        return 1;
    }
    return 0;
}

int DrawAnimGlobal(struct CREATURE *Cre, struct MODWIN *WinData, int Win)
{
    int Params[40];

    if (DrawAnimGlobalSetup(Cre, WinData, Params, Win)) {
        ModelGlobal_(Params, (void *)0x1f800000);
        if (Cre->Life > 0) {
            if (Cre->Control & 0x800000)
                OrderedSort(&WinData->Sort, &AreaData[WinData->SortArea].Data[Win].QuickSortLink);
            else
                OrderedSort(&WinData->Sort, &AreaData[WinData->SortArea].Data[Win].SortLink);
        } else
            OrderedSort(&WinData->Sort, &HeavenlySortLink.SortLink);
        return 1;
    }
    return 0;
}

int ClipGlobalModel(struct CREATURE *Cre, struct MODWIN *WinData, int Win)
{
    struct VECT Rel;
    struct VECT Vect;
    struct SVECTOR CentreOffset;
    struct SCRPNTZ ScreenXYZ;
    int a;
    int Edge;

    Rel.vx = Cre->Draw.Location.vx - CurrCamPos->vx;
    Rel.vy = Cre->Draw.Location.vy - CurrCamPos->vy;
    Rel.vz = Cre->Draw.Location.vz - CurrCamPos->vz;
    Vect.vz = (Rel.vx * CurrCamMat->m[2][0] + Rel.vy * CurrCamMat->m[2][1] + Rel.vz * CurrCamMat->m[2][2]) / 4096;
    if (Vect.vz < 0 && !(Cre->Control & 0x20))
        return 1;
    Vect.vx = (Rel.vx * CurrCamMat->m[0][0] + Rel.vy * CurrCamMat->m[0][1] + Rel.vz * CurrCamMat->m[0][2]) / 4096;
    Vect.vy = (Rel.vx * CurrCamMat->m[1][0] + Rel.vy * CurrCamMat->m[1][1] + Rel.vz * CurrCamMat->m[1][2]) / 4096;
    gte_SetTransMatrix((struct MATRIX *)((char *)&Vect - 20));
    WinData->SortArea = ChooseSortArea(Win, Cre->Box.Area, Cre->Box.AltArea);
    SetupScreenGivenArea(Win, WinData->SortArea);
    CalcSortList(Cre, &Rel, &WinData->Seed, &WinData->Offset);
    if (Cre->Control & 0x20) {
        WinData->ClipValidate = ChrisCount;
        return 0;
    }
    memset(&CentreOffset, 0, sizeof(CentreOffset));
    gte_ldv0(&CentreOffset);
    gte_rtps();
    gte_stsxy(&ScreenXYZ);
    gte_stsz(&ScreenXYZ.sz);
    if (!ScreenXYZ.sz)
        ScreenXYZ.sz = 1;
    a = (CurrWinSizeX >> 1) * (short)(Cre->ModelHdr->RadiusY < Cre->ModelHdr->Radius ? Cre->ModelHdr->Radius : Cre->ModelHdr->RadiusY) / ScreenXYZ.sz;
    Edge = ScreenXYZ.sx;
    if (Edge > CurrScreenHalfSize.sx)
        Edge = CurrScreenSize.sx - Edge;
    Edge += a;
    if (Edge < 0)
        return 1;
    Edge = ScreenXYZ.sy;
    if (Edge > CurrScreenHalfSize.sy)
        Edge = CurrScreenSize.sy - Edge;
    Edge += a;
    if (Edge < 0)
        return 1;
    ScreenXYZ.sz += CurrScreenMin.sx;
    ScreenXYZ.sy += CurrScreenMin.sy;
    WinData->ScreenXY = (struct SCRPNT){ScreenXYZ.sx, ScreenXYZ.sy};
    WinData->Sort.ScreenZ = ScreenXYZ.sz;
    WinData->ClipValidate = ChrisCount;
    return 0;

}

void InitCreFreeList(void)
{
    int a;

    CurrFreeCre = CreFreeList;
    for (a = 47; a >= 0; a--)
        CreFreeList[a] = &Creatures[a];
}

struct CREATURE *GetNextCre(int UseExpendable)
{
    struct CREATURE *Cre;

    if (CurrFreeCre - CreFreeList >= 48) {
        if (UseExpendable) {
            if (!KillExpendableCre()) {
                FailedGetCre++;
                return 0;
            }
        } else {
            FailedGetCre++;
            return 0;
        }
    }
    SucceedGetCre++;
    Cre = *CurrFreeCre++;
    memset(Cre, 0, 0x300);
    Cre->Life = 1;
    Cre->Box.PrimarySpace = -1;
    Cre->Box.MoveSpace = -1;
    Cre->Box.FloorNormMin = 0xa28;
    Cre->ModelOpaque = Cre->ModelBright = 0x1000;
    Cre->SoundID = 0;
    Cre->Ident = CrIdent++;
    Cre->Ambience = Cre->DefaultAmbience = *(struct COLOUR *)"@@@";
    return Cre;
}

void KillModel(struct CREATURE *Cre)
{
    if (Cre->SoundID)
        SoundStopOnCreDeath(Cre->SoundID, Cre->SoundChannel);
    GameFreeCreatureStuff(Cre);
    FreeCrePolyPackets(Cre);
    *--CurrFreeCre = Cre;
    Cre->Life = 0;
}

int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2)
{
    int a, b;

    b = ((int *)Name2)[0];
    if (b == 0)
        return 2;
    a = ((int *)Name1)[0];
    if (a != b)
        return 0;
    a = ((int *)Name1)[1];
    b = ((int *)Name2)[1];
    if (a != b)
        return 0;
    a = ((int *)Name1)[2];
    b = ((int *)Name2)[2];
    return a == b;
}

void *FindIdentNameInList(struct IDENTITYNAME *Name, void *VPntr, int NumBytes)
{
    char *Pntr;

    Pntr = VPntr;
    if (!Pntr)
        return 0;
    while (1) {
        switch (ModNameCmp(Name, (struct IDENTITYNAME *)Pntr)) {
        case 0:
            Pntr += NumBytes;
            break;
        case 1:
            return Pntr;
        case 2:
            return 0;
        }
    }
}

struct SHORTPAIR GetCreHitSize(struct CREATURE *Cre)
{
    if (Cre->ModelHdr)
        return (struct SHORTPAIR){Cre->ModelHdr->Radius, Cre->ModelHdr->RadiusY};
    return (struct SHORTPAIR){0x11e, 0x258};
}

/* while (1) + break keeps the exit test at the top: gcc does not rotate these loops */
void BodgeAnimData(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *AnimHdr;
    struct ANIMFRAMEHDRHDR *FrameHdrHdr;
    short NextOffset;
    int iSortDir;
    struct EXTRASORTDIRTABLE *lpExtraSortDirTable;

    FrameHdrHdr = Hdr->AnimData;
    AnimHdr = Hdr->AnimHdr;
    if ((void *)FrameHdrHdr != (void *)Hdr) {
        do {
            NextOffset = FrameHdrHdr->Offset;
            FrameHdrHdr->NumFrames *= 10;
            Relocate(&FrameHdrHdr, (void *)FrameHdrHdr->Offset);
        } while (NextOffset);
    }
    while (1) {
        if (!*(int *)AnimHdr->AnimName.Name)
            break;
        AnimHdr->AnimDetails.Start *= 5;
        AnimHdr->AnimDetails.Stop *= 5;
        AnimHdr->AnimDetails.Wrap *= 5;
        AnimHdr++;
    }

    lpExtraSortDirTable = (struct EXTRASORTDIRTABLE *)((char *)Hdr + (int)Hdr->ExtraSortData);
    for (iSortDir = 0; iSortDir < 8; iSortDir++) {
        struct RANGESHDR *lpRangesHdr;
        int i;
        int Offset;

        Offset = lpExtraSortDirTable->wOffset[iSortDir];
        while (1) {
            struct RANGE *lpRange;

            if (!Offset)
                break;
            lpRangesHdr = (struct RANGESHDR *)((char *)lpExtraSortDirTable + Offset);
            lpRange = (struct RANGE *)(lpRangesHdr + 1);
            for (i = 0; i < lpRangesHdr->wRanges; i++, lpRange++) {
                lpRange->wStart *= 10;
                lpRange->wStop *= 10;
            }
            Offset = lpRangesHdr->wOffset;
        }
    }
}

void InitialiseModelFile(struct GLOBMODHDR *Hdr)
{
    TotalModelsLoaded = 0;
    while (Hdr) {
        Hdr->ShadowVerts = ShortTerminator;
        Relocate(&Hdr->GlobPolys, Hdr);
        Relocate(&Hdr->GlobPoints, Hdr);
        Relocate(&Hdr->SubModels, Hdr);
        Relocate(&Hdr->SortData, Hdr);
        Relocate(&Hdr->AnimHdr, Hdr);
        Relocate(&Hdr->AnimData, Hdr);
        BodgeAnimData(Hdr);
        if (Hdr->Link)
            Relocate(&Hdr->Link, Hdr);
        TotalModelsLoaded++;
        Hdr = Hdr->Link;
    }
}

/* matches modulo the static's name (Temp.100 in the original) */
char *PrintIdentityName(struct IDENTITYNAME Name)
{
    static char Temp[13];
    int *Data2;

    Data2 = (int *)Temp;
    Data2[0] = ((int *)&Name)[0];
    Data2[1] = ((int *)&Name)[1];
    Data2[2] = ((int *)&Name)[2];
    return Temp;
}

int GetSceneNum(struct IDENTITYNAME Name)
{
    struct MAPNAME *MapName;

    MapName = MapNames;
    for (;;) {
        switch (ModNameCmp(&Name, &MapName->Name)) {
        case 0:
            MapName++;
            break;
        case 1:
            return MapName->ObjNum;
        case 2:
            return -1;
        }
    }
}

void LinkCastLists(void)
{
    struct GLOBMODHDR **HdrP;

    HdrP = &CastList;
    CastList = CommonCastList;
    while (*HdrP)
        HdrP = &(*HdrP)->Link;
    *HdrP = ZoneCastList;
    CastListLinkAddr = HdrP;
}

void UnLinkCastLists(void)
{
    *CastListLinkAddr = 0;
}

struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name)
{
    struct GLOBMODHDR *Hdr;

    for (Hdr = CastList; Hdr; Hdr = Hdr->Link) {
        switch (ModNameCmp(&Name, &Hdr->ModelName)) {
        case 0:
            break;
        case 1:
            return Hdr;
        case 2:
            return 0;
        }
    }
    return 0;
}

struct ANIMHDR *FindAnimHdr(struct GLOBMODHDR *Hdr, struct IDENTITYNAME Name)
{
    struct ANIMHDR *AnimHdr;

    AnimHdr = Hdr->AnimHdr;
    if (!Hdr)
        return 0;
    while (1) {
        switch (ModNameCmp(&Name, &AnimHdr->AnimName)) {
        case 0:
            AnimHdr++;
            break;
        case 1:
            return AnimHdr;
        case 2:
            PrintIdentityName(Name);
            return 0;
        }
    }
}

void PrintAnimData(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *AnimHdr;

    AnimHdr = Hdr->AnimHdr;
    do {
    } while (*(int *)(AnimHdr++)->AnimName.Name);
}

void AlterSubModAng(struct GLOBMODHDR *GlobHdr, short FrameNum, short SubModelNum, short AngX, short AngY, short AngZ)
{
    struct ANIMFRAMEHDRHDR *FrameHdrHdr;
    struct ANIMFRAMEHDR *FrameHdr;
    struct ANIMFRAME *BaseFrame;
    int ToDo;

    if (GlobHdr) {
        FrameHdrHdr = GlobHdr->AnimData;
        ToDo = FrameNum;
        while (ToDo >= FrameHdrHdr->NumFrames) {
            ToDo -= FrameHdrHdr->NumFrames;
            Relocate(&FrameHdrHdr, (void *)FrameHdrHdr->Offset);
        }
        FrameHdr = (struct ANIMFRAMEHDR *)(FrameHdrHdr + 1);
        FrameHdr += ToDo / 10;
        BaseFrame = (struct ANIMFRAME *)((char *)FrameHdr + FrameHdr->Offset);
        BaseFrame += SubModelNum;
        if (AngX >= 0)
            BaseFrame->Ang.x = (AngX & 0xfff) >> 1;
        if (AngY >= 0)
            BaseFrame->Ang.y = (AngY & 0xfff) >> 2;
        if (AngZ >= 0)
            BaseFrame->Ang.z = (AngZ & 0xfff) >> 1;
    }
}

struct QUATANIMFRAME *GetTweenedSubModXYZ(short SubModId, struct ANIMFRAMEHDRHDR *FrameHdrHdr, struct ANIMFRAMEHDRHDR *FrameHdrHdrB, short ToDo300FPSUnClipped)
{
    int ToDo;
    int ThisToDo;
    int Count;
    int Nibbles8;
    int Remainder;
    unsigned long *CountPntr;
    unsigned long CountData;
    struct TWEENHDR *TweenHdr;
    struct TWEENHDR *TweenHdr2;
    struct ANIMFRAME *AnimFrameA;
    struct ANIMFRAME *AnimFrameB;
    struct QUATANIMFRAME *QBaseFrame;
    struct MATRIX QuatMatrix;
    int ToDo300FPS;
    unsigned char *Temp;

    TweenHdr = (struct TWEENHDR *)(FrameHdrHdr + 1) + SubModId;
    QBaseFrame = GlobalTweenedQuatFrames;
    ToDo300FPS = ToDo300FPSUnClipped;
    if (!ToDo300FPS) {
        AnimFrameA = (struct ANIMFRAME *)(TweenHdr->AnimFrameOffset + (int)TweenHdr);
        PackedEulerToCamQuat(AnimFrameA->Ang, &QBaseFrame->Orient);
        QBaseFrame->Tran.vx = AnimFrameA->Tran.x << 1;
        QBaseFrame->Tran.vy = AnimFrameA->Tran.y << 2;
        QBaseFrame->Tran.vz = AnimFrameA->Tran.z << 1;
        return QBaseFrame;
    }
    if (ToDo300FPS > FrameHdrHdr->NumFrames - 10)
        ToDo300FPS = FrameHdrHdr->NumFrames - 10;
    /* The original wraps this in a loop construct (its line table has notes for the lines around
       it): gcc weights the two assignments by loop depth when it ranks pseudos for register
       allocation, which is what gives TweenHdr2 $t4 and TweenHdr $t5. */
    do {
        if (FrameHdrHdrB) {
            TweenHdr2 = (struct TWEENHDR *)(FrameHdrHdrB + 1) + SubModId;
        } else {
            TweenHdr2 = 0;
        }
    } while (0);
    ToDo = ToDo300FPS / 10;
    Remainder = ToDo300FPS - ToDo * 10;
    ThisToDo = ToDo;
    Temp = (unsigned char *)(TweenHdr->CountOffset + (int)TweenHdr);
    CountData = *(unsigned long *)Temp;
    CountPntr = (unsigned long *)Temp;
    Nibbles8 = 0;
    Count = CountData & 0xf;
    CountData >>= 4;
    while (ThisToDo * 10 + Remainder > Count * 10) {
        Nibbles8++;
        ThisToDo -= Count;
        if (!(Nibbles8 & 7))
            CountData = *++CountPntr;
        Count = CountData & 0xf;
        CountData >>= 4;
    }
    AnimFrameA = (struct ANIMFRAME *)(TweenHdr->AnimFrameOffset + (int)TweenHdr);
    AnimFrameA += Nibbles8;
    Count *= 10;
    ThisToDo = ThisToDo * 10 + Remainder;
    if (TweenHdr2) {
        AnimFrameA++;
        AnimFrameB = (struct ANIMFRAME *)(TweenHdr2->AnimFrameOffset + (int)TweenHdr2);
        ThisToDo = ToDo300FPSUnClipped - ToDo300FPS;
        Count = 10;
    } else {
        AnimFrameB = AnimFrameA + 1;
    }
    TweenQuatFrame(QBaseFrame, AnimFrameA, AnimFrameB, ThisToDo, Count);
    return QBaseFrame;
}

struct SVECT GetSubModXYZ(struct GLOBMODHDR *GlobHdr, struct MODSUBOFF SubOff, short AnimFrame, short AnimJump)
{
    struct ANIMFRAMEHDRHDR *FrameHdrHdr;
    struct ANIMFRAMEHDRHDR *FrameHdrHdrB;
    struct ANIMFRAMEHDR *FrameHdr;
    struct ANIMFRAME *BaseFrame;
    struct QUATANIMFRAME *QBaseFrame;
    struct SVECT RotReturn;
    struct SVECT UnPacked;
    struct MATRIX Matrix;
    int DoOffsetRot;

    FrameHdrHdr = GlobHdr->AnimData;
    DoOffsetRot = (SubOff.Offset.vx | SubOff.Offset.vy | SubOff.Offset.vz) != 0;
    while (AnimFrame >= FrameHdrHdr->NumFrames) {
        AnimFrame -= FrameHdrHdr->NumFrames;
        Relocate(&FrameHdrHdr, (void *)FrameHdrHdr->Offset);
    }
    if ((FrameHdrHdr->Flags & 1) && AnimJump != -1) {
        FrameHdrHdrB = GlobHdr->AnimData;
        while (AnimJump >= FrameHdrHdrB->NumFrames) {
            AnimJump -= FrameHdrHdrB->NumFrames;
            Relocate(&FrameHdrHdrB, (void *)FrameHdrHdrB->Offset);
        }
    } else {
        FrameHdrHdrB = 0;
    }
    if (FrameHdrHdr->Flags & 1) {
        QBaseFrame = GetTweenedSubModXYZ(SubOff.SubModel, FrameHdrHdr, FrameHdrHdrB, AnimFrame);
        if (DoOffsetRot) {
            QuatToMat(&QBaseFrame->Orient, &Matrix);
            UnPacked.vx = QBaseFrame->Tran.vx;
            UnPacked.vy = QBaseFrame->Tran.vy;
            UnPacked.vz = QBaseFrame->Tran.vz;
        } else {
            return QBaseFrame->Tran;
        }
    } else {
        FrameHdr = (struct ANIMFRAMEHDR *)(FrameHdrHdr + 1);
        FrameHdr += AnimFrame / 10;
        BaseFrame = (struct ANIMFRAME *)((char *)FrameHdr + FrameHdr->Offset);
        BaseFrame += SubOff.SubModel;
        if (!DoOffsetRot) {
            UnPacked.vx = BaseFrame->Tran.x << 1;
            UnPacked.vy = BaseFrame->Tran.y << 2;
            UnPacked.vz = BaseFrame->Tran.z << 1;
            return UnPacked;
        }
        UnPacked.vx = BaseFrame->Ang.x << 1;
        UnPacked.vy = BaseFrame->Ang.y << 2;
        UnPacked.vz = BaseFrame->Ang.z << 1;
        MYRotMatrixANIM(&UnPacked, &Matrix);
        UnPacked.vx = BaseFrame->Tran.x << 1;
        UnPacked.vy = BaseFrame->Tran.y << 2;
        UnPacked.vz = BaseFrame->Tran.z << 1;
    }
    RotateS(&Matrix, &SubOff.Offset, &RotReturn);
    RotReturn.vx += UnPacked.vx;
    RotReturn.vy += UnPacked.vy;
    RotReturn.vz += UnPacked.vz;
    return RotReturn;
}

int GetNumberVertices(struct GLOBMODHDR *Hdr)
{
    int Count;
    int SubModel;

    Count = 0;
    if (Hdr) {
        for (SubModel = Hdr->NumSubModels; SubModel; )
            Count += Hdr->SubModels[--SubModel].WriteCount;
        return Count;
    }
    return 0;
}

struct MODSUBOFF GetModSubOffFromVertex(struct GLOBMODHDR *Hdr, short VertexNum)
{
    int ToDo;
    int SubModel;
    struct MODSUBOFF Ret;

    ToDo = VertexNum;
    for (SubModel = -1; ToDo >= 0; SubModel++)
        ToDo -= Hdr->SubModels[SubModel + 1].WriteCount;
    Ret.Offset = *(struct SVECT *)&Hdr->GlobPoints[VertexNum];
    Ret.SubModel = SubModel;
    return Ret;
}

int GetModSubOffFrom2Verteces(struct GLOBMODHDR *Hdr, short VertexNum1, short VertexNum2, struct MODSUBOFF *ModSubOff)
{
    struct MODSUBOFF MuzData[2];

    MuzData[0] = GetModSubOffFromVertex(Hdr, VertexNum1);
    MuzData[1] = GetModSubOffFromVertex(Hdr, VertexNum2);
    if (MuzData[0].SubModel == MuzData[1].SubModel) {
        ModSubOff->SubModel = MuzData[0].SubModel;
        ModSubOff->Offset.vx = (MuzData[0].Offset.vx + MuzData[1].Offset.vx) / 2;
        ModSubOff->Offset.vy = (MuzData[0].Offset.vy + MuzData[1].Offset.vy) / 2;
        ModSubOff->Offset.vz = (MuzData[0].Offset.vz + MuzData[1].Offset.vz) / 2;
        return 1;
    }
    ModSubOff->SubModel = MuzData[0].SubModel;
    ModSubOff->Offset = MuzData[0].Offset;
    return 0;
}

void GetAbsoluteModelVert(struct CREATURE *Cre, short VertexNum, struct VECT *Pos)
{
    struct SVECT Rel;
    struct SVECT Rel1;

    if (VertexNum >= 0) {
        Rel = GetSubModXYZ(Cre->ModelHdr, GetModSubOffFromVertex(Cre->ModelHdr, VertexNum), Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
        RotateS(&Cre->Matrix, &Rel, &Rel1);
        Pos->vx = Cre->Draw.Location.vx + Rel1.vx;
        Pos->vy = Cre->Draw.Location.vy + Rel1.vy;
        Pos->vz = Cre->Draw.Location.vz + Rel1.vz;
    } else {
        *Pos = Cre->Draw.Location;
    }
}

void PolyOpaqueSetup(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD)
{
    UVS(FT4(PD)->u0) = UVS(FT4(PC)->u0) = UVS(PolyA->u0);
    UVS(FT4(PD)->u1) = UVS(FT4(PC)->u1) = UVS(PolyA->u1);
    UVS(FT4(PD)->u2) = UVS(FT4(PC)->u2) = UVS(PolyA->u2);
    UVS(FT4(PD)->u3) = UVS(FT4(PC)->u3) = UVS(PolyA->u3);
    FT4(PD)->tpage = FT4(PC)->tpage = (PolyB->tpage & 0x1f) | 0x40;
    FT4(PD)->code = FT4(PC)->code = 0x2e;
    FT4(PD)->tag = FT4(PC)->tag = 0x9000000;
    FT4(PD)->clut = FT4(PC)->clut = palette[73];
    PolyA->code = 0x3e;
    PolyA->tpage = (PolyA->tpage & 0x1f) | 0x20;
}

void PolyOpaqueSetupSecondary(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD)
{
    PolyA->code = 0x3e;
    PolyA->tpage = (PolyA->tpage & 0x1f) | 0x20;
}

void ResetPolyTpageCode(struct GLOBMODHDR *Hdr, struct GLOBPOL *GlobPol, struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB)
{
    PolyA->tpage = PolyB->tpage = alltpages[GlobPol->Tp & 0x1f] | tptranstable[GlobPol->Tp >> 5];
    PolyA->code = PolyB->code = semitranstable[GlobPol->Tp >> 5] | 0x3c;
}

void ForEachPolyInModel(struct CREATURE *Cre, void *VoidRoutine, int Win)
{
    short *fp;
    struct POLY_GT4 *PolyA;
    struct POLY_GT4 *PolyB;
    int f;
    int a;
    struct GLOBPOL *GlobPoly;
    struct GLOBMODHDR *Hdr;
    void (*Routine)();

    Routine = VoidRoutine;
    fp = &Cre->WinData[Win].Pols[1];
    Hdr = Cre->ModelHdr;
    GlobPoly = Hdr->GlobPolys;
    for (f = 0, a = Hdr->TotalPolys; a > 0; a--, GlobPoly++, f++) {
        if (!(f & 31)) {
            PolyA = currdb->modpolsg4 + *fp;
            PolyB = olddb->modpolsg4 + *fp++;
        }
        Routine(Hdr, GlobPoly, PolyA++, PolyB++);
    }
}

void ForEachPolyInModelParams(struct CREATURE *Cre, void *VoidRoutine, int Win, int ParamA, int ParamB)
{
    short *fp;
    struct POLY_GT4 *PolyA;
    struct POLY_GT4 *PolyB;
    int f;
    int a;
    struct GLOBMODHDR *Hdr;
    void (*Routine)();

    Routine = VoidRoutine;
    fp = &Cre->WinData[Win].Pols[1];
    Hdr = Cre->ModelHdr;
    for (f = 0, a = Hdr->TotalPolys; a > 0; a--, f++) {
        if (!(f & 31)) {
            PolyA = currdb->modpolsg4 + *fp;
            PolyB = olddb->modpolsg4 + *fp++;
        }
        Routine(PolyA++, PolyB++, ParamA, ParamB);
    }
}

void ForEachPolyInModelHdr(struct GLOBMODHDR *Hdr, void *VoidRoutine, int a, int b, int c)
{
    void (*Routine)();
    struct GLOBPOL *GlobPoly;

    Routine = VoidRoutine;
    for (GlobPoly = Hdr->GlobPolys; GlobPoly <= &Hdr->GlobPolys[Hdr->TotalPolys]; GlobPoly++)
        Routine(GlobPoly, a, b, c);
}

void ForEachPolyInModelOpaque(struct CREATURE *Cre, void *VoidRoutine, int Win)
{
    short *fp;
    short *fpO;
    struct POLY_GT4 *PolyA;
    struct POLY_GT4 *PolyB;
    struct POLY_GT4 *PolyC;
    struct POLY_GT4 *PolyD;
    int f;
    int a;
    struct GLOBMODHDR *Hdr;
    void (*Routine)();

    Routine = VoidRoutine;
    fp = &Cre->WinData[Win].Pols[1];
    fpO = &Cre->WinData[Win].OpaquePols[1];
    Hdr = Cre->ModelHdr;
    for (f = 0, a = Hdr->TotalPolys; a > 0; a--, f++) {
        if (!(f & 31)) {
            PolyA = currdb->modpolsg4 + *fp;
            PolyB = olddb->modpolsg4 + *fp++;
            PolyC = currdb->modpolsg4 + *fpO;
            PolyD = olddb->modpolsg4 + *fpO++;
        }
        Routine(PolyA++, PolyB++, PolyC++, PolyD++);
    }
}
