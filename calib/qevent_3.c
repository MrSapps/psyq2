#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[];
/* defined in Event.Obj, so $gp-relative */
struct BRUSHMOVEBUF *MoveBuf;

struct PSXSUBEVENT *PSXROTHATCHEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_ROTHATCH *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_ROTHATCH))
        return subevent;
    ev = (struct PSXEVUSRFUNC_ROTHATCH *)subevent;
    if (ev->crossobj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_ROTHATCH));
}
