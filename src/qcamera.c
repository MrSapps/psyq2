#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void MoveQuakeCameraInterMission(int Win);
void ResetAllCameras(void);
void UpdateCamerasOnReload(void);
void QGenWindowSortList(struct WINDOW *Window);
void CalcCreCameraTarg(struct CREATURE *Cre);
void QuakeCamera(struct WINDOW *Window, struct CREATURE *Cre);
void QuakeCameraInterMission(struct WINDOW *Window, struct CREATURE *Cre);
void MoveQuakeCamera(int Win);
struct SVECT CalcPlayerCameraModifications(struct CREATURE *Cre);

extern struct WINDOW GameWindows[];
extern int MaxWindows;
extern struct COLLHEADER CollMap;

void QuakeCameraInterMission(struct WINDOW *Window, struct CREATURE *Cre);
void UpdateWindowStuff(struct WINDOW *Win);
void ProcessWaterEffect(struct WINDOW *Window);
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);

extern int NewVcounts;
extern int LevelIsMultiPlayer;
extern int Paused;
extern int ChrisCount;
void QuakeCamera(struct WINDOW *Window, struct CREATURE *Cre);
void QGenWindowSortList(struct WINDOW *Window);
int SeekAng(int Start, int Stop, int Speed);
void DoFrontEndStuff(void);
int MapPadInputToFire(int Player);
void ReInitMultiQuakePlayer(int Win);

extern unsigned int LevelTicks;

extern int NewVcounts;
extern int Mission;
/* static in the original; non-static here so references name the symbol, not .data */
struct INTERMISSIONCAMERA TestCamera[4] = {
    {{-9314, -2562, 10594}, {500, 5490, 0}, 170, 31},
    {{16, -11273, -24059}, {500, 3327, 0}, 81, 27},
    {{-19162, -2748, -11494}, {500, 11464, 0}, 10, 17},
    {{3007, -6122, -17318}, {500, -619, 0}, 8, 15},
};
int SeekAng(int Start, int Stop, int Speed);
void bprintf(struct WINDOW *Window, char *_format);

void MoveQuakeCamera(int Win)
{
    struct WINDOW *Window;
    struct CREATURE *Cre;

    Window = &GameWindows[Win];
    Window->ControlBits |= 7;
    Cre = Window->Creature;
    if (Cre) {
        QuakeCamera(Window, Cre);
    } else {
        Window->CamAng.vz = SeekAng(Window->CamAng.vz, -0x180, NewVcounts * 50);
        if (!LevelIsMultiPlayer)
            DoFrontEndStuff();
        else if (MapPadInputToFire(Win) && !Paused && LevelIsMultiPlayer)
            ReInitMultiQuakePlayer(Win);
    }
    QGenWindowSortList(Window);
    UpdateWindowStuff(Window);
    ProcessWaterEffect(Window);
    if (ChrisCount) {
        Window->SkyClear = 0;
        /* nested ifs: with && gcc merges the vx/vy short compares into one word compare */
        if (Window->OldCamPos.vx == Window->CamPos.vx)
            if (Window->OldCamPos.vy == Window->CamPos.vy)
                if (Window->OldCamPos.vz == Window->CamPos.vz)
                    if (Window->OldCamAng.vx == Window->CamAng.vx)
                        if (Window->OldCamAng.vy == Window->CamAng.vy)
                            if (Window->OldCamAng.vz == Window->CamAng.vz)
                                if (Window->OldSpace == Window->CamSpace)
                                        Window->SkyClear = 1;
    } else
        Window->SkyClear = 1;
    Window->OldCamPos = Window->CamPos;
    Window->OldCamAng = Window->CamAng;
    Window->OldSpace = Window->CamSpace;
}

void MoveQuakeCameraInterMission(int Win)
{
    struct WINDOW *Window = &GameWindows[Win];
    struct CREATURE *Cre;

    Window->ControlBits |= 7;
    Cre = Window->Creature;
    if (Cre && Cre->Health > 0)
        QuakeCameraInterMission(Window, Cre);
    UpdateWindowStuff(Window);
    ProcessWaterEffect(Window);
}

struct SVECT CalcPlayerCameraModifications(struct CREATURE *Cre)
{
    struct WINDOW *Window;
    struct PLAYER *Player;
    struct PTIMERS *PTimers;
    unsigned int Ratio;
    short Pitch;
    short Roll;
    short Yaw;
    int Time;

    Pitch = 0;
    Roll = 0;
    Yaw = 0;
    Player = Cre->Player;
    if (Player) {
        PTimers = &Player->Extras.PTimers;
        Window = Player->Window;
        Time = PTimers->Kick_Timer - LevelTicks;
        if (Time >= 0) {
            Ratio = (Time << 12) / 30;
            if (Ratio) {
                Pitch = Ratio * Player->Extras.Kick_Angles.vx >> 12;
                Yaw = Ratio * Player->Extras.Kick_Angles.vy >> 12;
                Roll = Ratio * Player->Extras.Kick_Angles.vz >> 12;
            } else
                memset(&Player->Extras.Kick_Angles, 0, 6);
        }
        Time = PTimers->Damage_Timer - LevelTicks;
        if (Time >= 0) {
            Ratio = (Time << 12) / 150;
            if (Ratio) {
                Pitch += Ratio * Player->Extras.Dmg_Pitch >> 12;
                Roll += Ratio * Player->Extras.Dmg_Roll >> 12;
            }
        }
        Time = PTimers->Fall_Timer - LevelTicks;
        if (Time >= 0) {
            Ratio = (Time << 12) / 90;
            if (Ratio)
                Pitch += Ratio * Player->Extras.Fall_Value >> 12;
        }
        Window->CamAngOff.vx = Pitch;
        Window->CamAngOff.vy = Yaw;
        Window->CamAngOff.vz = Roll;
    } else {
        /* Window is not set here: the original uses it uninitialised */
        Window->CamAngOff.vz = 0;
        Window->CamAngOff.vy = 0;
        Window->CamAngOff.vx = 0;
    }
    return Window->CamAngOff;
}

void QuakeCamera(struct WINDOW *Window, struct CREATURE *Cre)
{
    struct SVECT AngOff;

    Window->CamPos.vx = Cre->Box.Location.vx;
    Window->CamPos.vy = Cre->Box.Location.vy + (0x11e - Cre->HeadHeight);
    Window->CamPos.vz = Cre->Box.Location.vz;
    if (Cre->Control & 0x80000) {
        Window->CamAng.vz = SeekAng(Window->CamAng.vz, -384, NewVcounts * 50);
        bprintf(GameWindows, "CreDead");
    } else {
        AngOff = Window->CamAngOff;
        Window->CamAng.vx = (Cre->Ang.vx + AngOff.vx) & 0xfff;
        Window->CamAng.vy = -(Cre->Ang.vy + AngOff.vy) & 0xfff;
        Window->CamAng.vz = (Cre->Ang.vz + AngOff.vz) & 0xfff;
    }
}

void QuakeCameraInterMission(struct WINDOW *Window, struct CREATURE *Cre)
{
    struct INTERMISSIONCAMERA *TCamera;
    TCamera = TestCamera;
    TCamera += Mission - 1;
    Window->CamPos.vx = TCamera->CamPos.vx;
    Window->CamPos.vy = TCamera->CamPos.vy;
    Window->CamPos.vz = TCamera->CamPos.vz;
    Window->CamSpace = TCamera->CamSpace;
    Window->Area = TCamera->CamArea;
    if (Cre->Control & 0x80000) {
        Window->CamAng.vz = SeekAng(Window->CamAng.vz, -384, NewVcounts * 50);
        bprintf(GameWindows, "CreDead");
    } else {
        Window->CamAng.vx = TCamera->CamAng.vx;
        Window->CamAng.vy = -TCamera->CamAng.vy;
        Window->CamAng.vz = TCamera->CamAng.vz;
    }
}

void QGenWindowSortList(struct WINDOW *Window)
{
    struct CREATURE *Cre = Window->Creature;

    if (Cre) {
        Window->CamSpace = Find3dSpaceGivenSecondary(&Window->CamPos, &Cre->Box.Location, Cre->Box.MoveSpace);
        if (Window->CamSpace <= 0) {
            Window->CamPos = CollMap.AreaIntersectLast->Intersect;
            Window->CamSpace = CollMap.ReturnSpaceNum;
        }
    } else
        Window->CamSpace = Find3dSpaceIndxLink(&CollMap, &Window->CamPos, Window->CamSpace, 1);
}

void ResetAllCameras(void)
{
    int Win;
    for (Win = 0; Win < MaxWindows; Win++) {
        GameWindows[Win].CamSpace = -1;
        GameWindows[Win].MapOverLoad = 255;
        GameWindows[Win].ModOverLoad = 0;
    }
}

void UpdateCamerasOnReload(void)
{
    ResetAllCameras();
}

void CalcCreCameraTarg(struct CREATURE *Cre)
{
}
