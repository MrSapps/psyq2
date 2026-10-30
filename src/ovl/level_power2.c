/* Power2: the reactor level of the power station (mission 3). Once both pumps are on, the
 * reactor's cooling is drained, its glass panels blow out one after another and the core
 * explodes; meanwhile a klaxon sounds and a laser burns in the core room. */
#include "ovl.h"
#include "bss.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* defined below; gcc defers these to the end of the file in this order */
void Nothing(void);
void DoCoreCountdown(void);

static void CoreCountdown(void);
static void Zone0Load(void);
static void Zone1Load(void);
static void Zone2Load(void);
static void Zone3Load(void);
static void Zone4Load(void);

/* the laser in the reactor room, and the point the core explosions come from */
struct VECT LaserStart = { -21760, -296, -10242 };
struct VECT LaserEnd = { -21760, 856, -10242 };
struct VECT CorePos = { -21760, 280, -10242 };
/* defined ahead of the zone loaders: its relocations come first in the original .rel */
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load };

/* bss: definition order (the BSS attribute; LaserSpace packs next to LaserArea) */
static int Unused0 BSS;
static short LaserArea BSS;
static short LaserSpace __attribute__((section(".bss")));
static int CoreTriggered BSS;    /* both pumps on: the countdown is running */
static int Unused1[2] BSS;
static int KlaxonTimer BSS;
static struct POWERDATA *MissionData BSS;
static unsigned int StartTime BSS;
static int LaserOff BSS;

static void Zone0Load(void)
{
    struct IDENTITYNAME Name = { "Glass" };
    int i;

    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
    LaserSpace = LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &LaserStart, -1, 1);
    LaserArea = LevelInterface.Engine->CollMap->Coll3dSpace[LaserSpace].Area;
    if (MissionData->DestroyedCore) {
        for (i = 0; i < 6; i++) {
            int n;

            Name.Name[5] = i + '1';
            n = LevelInterface.Engine->GetSceneNum(Name);
            if (n != -1)
                LevelInterface.Engine->Scene2Data[n].NoDraw = 1;
        }
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"DrainCool"}, 0);
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"ExploDoor"}, 0);
        LaserOff = 1;
    }
    CoreTriggered = 0;
}
static void Zone1Load(void)
{
    if (CoreTriggered)
        MissionData->DestroyedCore = 1;
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
}
static void Zone2Load(void)
{
    if (CoreTriggered)
        MissionData->DestroyedCore = 1;
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
}
static void Zone3Load(void)
{
    if (CoreTriggered)
        MissionData->DestroyedCore = 1;
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone3"});
}
static void Zone4Load(void)
{
    if (CoreTriggered)
        MissionData->DestroyedCore = 1;
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone4"});
}

/* The recorded pad input the self-play (attract mode) demo replays: none. */
short SelfPlayData[] = { 0xffff };

/* the laser beam: six sides joined round a hexagonal tube, and its two end caps */
#define LASERCOL { 0x64, 0, 0, 0x3a }
struct SIMPOL LaserModelData[6] = {
    { { 0, 1, 6, 7 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 1, 2, 7, 8 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 2, 3, 8, 9 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 3, 4, 9, 10 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 4, 5, 10, 11 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 5, 0, 11, 6 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
};
struct SIMPOL LaserEndModelDataA[2] = {
    { { 1, 0, 2, 3 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 4, 3, 5, 0 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
};
struct SIMPOL LaserEndModelDataB[2] = {
    { { 0, 1, 3, 2 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
    { { 3, 4, 0, 5 }, { LASERCOL, LASERCOL, LASERCOL, LASERCOL } },
};

/* ticks after the countdown starts at which each glass panel, the door and the drain go */
short GlassTimes[6] = { 4100, 4200, 4700, 4750, 4800, 5100 };
short ExploDoorTime = 10000;
short DrainCoolTime = 0;

static void CoreCountdown(void)
{
    int i = 0;
    int Elapsed = *LevelInterface.Engine->LevelTicks - StartTime;
    struct SHORTPAIR Parts;
    struct IDENTITYNAME Name = { "Glass" };
    struct VECT Pos;
    struct VECT End;
    struct SVECT Dir;
    int n;

    if (DrainCoolTime != -1 && Elapsed >= DrainCoolTime) {
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"DrainCool"}, 0);
        DrainCoolTime = -1;
    }
    if (ExploDoorTime != -1 && Elapsed >= ExploDoorTime) {
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"ExploDoor"}, 0);
        ExploDoorTime = -1;
        MissionData->DestroyedCore = 1;
    }
    while (GlassTimes[i] == -1 && i < 6)
        i++;
    if (i == 6) {
        if (!(LevelInterface.Engine->rand() & 3)) {
            Pos = CorePos;
            Pos.vx += (LevelInterface.Engine->rand() & 0x1ff) - 255;
            Pos.vy += (LevelInterface.Engine->rand() & 0x1ff) - 255;
            Pos.vz += (LevelInterface.Engine->rand() & 0x1ff) - 255;
            LevelInterface.Engine->QuakeGenExplosion(&Pos, LaserArea, (LevelInterface.Engine->rand() & 0x1ff) + 4000, 0);
        }
    } else if (Elapsed >= GlassTimes[i]) {
        Name.Name[5] = i + '1';
        GlassTimes[i] = -1;
        n = LevelInterface.Engine->GetSceneNum(Name);
        if (n != -1) {
            LaserOff = 1;
            Pos = LevelInterface.Engine->Scene2Data[n].VectBox.Min;
            Pos.vx += LevelInterface.Engine->Scene2Data[n].VectBox.Max.vx;
            Pos.vy += LevelInterface.Engine->Scene2Data[n].VectBox.Max.vy;
            Pos.vz += LevelInterface.Engine->Scene2Data[n].VectBox.Max.vz;
            Pos.vx >>= 1;
            Pos.vy >>= 1;
            Pos.vz >>= 1;
            Dir.vx = Pos.vx - CorePos.vx;
            Dir.vy = Pos.vy - CorePos.vy + (LevelInterface.Engine->rand() & 0xff) - 127;
            Dir.vz = Pos.vz - CorePos.vz;
            LevelInterface.Engine->VectorNormalSS(&Dir, &Dir);
            Dir.vx <<= 3;
            Dir.vy <<= 3;
            Dir.vz <<= 3;
            End = Pos;
            End.vx += Dir.vx;
            End.vy += Dir.vy;
            End.vz += Dir.vz;
            Parts.a = 0;
            Parts.b = 0;
            LevelInterface.Engine->AddLaser(&Pos, &End, LaserSpace, 2, Parts);
            LevelInterface.Engine->QuakeGenExplosion(&Pos, LaserArea, (LevelInterface.Engine->rand() & 0x1ff) + 4000, 0);
            LevelInterface.Engine->SceneObjFragments(n, 10, 0);
            LevelInterface.Engine->Scene2Data[n].NoDraw = 1;
        }
    }
}

static void DrawLaser(void)
{
    struct SVECT Vel[15];
    struct VECT Pos;
    int Sparks;
    int Sign;
    int i;
    int Num;

    Sparks = (*LevelInterface.Engine->LevelTicks & 0x3f) >= 16;
    LevelInterface.Engine->AddThickLine(&LaserStart, &LaserEnd, 256, LaserSpace, LaserModelData, LaserEndModelDataA, LaserEndModelDataB);
    if (Sparks) {
        if ((*LevelInterface.Engine->LevelTicks & 0x7f) < 63) {
            Sign = -1;
            Pos = LaserEnd;
        } else {
            Sign = 1;
            Pos = LaserStart;
        }
        for (i = 0; i < 15; i++) {
            Vel[i].vx = (LevelInterface.Engine->rand() - 16384) >> 13;
            Vel[i].vy = (LevelInterface.Engine->rand() >> 8) * Sign;
            Vel[i].vz = (LevelInterface.Engine->rand() - 16384) >> 13;
        }
        Pos.vx += (LevelInterface.Engine->rand() & 0xff) - 127;
        Pos.vz += (LevelInterface.Engine->rand() & 0xff) - 127;
        Num = (LevelInterface.Engine->rand() + 16) & 0xf;
        LevelInterface.Engine->AddParticlesSL(&Pos, Vel, LevelInterface.Engine->ParticleColoursTable[2],
            LevelInterface.Engine->ParticleColoursTable[0], 15, Num,
            (LevelInterface.Engine->rand() + 4000) & 0xfff, LaserArea);
    }
}

static void LevelSpecific(void)
{
    unsigned int Ticks;
    int On;

    if (!MissionData->KilledRocketGirl && !*LevelInterface.Engine->GameDeathWatchA) {
        MissionData->KilledRocketGirl = 1;
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"RockLaser"}, 1);
    }
    if (LevelInterface.Engine->ZoneName->Name[4] == '0' && *LevelInterface.Engine->NewVcounts > 0) {
        if (!CoreTriggered) {
            if (MissionData->TurnedOnPump1 && MissionData->TurnedOnPump2 && !MissionData->DestroyedCore) {
                CoreTriggered = 1;
                LaserOff = 0;
                LevelInterface.Engine->PrintF("Setting up reactor death to happen\n");
                StartTime = *LevelInterface.Engine->LevelTicks;
                LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit3Curr5"}, (struct IDENTITYNAME){"Unit3Miss2"}, 50);
            }
        } else {
            KlaxonTimer += *LevelInterface.Engine->NewVcounts;
            if (KlaxonTimer > 300) {
                KlaxonTimer -= 300;
                LevelInterface.Engine->SoundAtLocation(LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"amb_klax2n"}), &CorePos);
            }
        }
        if (!LaserOff)
            DrawLaser();
        if (CoreTriggered && !MissionData->DestroyedCore)
            CoreCountdown();
    } else if (LevelInterface.Engine->ZoneName->Name[4] == '4') {
        Ticks = *LevelInterface.Engine->LevelTicks;
        On = (Ticks & 0x1ff) >= 256;
        if ((Ticks & 0xff) < 47) {
            LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"Laser1"}, 1);
            LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"Laser2"}, 1);
        } else {
            LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"Laser1"}, On);
            LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"Laser2"}, 1 - On);
        }
    }
}

void InitLevel(void)
{
    if (LevelInterface.Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            LevelInterface.Engine->PrintF("Please get a programmer to recompile level %s\n",
                LevelInterface.Engine->PrintIdentityName(*LevelInterface.Engine->ThisLevelName));
        LevelInterface.Engine->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINELEVINTERFACE), LevelInterface.Engine->SizeOf);
    }
    KlaxonTimer = 0;
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Pow2Batches"}, 0);
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    MissionData = LevelInterface.Engine->MissionData;
    if (*LevelInterface.Engine->InSelfPlayMode) {
        LevelInterface.Engine->SelfPlayOn(0, SelfPlayData, LevelInterface.Engine->QuakeEndSelfPlay);
        *LevelInterface.Engine->NoInputTime = 900;
        LevelInterface.Engine->SetNoInputTimeOutRoutine(LevelInterface.Engine->QuakeNextDemoLevel);
    }
}

void Nothing(void)
{
}

void DoCoreCountdown(void)
{
    CoreCountdown();
}
