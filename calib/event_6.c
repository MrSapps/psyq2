#include "types.h"
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;

struct PSXSUBEVENT *PSXFLICKERINGLIGHT_T(struct PSXSUBEVENT *subevent)
{
    struct VECT *Loc;
    struct VECT *Loc2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT))
        return subevent;
    Loc = &((struct PSXEVUSRFUNC_FLICKERINGLIGHT *)subevent)->Location;
    Loc2 = (struct VECT *)((int)Loc - (int)eventlistbase + (int)neweventlistbase);
    ((struct PSXEVUSRFUNC_FLICKERINGLIGHT *)subevent)->Location = *Loc2;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT));
}
