#include "ovl.h"

#define Engine LevelInterface.Engine
#define IDNAME(s) ((struct IDENTITYNAME){s})

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* small functions: gcc emits them at the end of the file in this order */
struct CREATURE *FindCreature(struct IDENTITYNAME Name);
static void LaserButton0(void);
static void LaserButton1(void);
static void Stairs0(void);
static void Stairs1(void);
void LevelNull(void);

static void Zone0Load(void);
static void Zone1Load(void);
static void Zone2Load(void);
static void Zone3Load(void);
static void Zone4Load(void);

/* the laser beams in zone 1; the second of each pair sweeps round its start */
static struct VECT Laser0Start = { 16107, -19700, 3520 };
static struct VECT Laser0End = { 20600, -16477, 2239 };
static short Laser0Space = -1;
static struct VECT Laser1Start = { 16107, -19700, -2239 };
static struct VECT Laser1End = { 20600, -16477, -961 };
static short Laser1Space = -1;
static struct VECT Laser2Start = { 16323, -17156, 2246 };
static struct VECT Laser2End = { 16328, -16000, 2076 };
static short Laser2Space = -1;
static unsigned short Laser2Angle = 0;
static struct VECT Laser3Start = { 16323, -17156, -961 };
static struct VECT Laser3End = { 16328, -16000, -803 };
static short Laser3Space = -1;
static unsigned short Laser3Angle = 1000;
static short LaserArea = -1;

static struct IDENTITYNAME LiftNames[1] = { { "Lift A" } };

static struct IDENTITYNAME Zone0LaserNames[5] = {
    { "LaserNW" }, { "LaserNE" }, { "LaserSW" }, { "LaserSE" }, { "LaserMid" }
};
static struct VECT Zone0LaserStarts[5] = {
    { 650, -10848, 1042 }, { 650, -10646, 283 }, { 650, -10047, 910 }, { 650, -9885, 380 }, { 650, -10342, 638 }
};
static struct VECT Zone0LaserEnds[5] = {
    { -3000, -10848, 1042 }, { -3000, -10646, 283 }, { -3000, -10047, 910 }, { -3000, -9885, 380 }, { -3000, -10342, 638 }
};
static short Zone0LaserSpace = -1;
static short Zone0LaserUnused = -1;

static struct CREATURE *FoundCre;
static int RiderStandDone;
static int Unused[2];
static int Unused2[2];
struct BOSSDATA *MissionData;

/* put the lifts back where the saved game left them */
static void RestoreLifts(void)
{
    int i;
    struct SVECT Unused; /* the original's frame has 8 bytes of unused locals */

    for (i = 0; i < 1; i++) {
        MissionData->Boss1Lifts[i].Object = Engine->GetSceneNum(LiftNames[i]);
        MissionData->Boss1Lifts[i].MoveBuf = 0;
        if (MissionData->Boss1Lifts[i].Object != -1 && Engine->Scene2Data[MissionData->Boss1Lifts[i].Object].MoveBufId)
            MissionData->Boss1Lifts[i].MoveBuf = Engine->BrushMoveBufs + Engine->Scene2Data[MissionData->Boss1Lifts[i].Object].MoveBufId - 1;
        if (MissionData->Boss1Lifts[i].MoveBuf && MissionData->Boss1Lifts[i].ValidData) {
            MissionData->Boss1Lifts[i].MoveBuf->Time = MissionData->Boss1Lifts[i].Time;
            MissionData->Boss1Lifts[i].MoveBuf->Offset.vy = MissionData->Boss1Lifts[i].OffsetY;
            MissionData->Boss1Lifts[i].MoveBuf->OpenFlag = MissionData->Boss1Lifts[i].OpenFlag;
            if (MissionData->Boss1Lifts[i].MoveBuf->Link)
                MissionData->Boss1Lifts[i].MoveBuf->Link->Offset.vy = MissionData->Boss1Lifts[i].OffsetY;
            if (MissionData->Boss1Lifts[i].MoveBuf->SolidBox) {
                MissionData->Boss1Lifts[i].MoveBuf->SolidBox->MinMax = MissionData->Boss1Lifts[i].SolidMinMax;
                MissionData->Boss1Lifts[i].MoveBuf->SolidBox->CMinMax = MissionData->Boss1Lifts[i].SolidCMinMax;
                MissionData->Boss1Lifts[i].MoveBuf->SolidBox->Moved = MissionData->Boss1Lifts[i].SolidMoved;
            }
        }
    }
}

/* remember where the lifts are for the saved game */
static void StoreLifts(void)
{
    int i;

    for (i = 0; i < 1; i++) {
        if (MissionData->Boss1Lifts[i].MoveBuf) {
            MissionData->Boss1Lifts[i].ValidData = 1;
            MissionData->Boss1Lifts[i].OpenFlag = MissionData->Boss1Lifts[i].MoveBuf->OpenFlag;
            MissionData->Boss1Lifts[i].OffsetY = MissionData->Boss1Lifts[i].MoveBuf->Offset.vy;
            MissionData->Boss1Lifts[i].Time = MissionData->Boss1Lifts[i].MoveBuf->Time;
            if (MissionData->Boss1Lifts[i].MoveBuf->SolidBox) {
                MissionData->Boss1Lifts[i].SolidMinMax = MissionData->Boss1Lifts[i].MoveBuf->SolidBox->MinMax;
                MissionData->Boss1Lifts[i].SolidCMinMax = MissionData->Boss1Lifts[i].MoveBuf->SolidBox->CMinMax;
                MissionData->Boss1Lifts[i].SolidMoved = MissionData->Boss1Lifts[i].MoveBuf->SolidBox->Moved;
            }
        }
    }
}

static void CheckStairs(void)
{
    if (MissionData->Stairs0Pressed && MissionData->Stairs1Pressed) {
        Engine->centreprintf(Engine->GameWindows, "Sequence complete");
        Engine->Event_DoNamed(IDNAME("Steps"), 0);
    } else
        Engine->centreprintf(Engine->GameWindows, "One more to go...");
}

static struct MISSIONEVENT LevelEvents[] = {
    { { "LaserButton0" }, LaserButton0 },
    { { "LaserButton1" }, LaserButton1 },
    { { "Stairs0" }, Stairs0 },
    { { "Stairs1" }, Stairs1 },
    { { "" }, 0 },
};

static void Zone0Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone0"));
    RestoreLifts();
}

static void Zone1Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone1"));
    Laser0Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser0Start, -1, 1);
    Laser1Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser1Start, -1, 1);
    Laser2Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser2Start, -1, 1);
    Laser3Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser3Start, -1, 1);
    LaserArea = Engine->CollMap->Coll3dSpace[Laser2Space].Area;
    RestoreLifts();
}

static void Zone2Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone2"));
    RestoreLifts();
}

static void Zone3Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone3"));
    RestoreLifts();
}

static void Zone4Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone4"));
    RestoreLifts();
}

void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load };

struct CREATURE *FindCreature(struct IDENTITYNAME Name)
{
    int i;
    int *a, *b;
    int x, y;

    a = (int *)&Name;
    for (i = 0, FoundCre = Engine->Creatures; i < 48; i++, FoundCre++) {
        b = (int *)&FoundCre->ModelHdr->ModelName;
        /* the words go through x and y (not a[0] == b[0] && ...): as variables used in
         * several blocks they are left to global-alloc, which keeps $3 free for
         * %hi(FoundCre); the return inside the loop fills the found branch's delay slot */
        x = a[0];
        y = b[0];
        if (x == y) {
            x = a[1];
            y = b[1];
            if (x == y) {
                x = a[2];
                y = b[2];
                if (x == y)
                    return FoundCre;
            }
        }
    }
    return 0;
}

/* when the player gets near Makron, start the rider stand */
static int CheckNearMakron(void)
{
    struct CREATURE *Cre;

    if (!RiderStandDone && Engine->Event_GetDisableNamed(IDNAME("E_NearMakron"))) {
        RiderStandDone = 1;
        Cre = FindCreature(IDNAME("RiderStand"));
        if (Cre && Cre->EdictT)
            Cre->EdictT->use(Cre->EdictT, 0, 0);
    }
    return RiderStandDone;
}

static void Zone0Lasers(void);

static void LevelSpecific(void)
{
    struct SHORTPAIR Parts;
    struct VECT End;
    struct SVECT Vel[15];
    int Dx, Dz;
    int Vcount;
    short i;

    Vcount = *Engine->NewVcounts;
    if (Engine->ZoneName->Name[4] == '0') {
        Zone0Lasers();
    } else {
        Parts.a = Parts.b = 0;
        for (i = 0; i < 15; i++) {
            Vel[i].vx = (Engine->rand() - 16384) >> 8;
            Vel[i].vz = (Engine->rand() - 16384) >> 8;
            Vel[i].vy = -Engine->rand() >> 13;
        }
        if (!MissionData->Laser0Pressed) {
            if (Laser0Space >= 0) {
                Engine->AddLaser(&Laser0Start, &Laser0End, Laser0Space, 3, Parts);
                Engine->AddLight(&Laser0End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 400 / 32768 + 150, Engine->rand() * 400 / 32768 + 800 },
                                 *&(struct SHORTPAIR){ 1, 3 });
            }
            if (Laser3Space >= 0) {
                End = Laser3End;
                Laser3Angle += Vcount * 15;
                Dx = Engine->rcossin_tbl[Laser3Angle & 0xfff].sin * 250 / 4096;
                Dz = Engine->rcossin_tbl[Laser3Angle & 0xfff].cos * 250 / 4096;
                End.vx += Dx;
                End.vz += Dz;
                Engine->AddLaser(&Laser3Start, &End, Laser3Space, 1, Parts);
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 5, 10000, LaserArea);
                End.vy -= 50;
                Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                                 *&(struct SHORTPAIR){ 1, 3 });
                Engine->AddLight(&Laser3Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                                 *&(struct SHORTPAIR){ 1, 3 });
            }
        }
        if (!MissionData->Laser1Pressed) {
            if (Laser1Space >= 0) {
                Engine->AddLaser(&Laser1Start, &Laser1End, Laser1Space, 3, Parts);
                Engine->AddLight(&Laser1End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 400 / 32768 + 150, Engine->rand() * 400 / 32768 + 800 },
                                 *&(struct SHORTPAIR){ 1, 3 });
            }
            if (Laser2Space >= 0) {
                End = Laser2End;
                Laser2Angle += Vcount * 12;
                Dx = Engine->rcossin_tbl[Laser2Angle & 0xfff].sin * 250 / 4096;
                Dz = Engine->rcossin_tbl[Laser2Angle & 0xfff].cos * 250 / 4096;
                End.vx += Dx;
                End.vz += Dz;
                Engine->AddLaser(&Laser2Start, &End, Laser2Space, 1, Parts);
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 5, 10000, LaserArea);
                End.vy -= 50;
                Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                                 *&(struct SHORTPAIR){ 1, 3 });
                Engine->AddLight(&Laser2Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                                 (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                                 *&(struct SHORTPAIR){ 1, 3 });
            }
        }
        CheckNearMakron();
    }
    StoreLifts();
}

static void Zone0Lasers(void)
{
    struct SHORTPAIR Parts;
    int i;

    Parts.a = Parts.b = 0;
    if (!Engine->Event_GetDisableNamed(IDNAME("DoorBlown"))) {
        for (i = 0; i < 5; i++)
            if (Engine->Event_GetDisableNamed(Zone0LaserNames[i]))
                Engine->AddLaser(&Zone0LaserStarts[i], &Zone0LaserEnds[i], Zone0LaserSpace, 4, Parts);
    }
}

void InitLevel(void)
{
    if (Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            Engine->PrintF("Please get a programmer to recompile level %s\n",
                Engine->PrintIdentityName(*Engine->ThisLevelName));
        Engine->PrintF("Size In Level %d, Size In Engine %d\n", sizeof(struct QENGINELEVINTERFACE), Engine->SizeOf);
    }
    *Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    Engine->LoadSaveCreatureBatches(&IDNAME("Boss1Batches"), 0);
    *Engine->LevelSpecific = LevelSpecific;
    MissionData = Engine->MissionData;
    RestoreLifts();
    Engine->InitialiseQuakePlayer(0, *Engine->MainPlayerStart);
    Engine->QuakeSinglePlayerInit();
    Engine->LevelEvents = LevelEvents;
    Engine->AddLevelInterMission(Engine->GetStringFromStringtable(IDNAME("MapTitle")));
    RiderStandDone = 0;
    Engine->HelpComputer_NewMessage(IDNAME("Unit5Curr1"), IDNAME("Unit5Miss1"), 40);
    Engine->PlayThisXA(0);
}

static void LaserButton0(void)
{
    MissionData->Laser0Pressed = 1;
}

static void LaserButton1(void)
{
    MissionData->Laser1Pressed = 1;
}

static void Stairs0(void)
{
    MissionData->Stairs0Pressed = 1;
    CheckStairs();
}

static void Stairs1(void)
{
    MissionData->Stairs1Pressed = 1;
    CheckStairs();
}

void LevelNull(void)
{
}
