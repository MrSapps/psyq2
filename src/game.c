#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void InitGameEveryFrame(void);
char *GetNextGlobalMemory(int Size);
void GameCheckTimeout(void);
void MissionEvent(struct IDENTITYNAME Name);
extern struct WINDOW GameWindows[];
extern void (*VBlankSpecific)();
extern int RealVCounts;
extern short LevelComplete;
void dprintf(struct WINDOW *Window, char *_format, ...);
/* defined in Game.obj, so $gp-relative */
char *GameEveryFrameMemory;
char *GameEveryFramePntr;
char *GameEveryFrameLast;
static int GameNumAllocs;
static int GameNumFails;
unsigned int GameTimeOutTime;
unsigned int GameTime;
int GameRunningMode;
unsigned int GameNoInputTime;

struct MISSIONEVENT *MissionEvents;
extern struct QENGINELEVINTERFACE Engine;
char *PrintIdentityName(struct IDENTITYNAME Name);
void *FindIdentNameInList(struct IDENTITYNAME *Name, void *VPntr, int NumBytes);

void GameCheckTimeout(void)
{
    if (GameTimeOutTime) {
        if (!VBlankSpecific)
            GameTime += RealVCounts;
        if (GameRunningMode != 3 && GameWindows[0].Input.Original)
            GameTimeOutTime = GameTime + GameNoInputTime;
        if (GameTimeOutTime < GameTime) {
            LevelComplete = 8;
            GameRunningMode = 3;
            GameTimeOutTime = 0;
        }
    }
}

void MissionEvent(struct IDENTITYNAME Name)
{
    struct MISSIONEVENT *Event;
    struct MISSIONEVENT *List;
    int a;

    for (a = 0; a < 2; a++) {
        if (a == 0)
            List = MissionEvents;
        if (a == 1)
            List = Engine.LevelEvents;
        if (List) {
            Event = FindIdentNameInList(&Name, List, sizeof(struct MISSIONEVENT));
            if (Event) {
                dprintf(GameWindows, "Found %s\n", PrintIdentityName(Name));
                if (Event->Routine)
                    Event->Routine();
                else
                    dprintf(GameWindows, "No Routine for%s\n", PrintIdentityName(Name));
            } else if (a == 1) {
                dprintf(GameWindows, "Cant find %s\n", PrintIdentityName(Name));
            }
        }
    }
}

void InitGameEveryFrame(void)
{
    GameNumAllocs = 0;
    GameNumFails = 0;
    GameEveryFramePntr = GameEveryFrameMemory;
}

char *GetNextGlobalMemory(int Size)
{
    char *p;

    if (GameEveryFramePntr + Size < GameEveryFrameLast) {
        p = GameEveryFramePntr;
        GameEveryFramePntr += Size;
        GameNumAllocs++;
        return p;
    }
    GameNumFails++;
    dprintf(GameWindows, "Unable to allocate %d Global Memory\n", Size);
    dprintf(GameWindows, "GameEFP=%8x, GameEFL=%8x\n", GameEveryFramePntr, GameEveryFrameLast);
    return 0;
}
