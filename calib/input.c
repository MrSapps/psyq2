#include "types.h"

extern struct WINDOW GameWindows[];
extern int MaxWindows;
extern int ChrisCount;
extern struct IDENTITYNAME MainStartName;
int LastInputTime;
int NoInputTime;
int (*NoInputTimeOutRoutine)();
extern short MultiTapExists[];
struct ControllerPacket padbuffer1;
struct ControllerPacket padbuffer2;
struct SNAPSHOT SnapShot[4];
struct HHPADD PaddData[8];

void PadDualShock(int Win, struct HHPADD *Padds);
int NewReadPort(int Port, struct ControllerPacket *PadBuffer, struct HHPADD *Padds);
void SwapButtons(int Win);
void WriteOutRecordData(int Win);
void *MY_malloc_name(int s, char *name);
void FrontEndResetPlayerDefaults(void);
void FrontEndResetSoundDefaults(void);
void FrontEndResetVideoDefaults(void);
void Config_InstallVideo(void);

void ReadPaddHardware(void)
{
    int a;
    MultiTapExists[0] = (padbuffer1.dataFormat >> 4) == 8;
    MultiTapExists[1] = (padbuffer2.dataFormat >> 4) == 8;
    for (a = 7; a >= 0; a--)
        PaddData[a].CType = 0;
    NewReadPort(0, &padbuffer1, PaddData);
    NewReadPort(0x10, &padbuffer2, &PaddData[4]);
}

void SwapAllButtons(void)
{
    int Win;
    int TimeoutTime;
    for (Win = 0; Win < 4; Win++)
        SwapButtons(Win);
    if (NoInputTimeOutRoutine) {
        TimeoutTime = LastInputTime + NoInputTime;
        if (ChrisCount >= TimeoutTime)
            NoInputTimeOutRoutine();
    }
}

void SelfRecordOn(int Win, int Size)
{
    struct CTRLER *Input = &GameWindows[Win].Input;
    Input->SelfPlayMode = 2;
    Input->SelfPlayBasePntr = Input->SelfPlayPntr = MY_malloc_name(Size * 2, "Record Space");
    if (Input->SelfPlayPntr == 0) {
        Input->SelfPlayMode = 0;
        return;
    }
    Input->SelfPlayEndPntr = Input->SelfPlayBasePntr + Size;
}

void SelfPlayOn(int Win, short *Pntr, int (*SelfPlayComplete)())
{
    struct CTRLER *Input;
    FrontEndResetPlayerDefaults();
    FrontEndResetSoundDefaults();
    FrontEndResetVideoDefaults();
    Config_InstallVideo();
    MainStartName = *(struct IDENTITYNAME *)"Default";
    Input = &GameWindows[Win].Input;
    Input->SelfPlayMode = 1;
    Input->SelfPlayPntr = Pntr;
    Input->SelfPlayBasePntr = Pntr;
    Input->SelfPlayComplete = SelfPlayComplete;
}

void SelfPlayRecOff(int Win)
{
    if (GameWindows[Win].Input.SelfPlayMode == 2)
        WriteOutRecordData(Win);
    GameWindows[Win].Input.SelfPlayMode = 0;
}

short *SetNoInputTimeOutRoutine(int (*NoInputRoutine)())
{
    NoInputTimeOutRoutine = NoInputRoutine;
}

int IsPlayerPadShockable(int Player)
{
    struct HHPADD *Padds = &PaddData[SnapShot[Player].HHPadId];
    return Padds->IsShockable;
}

void DualShockStuff(void)
{
    int a;
    for (a = 0; a < MaxWindows; a++)
        PadDualShock(a, &PaddData[SnapShot[a].HHPadId]);
}
