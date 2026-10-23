#include "types.h"

/* defined in this file (Quake.obj), so $gp-relative */
int Paused;

int GameIsPaused(void)
{
    return Paused;
}

/* also defined in Quake.obj */
struct LINE_F2 *NextQuakeLine;
struct LINE_F2 *LastQuakeLine;
struct LEVELDATA QuakeLevelData[52];
struct GAMEDB QuakeGameBuffs[2];

extern struct DB *currdb;
extern struct DB db[];
extern short LevelComplete;
extern struct LEVELDATA *GlobalLevelData;
extern struct LEVELDATA *ThisLevelData;
extern int NextMission;
extern int NewMission;
extern int MaxWindows;
extern struct WINDOW GameWindows[];

void QEntsEveryLevel(void);
void FreePathTList(void);
void FreeEdictTList(void);
void KillDualShocks(void);
void DeleteAllMatchingMemory(int type);

char GetCurrentLevelNumber(void)
{
    return ThisLevelData - QuakeLevelData;
}

void GameEveryFrame(void)
{
    NextQuakeLine = &currdb->GameStuff->QuakeLines[0];
    LastQuakeLine = &currdb->GameStuff->QuakeLines[4];
}

void GameLevelStuff(void)
{
    QEntsEveryLevel();
}

void GameTerminateStuff(void)
{
    FreePathTList();
    FreeEdictTList();
}

void GameNewGame(void)
{
    KillDualShocks();
    LevelComplete = 6;
}

void SetupGameSpecific(void)
{
    GlobalLevelData = QuakeLevelData;
    db[0].GameStuff = &QuakeGameBuffs[0];
    db[1].GameStuff = &QuakeGameBuffs[1];
}

void GameReRunMPLevel(void)
{
    NextMission = 7;
    NewMission = 0;
}

void GameQuitGame(void)
{
    LevelComplete = 6;
    DeleteAllMatchingMemory(3);
}

void KillDualShocks(void)
{
    int Win;
    int Slot;

    for (Win = 0; Win < MaxWindows; Win++)
        for (Slot = 0; Slot < 4; Slot++)
            GameWindows[Win].Input.ShockSlots[Slot].ShockData = 0;
}
