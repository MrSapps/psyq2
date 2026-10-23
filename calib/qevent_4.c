#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern unsigned int LevelTicks;
extern short LaserWallEvents[][2];
void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType, struct VECT *Centre);
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;
short Event_GlobalEVOFFSET;
short Event_StartLevFlag;
short NumLaserWalls;
struct BRUSHMOVEBUF *MoveBuf;
static struct BRUSHMOVEBUF *GirderFallMB;
static struct BRUSHMOVEBUF *GirderRotateMB;

struct PSXSUBEVENT *PSXBUTTON(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_BUTTON *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_BUTTON))
        return subevent;
    ev = (struct PSXEVUSRFUNC_BUTTON *)subevent;
    if (ev->crossobj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            if (ev->range >= 0) {
                MoveBuf->Range = ev->range;
                MoveBuf->Speed = 1;
            } else {
                MoveBuf->Range = -ev->range;
                MoveBuf->Speed = -1;
            }
            MoveBuf->Time = ev->time * 300;
            if (*(signed char *)&ev->axis)
                MoveBuf->Range = -MoveBuf->Range;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_BUTTON));
}

struct PSXSUBEVENT *PSXLASERWALL_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    short *shortpnt;
    short *shortpnt2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERWALL))
        return subevent;
    shortpnt = &((struct PSXEVUSRFUNC_LASERWALL *)subevent)->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0)
        *shortpnt = *shortpnt2;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERWALL));
}

struct PSXSUBEVENT *PSXLASERWALL(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    int a;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERWALL))
        return subevent;
    if (((struct PSXEVUSRFUNC_LASERWALL *)subevent)->crossobj >= 0) {
        if (Event_StartLevFlag) {
            if (NumLaserWalls < 8) {
                a = NumLaserWalls++;
                LaserWallEvents[a][0] = Event_GlobalEVOFFSET;
                LaserWallEvents[a][1] = (short)((char *)subevent - (char *)eventlistbase);
            }
        } else {
            DoDamage(Cre, Cre, ((struct PSXEVUSRFUNC_LASERWALL *)subevent)->damage, 11, &Cre->Box.Location);
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERWALL));
}

/* not in the SYM types; layout from the asm */
struct PSXEVDISH {
    unsigned char type;
    unsigned char evsize;
    unsigned char func4;
    unsigned char unused;
    signed char axis;
    signed char speed;
    short obj;
};

struct PSXSUBEVENT *PSXDISH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVDISH *ev = (struct PSXEVDISH *)subevent;

    if (subevent->evsize != sizeof(struct PSXEVDISH))
        return subevent;
    if (ev->obj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->obj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            MoveBuf->Range = ev->speed * 32;
            MoveBuf->Speed = 1;
            MoveBuf->Time = LevelTicks + 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

struct PSXSUBEVENT *PSXB3ROCKS(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_B3ROCKS *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_B3ROCKS))
        return subevent;
    ev = (struct PSXEVUSRFUNC_B3ROCKS *)subevent;
    if (ev->RockObj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->RockObj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            MoveBuf->Time = LevelTicks + 300;
        }
        GirderFallMB = &BrushMoveBufs[ev->GirderFallObj];
        GirderRotateMB = &BrushMoveBufs[ev->GirderRotateObj];
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_B3ROCKS));
}

struct PSXSUBEVENT *PSXCAGELIFT1EVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_CAGELIFT1 *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_CAGELIFT1))
        return subevent;
    ev = (struct PSXEVUSRFUNC_CAGELIFT1 *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (!MoveBuf->OpenFlag) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 0xff)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    subevent = (struct PSXSUBEVENT *)(ev + 1);
    return subevent;
}
