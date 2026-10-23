#include "types.h"
extern struct WINDOW GameWindows[];
void MissionEvent(struct IDENTITYNAME Name);
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
int ActivateCreatureBatch(struct IDENTITYNAME BatchName);
void centreprintf(struct WINDOW *Window, char *_format, ...);
/* defined in Event.Obj, so $gp-relative */
short Event_MuteLevFlag;
short Event_StartLevFlag;

struct PSXSUBEVENT *PSXSIMBUTTON(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 8)
        return subevent;
    MissionEvent(*(struct IDENTITYNAME *)"Done Dish");
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

struct PSXSUBEVENT *PSXSTRING(struct PSXSUBEVENT *subevent)
{
    char *String;

    if (!Event_MuteLevFlag) {
        String = GetStringFromStringtable(*(struct IDENTITYNAME *)((char *)subevent + 4));
        if (String)
            centreprintf(GameWindows, String);
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXCREBATCH(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_MuteLevFlag)
        ActivateCreatureBatch(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXMISEVENT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_StartLevFlag)
        MissionEvent(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}
