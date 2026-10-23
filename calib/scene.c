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
