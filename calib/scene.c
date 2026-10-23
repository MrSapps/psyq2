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
