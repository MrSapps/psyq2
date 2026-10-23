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

extern struct WINDOW GameWindows[];
extern struct DB *currdb;
extern struct VECT *CurrCamPos;
void AddPrims(unsigned long *Ot, void *First, void *Last);
void SortObjectsClever(struct SORT *Sort, struct SORT *QSort, struct SORT **NewSort);
extern struct AREA AreaData[];
extern unsigned long *CurrOt;
int AreaValidPntr(int Win, struct AREA *Area);

void DrawSortedObjects(int Win)
{
    struct AREA *Area;
    struct SORT *Sort;
    struct SORT *QSort;
    struct SORT *NewSort;
    unsigned long *Ot;

    Area = &AreaData[1];
    Sort = Area->Data[Win].SortLink;
    QSort = Area->Data[Win].QuickSortLink;
    CurrCamPos = &GameWindows[Win].CamPos;
    CurrOt = currdb->Ot[Win];
    Ot = &currdb->Ot[Win][46];
    if (Sort || QSort) {
        for (; Sort; Sort = Sort->SortLink)
            AddPrims(Ot, Sort->FirstLast[0], Sort->FirstLast[1]);
        for (; QSort; QSort = QSort->SortLink)
            AddPrims(Ot, QSort->FirstLast[0], QSort->FirstLast[1]);
    }
    Area->Data[Win].SortLink = 0;
    Area->Data[Win].QuickSortLink = 0;

    Area = &AreaData[2];
    Sort = Area->Data[Win].SortLink;
    QSort = Area->Data[Win].QuickSortLink;
    Ot = CurrOt + 1;
    if (Sort || QSort) {
        SortObjectsClever(Sort, QSort, &NewSort);
        for (; NewSort; NewSort = NewSort->SortLink)
            AddPrims(Ot, NewSort->FirstLast[0], NewSort->FirstLast[1]);
    }
    Area->Data[Win].SortLink = 0;
    Area->Data[Win].QuickSortLink = 0;

    for (Area = &AreaData[3]; Area < &AreaData[96]; Area++) {
        if (AreaValidPntr(Win, Area)) {
            Sort = Area->Data[Win].SortLink;
            QSort = Area->Data[Win].QuickSortLink;
            Ot = CurrOt + Area->Data[Win].ScreenChange->OtLevel;
            if (Sort || QSort) {
                SortObjectsClever(Sort, QSort, &NewSort);
                for (; NewSort; NewSort = NewSort->SortLink)
                    AddPrims(Ot, NewSort->FirstLast[0], NewSort->FirstLast[1]);
            }
        }
        Area->Data[Win].SortLink = 0;
        Area->Data[Win].QuickSortLink = 0;
    }
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


int VectBoxUnOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB, struct VECTBOX *VBoxC, struct VECTBOX *VBoxD)
{
    int WhichAxis;
    int MinDist;
    int Temp;
    int *BoxA;
    int *BoxB;
    int a;

    WhichAxis = 6; MinDist = 0x7FFFFFFF;

    BoxA = (int *)VBoxA;
    BoxB = (int *)VBoxB;

    for (a = 0; a < 3; a++) {
        if (BoxA[a] >= BoxB[a + 3])
            return 0;
        if (BoxA[a + 3] <= BoxB[a])
            return 0;
    }
    for (a = 0; a < 3; a++) {
        if (BoxA[a + 3] > BoxB[a + 3]) {
            Temp = BoxB[a + 3] - BoxA[a];
            if (Temp < MinDist) {
                MinDist = Temp;
                WhichAxis = a * 2;
            }
        } else if (BoxA[a] < BoxB[a]) {
            Temp = BoxA[a + 3] - BoxB[a];
            if (Temp < MinDist) {
                MinDist = Temp;
                WhichAxis = a * 2 + 1;
            }
        }
    }
    if (WhichAxis == 6)
        return 0;
    a = WhichAxis / 2;
    *VBoxC = *VBoxA;
    *VBoxD = *VBoxB;
    BoxA = (int *)VBoxC;
    BoxB = (int *)VBoxD;
    if (!(WhichAxis & 1)) {
        BoxA[a] += MinDist / 2;
        BoxB[a + 3] = BoxA[a];
    } else {
        BoxA[a + 3] = BoxB[a] += MinDist / 2;
    }
    return 1;
}

void GenerateSortOrder(struct SORT *Sort, struct SORT *QSort, struct SORT **NewSort, int *Dependancies, int *QDependancies, struct SORT **Original, struct SORT **QOriginal, int NumElements, int QNumElements)
{
    int Indx;
    int QIndx;
    int Count;
    int SinceLastAdd;
    int NumLeft;
    int QNumLeft;
    int ZVal;
    int ShiftVal;
    int QShiftVal;
    int Added;
    int Temp;
    int QAddedT[4];
    int *QAddedP;
    int a;
    struct SORT *WinToAdd;
    struct SORT **FirstThisPass;
    struct SORT **Compare;
    struct SORT *TempLink;

    Added = 0;
    for (a = 3; a >= 0; a--)
        QAddedT[a] = 0;
    Indx = 0;
    ShiftVal = 1;
    NumLeft = NumElements;
    QNumLeft = QNumElements;
    SinceLastAdd = 0;
    while (NumLeft || QNumLeft) {
        if (QNumLeft) {
            *NewSort = 0;
            QShiftVal = 1;
            QAddedP = QAddedT;
            FirstThisPass = NewSort;
            for (QIndx = 0; QIndx < QNumElements; QIndx++) {
                if (!(*QAddedP & QShiftVal) && !QDependancies[QIndx]) {
                    *QAddedP |= QShiftVal;
                    WinToAdd = QOriginal[QIndx];
                    ZVal = WinToAdd->ScreenZ;
                    Compare = FirstThisPass;
                    QNumLeft--;
                    while (*Compare) {
                        if (ZVal < (*Compare)->ScreenZ) {
                            TempLink = *Compare;
                            *Compare = WinToAdd;
                            WinToAdd->SortLink = TempLink;
                            break;
                        }
                        Compare = &(*Compare)->SortLink;
                    }
                    if (!*Compare) {
                        TempLink = *Compare;
                        NewSort = (struct SORT **)WinToAdd;
                        *Compare = WinToAdd;
                        *NewSort = TempLink;
                    }
                }
                QShiftVal <<= 1;
                if (!QShiftVal) {
                    QShiftVal = 1;
                    QAddedP++;
                }
            }
        }
        if (NumLeft) {
            if (!(Added & ShiftVal)) {
            if (!Dependancies[Indx]) {
                Temp = ~ShiftVal;
                *NewSort = Original[Indx];
                NewSort = (struct SORT **)*NewSort;
                for (Count = 0; Count < NumElements; Count++)
                    Dependancies[Count] &= Temp;
                for (Count = 0; Count < QNumElements; Count++)
                    QDependancies[Count] &= Temp;
                NumLeft--;
                SinceLastAdd = 0;
                Added |= ShiftVal;
            } else {
                SinceLastAdd++;
            }
            }
            Indx++;
            ShiftVal <<= 1;
            if (Indx == NumElements) {
                Indx = 0;
                ShiftVal = 1;
            }
            if (NumLeft && SinceLastAdd == NumLeft) {
                dprintf(GameWindows, "Possible dependancy loop\n");
                {
                    int ShiftValB;
                    int ShiftVal;
                    int MinIndx;
                    int MinCount;
                    int a;
                    int Count;
                    int b;

                    MinCount = 33;
                    MinIndx = 32;
                    ShiftVal = 1;
                    for (b = 0; b < NumElements; b++, ShiftVal <<= 1) {
                        if (!(Added & ShiftVal)) {
                            ShiftValB = 1;
                            Count = 0;
                            for (a = 0; a < NumElements; a++) {
                                if (Dependancies[b] & ShiftValB)
                                    Count++;
                                ShiftValB <<= 1;
                            }
                            if (Count < MinCount) {
                                MinCount = Count;
                                MinIndx = b;
                            }
                        }
                    }
                    if (MinIndx == 32) {
                        dprintf(GameWindows, "Can't break dependency loop\n");
                        Indx = -1;
                    } else {
                        Dependancies[MinIndx] = 0;
                        Indx = MinIndx;
                    }
                }
                ShiftVal = 1 << Indx;
                SinceLastAdd--;
                if (Indx == -1)
                    break;
            }
        }
    }
    *NewSort = 0;
}
