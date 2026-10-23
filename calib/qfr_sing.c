#include "types.h"

extern struct BOTTOMPOLY bottomPoly[];
extern struct QENGINELEVINTERFACE Engine;
extern short targetcdvol;

/* defined in QFr_Sing.Obj, so $gp-relative */
short WaitTime;
char gViolatingPlayer;

short SnapShotPadViolation(short winNum, char *portId);
void killallsounds(void);
void Config_InstallVideo(void);
void InGame_GOTO_PAUSEDBACK(void);
void InGame_GOTO_PAUSED(void);
void InGame_GOTO_DIED(void);
void CHECK_FOR_SLIDER(void);
void CHECK_FOR_MCFILEWINDOW(void);

void MoveBottomPoly(short polyNum, short y)
{
    bottomPoly[polyNum].idealY = y - 8;
}

void FrontEndResetVideoDefaults(void)
{
    Engine.GameConfig.bSplitHoriz = 1;
    Engine.GameConfig.sScreenPosX = 0;
    Engine.GameConfig.sScreenPosY = 0;
    Config_InstallVideo();
}

void FrontEndResetSoundDefaults(void)
{
    Engine.GameConfig.bStereo = 1;
    Engine.GameConfig.sMusic = 0x30;
    Engine.GameConfig.sSoundFX = 0x60;
    targetcdvol = 0x60;
}

void CHECK_FOR_SLIDERANDSCROLL(void)
{
    CHECK_FOR_SLIDER();
    CHECK_FOR_MCFILEWINDOW();
}

void FrontEndCheckForReinsert(void)
{
    char dummyPortId;

    if (!SnapShotPadViolation(gViolatingPlayer, &dummyPortId))
        InGame_GOTO_PAUSED();
}

void InGame_GOTO_PAUSED(void)
{
    killallsounds();
    InGame_GOTO_PAUSEDBACK();
}

void InGame_GOTO_DIED2(void)
{
    InGame_GOTO_DIED();
    WaitTime = 600;
}
