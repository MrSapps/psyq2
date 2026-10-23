#include "types.h"
/* defined in Event.Obj, so $gp-relative */
short NumLaserBeams;
struct PSXMAINEVENTHDR *eventlistbase;
extern short LaserBeamEvents[][2];
struct CREATURE *AddLaser(struct VECT *Start, struct VECT *Stop, short Space, short LaserType, struct SHORTPAIR Parts);

void DrawLaserBeams(void)
{
    short a;
    struct PSXEVUSRFUNC_LASERBEAM *ev;
    struct PSXSUBEVHDR *eventhdr;

    for (a = 0; a < NumLaserBeams; a++) {
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + LaserBeamEvents[a][0]);
        if (!eventhdr->disabled) {
            ev = (struct PSXEVUSRFUNC_LASERBEAM *)((char *)eventlistbase + LaserBeamEvents[a][1]);
            AddLaser(&ev->pos0, &ev->pos1, ev->space, ev->Type, (struct SHORTPAIR){0});
        }
    }
}
