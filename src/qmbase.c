#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void BaseLoadSaveHub(int Write);
void PrintMissionDetails(void); void InitMissionBase(void);
void PrintMissionDetails(void);
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

char *PrintBool(int Bool);

extern char GameDeathWatchA;
extern char GameDeathWatchB;
extern char GameDeathWatchC;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
void HelpComputer_SetMessages(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective);
void HelpComputer_GetMessages(struct IDENTITYNAME *Orders, struct IDENTITYNAME *Objective);
int HelpComputer_Reset(void);
void *memset(void *s, int c, unsigned int n);

void BaseLoadSaveHub(int Write)
{
    struct SAVEFILEHDR *FileHdr;
    struct BASEDATA *SaveBaseData;

    FileHdr = OpenSaveMemory((struct IDENTITYNAME){"Mission 1"}, Write);
    if (FileHdr) {
        SaveBaseData = (struct BASEDATA *)(FileHdr + 1);
        if (Write) {
            HelpComputer_GetMessages(&BaseData->HelpOrders, &BaseData->HelpObjective);
            BaseData->DishGuardsToKill = GameDeathWatchA;
            *SaveBaseData = *BaseData;
            FileHdr->Size = sizeof(struct BASEDATA);
            CloseWritableSaveMemoryFile((struct IDENTITYNAME){"Mission 1"}, 2);
        } else {
            HelpComputer_SetMessages(BaseData->HelpOrders, BaseData->HelpObjective);
            *BaseData = *SaveBaseData;
            GameDeathWatchA = BaseData->DishGuardsToKill;
        }
    } else {
        memset(BaseData, 0, sizeof(struct BASEDATA));
        BaseData->DishGuardsToKill = 1;
        GameDeathWatchA = BaseData->DishGuardsToKill;
        GameDeathWatchC = 0;
        GameDeathWatchB = 0;
        if (!NextLevelName.Name[0]) {
            NextLevelName = (struct IDENTITYNAME){"Base0"};
            MainStartName = (struct IDENTITYNAME){"Default"};
            HelpComputer_Reset();
        }
    }
}

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

void PrintMissionDetails(void)
{
    PrintIdentityName(BaseData->HelpObjective);
    PrintIdentityName(BaseData->HelpOrders);
    PrintBool(BaseData->LinkUpComplete);
    PrintBool(BaseData->DrawbridgeDown);
    PrintBool(BaseData->AlreadyDeactivatedDishLasers);
    PrintBool(BaseData->HelpCompStatus);
}
