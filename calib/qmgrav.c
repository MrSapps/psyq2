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

extern char GameDeathWatchA;
extern char GameDeathWatchB;
extern char GameDeathWatchC;
extern struct IDENTITYNAME LastObjective;
extern struct IDENTITYNAME LastOrders;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
void HelpComputer_SetMessages(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective);
void HelpComputer_GetMessages(struct IDENTITYNAME *Orders, struct IDENTITYNAME *Objective);
int HelpComputer_Reset(void);
void *memset(void *s, int c, unsigned int n);

static const struct IDENTITYNAME MissionName = {"Mission 4"};

void MissionLoadSaveHub_800241A8(int Write)
{
    struct SAVEFILEHDR *FileHdr;
    struct GRAVDATA *SaveGravData;

    FileHdr = OpenSaveMemory(MissionName, Write);
    if (FileHdr) {
        SaveGravData = (struct GRAVDATA *)(FileHdr + 1);
        if (Write) {
            HelpComputer_GetMessages(&GravData->HelpOrders, &GravData->HelpObjective);
            *SaveGravData = *GravData;
            FileHdr->Size = sizeof(struct GRAVDATA);
            CloseWritableSaveMemoryFile(MissionName, 2);
        } else {
            *GravData = *SaveGravData;
            HelpComputer_SetMessages(GravData->HelpOrders, GravData->HelpObjective);
        }
    } else {
        memset(GravData, 0, sizeof(struct GRAVDATA));
        GravData->HelpObjective = LastObjective;
        GravData->HelpOrders = LastOrders;
        GameDeathWatchC = 0;
        GameDeathWatchB = 0;
        GameDeathWatchA = 0;
        if (!NextLevelName.Name[0]) {
            NextLevelName = *(struct IDENTITYNAME *)"Command";
            MainStartName = *(struct IDENTITYNAME *)"Default";
            HelpComputer_Reset();
        }
    }
}
