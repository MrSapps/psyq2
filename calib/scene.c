#include "types.h"
/* SYM says int, but the code loads them with lhu */
extern unsigned short ChrisCount;
extern unsigned short ChrisCount1;
extern struct AREA AreaData[];
extern unsigned long AreaNotHiddenData[];

int AreaValidPntr(int Win, struct AREA *Area)
{
    return Area->Data[Win].ValidCheck == ChrisCount;
}

int AreaValid(int Win, int Area)
{
    return AreaData[Area].Data[Win].ValidCheck == ChrisCount;
}

int AreaValid1(int Win, int Area)
{
    return AreaData[Area].Data[Win].ValidCheck == ChrisCount1;
}

void SetAreaValid(int Win, int Area)
{
    AreaData[Area].Data[Win].ValidCheck = ChrisCount;
}

int AreaHidden(int AreaIndx)
{
    return !(AreaNotHiddenData[AreaIndx / 32] & (1 << (AreaIndx & 31)));
}

short *pAreaConxFile;
short *pAreaConxLists;
short DoorWayActive;
short NumSceneModels;
struct SCENE2 *Scene2Data;
unsigned char *NextFreeMoveBuf;
unsigned char *LastFreeMoveBuf;
extern int MaxWindows;
extern int ScreenChangeIndx[];
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct WINDOW GameWindows[];
#define MOVEBUF(i) (&BrushMoveBufs[(i) - 1])
extern unsigned char FreeListMoveBuf[];
extern struct COLLHEADER CollMap;
void TurnOffSceneModel(struct SCENE2 *Scn2, int Win);

void CalcActiveAreaList(void)
{
    unsigned long ToConsider[3];
    unsigned long *Pntr;
    unsigned long Consider;
    unsigned long Considered[3];
    unsigned long NotHidden[3];
    int a;
    int Area;
    struct PORTDATA *PortalData;
    struct PORTDATA *Portal;
    char *AreaList;
    char *Data;
    int NumPortals;
    int SomeToDo;

    /* ChrisCount is loaded with lw here (lhu elsewhere) */
    if (*(int *)&ChrisCount == 1 || MaxWindows != 1) {
        for (a = 0; a < 3; a++)
            AreaNotHiddenData[a] = ~0;
        return;
    }
    for (a = 0; a < 3; a++) {
        ToConsider[a] = 0;
        Considered[a] = 0;
        NotHidden[a] = 0;
    }
    SomeToDo = 0;
    for (a = 0; a < MaxWindows; a++) {
        if (GameWindows[a].Area) {
            ToConsider[GameWindows[a].Area / 32] |= 1 << (GameWindows[a].Area & 31);
            SomeToDo = 1;
        }
    }
    while (SomeToDo) {
        Pntr = ToConsider;
        Area = 0;
        while (!*Pntr) {
            Pntr++;
            Area += 32;
        }
        Consider = *Pntr;
        while (!(Consider & 1)) {
            Consider >>= 1;
            Area++;
        }
        Considered[Area / 32] |= 1 << (Area & 31);
        ToConsider[Area / 32] &= ~(1 << (Area & 31));
        NotHidden[Area / 32] |= 1 << (Area & 31);
        if (pAreaConxLists[Area]) {
            Data = (char *)pAreaConxFile + pAreaConxLists[Area];
            NumPortals = *(unsigned char *)Data;
            AreaList = Data + 1;
        } else
            NumPortals = 0;
        if (NumPortals & 1)
            Data = AreaList + NumPortals;
        else
            Data = AreaList + NumPortals + 1;
        PortalData = (struct PORTDATA *)Data;
        for (Portal = PortalData; Portal < PortalData + NumPortals; Portal++, AreaList++) {
            SomeToDo = *AreaList >> 5;
            if (!(Considered[SomeToDo] & (1 << (*AreaList & 31))) && !Scene2Data[Portal->Obj].NoDraw)
                /* the cast keeps CSE from reusing the scaled Considered[] index */
                ToConsider[(unsigned char)SomeToDo] |= 1 << (*AreaList & 31);
        }
        SomeToDo = 0;
        for (a = 0; a < 3; a++)
            SomeToDo |= ToConsider[a];
    }
    for (a = 0; a < 3; a++)
        AreaNotHiddenData[a] = NotHidden[a];
}


void ReInitAreaConxData(void *ConxFile)
{
    pAreaConxFile = ConxFile;
    pAreaConxLists = (short *)ConxFile + 1;
}

short VectBoxOverlapsArea(struct VECTBOX *VectBox, int Area)
{
    struct SVECT Temp;
    return 0;
}

void InitialiseScreenIndxStuff(void)
{
    int Win;
    for (Win = 0; Win < MaxWindows; Win++)
        ScreenChangeIndx[Win] = 0;
}

void InitialiseSceneEveryLevel(void)
{
    DoorWayActive = 1;
    memset(AreaData, 0, sizeof(struct AREA) * 96);
}

void InitialiseSceneEveryFrame(void)
{
}

void ResetSceneValidates(void)
{
    struct SCENE2 *Scn2;
    for (Scn2 = Scene2Data; Scn2 < Scene2Data + NumSceneModels; Scn2++)
        Scn2->Validate = 0x8000;
}

void TurnOffScene(void)
{
    struct SCENE2 *Scn2;
    int Win;
    for (Scn2 = Scene2Data; Scn2 < Scene2Data + NumSceneModels; Scn2++)
        for (Win = 0; Win < MaxWindows; Win++)
            TurnOffSceneModel(Scn2, Win);
}

int FindAreaNum(int Area, struct VECT *Pos)
{
    int Space;
    if (!Area) {
        Space = Find3dSpaceIndxLink(&CollMap, Pos, -1, 1);
        if (Space != -1)
            return CollMap.Coll3dSpace[Space].Area;
        return 0;
    }
    return Area;
}

int AttatchSolidBoxForObj(int Obj)
{
    unsigned char MoveBufIdent;
    struct BRUSHMOVEBUF *MoveBuf;
    struct SOLIDBOX *SolidBox;
    struct SCENE2 *Brush;
    Brush = &Scene2Data[Obj];
    SolidBox = GenSolidBox(&Brush->VectBox, 0, 1);
    MoveBufIdent = Brush->MoveBufId;
    if (SolidBox) {
        if (MoveBufIdent) {
            MoveBuf = MOVEBUF(MoveBufIdent);
            Brush->MoveBufId = MoveBufIdent;
            memset(&MoveBuf->Ang, 0, 6);
            memset(&MoveBuf->Offset, 0, 6);
            MoveBuf->Obj = Obj;
            MoveBuf->Primary = 0;
            MoveBuf->SolidBox = SolidBox;
        } else
            FreeSolidBoxs(SolidBox);
    }
}

void HideObjWithSolid(int Obj)
{
    struct BRUSHMOVEBUF *MoveBuf;
    struct SOLIDBOX *SolidBox;
    struct SCENE2 *Brush;
    unsigned char id;
    Brush = &Scene2Data[Obj];
    Brush->NoDraw = 1;
    id = Brush->MoveBufId;
    if (id) {
        MoveBuf = MOVEBUF(id);
        SolidBox = MoveBuf->SolidBox;
        if (SolidBox)
            FreeSolidBoxs(SolidBox);
    }
}

void InitMoveBuffer(void)
{
    int Indx;
    memset(BrushMoveBufs, 0, sizeof(struct BRUSHMOVEBUF) * 48);
    for (Indx = 0; Indx < 48; Indx++)
        FreeListMoveBuf[Indx] = Indx + 1;
    NextFreeMoveBuf = FreeListMoveBuf;
    LastFreeMoveBuf = FreeListMoveBuf + 48;
}

int ChooseSortArea(int Win, int Area, int AltArea)
{
    if (AltArea) {
        if (AreaData[AltArea].Data[Win].ValidCheck == ChrisCount
            && AreaData[AltArea].Data[Win].Depth < AreaData[Area].Data[Win].Depth)
            return AltArea;
        return Area;
    }
    return Area;
}

extern struct SCRPNT ScreenSize;
extern struct SCRPNT CurrScreenSize;
extern struct SCRPNT CurrScreenMin;
extern struct SCRPNT CurrScreenHalfSize;
extern struct WINDOW *CurrWin;

void SetupScreenGivenArea(int Win, int AreaIndx)
{
    struct SCREENCHANGE *Sc;

    if (AreaIndx < 0)
        return;
    if (AreaIndx == 0) {
        CurrScreenSize = ScreenSize;
        memset(&CurrScreenMin, 0, sizeof(struct SCRPNT));
    } else if (AreaIndx == 1) {
        CurrScreenSize = CurrWin->Size;
        memset(&CurrScreenMin, 0, sizeof(struct SCRPNT));
    } else {
        Sc = AreaData[AreaIndx].Data[Win].ScreenChange;
        CurrScreenMin = Sc->MinMax.Min;
        CurrScreenSize.sx = Sc->MinMax.Max.sx - Sc->MinMax.Min.sx;
        CurrScreenSize.sy = Sc->MinMax.Max.sy - Sc->MinMax.Min.sy;
    }
    SetGeomOffset(CurrWin->Geom.sx - CurrScreenMin.sx, CurrWin->Geom.sy - CurrScreenMin.sy);
    CurrScreenHalfSize.sx = (CurrScreenSize.sx + 1) / 2;
    CurrScreenHalfSize.sy = (CurrScreenSize.sy + 1) / 2;
}

extern unsigned short *GamePals;
extern unsigned char semitranstable[];
extern short alltpages[];
extern unsigned short tptranstable[];
extern struct DB db[];
unsigned char *SceneBuffFreePntr;
extern struct SCENE SceneBuffs[];
void FreeSubDivFromPoly(struct POLY_GT4 *Poly);
void FreePolyPackets(short *Source);

static void FreeSceneBuff(unsigned char *BuffIndx)
{
    *--SceneBuffFreePntr = *BuffIndx;
    *BuffIndx = 0;
}

void TurnOffSceneModel(struct SCENE2 *Scn2, int Win)
{
    int b;
    int c;
    struct POLY_GT4 *Spr0;
    struct SCENE *Scn;

    if (Scn2->BuffIndx[Win]) {
        Scn = &SceneBuffs[Scn2->BuffIndx[Win] - 1];
        for (b = 0; b < Scn2->ModelHdr->NumPolys >> 5; b++) {
            Spr0 = &db[0].modpolsg4[Scn->Pols[b + 1]];
            for (c = 0; c < 32; c++, Spr0++)
                if (Spr0->pad3)
                    FreeSubDivFromPoly(Spr0);
        }
        Spr0 = &db[0].modpolsg4[Scn->Pols[b + 1]];
        for (c = 0; c < (Scn2->ModelHdr->NumPolys & 31); c++, Spr0++)
            if (Spr0->pad3)
                FreeSubDivFromPoly(Spr0);
        FreePolyPackets(Scn->Pols);
        FreeSceneBuff(&Scn2->BuffIndx[Win]);
    }
}

extern struct SCRPNT CurrOffset;
extern int CurrWinIndx;
extern unsigned long *CurrOt;
extern struct SCREENCHANGE ScreenChanges[4][24];
extern struct DRAWENV *ChangeDrawEnv;
extern struct DRAWENV *LastChangeDrawEnv;
extern unsigned char DbFrame[];
unsigned char PortalDepth;
void dprintf(struct WINDOW *Window, char *_format, ...);
void PortalTestWibble(struct SCREENCHANGE *SChange, struct SCENE2 *Scn2);
struct DRAWENV *SetDefDrawEnv(struct DRAWENV *env, int x, int y, int w, int h);
void SetDrawEnv(struct DR_ENV *dr_env, struct DRAWENV *env);
void AddPrim_(void *ot, void *p);
void *memset(void *, int, int);

#define gte_SetGeomOffset(r0, r1) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12", "memory"); \
    __asm__ volatile("move $13,%0" : : "r"(r1) : "$13", "memory"); \
    __asm__ volatile("sll $12,$12,16\n\tsll $13,$13,16\n\tctc2 $12,$24\n\tctc2 $13,$25" : : : "$12", "$13"); }

int AlterScreenClipping(struct SCRPNT Min, struct SCRPNT Max, struct SCRPNT Offset, struct SHORTPAIR OtArea, struct SHORTPAIR ObjClip)
{
    struct SCRPNT Size;
    int ReturnVal;
    struct SCREENCHANGE *SChange;
    int NoDraw;

    NoDraw = 0;
    if (Min.sx == Max.sx || Min.sy == Max.sy) {
        memset(&Min, 0, sizeof(Min));
        Max = Min;
        NoDraw = 1;
    }
    if (++ScreenChangeIndx[CurrWinIndx] >= 24) {
        if (!(*(int *)&ChrisCount & 31))
            dprintf(GameWindows, "Out of ScreenChanges on frame %d\n", *(int *)&ChrisCount);
        NoDraw = 1;
    }
    SChange = &ScreenChanges[CurrWinIndx][ScreenChangeIndx[CurrWinIndx]];
    SChange->Offset = Offset;
    SChange->MinMax.Min = Min;
    SChange->MinMax.Max = Max;
    SChange->Area = OtArea.b;
    SChange->OtLevel = OtArea.a;
    SChange->Obj = ObjClip.a;
    SChange->ClipLevel = ObjClip.b;
    if (!NoDraw && (ObjClip.a & 0xfff) != 0xfff && Scene2Data[ObjClip.a].SubDiv)
        PortalTestWibble(SChange, &Scene2Data[ObjClip.a]);
    Size.sx = SChange->MinMax.Max.sx - SChange->MinMax.Min.sx;
    Size.sy = SChange->MinMax.Max.sy - SChange->MinMax.Min.sy;
    if (Size.sx != CurrScreenSize.sx || Size.sy != CurrScreenSize.sy
        || SChange->MinMax.Min.sx != CurrScreenMin.sx || SChange->MinMax.Min.sy != CurrScreenMin.sy
        || SChange->Offset.sx != CurrOffset.sx || SChange->Offset.sy != CurrOffset.sy) {
        if (ChangeDrawEnv != LastChangeDrawEnv) {
            struct SCRPNT Pos;
            if (DbFrame[0])
                Pos = (struct SCRPNT){ScreenSize.sx + CurrWin->Pos.sx + CurrScreenMin.sx + CurrOffset.sx,
                                      CurrWin->Pos.sy + CurrScreenMin.sy + CurrOffset.sy};
            else
                Pos = (struct SCRPNT){CurrWin->Pos.sx + CurrScreenMin.sx + CurrOffset.sx,
                                      CurrWin->Pos.sy + CurrScreenMin.sy + CurrOffset.sy};
            SetDefDrawEnv(ChangeDrawEnv, Pos.sx, Pos.sy, CurrScreenSize.sx, CurrScreenSize.sy);
            ChangeDrawEnv->isbg = 0;
            SetDrawEnv(&ChangeDrawEnv->dr_env, ChangeDrawEnv);
            AddPrim_(CurrOt + OtArea.a, &ChangeDrawEnv->dr_env);
            ChangeDrawEnv++;
        }
        CurrOffset = SChange->Offset;
        CurrScreenSize = Size;
        CurrScreenMin = SChange->MinMax.Min;
    }
    gte_SetGeomOffset(CurrWin->Geom.sx - SChange->MinMax.Min.sx, CurrWin->Geom.sy - SChange->MinMax.Min.sy);
    if (OtArea.b == -1)
        return ReturnVal;
    ReturnVal = *(long *)&SChange->MinMax.Min != *(long *)&SChange->MinMax.Max;
    if (ReturnVal) {
        AreaData[OtArea.b].Data[CurrWinIndx].Depth = PortalDepth++;
        AreaData[OtArea.b].Data[CurrWinIndx].ValidCheck = ChrisCount;
        AreaData[OtArea.b].Data[CurrWinIndx].ScreenChange = SChange;
    }
    return ReturnVal;
}

/* PolyNum is range-checked but then unused (the original always reads the first poly); the dead check makes the ModelHdr pseudo span two blocks, which gives the original register allocation */
void GetLimitedPolyData(struct SCENE2 *Scene2, int PolyNum, struct GLOBTEXTDATA *GlobTextData)
{
    struct GLOBSMODGT4 *modgt4;
    struct UV4 *UV4s;
    struct UV4 *PolyUV4;
    char *Temp;
    int NumPolys;

    NumPolys = Scene2->ModelHdr->NumPolys;
    if (PolyNum >= NumPolys) PolyNum = 0;
    UV4s = (struct UV4 *)((char *)Scene2->ModelHdr + Scene2->ModelHdr->UVOffset);
    Temp = (char *)Scene2->ModelHdr + Scene2->ModelHdr->RGBOffset;
    modgt4 = (struct GLOBSMODGT4 *)(Scene2->ModelHdr + 1);
    PolyUV4 = &UV4s[modgt4->UV];
    GlobTextData->Uv0 = PolyUV4->uv[0];
    GlobTextData->Uv1 = PolyUV4->uv[1];
    GlobTextData->Uv2 = PolyUV4->uv[2];
    GlobTextData->Uv3 = PolyUV4->uv[3];
    GlobTextData->Clut = GamePals[modgt4->Pal];
    *(struct RGB3 *)&GlobTextData->Colour = *(struct RGB3 *)(Temp + modgt4->Col0 * 3);
    if (modgt4->trans) {
        GlobTextData->Colour.Pad = 0x3e;
        GlobTextData->TPage = alltpages[modgt4->Tp & 0x1f];
    } else {
        GlobTextData->Colour.Pad = semitranstable[modgt4->trans] | 0x3c;
        GlobTextData->TPage = alltpages[modgt4->Tp & 0x1f] |= tptranstable[2];
    }
}

struct MODELREF *GlobalSubModelRef;
struct SCRPNTZ *SceneBuffer;
struct SCRPNTZ *CurrentCoordBuffer;
extern long *primlistarray[];
extern struct DUMMYGPU *NextDummyGpu;
extern struct DUMMYGPU *LastDummyGpu;
extern struct DB *currdb;
void RotateSceneCoords_(struct MODELREF *Ref, int Many, struct SCRPNTZ *Buffer);
void DrawSceneBitAllSubDiv_(short *Model, struct POLY_GT4 *Poly, unsigned long *Ot, short *NextModel);
void DrawSceneBitNoSubDiv_(short *Model, struct POLY_GT4 *Poly, unsigned long *Ot, short *NextModel);
void DrawSceneBitSomeSubDiv_(short *Model, struct POLY_GT4 *Poly, unsigned long *Ot, short *NextModel);
char *GetNextGlobalMemory(int Size);
void OrderedSort(struct SORT *Self, struct SORT **SortLink);
struct SORT *GetNextSortData(void);


#define gte_ldv0(r0) __asm__ volatile("move $12,%0\n\tlwc2 $0,0($12)\n\tlwc2 $1,4($12)" : : "r"(r0) : "$12")
#define gte_rtps() __asm__ volatile("nop\n\tnop\n\t.word 0x4A180001")
#define gte_stsz(r0) __asm__ volatile("move $12,%0\n\tswc2 $19,0($12)" : : "r"(r0) : "$12", "memory")
#define SetCoordBuffer(r0) __asm__ volatile("move $12,%0" : : "r"(r0) : "$12")

static int OriginZ(void)
{
    struct SVECTOR NullSVector;
    int ZVal;
    memset(&NullSVector, 0, sizeof(NullSVector));
    gte_ldv0(&NullSVector);
    gte_rtps();
    gte_stsz(&ZVal);
    return ZVal;
}

/* the two failure paths of the sort-data test are separate else branches (cross-jumped into one 'GeneralSort = 0' block); this lengthens Ot's lifetime enough to give Packets/OtIndx s2 and Ot s3 */
void DrawBrush(short OtIndx, struct SCENE2 *Scn2, struct SCENE *Scn, struct MODELREFINDX *MRef)
{
    int Many;
    int ThisPass;
    short *Packets;
    short *Model;
    short *NextModel;
    unsigned long *Ot;
    struct POLY_GT4 *Poly;
    struct SCRPNTZ Buffer[15];
    int GeneralSort;
    struct SORT *Sort;

    GlobalSubModelRef = MRef->Ref;
    Many = Scn2->ModelHdr->NumPolys;
    Model = (short *)(Scn2->ModelHdr + 1);
    if (MRef->Many >= 16)
        SceneBuffer = (struct SCRPNTZ *)primlistarray;
    else
        SceneBuffer = Buffer;
    CurrentCoordBuffer = SceneBuffer;
    RotateSceneCoords_(MRef->Ref, MRef->Many, SceneBuffer);
    GeneralSort = Scn2->FreeSort;
    if (GeneralSort) {
        if (NextDummyGpu != LastDummyGpu) {
            Sort = GetNextSortData();
            if (Sort) {
                Sort->Flags = 0;
                Sort->ScreenZ = OriginZ();
                NextDummyGpu->Tag = 0;
                Sort->FirstLast[0] = (struct POLY_GT4 *)NextDummyGpu;
                Sort->FirstLast[1] = (struct POLY_GT4 *)NextDummyGpu;
                if (Scn2->MoveBufId) {
                    struct BRUSHMOVEBUF *MoveBuf;
                    MoveBuf = MOVEBUF(Scn2->MoveBufId);
                    if (MoveBuf->Offset.vx || *(long *)&MoveBuf->Offset.vy) {
                        if ((Sort->VectBox = (struct VECTBOX *)GetNextGlobalMemory(0x18))) {
                            ((long *)Sort->VectBox)[0] = ((long *)&Scn2->VectBox)[0] + MoveBuf->Offset.vx;
                            ((long *)Sort->VectBox)[1] = ((long *)&Scn2->VectBox)[1] + MoveBuf->Offset.vy;
                            ((long *)Sort->VectBox)[2] = ((long *)&Scn2->VectBox)[2] + MoveBuf->Offset.vz;
                            ((long *)Sort->VectBox)[3] = ((long *)&Scn2->VectBox)[3] + MoveBuf->Offset.vx;
                            ((long *)Sort->VectBox)[4] = ((long *)&Scn2->VectBox)[4] + MoveBuf->Offset.vy;
                            ((long *)Sort->VectBox)[5] = ((long *)&Scn2->VectBox)[5] + MoveBuf->Offset.vz;
                        } else
                            Sort->VectBox = &Scn2->VectBox;
                    } else
                        Sort->VectBox = &Scn2->VectBox;
                } else
                    Sort->VectBox = &Scn2->VectBox;
                Ot = (unsigned long *)&Sort->FirstLast[0];
            } else
                GeneralSort = 0;
        } else
            GeneralSort = 0;
    }
    if (!GeneralSort)
        Ot = CurrOt + OtIndx;
    if (!Many)
        return;
    Packets = &Scn->Pols[1];
    while (Many > 0) {
        Poly = &currdb->modpolsg4[*Packets++];
        ThisPass = Many;
        if (Many >= 32)
            ThisPass = 32;
        Many -= ThisPass;
        NextModel = Model + ThisPass * 6;
        SetCoordBuffer(SceneBuffer);
        switch (Scn2->SubDiv & 3) {
        case 0:
            DrawSceneBitAllSubDiv_(Model, Poly, Ot, NextModel);
            break;
        case 1:
            DrawSceneBitNoSubDiv_(Model, Poly, Ot, NextModel);
            break;
        case 2:
            DrawSceneBitSomeSubDiv_(Model, Poly, Ot, NextModel);
            break;
        }
        Model = NextModel;
    }
    if (GeneralSort && (struct DUMMYGPU *)Sort->FirstLast[0] != NextDummyGpu) {
        NextDummyGpu++;
        OrderedSort(Sort, &AreaData[Scn2->Area].Data[CurrWinIndx].SortLink);
    }
}

struct MODELREFINDX *ModelRefIndx;
extern struct VECT *CurrCamPos;
void RotateSceneCoords_(struct MODELREF *Ref, int Many, struct SCRPNTZ *Buffer);
#define gte_mvr(reg, r0) __asm__ volatile("move $" #reg ",%0" : : "r"(r0) : "$" #reg, "memory")
#define gte_op(s) __asm__ volatile(s)
#define gte_st(s) __asm__ volatile(s : : : "memory")
#define gte_ldv0_s(r0) { gte_mvr(12, r0); gte_op("lwc2 $0,0($12)"); gte_op("lwc2 $1,4($12)"); }
#define gte_rtv0_s() { gte_op("nop"); gte_op("nop"); gte_op(".word 0x4A486012"); }
#define gte_stlvnl_s(r0) { gte_mvr(12, r0); \
    gte_st("swc2 $25,0($12)"); gte_st("swc2 $26,4($12)"); gte_st("swc2 $27,8($12)"); }
#define gte_SetTransMatrix_s(r0) { gte_mvr(12, r0); \
    gte_op("lw $13,20($12)"); gte_op("lw $14,24($12)"); gte_op("ctc2 $13,$5"); \
    gte_op("lw $15,28($12)"); gte_op("ctc2 $14,$6"); gte_op("ctc2 $15,$7"); }

static void RelRotVect(struct VECT *P, struct VECT *Q, struct SVECTOR *Rel, struct VECT *Out)
{
    Rel->vx = P->vx - Q->vx;
    Rel->vy = P->vy - Q->vy;
    Rel->vz = P->vz - Q->vz;
    gte_ldv0_s(Rel);
    gte_rtv0_s();
    gte_stlvnl_s(Out);
}

static void RotVect(struct VECT *P, struct VECT *Q, struct VECT *Out)
{
    struct SVECT Rel;
    Rel.vx = P->vx - Q->vx;
    Rel.vy = P->vy - Q->vy;
    Rel.vz = P->vz - Q->vz;
    gte_ldv0_s(&Rel);
    gte_rtv0_s();
    gte_stlvnl_s(Out);
}

/* an anonymous zero SCRPNT in scene.obj's .sdata (a function-local static), which
   the original reaches with lui/addiu %hi/%lo: declared as an array of unknown size
   so that gcc splits the %hi/%lo itself, as in qents.c */


#define Null (*&(struct SCRPNT){0, 0})
int ProcessDoorway(int Obj, int ClipLevel, int Area, int BrushOt)
{
    struct MODELREFINDX *MRef;
    struct SCENE2 *Scn2;
    int Point;
    int AllZClip;
    int NoZClip;
    struct SCRPNTZ StackData[12];
    struct SCRPNTZ *Rotated;
    struct SCRPNT MinMax[2];
    struct SCRPNT SMinMax[2];
    struct SCRPNT *ThisMinMax;
    struct SVECTOR Rel;
    struct VECT Tran;

    Scn2 = &Scene2Data[Obj];
    if (Scn2->NoDraw)
        return AlterScreenClipping(Null, Null, Null, (struct SHORTPAIR){BrushOt, Area}, (struct SHORTPAIR){Obj, ClipLevel});
    MRef = &ModelRefIndx[Obj];
    if (MRef->Many > 12)
        Rotated = (struct SCRPNTZ *)primlistarray;
    else
        Rotated = StackData;
    gte_SetGeomOffset(CurrWin->Geom.sx, CurrWin->Geom.sy);
    RelRotVect(&Scene2Data[Obj].Origin, CurrCamPos, &Rel, &Tran);
    gte_SetTransMatrix_s((struct MATRIX *)((char *)&Tran - 20));
    RotateSceneCoords_(MRef->Ref, MRef->Many, Rotated);
    /* the line table runs 918 (MinMax[0]), 923/924 (AllZClip, NoZClip), 919..921: the
       do {} while (0) loop notes are a sched barrier that keeps the two flags between
       the MinMax[0] and MinMax[1] stores, as in the original */
    do {
        MinMax[0].sx = 0x400;
        MinMax[0].sy = 0x400;
    } while (0);
    AllZClip = 1;
    NoZClip = 1;
    MinMax[1].sx = -0x400;
    MinMax[1].sy = -0x400;
    SMinMax[0] = MinMax[0];
    SMinMax[1] = MinMax[1];
    for (Point = 0; Point < MRef->Many; Point++) {
        if (Rotated[Point].sz) {
            ThisMinMax = MinMax;
            AllZClip = 0;
        } else {
            NoZClip = 0;
            ThisMinMax = SMinMax;
        }
        if (Rotated[Point].sx < ThisMinMax[0].sx)
            ThisMinMax[0].sx = Rotated[Point].sx;
        if (ThisMinMax[1].sx < Rotated[Point].sx)
            ThisMinMax[1].sx = Rotated[Point].sx;
        if (Rotated[Point].sy < ThisMinMax[0].sy)
            ThisMinMax[0].sy = Rotated[Point].sy;
        if (ThisMinMax[1].sy < Rotated[Point].sy)
            ThisMinMax[1].sy = Rotated[Point].sy;
    }
    if (!AllZClip) {
        if (!NoZClip && ((MinMax[0].sx > 0 && MinMax[1].sx < CurrWin->Size.sx) || (MinMax[0].sy > 0 && MinMax[1].sy < CurrWin->Size.sy)))
            return AlterScreenClipping(ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Min, ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Max,
                                       ScreenChanges[CurrWinIndx][ClipLevel].Offset, (struct SHORTPAIR){BrushOt, Area}, (struct SHORTPAIR){Obj, ClipLevel});
    } else {
        struct VECT Location;
        Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
        Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
        Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
        RotVect(&Location, CurrCamPos, &Tran);
        if (Tran.vz >= -200)
            return AlterScreenClipping(ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Min, ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Max,
                                       ScreenChanges[CurrWinIndx][ClipLevel].Offset, (struct SHORTPAIR){BrushOt, Area}, (struct SHORTPAIR){Obj, ClipLevel});
        return AlterScreenClipping(Null, Null, Null, (struct SHORTPAIR){BrushOt, Area}, (struct SHORTPAIR){Obj, ClipLevel});
    }
    if (SMinMax[0].sx < MinMax[0].sx)
        MinMax[0].sx = SMinMax[0].sx;
    if (SMinMax[0].sy < MinMax[0].sy)
        MinMax[0].sy = SMinMax[0].sy;
    if (SMinMax[1].sx > MinMax[1].sx)
        MinMax[1].sx = SMinMax[1].sx;
    if (SMinMax[1].sy > MinMax[1].sy)
        MinMax[1].sy = SMinMax[1].sy;
    SMinMax[0] = ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Min;
    SMinMax[1] = ScreenChanges[CurrWinIndx][ClipLevel].MinMax.Max;
    for (Point = 0; Point < 2; Point++) {
        if (MinMax[Point].sx < SMinMax[0].sx)
            MinMax[Point].sx = SMinMax[0].sx;
        else if (SMinMax[1].sx < MinMax[Point].sx)
            MinMax[Point].sx = SMinMax[1].sx;
        if (MinMax[Point].sy < SMinMax[0].sy)
            MinMax[Point].sy = SMinMax[0].sy;
        else if (SMinMax[1].sy < MinMax[Point].sy)
            MinMax[Point].sy = SMinMax[1].sy;
    }
    return AlterScreenClipping(MinMax[0], MinMax[1], ScreenChanges[CurrWinIndx][ClipLevel].Offset,
                               (struct SHORTPAIR){BrushOt, Area}, (struct SHORTPAIR){Obj, ClipLevel});
}

int PolysAvailable(int NumPolys);
void MallocPolyPackets(short *Dest, int Count);
#define UVP(x) (*(struct UV *)&(x))

/* modgt4 is not in the SYM; its increment is line 2164, and loop.c turns the field accesses
   into one pointer at &modgt4->Tp (the last one in the body) */
int InitBrush(struct SCENE2 *sc2, struct SCENE *scn)
{
    struct POLY_GT4 *pol1g4;
    struct POLY_GT4 *pol2g4;
    short *fp;
    int NumPolys;
    int f;
    struct RGB3 *Rgbs;
    struct UV4 *UV4s;
    struct UV4 *PolyUV4;
    struct GLOBSMODHDR *Hdr;
    struct GLOBSMODGT4 *modgt4;

    pol1g4 = 0;
    pol2g4 = 0;
    Hdr = sc2->ModelHdr;
    NumPolys = Hdr->NumPolys;
    UV4s = (struct UV4 *)((char *)Hdr + Hdr->UVOffset);
    Rgbs = (struct RGB3 *)((char *)Hdr + Hdr->RGBOffset);
    if (NumPolys >= 64 || !PolysAvailable(NumPolys))
        return 0;
    MallocPolyPackets(scn->Pols, NumPolys);
    modgt4 = (struct GLOBSMODGT4 *)(Hdr + 1);
    f = 0;
    for (fp = &scn->Pols[1]; NumPolys > 0; NumPolys--) {
        if (!(f & 31)) {
            pol1g4 = &db[0].modpolsg4[*fp];
            pol2g4 = &db[1].modpolsg4[*fp];
            fp++;
        }
        f++;
        PolyUV4 = &UV4s[modgt4->UV];
        UVP(pol1g4->u0) = PolyUV4->uv[(3 - modgt4->UVRot) & 3];
        UVP(pol1g4->u1) = PolyUV4->uv[(2 - modgt4->UVRot) & 3];
        UVP(pol1g4->u2) = PolyUV4->uv[(0 - modgt4->UVRot) & 3];
        UVP(pol1g4->u3) = PolyUV4->uv[(1 - modgt4->UVRot) & 3];
        *(struct RGB3 *)&pol1g4->r0 = Rgbs[modgt4->Col0];
        *(struct RGB3 *)&pol1g4->r1 = Rgbs[modgt4->Col1];
        *(struct RGB3 *)&pol1g4->r2 = Rgbs[modgt4->Col3];
        *(struct RGB3 *)&pol1g4->r3 = Rgbs[modgt4->Col2];
        pol1g4->clut = GamePals[modgt4->Pal];
        if (modgt4->trans) {
            pol1g4->code = pol2g4->code = 0x3e;
            /* '|= 0' (folded away, no store) gives the loaded value its own register, as in the original */
            pol1g4->tpage = alltpages[modgt4->Tp & 0x1f] |= 0;
        } else {
            pol1g4->code = pol2g4->code = semitranstable[modgt4->trans] | 0x3c;
            pol1g4->tpage = alltpages[modgt4->Tp & 0x1f] |= tptranstable[2];
        }
        pol1g4->tag = pol2g4->tag = 0xc000000;
        *(unsigned short *)&pol2g4->u0 = *(unsigned short *)&pol1g4->u0;
        *(unsigned short *)&pol2g4->u1 = *(unsigned short *)&pol1g4->u1;
        *(unsigned short *)&pol2g4->u2 = *(unsigned short *)&pol1g4->u2;
        *(unsigned short *)&pol2g4->u3 = *(unsigned short *)&pol1g4->u3;
        *(long *)&pol2g4->r0 = *(long *)&pol1g4->r0;
        *(long *)&pol2g4->r1 = *(long *)&pol1g4->r1;
        *(long *)&pol2g4->r2 = *(long *)&pol1g4->r2;
        *(long *)&pol2g4->r3 = *(long *)&pol1g4->r3;
        pol2g4->tpage = pol1g4->tpage;
        pol2g4->clut = pol1g4->clut;
        pol1g4->pad3 = 0;
        pol2g4->pad3 = 0;
        pol1g4->pad2 = pol1g4->clut;
        pol2g4->pad2 = pol1g4->clut;
        pol1g4++;
        pol2g4++;
        modgt4++;
    }
    return 1;
}
