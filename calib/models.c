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
