#include "types.h"

extern struct IDENTITYNAME ThisLevelName;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
extern short LevelComplete;
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);

struct PSXSUBEVENT *PSXLOADMAP(struct PSXSUBEVENT *subevent)
{
    if (ModNameCmp((struct IDENTITYNAME *)((char *)subevent + 4), &ThisLevelName) != 1) {
        NextLevelName = *(struct IDENTITYNAME *)((char *)subevent + 4);
        MainStartName = *(struct IDENTITYNAME *)((char *)subevent + 0x10);
        LevelComplete = 2;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 0x1c);
}

struct PSXSUBEVENT *PSXLOADMAP_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x1c);
}
