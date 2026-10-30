#include "ovl.h"

/* The level's mission data (not in types.h). */
struct COMMANDDATA {
    struct IDENTITYNAME HelpObjective;  /* 0x0 */
    struct IDENTITYNAME HelpOrders;     /* 0xc */
    int Unknown18;                      /* 0x18 */
    int Comp1Done;                      /* 0x1c */
    int Comp2Done;                      /* 0x20 */
};

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };
static void Comp1(void);
static void Comp2(void);

struct MISSIONEVENT LevelEvents[] = {
    { {"Comp1"}, Comp1 },
    { {"Comp2"}, Comp2 },
    { {""}, 0 },
};
static int Unused[3];
static unsigned long OldSp;
static struct COMMANDDATA *MissionData;

/* Both computers done: switch the lasers off. */
static int LasersOff(void)
{
    if (MissionData->Comp1Done && MissionData->Comp2Done) {
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"LasersOff"}, 0);
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"TheLasers"}, 1);
        return 1;
    }
    return 0;
}

static void LasersOffMessage(void)
{
    LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit4Curr2"}, (struct IDENTITYNAME){"Unit4Miss1"}, 50);
}

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
}
static void Zone1Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    LasersOff();
}
static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
}
static void Zone3Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone3"});
}
static void Zone4Load(void)
{
    if (!(*LevelInterface.Engine->GlobalKey & 0x800))
        LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone4"});
}
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load };

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
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"CommBatches"}, 0);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    MissionData = LevelInterface.Engine->MissionData;
    LevelInterface.Engine->LevelEvents = LevelEvents;
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
}

static void Comp1(void)
{
    MissionData->Comp1Done = 1;
    if (LasersOff()) {
        OldSp = LevelInterface.Engine->SetSp(*LevelInterface.Engine->usp);
        LasersOffMessage();
        LevelInterface.Engine->SetSp(OldSp);
    }
}

static void Comp2(void)
{
    MissionData->Comp2Done = 1;
    if (LasersOff()) {
        OldSp = LevelInterface.Engine->SetSp(*LevelInterface.Engine->usp);
        LasersOffMessage();
        LevelInterface.Engine->SetSp(OldSp);
    }
}
