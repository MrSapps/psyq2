#include "types.h"
extern short LaserBeamEvents[][2];
extern struct COLLHEADER CollMap;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;
short Event_GlobalEVOFFSET;
short Event_StartLevFlag;
short NumLaserBeams;

struct PSXSUBEVENT *PSXLASERBEAM(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    int a;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERBEAM))
        return subevent;
    if (Event_StartLevFlag && (((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0.vx & 1) && NumLaserBeams < 32) {
        a = NumLaserBeams++;
        LaserBeamEvents[a][0] = Event_GlobalEVOFFSET;
        LaserBeamEvents[a][1] = (short)((char *)subevent - (char *)eventlistbase);
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERBEAM));
}

struct PSXSUBEVENT *PSXLASERBEAM_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_LASERBEAM *ev2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERBEAM))
        return subevent;
    ev2 = (struct PSXEVUSRFUNC_LASERBEAM *)((int)subevent - (int)eventlistbase + (int)neweventlistbase);
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0.vx = ev2->pos0.vx;
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos1.vx = ev2->pos1.vx;
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->space = Find3dSpaceIndxLink(&CollMap, &((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0, -1, 1);
    if (((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->Type >= 6)
        ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->Type = 0;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERBEAM));
}
