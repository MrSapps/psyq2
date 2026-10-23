#include "types.h"
struct POWERDATA *PowerData;
void Power_NextPump(void);
void Power_CheckPumps(void);
int Event_DoNamed(struct IDENTITYNAME name3, struct CREATURE *Cre);

#define IDNAME(s) ((struct IDENTITYNAME){s})

void Power_SetPump1On(void)
{
    if (!PowerData->TurnedOnPump1) {
        PowerData->TurnedOnPump1 = 1;
        Power_NextPump();
    }
    Power_CheckPumps();
}

void Power_CheckPumps(void)
{
    if (PowerData->TurnedOnPump2 && PowerData->TurnedOnPump1)
        Event_DoNamed(IDNAME("WasteFill"), 0);
}

void Power_SetPump2On(void)
{
    if (!PowerData->TurnedOnPump2) {
        PowerData->TurnedOnPump2 = 1;
        Power_NextPump();
    }
    Power_CheckPumps();
}

void HelpComputer_NewMessage(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective, int FrameDelay);

void Power_NextPump(void)
{
    if (PowerData->TurnedOnPump1 && PowerData->TurnedOnPump2)
        HelpComputer_NewMessage(IDNAME("Unit3Curr4"), IDNAME("Unit3Miss1"), 50);
    else
        HelpComputer_NewMessage(IDNAME("Unit3Curr3"), IDNAME("Unit3Miss1"), 50);
}
