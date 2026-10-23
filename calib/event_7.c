#include "types.h"
extern struct SCENE2 *Scene2Data;
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;

struct PSXSUBEVENT *PSXSETWIBBLE(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SETWIBBLE *SetWibble = (struct PSXEVUSRFUNC_SETWIBBLE *)subevent;
    Scene2Data[SetWibble->PortalNum].SubDiv = SetWibble->WibbleMode;
}

struct PSXSUBEVENT *PSXSETWIBBLE_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SETWIBBLE *SetWibble;
    short *Obj;
    short *Obj2;
    SetWibble = (struct PSXEVUSRFUNC_SETWIBBLE *)subevent;
    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_SETWIBBLE))
        return subevent;
    Obj = &SetWibble->PortalNum;
    Obj2 = (short *)((int)Obj - (int)eventlistbase + (int)neweventlistbase);
    SetWibble->PortalNum = *Obj2;
}



