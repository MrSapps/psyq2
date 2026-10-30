#include "ovl.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };
static void Zone0Load(void);
static void Zone1Load(void);
static void Zone2Load(void);
static void CheckDrawbridge(void);
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load };
static int Unused[2];
static struct BASEDATA *MissionData;
static int Unused2;
static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
}
static void Zone1Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    CheckDrawbridge();
}
static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
    CheckDrawbridge();
}
/* Once the drawbridge is down, give it a solid box to walk on. */
static void CheckDrawbridge(void)
{
    struct SVECT unused;    /* unused local: the original frame has 8 bytes of locals */
    if ((MissionData->DrawbridgeDown = LevelInterface.Engine->Event_GetDisableNamed((struct IDENTITYNAME){"EDrawbridge"})))
        LevelInterface.Engine->GenSolidBox(&(struct VECTBOX){{-5063, 100, 7039}, {-3918, 304, 9530}}, 0, 1);
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
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Base2Batches"}, 0);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    MissionData = LevelInterface.Engine->MissionData;
    LevelInterface.Engine->PlayThisXA(2);
}
