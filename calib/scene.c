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
/* splat names BrushMoveBufs - 1 as GameWindows + 0xF24 */
#define MOVEBUF(i) ((struct BRUSHMOVEBUF *)((char *)GameWindows + 0xF24) + (i))
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
/* the RGB part of a COLOUR, copied as a 3-byte struct */
struct RGB3 {
    unsigned char r, g, b;
};

