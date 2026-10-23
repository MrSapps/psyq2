#include "types.h"
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
