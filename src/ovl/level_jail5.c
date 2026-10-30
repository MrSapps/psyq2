#include "ovl.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };
static void ButtonPressed(void);
static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
}
static void Zone1Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
}
static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
}
static void Zone3Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone3"});
}
struct MISSIONEVENT LevelEvents[] = {
    { {"BUTPRESSED"}, ButtonPressed },
    { {""}, 0 },
};
void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load };
static int Unused[2];
static void *MissionData;
static int Unused2;
/* Every frame: once the exploding box has gone, release the quad damage that was in it. */
static void LevelSpecific(void)
{
    if (LevelInterface.Engine->Event_GetDisableNamed((struct IDENTITYNAME){"ExplodingBox"}))
        LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"QuadInCrate"});
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
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Jail5 Bat"}, 0);
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->LevelEvents = LevelEvents;
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    MissionData = LevelInterface.Engine->MissionData;
}
static void ButtonPressed(void)
{
}
