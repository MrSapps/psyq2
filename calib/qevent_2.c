#include "types.h"
extern struct IDENTITYNAME MainStartName;
extern short LevelComplete;

struct PSXSUBEVENT *PSXMISSIONCOMPLETE(struct PSXSUBEVENT *subevent)
{
    MainStartName = *(struct IDENTITYNAME *)"Default";
    LevelComplete = 7;
    return subevent;
}

struct PSXSUBEVENT *PSXSIMBUTTON_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 8)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}
