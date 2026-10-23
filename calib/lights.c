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

extern unsigned char DbFrame[];
void ResetModelLights_Many(struct GLOBSMODHDR *ModelHdr, char **Spr, int Many);

int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize)
{
    if (LightListCurrent < HeavenlyLightList) {
        LightListCurrent->LensFlareType = FlareTypeSize.a;
        LightListCurrent->LensFlareScale = FlareTypeSize.b;
        LightListCurrent->Location = *Location;
        LightListCurrent->Colour = Colour;
        LightListCurrent->PowerSqrd.Full2 = Power.Full * Power.Full;
        LightListCurrent->PowerSqrd.Fall2 = Power.Fall * Power.Fall;
        LightListCurrent->Radius = Power.Fall;
        LightListCurrent++;
        return 1;
    }
    return 0;
}


static struct TIMEDLIGHT *FindFreeTimedLight(void)
{
    int a;
    for (a = 0; a < 16; a++)
        if (!TimedLights[a].Life)
            return &TimedLights[a];
    return 0;
}

int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life)
{
    struct TIMEDLIGHT *TimedLight;
    TimedLight = FindFreeTimedLight();
    if (TimedLight) {
        TimedLight->Light.Location = *Location;
        TimedLight->Light.Colour = Colour;
        TimedLight->Light.PowerSqrd.Full2 = Power.Full * Power.Full;
        TimedLight->Light.PowerSqrd.Fall2 = Power.Fall * Power.Fall;
        TimedLight->Light.Radius = Power.Fall;
        TimedLight->Power = Power;
        TimedLight->InitLife = Life;
        TimedLight->Life = Life;
        return 1;
    }
    return 0;
}


void UndoMobileLights(void)
{
    struct LIGHTBACK *LightBack;
    for (LightBack = LightBackList2[DbFrame[0]]; LightBack < LightBackEnd2[DbFrame[0]]; LightBack++)
        ResetModelLights_Many(LightBack->Scn2->ModelHdr, LightBack->Spr[0], LightBack->Count);
    LightBackEnd2[DbFrame[0]] = LightBackList2[DbFrame[0]];
}

int CalcLightBright(struct LIGHTS *Light, struct SVECT *Rel, short OverBright)
{
    unsigned long FallStart;
    unsigned long FallEnd;
    unsigned long Dist2;
    unsigned long Numerator;
    unsigned long Denominator;

    FallEnd = (Light->PowerSqrd.Fall2 * OverBright) >> 6;
    Dist2 = Rel->vx * Rel->vx + Rel->vy * Rel->vy + Rel->vz * Rel->vz;
    if (Dist2 < FallEnd) {
        Numerator = FallEnd - Dist2;
        FallStart = (Light->PowerSqrd.Full2 * OverBright) >> 6;
        Denominator = FallEnd - FallStart;
        while (Numerator & 0xfff00000) {
            Numerator >>= 2;
            Denominator >>= 2;
        }
        if (Denominator)
            return (Numerator << 12) / Denominator;
        return 0x1000;
    }
    return 0;
}

void TendToDefaultAmbience(struct CREATURE *Cre, int InverseRate)
{
    short dif;

    if (!InverseRate)
        InverseRate = 1;
    dif = Cre->DefaultAmbience.r - Cre->Ambience.r;
    if (dif < InverseRate)
        Cre->Ambience.r += dif;
    else
        Cre->Ambience.r += dif / InverseRate;
    dif = Cre->DefaultAmbience.g - Cre->Ambience.g;
    if (dif < InverseRate)
        Cre->Ambience.g += dif;
    else
        Cre->Ambience.g += dif / InverseRate;
    dif = Cre->DefaultAmbience.b - Cre->Ambience.b;
    if (dif < InverseRate)
        Cre->Ambience.b += dif;
    else
        Cre->Ambience.b += dif / InverseRate;
}
