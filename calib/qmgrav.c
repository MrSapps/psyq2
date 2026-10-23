#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern struct MISSIONEVENT ThisMissionEvents_8009B914[];
extern struct MISSIONEVENT *MissionEvents;
extern char *MissionSpecificMemory;
extern void (*MissionPrintStatus)();
extern void (*GameHubLoadSaveData)();
extern short SelfPlay;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
/* defined in QMGrav.obj, so $gp-relative */
struct GRAVDATA *GravData;
void *MY_malloc_namehb(int s, char *name);
char *PrintIdentityName(struct IDENTITYNAME Name);
char *PrintBool(int Bool);
/* static in the original; splat's suffixed names */
void MissionLoadSaveHub_800241A8(int Write);
void PrintMissionDetails_8002478C(void);

void InitMissionGrav(void)
{
    Engine.MissionData = GravData = (struct GRAVDATA *)(MissionSpecificMemory = MY_malloc_namehb(sizeof(struct GRAVDATA), "Grav Mission"));
    MissionEvents = ThisMissionEvents_8009B914;
    MissionPrintStatus = PrintMissionDetails_8002478C;
    SelfPlay = 0;
    GameHubLoadSaveData = MissionLoadSaveHub_800241A8;
    GameHubLoadSaveData(0);
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
}

void PrintMissionDetails_8002478C(void)
{
    PrintIdentityName(GravData->HelpObjective);
    PrintIdentityName(GravData->HelpOrders);
    PrintBool(GravData->ReactorDestroyed);
    PrintBool(GravData->ShotComp[0]);
    PrintBool(GravData->ShotComp[1]);
    PrintBool(GravData->HelpCompStatus);
}
