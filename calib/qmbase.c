#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern struct MISSIONEVENT ThisMissionEvents[];
extern struct MISSIONEVENT *MissionEvents;
extern char *MissionSpecificMemory;
extern void (*MissionPrintStatus)();
extern void (*GameHubLoadSaveData)();
extern short SelfPlay;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
/* defined in QMBase.obj, so $gp-relative */
struct BASEDATA *BaseData;
void *MY_malloc_namehb(int s, char *name);
char *PrintIdentityName(struct IDENTITYNAME Name);
void BaseLoadSaveHub(int Write);
void PrintMissionDetails(void);

void InitMissionBase(void)
{
    Engine.MissionData = MY_malloc_namehb(sizeof(struct BASEDATA), "Base Mission");
    MissionEvents = ThisMissionEvents;
    MissionSpecificMemory = Engine.MissionData;
    MissionPrintStatus = PrintMissionDetails;
    BaseData = (struct BASEDATA *)MissionSpecificMemory;
    GameHubLoadSaveData = BaseLoadSaveHub;
    SelfPlay = 0;
    GameHubLoadSaveData(0);
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
}

char *PrintBool(int Bool);

void PrintMissionDetails(void)
{
    PrintIdentityName(BaseData->HelpObjective);
    PrintIdentityName(BaseData->HelpOrders);
    PrintBool(BaseData->LinkUpComplete);
    PrintBool(BaseData->DrawbridgeDown);
    PrintBool(BaseData->AlreadyDeactivatedDishLasers);
    PrintBool(BaseData->HelpCompStatus);
}
