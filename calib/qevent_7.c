#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct SCENE2 *Scene2Data;
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;
short Event_GlobalEVOFFSET;
struct BRUSHMOVEBUF *MoveBuf;
void HideObjWithSolid();

struct PSXSUBEVENT *PSXDONTJUMP(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (subevent->evsize != 4)
        return subevent;
    cre->Box.DontJump = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + 4);
}

struct PSXSUBEVENT *PSXDISABLEME(struct PSXSUBEVENT *subevent)
{
    struct PSXSUBEVHDR *eventhdr;

    if (subevent->evsize != 4)
        return subevent;
    eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + Event_GlobalEVOFFSET);
    eventhdr->disabled = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + 4);
}

void PSXOBJDRAWOFF(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_OBJDRAWOFF *ev;
    short a;

    ev = (struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->obj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->obj[a]];
            HideObjWithSolid(MoveBuf->Obj);
        }
    }
}

void PSXOBJDRAWON(struct PSXSUBEVENT *subevent)
{
    int a;

    for (a = 0; a < 4; a++) {
        if (((struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent)->obj[a] >= 0)
            Scene2Data[((struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent)->obj[a]].NoDraw = 0;
    }
}

void PSXPISTON_M(struct BRUSHMOVEBUF *MoveBuf);
int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life);

/* not in the SYM types; layout from the asm */
struct PSXEVTLIGHT {
    unsigned char type;
    unsigned char evsize;
    short unused;
    struct VECT Loc;
    short unused2;
    unsigned short Life;
    struct POWER Power;
    struct COLOUR Colour;
};

struct PSXSUBEVENT *PSXTLIGHT(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVTLIGHT *ev = (struct PSXEVTLIGHT *)subevent;

    AddTimedLight(&ev->Loc, ev->Colour, ev->Power, ev->Life * 3);
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVTLIGHT));
}

struct PSXSUBEVENT *PSXPISTON(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PISTON *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_PISTON))
        return subevent;
    ev = (struct PSXEVUSRFUNC_PISTON *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->Primary = PSXPISTON_M;
            MoveBuf->Time = ev->thrusts;
            MoveBuf->Speed = ev->speed;
            MoveBuf->Range = ev->range;
            MoveBuf->OpenFlag = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_PISTON));
}
