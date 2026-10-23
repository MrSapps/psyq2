#include "types.h"
extern struct BLURHDR BlurHdrs[];
extern char BlurFreeList[];
extern struct BLURPOINT BlurPointsA[];
/* defined in Blur.obj, so $gp-relative */
char *BlurFreeListPntr;
struct BLURPOINT *BlurPoints;

void InitBlurPoints(void)
{
    int a;
    memset(BlurHdrs, 0, sizeof(struct BLURHDR) * 6);
    for (a = 0; a < 96; a++)
        BlurFreeList[a] = a + 1;
    /* BlurPointsA - 1 and BlurFreeList + 96: splat names them by the neighbouring symbol */
    BlurPoints = (struct BLURPOINT *)(BlurFreeList + 0x58);
    BlurFreeListPntr = (char *)BlurPointsA;
}
