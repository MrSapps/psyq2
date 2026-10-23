#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;

int Event_GetNamedOffset(struct IDENTITYNAME name3)
{
    int *name2;
    int *name;
    name2 = (int *)&name3;
    name = (int *)eventlistbase + 1;
    while (*name) {
        if (name[0] == name2[0] && name[1] == name2[1] && name[2] == name2[2])
            return name[3];
        name += 4;
    }
    return 0;
}

void ProcessMoveBufs(void)
{
    struct BRUSHMOVEBUF *MoveBuf;
    for (MoveBuf = BrushMoveBufs; MoveBuf < &BrushMoveBufs[48]; MoveBuf++)
        if (MoveBuf->Primary)
            MoveBuf->Primary(MoveBuf);
}
