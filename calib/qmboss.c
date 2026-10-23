#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern struct MISSIONEVENT ThisMissionEvents_8009B924[];
extern struct MISSIONEVENT *MissionEvents;
extern char *MissionSpecificMemory;
extern void (*MissionPrintStatus)();
extern void (*GameHubLoadSaveData)();
extern short SelfPlay;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
/* defined in QMBoss.obj, so $gp-relative */
struct BOSSDATA *BossData;
void *MY_malloc_namehb(int s, char *name);
char *PrintIdentityName(struct IDENTITYNAME Name);
char *PrintBool(int Bool);
void MissionLoadSaveHub_80024918(int Write);
void PrintMissionDetails_80024F94(void);

void InitMissionBoss(void)
{
    MissionSpecificMemory = MY_malloc_namehb(sizeof(struct BOSSDATA), "Boss Mission");
    BossData = (struct BOSSDATA *)MissionSpecificMemory;
    Engine.MissionData = BossData;
    MissionEvents = ThisMissionEvents_8009B924;
    MissionPrintStatus = PrintMissionDetails_80024F94;
    GameHubLoadSaveData = MissionLoadSaveHub_80024918;
    SelfPlay = 0;
    GameHubLoadSaveData(0);
    PrintIdentityName(NextLevelName);
    PrintIdentityName(MainStartName);
}

void PrintMissionDetails_80024F94(void)
{
    PrintIdentityName(BossData->HelpObjective);
    PrintIdentityName(BossData->HelpOrders);
    PrintBool(BossData->KilledJorg);
    PrintBool(BossData->KilledMakron);
    PrintBool(BossData->Laser0Pressed);
    PrintBool(BossData->Laser1Pressed);
    PrintBool(BossData->Stairs0Pressed);
    PrintBool(BossData->Stairs1Pressed);
    PrintBool(BossData->HelpCompStatus);
}
