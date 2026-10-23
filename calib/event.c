#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
/* defined in Event.Obj, so $gp-relative */
struct BRUSHMOVEBUF *MoveBuf;

struct PSXSUBEVENT *PSXDOOR2SIDE(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR2SIDE *ev_door2side;

    if (subevent->evsize != sizeof(struct PSXEVDOOR2SIDE))
        return subevent;
    ev_door2side = (struct PSXEVDOOR2SIDE *)subevent;
    if (ev_door2side->crossobjl[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door2side->crossobjl[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door2side->StartDelay * 300;
            if (ev_door2side->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door2side->EndDelay * 300;
        }
    }
    if (ev_door2side->crossobjr[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door2side->crossobjr[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door2side->StartDelay * 300;
            if (ev_door2side->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door2side->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR2SIDE));
}
