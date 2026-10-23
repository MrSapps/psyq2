#include "types.h"

extern struct WINDOW GameWindows[];
extern unsigned int LevelTicks;
extern int Paused;
static int StopTime;
int HelpComputerOn;
struct IDENTITYNAME LastObjective;
struct IDENTITYNAME LastOrders;

extern int Pauser;
extern int PalNtscRate;
extern struct CREATURE Creatures[48];
int ActivationCountdown;
static int NextDisplayPeriod;
static int AllowButtonExit;
void HelpComputer_SetMessages(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective);
void HelpComputer_StartDisplay(int FramesB4Start, int SecondsToStay);
int GetSoundNum(struct IDENTITYNAME Name);
int SoundAtLocation(int SFXNumber, struct VECT *loc);

void HelpComputer_NewMessage(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective, int FrameDelay)
{
    if (!GameWindows[0].Input.SelfPlayMode) {
        HelpComputer_SetMessages(Orders, Objective);
        if (FrameDelay < 5)
            FrameDelay = 5;
        HelpComputer_StartDisplay(FrameDelay, 15);
    }
}

void HelpComputer_GetMessages(struct IDENTITYNAME *Orders, struct IDENTITYNAME *Objective)
{
    *Orders = LastOrders;
    *Objective = LastObjective;
}

void HelpComputer_StopDisplay(int DoPad)
{
    HelpComputerOn = 0;
    Paused = 0;
    StopTime = LevelTicks;
    if (DoPad) {
        GameWindows[0].Input.Padd &= ~0x4000;
        GameWindows[0].Input.Original &= ~0x4000;
    }
}

int HelpComputer_Reset(void)
{
    LastOrders = *(struct IDENTITYNAME *)"";
    LastObjective = *(struct IDENTITYNAME *)"";
    HelpComputer_StopDisplay(0);
}
