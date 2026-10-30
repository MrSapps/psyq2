/* Security (mission 2, unit 2): lifts, the laser grid, the exploding walls and the orb */
#include "ovl.h"

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

static void OrbActive(void);
static void LockLifts(void);

/* the five sparks the orb shoots its lasers at */
struct ORBSPARK {
    struct SVECT Vel;
    short Time;
    struct VECT Pos;
};

static struct VECT OrbPos = { -1918, -10178, -23996 };
static struct VECT OrbLaserEndA = { -1918, -10519, -23996 };
static struct VECT OrbLaserEndB = { -1918, -9259, -23996 };
static struct MISSIONEVENT MissionEvents[] = {
    { { "OrbActive" }, OrbActive },
    { { "LOCKLIFTS" }, LockLifts },
    { { "" }, 0 },
};
static struct IDENTITYNAME LiftNames[2] = { { "Lift A" }, { "Lift B" } };
static struct VECT GridPos[4] = {
    { 79, -5921, -15040 },
    { 79, -5921, -11840 },
    { -3920, -5921, -15040 },
    { -3920, -5921, -11840 },
};

static int GridSpace[4];
static int Unused0;        /* unused statics: the original's bss has space here */
static int GridInit;
static int Unused1[2];
static int GrenSound;
static int LiftRingScene;
static int LevelFlag;      /* cleared by InitLevel, nothing reads it */
static int Zone2FlagA;     /* cleared by Zone2Load, nothing reads them */
static int Zone2FlagB;
static int OrbArea;
static int OrbScene;
static int Shake;
static int Unused5[33];
static struct JAILDATA *MissionData;
static int Unused6;
static int OrbSpace;
static int WallTimer[4];
static struct BRUSHMOVEBUF *HexLiftBuf;
static int Unused7;
/* commons: the linker lays these out after the statics, in ASPSX's symbol hash order
   (sum of the name's characters mod 256), which these names reproduce */
struct ORBSPARK Sparks[5];
int HexLiftScene;
struct BRUSHMOVEBUF *OrbMoveBuf;
int QuakeTimer;

static void RestoreLifts(void)
{
    struct SVECT Unused;    /* unreferenced, but the original's frame has its 8 bytes */
    int i;

    for (i = 0; i < 2; i++) {
        MissionData->SecLifts[i].Object = LevelInterface.Engine->GetSceneNum(LiftNames[i]);
        MissionData->SecLifts[i].MoveBuf = 0;
        if (MissionData->SecLifts[i].Object != -1 && LevelInterface.Engine->Scene2Data[MissionData->SecLifts[i].Object].MoveBufId)
            MissionData->SecLifts[i].MoveBuf = LevelInterface.Engine->BrushMoveBufs + LevelInterface.Engine->Scene2Data[MissionData->SecLifts[i].Object].MoveBufId - 1;
        if (MissionData->SecLifts[i].MoveBuf && MissionData->SecLifts[i].ValidData) {
            MissionData->SecLifts[i].MoveBuf->Time = MissionData->SecLifts[i].Time;
            MissionData->SecLifts[i].MoveBuf->Offset.vy = MissionData->SecLifts[i].OffsetY;
            MissionData->SecLifts[i].MoveBuf->OpenFlag = MissionData->SecLifts[i].OpenFlag;
            if (MissionData->SecLifts[i].MoveBuf->Link)
                MissionData->SecLifts[i].MoveBuf->Link->Offset.vy = MissionData->SecLifts[i].OffsetY;
            if (MissionData->SecLifts[i].MoveBuf->SolidBox) {
                MissionData->SecLifts[i].MoveBuf->SolidBox->MinMax = MissionData->SecLifts[i].SolidMinMax;
                MissionData->SecLifts[i].MoveBuf->SolidBox->CMinMax = MissionData->SecLifts[i].SolidCMinMax;
                MissionData->SecLifts[i].MoveBuf->SolidBox->Moved = MissionData->SecLifts[i].SolidMoved;
            }
        }
    }
}

static void SaveLifts(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (MissionData->SecLifts[i].MoveBuf) {
            MissionData->SecLifts[i].ValidData = 1;
            MissionData->SecLifts[i].OpenFlag = MissionData->SecLifts[i].MoveBuf->OpenFlag;
            MissionData->SecLifts[i].OffsetY = MissionData->SecLifts[i].MoveBuf->Offset.vy;
            MissionData->SecLifts[i].Time = MissionData->SecLifts[i].MoveBuf->Time;
            if (MissionData->SecLifts[i].MoveBuf->SolidBox) {
                MissionData->SecLifts[i].SolidMinMax = MissionData->SecLifts[i].MoveBuf->SolidBox->MinMax;
                MissionData->SecLifts[i].SolidCMinMax = MissionData->SecLifts[i].MoveBuf->SolidBox->CMinMax;
                MissionData->SecLifts[i].SolidMoved = MissionData->SecLifts[i].MoveBuf->SolidBox->Moved;
            }
        }
    }
}

static void Earthquake(void)
{
    if (LevelInterface.Engine->GameWindows->Creature && Shake) {
        for (QuakeTimer += *LevelInterface.Engine->NewVcounts; QuakeTimer > 0; QuakeTimer -= 30) {
            ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vx += (LevelInterface.Engine->rand() - 16384) / 6;
            ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vy += (LevelInterface.Engine->rand() - 16384) / 16;
            ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vz += (LevelInterface.Engine->rand() - 16384) / 6;
            ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->Control |= 0x4000;
            LevelInterface.Engine->SetShockData(4, 13, (LevelInterface.Engine->rand() & 0xfff) + 1024, 0, (struct POWER){ 2048, 4096 });
            if (LevelInterface.Engine->rand() < 8192)
                LevelInterface.Engine->SoundAtLocation(GrenSound, &((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->Box.Location);
        }
    }
}

static void OrbUpdate(void)
{
    struct SHORTPAIR Parts;
    struct VECT Start;
    struct VECT Start2;
    struct VECT Centre;
    struct ORBSPARK *Spark;
    int Bob;
    int i;
    int n;
    short Sin;

    Parts.b = 0;
    Parts.a = 0;
    if (OrbScene != -1 && LevelInterface.Engine->Scene2Data[OrbScene].NoDraw) {
        LevelInterface.Engine->QuakeGenExplosion(&OrbPos, OrbArea, 4096, 0);
        MissionData->OrbState = 2;
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr9"}, (struct IDENTITYNAME){"Unit2Miss2"}, 140);
        return;
    }
    Sin = LevelInterface.Engine->rcossin_tbl[((*LevelInterface.Engine->LevelTicks << 12) / 18756) & 0xfff].sin;
    Bob = LevelInterface.Engine->rcossin_tbl[((*LevelInterface.Engine->LevelTicks << 12) / 1200) & 0xfff].sin * (Sin + 8192) / 409600 - 100;
    Start = OrbPos;
    Start.vy += Bob;
    Centre = Start;
    Centre.vy += 281;
    Start2 = Start;
    Start2.vy += 562;
    LevelInterface.Engine->AddLaser(&Start, &OrbLaserEndA, (short)OrbSpace, 3, Parts);
    LevelInterface.Engine->AddLaser(&Start2, &OrbLaserEndB, (short)OrbSpace, 3, Parts);
    if (OrbMoveBuf)
        OrbMoveBuf->Offset.vy = Bob;
    for (i = 0; i < 5; i++) {
        Spark = &Sparks[i];
        if (Spark->Time <= 0) {
            int Angle;
            short Cos;

            Spark->Time = LevelInterface.Engine->rand() * 1500 / 32768 + 300;
            Angle = LevelInterface.Engine->rand() & 0xfff;
            Sin = LevelInterface.Engine->rcossin_tbl[Angle].sin;
            Cos = LevelInterface.Engine->rcossin_tbl[Angle].cos;
            Spark->Pos.vx = Centre.vx + Sin * 2;
            Spark->Pos.vz = Centre.vz + Cos * 2;
            Spark->Pos.vy = Centre.vy + 3072 + (LevelInterface.Engine->rand() - 16384) / 8;
            Spark->Vel = (struct SVECT){ (LevelInterface.Engine->rand() - 16384) / 256, (LevelInterface.Engine->rand() - 16384) / 512, (LevelInterface.Engine->rand() - 16384) / 256 };
        }
        Spark->Pos.vx += Spark->Vel.vx;
        Spark->Pos.vy += Spark->Vel.vy;
        Spark->Pos.vz += Spark->Vel.vz;
        Spark->Time -= *LevelInterface.Engine->NewVcounts;
    }
    *LevelInterface.Engine->DebugWatch = 305;
    /* the count in a variable: the original compares against a register holding 5 */
    for (i = 0, n = 5; i < n; i++)
        LevelInterface.Engine->AddLaser(&Centre, &Sparks[i].Pos, (short)OrbSpace, 1, Parts);
    LevelInterface.Engine->AddLight(&Centre, (struct COLOUR){ 0, 0, 255 },
        (struct POWER){ LevelInterface.Engine->rand() * 200 / 32768 + 850, LevelInterface.Engine->rand() * 200 / 32768 + 1100 },
        (struct SHORTPAIR){ 1, 3 });
}

static void LaserGrid(void)
{
    struct IDENTITYNAME Name;
    struct SHORTPAIR Parts;
    struct VECT End;
    int Mask;
    int Off;
    int n;
    int i;
    int j;
    int Dist;
    unsigned char c;
    unsigned char Last;

    Mask = 0;
    Parts.b = 0;
    Parts.a = 0;
    if (GridInit) {
        GridInit = 0;
        for (j = 0; j < 4; j++)
            GridSpace[j] = LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &GridPos[j], -1, 1);
    }
    n = *LevelInterface.Engine->LevelTicks % 600 / 30;
    if (n & 1) {
        n >>= 1;
        if (n >= 6)
            n = 10 - n;
        Mask |= ((*LevelInterface.Engine->ChrisCount & 1) ? 1 : 64) << n;
    }
    Mask |= ((*LevelInterface.Engine->ChrisCount & 1) ? 0x1000 : 0x10000) << (*LevelInterface.Engine->LevelTicks % 200 / 50);
    Mask |= ((*LevelInterface.Engine->ChrisCount & 1) ? 0x100000 : 0x400000) << (*LevelInterface.Engine->LevelTicks % 300 / 150);
    Off = ~Mask;
    Name = (struct IDENTITYNAME){"LaserBank L"};
    Last = 'F';
    /* Dist doubles as the bank counter (register allocation) */
    for (Dist = 0; Dist < 6; Dist++) {
        for (c = 'A'; c <= Last; c++) {
            Name.Name[11] = c;
            LevelInterface.Engine->Event_SetDisableNamed(Name, Off & 1);
            Off >>= 1;
        }
        if (Dist == 1) {
            Name = (struct IDENTITYNAME){"LaserRing L"};
            Last = 'D';
        } else if (Dist == 3) {
            Name = (struct IDENTITYNAME){"LaserGrid L"};
            Last = 'B';
        } else
            Name.Name[10] = 'R';
    }
    Dist = *LevelInterface.Engine->LevelTicks % 1800 * 8000 / 1800;
    if (Dist > 4000)
        Dist = 8000 - Dist;
    /* Start declared in an inner block: a later pseudo, so it is allocated after loop.c's invariants */
    {
        int Start = (*LevelInterface.Engine->ChrisCount & 1) * 2;

        for (i = Start; i < Start + 2; i++) {
            End.vx = i < 2 ? 1276 : -5120;
            if (Dist < 800) {
                End.vy = -5121 - Dist;
                End.vz = (i & 1) ? -11840 : -15040;
            } else {
                End.vy = -5921;
                End.vz = (i & 1) ? -11040 - Dist : Dist - 15840;
            }
            LevelInterface.Engine->AddLaser(&GridPos[i], &End, (short)GridSpace[i], 1, Parts);
        }
    }
}

static void ExplodeWalls(void)
{
    struct IDENTITYNAME Name;
    int i;

    Name = (struct IDENTITYNAME){"ExplodeWall"};
    for (i = 0; i < 4; i++) {
        if (WallTimer[i] && (WallTimer[i] -= *LevelInterface.Engine->NewVcounts) <= 0) {
            WallTimer[i] = 0;
            Name.Name[11] = '1' + i;
            LevelInterface.Engine->Event_DoNamed(Name, 0);
            if (i == 3)
                Shake = 0;
        }
    }
}

static void HexLiftCheck(void)
{
    int Down;

    if (HexLiftBuf && HexLiftScene != -1) {
        Down = HexLiftBuf->Offset.vy == HexLiftBuf->Range;
        LevelInterface.Engine->Scene2Data[LiftRingScene].NoDraw = Down;
        if (Down && LevelInterface.Engine->Creatures->Box.Location.vy < -8899)
            MissionData->SecurityLiftBlocked = 1;
        if (MissionData->SecurityLiftBlocked) {
            HexLiftBuf->Offset.vy = HexLiftBuf->Range;
            HexLiftBuf->Primary = 0;
        }
    }
}

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
    GridInit = 1;
    RestoreLifts();
    if (!(MissionData->HelpCompStatus & 4)) {
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr8"}, (struct IDENTITYNAME){"Unit2Miss1"}, 40);
        MissionData->HelpCompStatus |= 4;
    }
}

static void Zone1Load(void)
{
    int i;

    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    RestoreLifts();
    for (i = 0; i < 4; i++)
        WallTimer[i] = 0;
}

static void Zone2Load(void)
{
    int i;

    Zone2FlagA = 0;
    Zone2FlagB = 0;
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
    RestoreLifts();
    for (i = 0; i < 5; i++)
        Sparks[i].Time = 0;
    LiftRingScene = LevelInterface.Engine->GetSceneNum((struct IDENTITYNAME){"LiftRing1"});
    HexLiftScene = LevelInterface.Engine->GetSceneNum((struct IDENTITYNAME){"HexLift"});
    HexLiftBuf = 0;
    if (HexLiftScene != -1 && LevelInterface.Engine->Scene2Data[HexLiftScene].MoveBufId)
        HexLiftBuf = LevelInterface.Engine->BrushMoveBufs + LevelInterface.Engine->Scene2Data[HexLiftScene].MoveBufId - 1;
    OrbSpace = LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &OrbPos, -1, 1);
    OrbArea = LevelInterface.Engine->CollMap->Coll3dSpace[OrbSpace].Area;
    OrbScene = LevelInterface.Engine->GetSceneNum((struct IDENTITYNAME){"Orb"});
    OrbMoveBuf = 0;
    if (OrbScene != -1 && LevelInterface.Engine->Scene2Data[OrbScene].MoveBufId)
        OrbMoveBuf = LevelInterface.Engine->BrushMoveBufs + LevelInterface.Engine->Scene2Data[OrbScene].MoveBufId - 1;
}

/* defined here, not with the other data: its relocations follow Zone2Load's in the .rel */
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load };

static void LevelSpecific(void)
{
    int i;
    char Zone;

    SaveLifts();
    Earthquake();
    if (*(signed char *)&MissionData->LockLifts) {   /* lb: signed char in the original */
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"LIFTA"}, 0);
        LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"LIFTB"}, 0);
    }
    Zone = LevelInterface.Engine->ZoneName->Name[4];
    if (Zone == '0')
        LaserGrid();
    else if (Zone == '1') {
        if (!MissionData->BlownUpWalls && !*LevelInterface.Engine->GameDeathWatchA) {
            for (i = 0; i < 4; i++)
                WallTimer[i] = i * 450 + 600;
            Shake = 1;
            LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Boss1"});
            MissionData->BlownUpWalls = 1;
        }
        if (!MissionData->BlownUpHatch && !*LevelInterface.Engine->GameDeathWatchB) {
            LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"ExplodeHatch"}, 0);
            MissionData->BlownUpHatch = 1;
        }
        ExplodeWalls();
    } else if (Zone == '2') {
        if (MissionData->OrbState == 1)
            OrbUpdate();
        HexLiftCheck();
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
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Security Bat"}, 0);
    GrenSound = LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
    Shake = 0;
    QuakeTimer = 0;
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    MissionData = LevelInterface.Engine->MissionData;
    RestoreLifts();
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->LevelEvents = MissionEvents;
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    LevelFlag = 0;
}

static void OrbActive(void)
{
    MissionData->OrbState = 1;
}

static void LockLifts(void)
{
    MissionData->LockLifts = 1;
}
