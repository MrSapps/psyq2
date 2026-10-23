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

extern struct WINDOW GameWindows[];
extern struct DB *currdb;
extern struct VECT *CurrCamPos;
extern struct MATRIX *CurrCamMat;
extern struct COLLHEADER MoveCollMap;
extern short alltpages[];
short *FixedLightList;
struct LIGHTS *FixedLightData;
extern struct WINDOW *CurrWin;
void SetRotMatrix(struct MATRIX *m);
void ProcessLensFlare(struct LIGHTS *Light, unsigned long *Ot);

/* inline_c.h */
#define gte_SetTransMatrix(r0) __asm__ volatile (	\
	"move	$12,%0\n"			\
	"lw	$13,20($12)\n"			\
	"lw	$14,24($12)\n"			\
	"ctc2	$13,$5\n"			\
	"lw	$15,28($12)\n"			\
	"ctc2	$14,$6\n"			\
	"ctc2	$15,$7"				\
	:					\
	: "r"( r0 )				\
	: "$12", "$13", "$14", "$15")

void ProcessLensFlares(int Win)
{
    struct CREATURE *Cre;
    unsigned long *Ot;
    short *LightStop;
    short *LightNumPntr;
    struct LIGHTS *Lighting;

    CurrWin = &GameWindows[Win];
    CurrCamPos = &CurrWin->CamPos;
    CurrCamMat = &CurrWin->CamMat;
    Ot = &currdb->Ot[Win][50];
    SetRotMatrix(CurrCamMat);
    /* splat names this address alltpages+0x2C */
    gte_SetTransMatrix((struct MATRIX *)((char *)alltpages + 0x2C));
    for (Lighting = LightList; Lighting < LightListCurrent; Lighting++)
        if (Lighting->LensFlareType)
            ProcessLensFlare(Lighting, Ot);
    Cre = CurrWin->Creature;
    if (Cre && Cre->Box.MoveSpace >= 0) {
        LightNumPntr = FixedLightList + MoveCollMap.Coll3dSpace[Cre->Box.MoveSpace].StartLight;
        LightStop = FixedLightList + MoveCollMap.Coll3dSpace[Cre->Box.MoveSpace + 1].StartLight;
        for (; LightNumPntr < LightStop; LightNumPntr++)
            if (FixedLightData[*LightNumPntr].LensFlareType)
                ProcessLensFlare(&FixedLightData[*LightNumPntr], Ot);
    }
}

void ProcessTimedLights(void)
{
    int a;
    struct TIMEDLIGHT *TimedLight;

    /* AddLight is inlined; the scaled power goes through two int temporaries (a compound literal
       with the divisions in it builds an extra stack copy) */
    TimedLight = TimedLights;
    for (a = 0; a < 16; a++, TimedLight++) {
        if (TimedLight->Life) {
            int Full, Fall;
            Full = TimedLight->Power.Full * TimedLight->Life / TimedLight->InitLife;
            Fall = TimedLight->Power.Fall * TimedLight->Life / TimedLight->InitLife;
            AddLight(&TimedLight->Light.Location, TimedLight->Light.Colour, (struct POWER){Full, Fall}, (struct SHORTPAIR){0});

            TimedLight->Life--;
        }
    }
}

void ResetModelLights_Many(struct GLOBSMODHDR *ModelHdr, char **Spr, int Many)
{
    struct GLOBSMODGT4 *ModGt4;
    struct POLY_GT4 *SprM[4];
    struct RGB3 *Rgbs;
    short Num;
    short NumPolys;
    char *Temp;
    long a;
    long b;
    long c;
    long d;
    long Count;

    ModGt4 = (struct GLOBSMODGT4 *)(ModelHdr + 1);
    NumPolys = ModelHdr->NumPolys;
    Temp = (char *)ModelHdr;
    Rgbs = (struct RGB3 *)(Temp + ModelHdr->RGBOffset);

    while (NumPolys) {
        Temp = 0;
        for (a = 0; a < Many; a++) {
            Temp += (long)Spr[a];
            SprM[a] = (struct POLY_GT4 *)Temp;
        }
        Spr += 4;
        Num = NumPolys > 32 ? 32 : NumPolys;
        NumPolys -= Num;
        while (Num) {
            *(struct RGB3 *)&SprM[0]->r0 = Rgbs[ModGt4->Col0];
            *(struct RGB3 *)&SprM[0]->r1 = Rgbs[ModGt4->Col1];
            *(struct RGB3 *)&SprM[0]->r2 = Rgbs[ModGt4->Col3];
            *(struct RGB3 *)&SprM[0]->r3 = Rgbs[ModGt4->Col2];
            if (Many >= 2) {
                a = *(long *)&SprM[0]->r0;
                b = *(long *)&SprM[0]->r1;
                c = *(long *)&SprM[0]->r2;
                d = *(long *)&SprM[0]->r3;
                for (Count = 1; Count < Many; Count++) {
                    *(long *)&SprM[Count]->r0 = a;
                    *(long *)&SprM[Count]->r1 = b;
                    *(long *)&SprM[Count]->r2 = c;
                    *(long *)&SprM[Count]->r3 = d;
                    SprM[Count]++;
                }
            }
            ModGt4++;
            Num--;
            SprM[0]++;
        }
    }
}

extern short rcossin_tbl[][2];

void SetLensFlarePointsHex(struct SCRPNT Coord, int Size, struct SCRPNT *Points)
{
    int x0;
    int y0;
    int y1;

    x0 = rcossin_tbl[682][0] * Size * CurrWin->StandardSize.sx / 1310720;
    y0 = rcossin_tbl[682][1] * Size * CurrWin->StandardSize.sy / 983040;
    y1 = Size * CurrWin->StandardSize.sy / 240;
    Points[0] = Coord;
    Points[1] = (struct SCRPNT){Coord.sx, Coord.sy - y1};
    Points[2] = (struct SCRPNT){Coord.sx + x0, Coord.sy - y0};
    Points[3] = (struct SCRPNT){Coord.sx + x0, Coord.sy + y0};
    Points[4] = (struct SCRPNT){Coord.sx, Coord.sy + y1};
    Points[5] = (struct SCRPNT){Coord.sx - x0, Coord.sy + y0};
    Points[6] = (struct SCRPNT){Coord.sx - x0, Coord.sy - y0};
    Points[7] = Points[1];
    Points[8] = Points[2];
}

void SetLensFlarePointsAny(struct SCRPNT Coord, int Size, struct SCRPNT *Points, int NumPoints)
{
    int x;
    int a;
    int AngStep;
    int Ang; /* not in SYM, but the table address is reduced from an angle stepped by AngStep */

    AngStep = 4096 / NumPoints;
    Points[0] = Coord;
    for (a = 0, Ang = 0; a <= NumPoints / 2; a++, Ang += AngStep) {
        x = rcossin_tbl[Ang][0] * Size * CurrWin->StandardSize.sx / 1310720;
        Points[a + 1].sx = Coord.sx + x;
        (&Points[NumPoints + 1])[-a].sx = Coord.sx - x;
        (&Points[NumPoints + 1])[-a].sy = Points[a + 1].sy =
            Coord.sy + rcossin_tbl[Ang][1] * Size * CurrWin->StandardSize.sy / 983040;
    }
    Points[NumPoints + 1] = Points[1];
    Points[NumPoints + 2] = Points[2];
}

extern struct LENSFLARE LensFlare_TypeA[7];
extern struct LENSFLARE LensFlare_TypeB[2];
extern struct LENSFLARE LensFlare_TypeC[3];
extern struct LENSFLARE LensFlare_TypeD[6];
void DrawLensFlares(struct LENSFLARE *LensFlare, struct SCRPNT BaseCoord, int BaseSize, unsigned long *Ot, struct BIGCOLOUR *BigColour);

/* inline_c.h */
#define gte_ldv0(r0) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)"); }
#define gte_rtv0() __asm__ volatile("nop\nnop\n.word 0x4A486012")
#define gte_rtps() __asm__ volatile("nop\nnop\n.word 0x4A180001")
#define gte_stlvnl(r0) __asm__ volatile("move $12,%0\nswc2 $25,0($12)\nswc2 $26,4($12)\nswc2 $27,8($12)" : : "r"(r0) : "$12", "memory")
#define gte_stsxy(r0) __asm__ volatile("move $12,%0\nswc2 $14,0($12)" : : "r"(r0) : "$12", "memory")

void ProcessLensFlare(struct LIGHTS *Light, unsigned long *Ot)
{
    struct SCRPNT Coord;
    struct VECT Tran;
    struct SVECT RelSVect;
    int Bright;
    int Size;
    struct BIGCOLOUR BigColour;
    struct LENSFLARE *LensFlareTable;
    short FlareScale;

    FlareScale = 64 << Light->LensFlareScale;
    switch (Light->LensFlareType) {
    case 1:
        LensFlareTable = LensFlare_TypeA;
        break;
    case 2:
        LensFlareTable = LensFlare_TypeB;
        break;
    case 3:
        LensFlareTable = LensFlare_TypeC;
        break;
    case 4:
        LensFlareTable = LensFlare_TypeD;
        break;
    }
    RelSVect.vx = Light->Location.vx - CurrCamPos->vx;
    RelSVect.vy = Light->Location.vy - CurrCamPos->vy;
    RelSVect.vz = Light->Location.vz - CurrCamPos->vz;
    gte_ldv0(&RelSVect);
    gte_rtv0();
    gte_stlvnl(&Tran);
    if (Tran.vz >= 256) {
        Bright = CalcLightBright(Light, &RelSVect, FlareScale);
        if (Bright) {
            gte_rtps();
            gte_stsxy(&Coord);
            if (Bright >= 0x1000) {
                BigColour.r = Light->Colour.r << 4;
                BigColour.g = Light->Colour.g << 4;
                BigColour.b = Light->Colour.b << 4;
                Size = Bright / 128;
            } else {
                BigColour.r = Bright * Light->Colour.r / 256;
                BigColour.g = Bright * Light->Colour.g / 256;
                BigColour.b = Bright * Light->Colour.b / 256;
                Size = 32;
            }
            DrawLensFlares(LensFlareTable, Coord, Size, Ot, &BigColour);
        }
    }
}

extern struct POLY_G4 *NextGeneralG4;
extern struct POLY_G4 *LastGeneralG4;
void AddPrim_(void *ot, void *p);
void *memset();
#define setLineG2(p) (*(((unsigned char *)(p)) + 3) = 4), (*(((unsigned char *)(p)) + 7) = 0x52)

void DrawLensFlareSparkle(struct SCRPNT Coord, int NumPoints, int Size, unsigned long *Ot, struct COLOUR Colour)
{
    int x;
    int y;
    int a;
    int AngStep;
    struct LINE_G2 *Line;
    int Ang;

    AngStep = 4096 / NumPoints;
    if (NextGeneralG4 + NumPoints > LastGeneralG4)
        return;
    for (a = 0, Ang = 0; a < NumPoints / 2; a++, Ang += AngStep) {
        x = rcossin_tbl[Ang][0] * Size * CurrWin->StandardSize.sx;
        y = rcossin_tbl[Ang][1] * Size * CurrWin->StandardSize.sy;
        if (a & 1) {
            x /= 1120000;
            y /= 840000;
        } else {
            x /= 800000;
            y /= 600000;
        }
        Line = (struct LINE_G2 *)NextGeneralG4;
        *(struct COLOUR *)&Line->r0 = Colour;
        *(struct COLOUR *)&Line->r1 = (struct COLOUR){0, 0, 0, 0x3a};
        *(struct SCRPNT *)&Line->x0 = Coord;
        *(struct SCRPNT *)&Line->x1 = (struct SCRPNT){Coord.sx - x, Coord.sy - y};
        setLineG2(Line);
        AddPrim_(Ot, Line);
        Line = (struct LINE_G2 *)++NextGeneralG4;
        *(struct COLOUR *)&Line->r0 = Colour;
        *(struct COLOUR *)&Line->r1 = (struct COLOUR){0, 0, 0, 0x3a};
        *(struct SCRPNT *)&Line->x0 = Coord;
        *(struct SCRPNT *)&Line->x1 = (struct SCRPNT){Coord.sx + x, Coord.sy + y};
        setLineG2(Line);
        AddPrim_(Ot, Line);
        NextGeneralG4++;
    }
}
