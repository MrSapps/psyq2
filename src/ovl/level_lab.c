/* Lab: the laboratory level (mission 4). Two lasers can be switched off for a while, and
 * picking up the head wakes the dead commander. */
#include "ovl.h"

#define Engine LevelInterface.Engine
#define IDNAME(s) ((struct IDENTITYNAME){s})

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* small functions: gcc emits them at the end of the file in this order */
struct CREATURE *FindCreature(struct IDENTITYNAME Name);
static void Laser0Off(void);
static void Laser1Off(void);

static void Zone0Load(void);
static void Zone1Load(void);
static void Zone2Load(void);
static void Zone3Load(void);
static void Zone4Load(void);

static struct IDENTITYNAME LiftNames[2] = { { "Lift A" }, { "Lift C" } };

/* the two lasers in zone 1; their ends wobble while they are switched off */
static struct VECT Laser0Start = { -1521, -1665, 4062 };
static struct VECT Laser0End = { -1511, -619, 3441 };
static struct VECT Laser1Start = { 2382, -1670, 5041 };
static struct VECT Laser1End = { 1765, -639, 5022 };
static int Laser0Timer = 0;
static int Laser1Timer = 0;
static short Laser0Space = -1;
static short Laser1Space = -1;
static short Laser0Area = -1;
static short Laser1Area = -1;

static struct CREATURE *FoundCre;
static int DeadCommDone;
static int Unused0, Unused1, Unused2; /* bss the original has but no code uses */
static struct GRAVDATA *MissionData;

/* put the lifts back where the saved game left them */
static void RestoreLifts(void)
{
    int i;
    struct SVECT Unused; /* the original's frame has 8 bytes of unused locals */

    for (i = 0; i < 2; i++) {
        MissionData->LabLifts[i].Object = Engine->GetSceneNum(LiftNames[i]);
        MissionData->LabLifts[i].MoveBuf = 0;
        if (MissionData->LabLifts[i].Object != -1 && Engine->Scene2Data[MissionData->LabLifts[i].Object].MoveBufId)
            MissionData->LabLifts[i].MoveBuf = Engine->BrushMoveBufs + Engine->Scene2Data[MissionData->LabLifts[i].Object].MoveBufId - 1;
        if (MissionData->LabLifts[i].MoveBuf && MissionData->LabLifts[i].ValidData) {
            MissionData->LabLifts[i].MoveBuf->Time = MissionData->LabLifts[i].Time;
            MissionData->LabLifts[i].MoveBuf->Offset.vy = MissionData->LabLifts[i].OffsetY;
            MissionData->LabLifts[i].MoveBuf->OpenFlag = MissionData->LabLifts[i].OpenFlag;
            if (MissionData->LabLifts[i].MoveBuf->Link)
                MissionData->LabLifts[i].MoveBuf->Link->Offset.vy = MissionData->LabLifts[i].OffsetY;
            if (MissionData->LabLifts[i].MoveBuf->SolidBox) {
                MissionData->LabLifts[i].MoveBuf->SolidBox->MinMax = MissionData->LabLifts[i].SolidMinMax;
                MissionData->LabLifts[i].MoveBuf->SolidBox->CMinMax = MissionData->LabLifts[i].SolidCMinMax;
                MissionData->LabLifts[i].MoveBuf->SolidBox->Moved = MissionData->LabLifts[i].SolidMoved;
            }
        }
    }
}

/* remember where the lifts are for the saved game */
static void StoreLifts(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (MissionData->LabLifts[i].MoveBuf) {
            MissionData->LabLifts[i].ValidData = 1;
            MissionData->LabLifts[i].OpenFlag = MissionData->LabLifts[i].MoveBuf->OpenFlag;
            MissionData->LabLifts[i].OffsetY = MissionData->LabLifts[i].MoveBuf->Offset.vy;
            MissionData->LabLifts[i].Time = MissionData->LabLifts[i].MoveBuf->Time;
            if (MissionData->LabLifts[i].MoveBuf->SolidBox) {
                MissionData->LabLifts[i].SolidMinMax = MissionData->LabLifts[i].MoveBuf->SolidBox->MinMax;
                MissionData->LabLifts[i].SolidCMinMax = MissionData->LabLifts[i].MoveBuf->SolidBox->CMinMax;
                MissionData->LabLifts[i].SolidMoved = MissionData->LabLifts[i].MoveBuf->SolidBox->Moved;
            }
        }
    }
}

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
    Laser0Area = Engine->CollMap->Coll3dSpace[Laser0Space].Area;
    Laser1Area = Engine->CollMap->Coll3dSpace[Laser1Space].Area;
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

/* once the head has been picked up, wake the dead commander */
void CheckGotHead(void)
{
    struct CREATURE *Cre;

    if (!DeadCommDone && Engine->Event_GetDisableNamed(IDNAME("E_GotHead"))) {
        DeadCommDone = 1;
        Cre = FindCreature(IDNAME("DeadComm"));
        if (Cre && Cre->EdictT)
            Cre->EdictT->use(Cre->EdictT, 0, 0);
    }
}

static void LevelSpecific(void)
{
    struct SHORTPAIR Parts;
    struct VECT End;
    struct SVECT Vel[15];
    int Vcount;
    short i;

    Vcount = *Engine->NewVcounts;
    Parts.a = Parts.b = 0;
    for (i = 0; i < 15; i++) {
        Vel[i].vx = (Engine->rand() - 16384) >> 8;
        Vel[i].vz = (Engine->rand() - 16384) >> 8;
        Vel[i].vy = -Engine->rand() >> 13;
    }
    if (Laser0Timer > 0) {
        Laser0Timer -= Vcount * 2;
        if (Laser0Space >= 0) {
            End = Laser0End;
            End.vx += Engine->rcossin_tbl[Laser0Timer & 0xfff].sin * 400 / 4096;
            End.vz += Engine->rcossin_tbl[Laser0Timer * 8 & 0xfff].sin * 100 / 4096;
            if (!Engine->AddLaser(&Laser0Start, &End, Laser0Space, 1, Parts))
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 10, 10000, Laser0Area);
            End.vy -= 50;
            Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                             *&(struct SHORTPAIR){ 1, 3 });
            Engine->AddLight(&Laser0Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                             *&(struct SHORTPAIR){ 1, 3 });
        }
    }
    if (Laser1Timer > 0) {
        Laser1Timer -= Vcount * 2;
        if (Laser1Space >= 0) {
            End = Laser1End;
            End.vz += Engine->rcossin_tbl[Laser1Timer & 0xfff].sin * 400 / 4096;
            End.vx += Engine->rcossin_tbl[Laser1Timer * 8 & 0xfff].sin * 100 / 4096;
            if (!Engine->AddLaser(&Laser1Start, &End, Laser1Space, 1, Parts))
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 10, 10000, Laser1Area);
            End.vy -= 50;
            Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                             *&(struct SHORTPAIR){ 1, 3 });
            Engine->AddLight(&Laser1Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                             *&(struct SHORTPAIR){ 1, 3 });
        }
    }
    CheckGotHead();
    StoreLifts();
}

static struct MISSIONEVENT LevelEvents[] = {
    { { "Laser0" }, Laser0Off },
    { { "Laser1" }, Laser1Off },
    { { "" }, 0 },
};

void InitLevel(void)
{
    if (Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            Engine->PrintF("Please get a programmer to recompile level %s\n",
                Engine->PrintIdentityName(*Engine->ThisLevelName));
        Engine->PrintF("Size In Level %d, Size In Engine %d\n", sizeof(struct QENGINELEVINTERFACE), Engine->SizeOf);
    }
    DeadCommDone = 0;
    Engine->LoadSaveCreatureBatches(&IDNAME("LabBatches"), 0);
    *Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *Engine->LevelSpecific = LevelSpecific;
    Engine->LevelEvents = LevelEvents;
    MissionData = Engine->MissionData;
    RestoreLifts();
    Engine->InitialiseQuakePlayer(0, *Engine->MainPlayerStart);
    Engine->QuakeSinglePlayerInit();
    Engine->AddLevelInterMission(Engine->GetStringFromStringtable(IDNAME("MapTitle")));
    Engine->PlayThisXA(1);
}

/* the "Laser0"/"Laser1" events switch a laser off for a while */
static void Laser0Off(void)
{
    if (Laser0Timer <= 0)
        Laser0Timer = 8192;
}

static void Laser1Off(void)
{
    if (Laser1Timer <= 0)
        Laser1Timer = 8192;
}
