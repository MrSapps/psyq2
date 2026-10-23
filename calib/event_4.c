#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
/* defined in Event.Obj, so $gp-relative */
struct BRUSHMOVEBUF *MoveBuf;

struct PSXSUBEVENT *PSXSIMROTEVENT2(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT2 *ev;
    int a;

    ev = (struct PSXEVUSRFUNC_SIMROT2 *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->crossobj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->crossobj[a]];
            MoveBuf->Triggered = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_SIMROT2));
}

struct PSXSUBEVENT *PSXSIMROTEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT *ev;
    int a;

    ev = (struct PSXEVUSRFUNC_SIMROT *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->crossobj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->crossobj[a]];
            MoveBuf->Triggered = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_SIMROT));
}

struct PSXSUBEVENT *PSXLIFT1EVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_LIFT1 *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LIFT1))
        return subevent;
    ev = (struct PSXEVUSRFUNC_LIFT1 *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    subevent = (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_LIFT1));
    return subevent;
}

