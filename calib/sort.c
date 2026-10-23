#include "types.h"
extern int MaxWindows;
extern struct DB *currdb;
struct SORT *HeavenlySortLink;
extern char *GetNextGlobalMemory(int Size);
extern void DrawSortedObjects(int Win);
extern void AddPrims(unsigned long *Ot, void *First, void *Last);

void DrawAllSortedObjs(void)
{
    int a;

    for (a = 0; a < MaxWindows; a++)
        DrawSortedObjects(a);
}

void DrawSortedObjectsHeavens(void)
{
    struct SORT *Sort;
    unsigned long *Ot;

    Ot = &currdb->BigOt[10];
    for (Sort = HeavenlySortLink; Sort; Sort = Sort->SortLink)
        AddPrims(Ot, Sort->FirstLast[0], Sort->FirstLast[1]);
    HeavenlySortLink = 0;
}

struct SORT *GetNextSortData(void)
{
    return (struct SORT *)GetNextGlobalMemory(sizeof(struct SORT));
}

void FreeLastSortData(void)
{
}

int NoDependancyCheckSinglePointBox(int PointA, int *MinMaxB, int Base)
{
    int MinB;
    int MaxB;

    MinB = MinMaxB[0];
    MaxB = MinMaxB[3];
    if (MinB >= Base)
        return MinB + 10 >= PointA;
    return Base >= MaxB && PointA >= MaxB - 10;
}

int NoDependancyCheck(int *MinMaxA, int *MinMaxB, int Base)
{
    int MinA;
    int MaxA;
    int MinB;
    int MaxB;

    MinB = MinMaxB[0];
    MinA = MinMaxA[0];
    MaxA = MinMaxA[3];
    MaxB = MinMaxB[3];
    if (MinB >= Base)
        return MinB >= MaxA;
    return Base >= MaxB && MinA >= MaxB;
}
