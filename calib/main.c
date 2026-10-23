#include "types.h"

/* defined in dbuffer.obj: externs, so %hi/%lo; the arrays must be incomplete to get the original addressing */
extern unsigned char DbFrame[];
extern unsigned char TbFrame[];
extern struct DB *olddb;
extern struct DB *currdb;
extern struct DB db[2];

void DoubleTrebleBuffer(void)
{
    int Temp;

    Temp = TbFrame[0];
    TbFrame[0] = TbFrame[1];
    TbFrame[1] = TbFrame[2];
    TbFrame[2] = Temp;
    DbFrame[1] = DbFrame[0];
    DbFrame[0] = 1 - DbFrame[0];
    currdb = &db[DbFrame[0]];
    olddb = &db[DbFrame[1]];
}
