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

char nomoresubevents;
extern int GlobalKey;
void FreeSolidBoxs(struct SOLIDBOX *SolidBox);
int Event_DoFromSub(int eventhdroffset, struct CREATURE *Cre, struct PSXSUBEVENT *subevent2);

struct PSXSUBEVENT *PSXSHOOTTHEN(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct BRUSHMOVEBUF *MoveBuf;

    if (subevent->evsize != 8)
        return subevent;
    if (damage == 0)
        return 0;
    MoveBuf = &BrushMoveBufs[((short *)subevent)[2]];
    ((short *)subevent)[3] -= damage;
    if (((short *)subevent)[3] <= 0) {
        FreeSolidBoxs(MoveBuf->SolidBox);
        MoveBuf->SolidBox = 0;
        Event_GlobalEVOFFSET = MoveBuf->EvHdrOffset;
        Event_DoFromSub(MoveBuf->EvHdrOffset, 0, subevent);
    }
    return 0;
}

void PSXONKEYDO(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_ONKEYDO *ev;
    struct VECT Location;

    ev = (struct PSXEVUSRFUNC_ONKEYDO *)subevent;
    if (ev->KeyAnd && (GlobalKey & ev->KeyAnd) != ev->KeyAnd) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyOr && !(GlobalKey & ev->KeyOr)) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyNotAnd && (~GlobalKey & ev->KeyNotAnd) != ev->KeyNotAnd) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyNotOr && !(~GlobalKey & ev->KeyNotOr)) {
        nomoresubevents = 1;
        return;
    }
}

struct PSXSUBEVENT *PSXPLATFORMEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PLATFORM *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_PLATFORM))
        return subevent;
    ev = (struct PSXEVUSRFUNC_PLATFORM *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    /* assigning back to subevent keeps ev in its own register ($a2) */
    subevent = (struct PSXSUBEVENT *)(ev + 1);
    return subevent;
}
