#include "types.h"
extern int ChrisCount;
extern int MaxWindows;
extern struct LIGHTBACK *LightBackEnd2[];
struct LIGHTBACK LightBackList2[2][128];
struct LIGHTS LightList[16];
struct LIGHTS *LightListCurrent;
struct LIGHTS HeavenlyLightList[3];
struct LIGHTS *HeavenlyLightListCurrent;
int FlareType;
short Conv[256];
struct TIMEDLIGHT TimedLights[16];
void ProcessTimedLights(void);
void ProcessMobileLights(void);
void ProcessLensFlares(int Win);
void UpdateSubdivisionLighting(void);

void ResetLightBackList(void);

void InitLights(void)
{
    int a;
    int b;

    LightBackEnd2[0] = LightBackList2[0];
    LightBackEnd2[1] = LightBackList2[1];
    for (a = 0; a < 64; a++)
        for (b = 0; b < 4; b++)
            Conv[a * 4 + b] = (a & 31) * 52 + b * 12 + 4;
    for (a = 15; a >= 0; a--)
        TimedLights[a].Life = 0;
}

void ProcessLights(void)
{
    ProcessTimedLights();
    ProcessMobileLights();
    LightListCurrent = LightList;
    HeavenlyLightListCurrent = HeavenlyLightList;
    UpdateSubdivisionLighting();
}

void ProcessAllLensFlare(void)
{
    int Win;

    if (FlareType)
        for (Win = 0; Win < MaxWindows; Win++)
            ProcessLensFlares(Win);
}

void InitialiseLightEveryFrame(void)
{
    if (ChrisCount == 1) {
        LightListCurrent = LightList;
        HeavenlyLightListCurrent = HeavenlyLightList;
    }
}

void ResetLightBackList(void)
{
    LightBackEnd2[0] = LightBackList2[0];
    LightBackEnd2[1] = LightBackList2[1];
}
