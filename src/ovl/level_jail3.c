/* Jail3 (mission 2, unit 2): the drawbridge and the two pyramid keys */
#include "ovl.h"
#include "bss.h"

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

static void Zone1Load(void);
static void Zone2Load(void);
static void Zone3Load(void);
void DrawBridgeCheck(void);

static void DrawBridge(void);
static void BothKeys(void);

static struct MISSIONEVENT MissionEvents[] = {
    { { "Bridge" }, DrawBridge },
    { { "BothKeys" }, BothKeys },
    { { "" }, 0 },
};

/* defined into .bss in order (word aligned), as the original lays them out;
   the unused arrays fill the space the original's bss has */
static int CurrentZone BSS;
static int Unused0[72] BSS;
static struct JAILDATA *MissionData BSS;
static int Unused1[70] BSS;

static void Zone0Load(void)
{
    if (!(MissionData->HelpCompStatus & 1)) {
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr2"}, (struct IDENTITYNAME){"Unit2Miss1"}, 50);
        MissionData->HelpCompStatus |= 1;
    }
    CurrentZone = 0;
}

static void Zone4Load(void)
{
    if (*LevelInterface.Engine->GlobalKey & 0x20) {
        LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Jail5Return"});
        if (CurrentZone == -1) {
            if (*LevelInterface.Engine->GlobalKey & 0x40)
                LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr7"}, (struct IDENTITYNAME){"Unit2Miss1"}, 50);
            else
                LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr3"}, (struct IDENTITYNAME){"Unit2Miss1"}, 50);
        }
    } else if (CurrentZone == -1 && !(MissionData->HelpCompStatus & 2))
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr2"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
    CurrentZone = 4;
}

static void Zone5Load(void)
{
    if (*LevelInterface.Engine->GlobalKey & 0x40) {
        LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Jail4Return"});
        if (CurrentZone == -1) {
            if (*LevelInterface.Engine->GlobalKey & 0x20)
                LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr7"}, (struct IDENTITYNAME){"Unit2Miss1"}, 50);
            else
                LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr6"}, (struct IDENTITYNAME){"Unit2Miss1"}, 50);
        }
    } else if (CurrentZone == -1 && !(MissionData->HelpCompStatus & 2))
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr2"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
    CurrentZone = 5;
}

void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load, Zone5Load };

void InitLevel(void)
{
    if (LevelInterface.Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            LevelInterface.Engine->PrintF("Please get a programmer to recompile level %s\n",
                LevelInterface.Engine->PrintIdentityName(*LevelInterface.Engine->ThisLevelName));
        LevelInterface.Engine->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINELEVINTERFACE), LevelInterface.Engine->SizeOf);
    }
    LevelInterface.Engine->PrintF("InitLevQuakeJail3\n");
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Jail3Batches"}, 0);
    LevelInterface.Engine->LevelEvents = MissionEvents;
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    CurrentZone = -1;
    MissionData = LevelInterface.Engine->MissionData;
}

static void BothKeys(void)
{
    int Keys;

    Keys = *LevelInterface.Engine->GlobalKey & 0x60;
    switch (Keys) {
    case 0:
        LevelInterface.Engine->centreprintf(LevelInterface.Engine->GameWindows, "You need both Pyramid Keys");
        if (!(MissionData->HelpCompStatus & 2))
            LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr3"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
        break;
    case 0x20:
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr3"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
        break;
    case 0x40:
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2Curr6"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
        break;
    case 0x60:
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit2CurrA"}, (struct IDENTITYNAME){"Unit2Miss1"}, 100);
        break;
    }
    MissionData->HelpCompStatus |= 2;
    LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"PyramidOpen"}, Keys == 0);
}

static void DrawBridge(void)
{
    struct SOLIDBOX *Box;
    int Scene;

    Box = LevelInterface.Engine->GenSolidBox(&(struct VECTBOX){ -10878, -7039, 6639, -7839, -6839, 7919 }, 0, 1);
    if (Box) {
        MissionData->DrawBridgeDown = 1;
        Scene = LevelInterface.Engine->GetSceneNum((struct IDENTITYNAME){"DrawBridge"});
        if (Scene != -1)
            LevelInterface.Engine->Scene2Data[Scene].VectBox = Box->MinMax;
    }
}

static void Zone1Load(void)
{
    DrawBridgeCheck();
    CurrentZone = 1;
}

static void Zone2Load(void)
{
    CurrentZone = 2;
}

static void Zone3Load(void)
{
    CurrentZone = 3;
}

void DrawBridgeCheck(void)
{
    if (MissionData->DrawBridgeDown)
        DrawBridge();
}
