#include "types.h"

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


extern struct MAPNAME *MapNames;

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

void UnLinkCastLists(void)
{
    *CastListLinkAddr = 0;
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

extern void SoundStopOnCreDeath(int, int);
extern void GameFreeCreatureStuff(struct CREATURE *Cre);
void FreeCrePolyPackets(struct CREATURE *Cre);

void KillModel(struct CREATURE *Cre)
{
    if (Cre->SoundID)
        SoundStopOnCreDeath(Cre->SoundID, Cre->SoundChannel);
    GameFreeCreatureStuff(Cre);
    FreeCrePolyPackets(Cre);
    *--CurrFreeCre = Cre;
    Cre->Life = 0;
}

extern struct WINDOW *CurrWin;

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

extern short TotalModelsLoaded;
extern int MaxWindows;
static short ShortTerminator[1];
extern void Relocate(void *Pntr, void *Base);
void BodgeAnimData(struct GLOBMODHDR *Hdr);
void FreePolyPackets(short *Source);

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

void ForEachPolyInModelHdr(struct GLOBMODHDR *Hdr, void *VoidRoutine, int a, int b, int c)
{
    void (*Routine)();
    struct GLOBPOL *GlobPoly;

    Routine = VoidRoutine;
    for (GlobPoly = Hdr->GlobPolys; GlobPoly <= &Hdr->GlobPolys[Hdr->TotalPolys]; GlobPoly++)
        Routine(GlobPoly, a, b, c);
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

#define FT4(p) ((struct POLY_FT4 *)(p))
#define UVS(x) (*(unsigned short *)&(x))

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

void ResetPolyTpageCode(struct GLOBMODHDR *Hdr, struct GLOBPOL *GlobPol, struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB)
{
    PolyA->tpage = PolyB->tpage = alltpages[GlobPol->Tp & 0x1f] | tptranstable[GlobPol->Tp >> 5];
    PolyA->code = PolyB->code = semitranstable[GlobPol->Tp >> 5] | 0x3c;
}

void ProcessPostDraw(void)
{
    struct CREATURE *Cre;

    for (Cre = Creatures; Cre < (struct CREATURE *)CreFreeList; Cre++)
        if (Cre->Life && Cre->PostDraw)
            Cre->PostDraw(Cre);
}

extern struct DB *currdb;
extern struct DB *olddb;

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

extern struct DB *currdb;
extern struct DB *olddb;

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

extern struct DB *currdb;
void FilterInOpaquePolys_(struct POLY_GT4 **FirstLast, struct POLY_GT4 *Last, struct POLY_GT4 *ShadowTable[2][16], int Colour);

void FilterInOpaquePolys(struct MODWIN *WinData, int Opaque)
{
    struct POLY_GT4 *ShadowTable[2][16];
    int Green;
    int Blue;
    int Colour;
    int a;

    Green = Opaque * 129 / 4096;
    Blue = Opaque * 133 / 4096;
    Colour = 0x2e000000 + (Blue << 16) + (Green << 8) + Blue;
    for (a = 0; a < WinData->Pols[0]; a++) {
        ShadowTable[0][a] = currdb->modpolsg4 + WinData->Pols[a + 1];
        ShadowTable[1][a] = currdb->modpolsg4 + WinData->OpaquePols[a + 1];
    }
    FilterInOpaquePolys_(WinData->Sort.FirstLast, WinData->Sort.FirstLast[1], ShadowTable, Colour);
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

extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
extern struct CREATURE **CurrFreeCre;
extern short *FreeListEnd;
extern short *FreeListPntr;
struct GLOBMODHDR **CastListLinkAddr;
struct GLOBMODHDR *CastList;
struct GLOBMODHDR *CommonCastList;
struct GLOBMODHDR *ZoneCastList;

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

void PolyOpaqueSetupSecondary(struct POLY_GT4 *PolyA, struct POLY_GT4 *PolyB, struct POLY_GT4 *PC, struct POLY_GT4 *PD)
{
    PolyA->code = 0x3e;
    PolyA->tpage = (PolyA->tpage & 0x1f) | 0x20;
}

int PolysAvailable(int NumPolys)
{
    return (FreeListEnd - FreeListPntr) * 32 >= NumPolys;
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

void InitCreFreeList(void)
{
    int a;

    CurrFreeCre = CreFreeList;
    for (a = 47; a >= 0; a--)
        CreFreeList[a] = &Creatures[a];
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

extern void Relocate(void *Pntr, void *Base);
char *PolsToFreeListPntr;
char PolsToFreeList[256];

void FreePolyPackets(short *Source)
{
    int Count;

    Count = *Source;
    *Source++ = 0;
    for (; Count > 0; Count--)
        *PolsToFreeListPntr++ = *Source++ / 32;
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

void ReallyFreePolyPackets(void)
{
    char *Data;

    for (Data = PolsToFreeList; Data < PolsToFreeListPntr; Data++)
        *--FreeListPntr = *Data * 32;
    PolsToFreeListPntr = PolsToFreeList;
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

void MallocPolyPackets(short *Dest, int Count)
{
    Count += 31;
    Count /= 32;
    *Dest++ = Count;
    for (; Count > 0; Count--)
        *Dest++ = *FreeListPntr++;
}
