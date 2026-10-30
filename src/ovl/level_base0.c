#include "ovl.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };
static void Zone0Load(void);

/* Moves the four crates of the "CRATES" event along y, wrapping them back up once they pass below -1044. */
static void DoCrates(void)
{
    short offset;
    struct PSXEVDOOR1UP *ev;
    short i;

    offset = LevelInterface.Engine->Event_GetNamedOffset((struct IDENTITYNAME){"CRATES"});
    if (offset) {
        ev = (struct PSXEVDOOR1UP *)(offset + (int)*LevelInterface.Engine->eventlistbase + 4);
        for (i = 0; i < 4; i++) {
            if (ev->crossobj[i] >= 0) {
                struct BRUSHMOVEBUF *mb;
                short sceneobj;
                int speed;
                short step;

                mb = LevelInterface.Engine->BrushMoveBufs;
                mb += ev->crossobj[i];
                mb->Primary = 0;
                sceneobj = mb->Obj;
                speed = 16;
                if (i >= 2)
                    speed = 20;
                step = speed * *LevelInterface.Engine->NewVcounts;
                mb->Offset.vy += step / 8;
                if ((short)(mb->Offset.vy + (LevelInterface.Engine->Scene2Data[sceneobj].VectBox.Min.vy + LevelInterface.Engine->Scene2Data[sceneobj].VectBox.Max.vy) / 2) >= -1044)
                    mb->Offset.vy -= 3500;
            }
        }
    }
}

struct MISSIONEVENT LevelEvents[] = {
    { {"DOCRATES"}, DoCrates },
    { {""}, 0 },
};
void *ZoneLoadTable[] = { Zone0Load };
static int Unused[2];
static void *MissionData;
static int Unused2;

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
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
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"Base0Batches"}, 0);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    MissionData = LevelInterface.Engine->MissionData;
    LevelInterface.Engine->LevelEvents = LevelEvents;
    LevelInterface.Engine->PlayThisXA(4);
}
