#include "ovl.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

struct IDENTITYNAME LiftNames[2] = { {"LiftStemA"}, {"LiftStemB"} };
static int QuakeTime;
static int Unused0;
static int QuakeSound;
static int Unused1, Unused2;
static struct BOSSDATA *MissionData;
static int ShakeTime;

/* Put the two lifts back where StoreLifts saw them last. */
static void RestoreLifts(void)
{
    int i;
    struct SVECT unused;    /* unused local: the original frame has 8 bytes of locals */

    for (i = 0; i < 2; i++) {
        MissionData->Boss2Lifts[i].Object = LevelInterface.Engine->GetSceneNum(LiftNames[i]);
        MissionData->Boss2Lifts[i].MoveBuf = 0;
        if (MissionData->Boss2Lifts[i].Object != -1 && LevelInterface.Engine->Scene2Data[MissionData->Boss2Lifts[i].Object].MoveBufId)
            MissionData->Boss2Lifts[i].MoveBuf = LevelInterface.Engine->BrushMoveBufs + LevelInterface.Engine->Scene2Data[MissionData->Boss2Lifts[i].Object].MoveBufId - 1;
        if (MissionData->Boss2Lifts[i].MoveBuf && MissionData->Boss2Lifts[i].ValidData) {
            MissionData->Boss2Lifts[i].MoveBuf->Time = MissionData->Boss2Lifts[i].Time;
            MissionData->Boss2Lifts[i].MoveBuf->Offset.vy = MissionData->Boss2Lifts[i].OffsetY;
            MissionData->Boss2Lifts[i].MoveBuf->OpenFlag = MissionData->Boss2Lifts[i].OpenFlag;
            if (MissionData->Boss2Lifts[i].MoveBuf->Link)
                MissionData->Boss2Lifts[i].MoveBuf->Link->Offset.vy = MissionData->Boss2Lifts[i].OffsetY;
            if (MissionData->Boss2Lifts[i].MoveBuf->SolidBox) {
                MissionData->Boss2Lifts[i].MoveBuf->SolidBox->MinMax = MissionData->Boss2Lifts[i].SolidMinMax;
                MissionData->Boss2Lifts[i].MoveBuf->SolidBox->CMinMax = MissionData->Boss2Lifts[i].SolidCMinMax;
                MissionData->Boss2Lifts[i].MoveBuf->SolidBox->Moved = MissionData->Boss2Lifts[i].SolidMoved;
            }
        }
    }
}

/* Remember where the lifts are, for RestoreLifts after a zone load. */
static void StoreLifts(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (MissionData->Boss2Lifts[i].MoveBuf) {
            MissionData->Boss2Lifts[i].ValidData = 1;
            MissionData->Boss2Lifts[i].OpenFlag = MissionData->Boss2Lifts[i].MoveBuf->OpenFlag;
            MissionData->Boss2Lifts[i].OffsetY = MissionData->Boss2Lifts[i].MoveBuf->Offset.vy;
            MissionData->Boss2Lifts[i].Time = MissionData->Boss2Lifts[i].MoveBuf->Time;
            if (MissionData->Boss2Lifts[i].MoveBuf->SolidBox) {
                MissionData->Boss2Lifts[i].SolidMinMax = MissionData->Boss2Lifts[i].MoveBuf->SolidBox->MinMax;
                MissionData->Boss2Lifts[i].SolidCMinMax = MissionData->Boss2Lifts[i].MoveBuf->SolidBox->CMinMax;
                MissionData->Boss2Lifts[i].SolidMoved = MissionData->Boss2Lifts[i].MoveBuf->SolidBox->Moved;
            }
        }
    }
}

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
    RestoreLifts();
    if (MissionData->KilledMakron) {
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"DoorToTrain"}, 0);
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"DoorString"}, 1);
    }
}
static void Zone1Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    RestoreLifts();
}
static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
    RestoreLifts();
}
static void Zone3Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone3"});
    RestoreLifts();
}
static void Zone4Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone4"});
    RestoreLifts();
}

static void LevelSpecific(void)
{
    StoreLifts();
    /* the earthquake: shake the player now and then */
    if (LevelInterface.Engine->GameWindows->Creature) {
        if (QuakeTime != -9999)
            QuakeTime += *LevelInterface.Engine->NewVcounts;
        if (QuakeTime > 300) {
            ShakeTime += *LevelInterface.Engine->NewVcounts;
            while (ShakeTime > 0) {
                ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vx += (LevelInterface.Engine->rand() - 0x4000) / 12;
                ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vy += (LevelInterface.Engine->rand() - 0x4000) / 16;
                ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->ForceVel.vz += (LevelInterface.Engine->rand() - 0x4000) / 12;
                ((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->Control |= 0x4000;
                LevelInterface.Engine->SetShockData(4, 13, (LevelInterface.Engine->rand() & 0xfff) + 1024, 0, (struct POWER){0x800, 0x1000});
                if (LevelInterface.Engine->rand() < 0x2000)
                    LevelInterface.Engine->SoundAtLocation(QuakeSound, &((struct CREATURE *)LevelInterface.Engine->GameWindows->Creature)->Box.Location);
                ShakeTime -= 30;
            }
            if (QuakeTime > 3000)
                QuakeTime = -3000;
        }
    }
    if (!MissionData->KilledMakron && !*LevelInterface.Engine->GameDeathWatchB) {
        MissionData->KilledMakron = 1;
        LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone4"});
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"DoorToTrain"}, 0);
        LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"DoorString"}, 1);
        LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit5Curr4"}, (struct IDENTITYNAME){"Unit5Miss2"}, 50);
        QuakeTime = 0;
    }
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
    QuakeSound = LevelInterface.Engine->GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
    QuakeTime = -9999;
    ShakeTime = 0;
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Boss2Batches"}, 0);
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    MissionData = LevelInterface.Engine->MissionData;
    LevelInterface.Engine->PrintF("Bossdata=%x\n", MissionData);
    RestoreLifts();
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit5Curr2"}, (struct IDENTITYNAME){"Unit5Miss1"}, 50);
}
