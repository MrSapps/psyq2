#include "types.h"

extern struct WINDOW GameWindows[];
extern int MaxWindows;
extern struct COLLHEADER CollMap;

void QuakeCameraInterMission(struct WINDOW *Window, struct CREATURE *Cre);
void UpdateWindowStuff(struct WINDOW *Win);
void ProcessWaterEffect(struct WINDOW *Window);
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);

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

void CalcCreCameraTarg(struct CREATURE *Cre)
{
}

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
