#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void InitMissionState(void);
void MissionLoadSaveHub_8002522C(int Write);
void InitMissionTribute(void);

/* defined in QMTrib.obj, so $gp-relative */
struct TRIBUTEDATA *TributeData;

extern struct QENGINELEVINTERFACE Engine;
extern char GameDeathWatchA;
extern char GameDeathWatchB;
extern char GameDeathWatchC;
extern char *MissionSpecificMemory;
extern struct MISSIONEVENT *MissionEvents;
extern void (*GameHubLoadSaveData)();
extern short SelfPlay;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
/* QMTrib.obj's static ThisMissionEvents, under its splat name */
extern struct MISSIONEVENT ThisMissionEvents_8009B934[];

void DeleteSaveMemory(struct IDENTITYNAME Name);
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
void *MY_malloc_namehb(int s, char *name);
void *memset(void *s, int c, unsigned int n);

static const struct IDENTITYNAME MissionName = {"Mission 6"};

/* static in the original */
void InitMissionState(void)
{
    DeleteSaveMemory(MissionName);
    memset(TributeData, 0, 4);
    GameDeathWatchA = 0;
    GameDeathWatchB = 0;
    GameDeathWatchC = 0;
}

/* static MissionLoadSaveHub in the original; splat named it by address */
void MissionLoadSaveHub_8002522C(int Write)
{
    struct SAVEFILEHDR *FileHdr;

    FileHdr = OpenSaveMemory(MissionName, Write);
    if (FileHdr) {
        if (Write) {
            *(struct TRIBUTEDATA *)(FileHdr + 1) = *TributeData;
            FileHdr->Size = 4;
            CloseWritableSaveMemoryFile(MissionName, 2);
        } else {
            *TributeData = *(struct TRIBUTEDATA *)(FileHdr + 1);
        }
    }
}

void InitMissionTribute(void)
{
    Engine.MissionData = TributeData = (struct TRIBUTEDATA *)(MissionSpecificMemory = MY_malloc_namehb(4, "Tribute Mission"));
    MissionEvents = ThisMissionEvents_8009B934;
    GameHubLoadSaveData = MissionLoadSaveHub_8002522C;
    SelfPlay = 0;
    InitMissionState();
    if (!*(int *)&NextLevelName) {
        NextLevelName = *(struct IDENTITYNAME *)"HalfLife";
        MainStartName = *(struct IDENTITYNAME *)"Default";
    }
}
