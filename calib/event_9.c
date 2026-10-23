#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern unsigned int usp;
extern int NewVcounts;
/* defined in Event.Obj, so $gp-relative */
struct BRUSHMOVEBUF *MoveBuf;
short numbrushes;
struct PSXMAINEVENTHDR *eventlistbase;
short Event_GlobalEVOFFSET;
short ggg;
int Event_DoFromSub(int eventhdroffset, struct CREATURE *Cre, struct PSXSUBEVENT *subevent2);

int RestoreEvents(void)
{
    usp = SetSp(0x1f8003fc);
    ggg = RestoreEvents2();
    SetSp(usp);
    return ggg;
}

void PrimarySimRot_PDH2(struct BRUSHMOVEBUF *MoveBuf)
{
    short *ang;
    if (MoveBuf->Triggered) {
        ang = (short *)&MoveBuf->Ang;
        ang += MoveBuf->Axis;
        *ang = (*ang + MoveBuf->Speed) & 0xfff;
        MoveBuf->Triggered = 0;
    }
}

void PrimarySimRot_PDH(struct BRUSHMOVEBUF *MoveBuf)
{
    short *ang;
    if (MoveBuf->Triggered) {
        ang = (short *)&MoveBuf->Ang;
        ang += MoveBuf->Axis;
        MoveBuf->PrecisionRange += MoveBuf->Speed * NewVcounts;
        *ang = (MoveBuf->PrecisionRange >> 8) & 0xfff;
        MoveBuf->Triggered = 0;
    }
}






void PSXINLOWCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InLowCrouch = 1;
}

void dofunc_error(void)
{
}

void transfunc_error(void)
{
}

struct PSXSUBEVENT *PSXDOOR1UP(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR1UP *ev_door1up;

    if (subevent->evsize != sizeof(struct PSXEVDOOR1UP))
        return subevent;
    ev_door1up = (struct PSXEVDOOR1UP *)subevent;
    if (damage) {
        ev_door1up->shotdamage -= damage;
        if (ev_door1up->shotdamage > 0)
            return 0;
    }
    if (ev_door1up->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door1up->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door1up->StartDelay * 300;
            if (ev_door1up->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door1up->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1UP));
}
