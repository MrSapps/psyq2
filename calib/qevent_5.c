#include "types.h"

struct PSXSUBEVENT *PSXCREBATCH_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 0x10)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXMISEVENT_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 0x10)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXSTRING_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXEXPLO(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x18);
}

struct PSXSUBEVENT *PSXEXPLO_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x18);
}

struct PSXSUBEVENT *PSXHELPCOMPUTER_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x20);
}
