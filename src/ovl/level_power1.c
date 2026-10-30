#include "ovl.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };
void Zone0Explosions(void);
void DamagePlayerNear(struct VECT *Pos);

/* Zone 0: the two places that keep exploding (the third is unused) and their areas. */
struct VECT ExplosionPos0 = { 1975, -6500, 9653 };
struct VECT ExplosionPos1 = { -1880, -6500, 9653 };
struct VECT ExplosionPos2 = { -600, 3200, -13120 };
int ExplosionArea0 = 19;
int ExplosionArea1 = 19;
/* Zone 1: the explosions running down the corridor. */
struct VECT CorridorPos[8] = {
    { -600, 5200, -13120 },
    { -600, 4600, -13120 },
    { -600, 4000, -13120 },
    { -600, 3400, -13120 },
    { -600, 2800, -13120 },
    { -600, 2200, -13120 },
    { -600, 1600, -13120 },
    { -600, 1000, -13120 },
};
static int Counter;
static int CorridorArea[8];
static unsigned int NextTime;
static int Unused0, Unused1, Unused2;
static void *MissionData;

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
    ExplosionArea0 = LevelInterface.Engine->CollMap->Coll3dSpace[LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &ExplosionPos0, -1, 1)].Area;
    ExplosionArea1 = LevelInterface.Engine->CollMap->Coll3dSpace[LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &ExplosionPos1, -1, 1)].Area;
    NextTime = 0;
}
static void Zone1Load(void)
{
    int i;

    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    for (i = 0; i < 8; i++)
        CorridorArea[i] = LevelInterface.Engine->CollMap->Coll3dSpace[LevelInterface.Engine->Find3dSpaceIndxLink(LevelInterface.Engine->CollMap, &CorridorPos[i], -1, 1)].Area;
    NextTime = 0;
}
static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
    NextTime = 0;
}
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load };

void DamagePlayerNear(struct VECT *Pos)
{
    struct VECT loc;

    loc = LevelInterface.Engine->Creatures->Box.Location;
    if (Pos->vx - 500 < loc.vx && loc.vx < Pos->vx + 500
        && Pos->vy - 1500 < loc.vy && loc.vy < Pos->vy + 1500
        && Pos->vz - 500 < loc.vz && loc.vz < Pos->vz + 500)
        LevelInterface.Engine->DoDamage(LevelInterface.Engine->Creatures, LevelInterface.Engine->Creatures, 30, 10, Pos);
}

static void CorridorExplosions(void)
{
    struct VECT loc = LevelInterface.Engine->Creatures->Box.Location;

    if (*LevelInterface.Engine->LevelTicks > NextTime) {
        NextTime = *LevelInterface.Engine->LevelTicks + 100;
        Counter = (Counter + 1) & 15;
        if (Counter && (loc.vy < 5200 || Counter < 3) && Counter < 8) {
            LevelInterface.Engine->QuakeGenExplosion(&CorridorPos[Counter], CorridorArea[Counter], (LevelInterface.Engine->rand() & 0x1ff) + 4000, 0);
            DamagePlayerNear(&CorridorPos[Counter]);
        }
    }
}

void Zone0Explosions(void)
{
    if (*LevelInterface.Engine->LevelTicks > NextTime) {
        NextTime = *LevelInterface.Engine->LevelTicks + 900;
        LevelInterface.Engine->QuakeGenExplosion(&ExplosionPos0, ExplosionArea0, (LevelInterface.Engine->rand() & 0x7ff) + 5000, 0);
        LevelInterface.Engine->QuakeGenExplosion(&ExplosionPos1, ExplosionArea1, (LevelInterface.Engine->rand() & 0x7ff) + 5000, 0);
    }
}

static void LevelSpecific(void)
{
    struct VECT unused;    /* unused local: the original frame has 16 bytes of locals */

    if (!*LevelInterface.Engine->GameDeathWatchB)
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"MakeBlitzes"}, 0);
    switch (LevelInterface.Engine->ZoneName->Name[4]) {
    case '0':
        Zone0Explosions();
        break;
    case '1':
        CorridorExplosions();
        break;
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
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Pow1Batches"}, 0);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    NextTime = 0;
    MissionData = LevelInterface.Engine->MissionData;
}
