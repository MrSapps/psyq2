#include "types.h"
extern long *primlistarray[];
int GenerateLinearFromLinked(struct SORT *Sort, struct SORT **Original, int Max, char *TypeName);
void GenerateDependancies(struct SORT *Sort, int *Dependancies);
void GenerateQuickDependancies(struct SORT *Sort, struct SORT *QSort, int *QDependancies);
void GenerateSortOrder(struct SORT *Sort, struct SORT *QSort, struct SORT **NewSort, int *Dependancies, int *QDependancies, struct SORT **Original, struct SORT **QOriginal, int NumElements, int QNumElements);

/* name unknown: SYM only shows it inlined into SortObjectsClever */
static void ClearDependancies(int *Dependancies, int NumElements)
{
    int a;

    for (a = 0; a < NumElements; a++)
        Dependancies[a] = 0;
}

void SortObjectsClever(struct SORT *Sort, struct SORT *QSort, struct SORT **NewSort)
{
    struct SORT **Original;
    struct SORT **QOriginal;
    int NumElements;
    int QNumElements;
    int Dependancies[32];
    int QDependancies[128];

    Original = (struct SORT **)primlistarray;
    QOriginal = (struct SORT **)primlistarray + 32;
    NumElements = GenerateLinearFromLinked(Sort, Original, 32, "Standard");
    QNumElements = GenerateLinearFromLinked(QSort, QOriginal, 128, "Quick");
    ClearDependancies(Dependancies, NumElements);
    ClearDependancies(QDependancies, QNumElements);
    GenerateDependancies(Sort, Dependancies);
    GenerateQuickDependancies(Sort, QSort, QDependancies);
    GenerateSortOrder(Sort, QSort, NewSort, Dependancies, QDependancies, Original, QOriginal, NumElements, QNumElements);
}

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

extern short DrawSortObjectsOn;
void AddCross(struct VECT *Pos, int Radius, struct COLOUR Colour);
void DrawVectBox(struct VECTBOX *VectBox);
int CheckDependancies(struct SORT *SortA, struct SORT *SortB);

void OrderedSort(struct SORT *Self, struct SORT **SortLink)
{
    Self->SortLink = *SortLink;
    *SortLink = Self;
    if (DrawSortObjectsOn) {
        if (Self->Flags & 1)
            AddCross(&Self->VectBox->Min, 0x50, (struct COLOUR){ 255, 0, 0 });
        else
            DrawVectBox(Self->VectBox);
    }
}

void GenerateQuickDependancies(struct SORT *Sort, struct SORT *QSort, int *QDependancies)
{
    struct SORT *SortA;
    struct SORT *SortB;
    int ShiftB;
    int a; /* not in SYM: an index eliminated by strength reduction */

    for (SortA = QSort, a = 0; SortA; SortA = SortA->SortLink, a++) {
        for (SortB = Sort, ShiftB = 1; SortB; SortB = SortB->SortLink, ShiftB <<= 1)
            if (CheckDependancies(SortA, SortB))
                QDependancies[a] |= ShiftB;
    }
}

int CheckDependancies(struct SORT *SortA, struct SORT *SortB);
extern struct WINDOW GameWindows[];
void dprintf();

void GenerateDependancies(struct SORT *Sort, int *Dependancies)
{
    struct SORT *SortA;
    struct SORT *SortB;
    int ShiftA;
    int ShiftB;
    int a;
    int b;

    for (SortA = Sort, ShiftA = 1, a = 0; SortA; SortA = SortA->SortLink, ShiftA <<= 1, a++) {
        for (SortB = Sort, ShiftB = 1, b = 0; SortB; ShiftB <<= 1, SortB = SortB->SortLink, b++)
            if (!(Dependancies[b] & ShiftA) && CheckDependancies(SortA, SortB))
                Dependancies[a] |= ShiftB;
    }
}

int GenerateLinearFromLinked(struct SORT *Sort, struct SORT **Original, int Max, char *TypeName)
{
    int NumElements;
    struct SORT **Temp;

    NumElements = 0;
    if (Sort) {
        while (Sort->SortLink && NumElements < Max - 1) {
            Original[NumElements++] = Sort;
            Sort = Sort->SortLink;
        }
        Original[NumElements++] = Sort;
        if (Sort->SortLink) {
            Temp = &Sort->SortLink;
            while (Sort->SortLink) {
                Sort = Sort->SortLink;
                NumElements++;
            }
            dprintf(GameWindows, "Too many %s (%d, %d)", TypeName, NumElements, Max);
            NumElements = Max;
            *Temp = 0;
        }
    }
    return NumElements;
}

extern struct VECT *CurrCamPos;
int VectBoxUnOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB, struct VECTBOX *VBoxC, struct VECTBOX *VBoxD);
int NoDependancyCheckSinglePointBox(int PointA, int *MinMaxB, int Base);
int NoDependancyCheck(int *MinMaxA, int *MinMaxB, int Base);

int CheckDependancies(struct SORT *SortA, struct SORT *SortB)
{
    if (SortA == SortB)
        return 0;
    if (SortA->Flags & 1) {
        if (NoDependancyCheckSinglePointBox(SortA->VectBox->Min.vx, &SortB->VectBox->Min.vx, CurrCamPos->vx))
            return 0;
        if (NoDependancyCheckSinglePointBox(SortA->VectBox->Min.vy, &SortB->VectBox->Min.vy, CurrCamPos->vy))
            return 0;
        return !NoDependancyCheckSinglePointBox(SortA->VectBox->Min.vz, &SortB->VectBox->Min.vz, CurrCamPos->vz);
    } else {
        struct VECTBOX VectBoxA;
        struct VECTBOX VectBoxB;
        struct VECTBOX *VectBoxPA;
        struct VECTBOX *VectBoxPB;

        if (VectBoxUnOverlap(SortA->VectBox, SortB->VectBox, &VectBoxA, &VectBoxB)) {
            VectBoxPA = &VectBoxA;
            VectBoxPB = &VectBoxB;
        } else {
            VectBoxPA = SortA->VectBox;
            VectBoxPB = SortB->VectBox;
        }
        if (NoDependancyCheck(&VectBoxPA->Min.vx, &VectBoxPB->Min.vx, CurrCamPos->vx))
            return 0;
        if (NoDependancyCheck(&VectBoxPA->Min.vy, &VectBoxPB->Min.vy, CurrCamPos->vy))
            return 0;
        return !NoDependancyCheck(&VectBoxPA->Min.vz, &VectBoxPB->Min.vz, CurrCamPos->vz);
    }
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
