#include "types.h"

extern struct MISSIONEND MissionEnd[];
extern int GlobalKillsCount;
extern int GlobalKills;
extern int GlobalSecretsCount;
extern int GlobalSecretsFound;
extern struct PLAYER Players[];
extern struct WINDOW GameWindows[];

struct RECT MissionRect = { 84, 30, 344, 124 };
static char MissionName_Line2[42];

int strcmp();
int strlen();
void MoveQuakeCameraInterMission(int Win);
void GetMissionName(void);
short PreProcessInterMission(void);

void ResetGlobalSecretsKills(char *CurrentMapTitle)
{
    struct MISSIONEND *MissionInfo;
    char Count;

    MissionInfo = MissionEnd;
    for (Count = 0; Count < 6; Count++, MissionInfo++) {
        if (!strcmp(MissionInfo->MapTitle, CurrentMapTitle)) {
            GlobalSecretsFound = MissionInfo->SecretsFound;
            GlobalSecretsCount = MissionInfo->SecretsTotal;
            GlobalKills = MissionInfo->Kills;
            GlobalKillsCount = MissionInfo->KillsTotal;
            return;
        }
    }
}

char AddSecretInterMission(char *CurrentMapTitle)
{
    struct MISSIONEND *MissionInfo;
    char Count;

    MissionInfo = MissionEnd;
    for (Count = 0; Count < 6; Count++, MissionInfo++) {
        if (!strcmp(MissionInfo->MapTitle, CurrentMapTitle)) {
            MissionInfo->SecretsFound = GlobalSecretsFound;
            return 1;
        }
    }
    return 0;
}

char AddKillInterMission(char *CurrentMapTitle)
{
    struct MISSIONEND *MissionInfo;
    char Count;

    MissionInfo = MissionEnd;
    for (Count = 0; Count < 6; Count++, MissionInfo++) {
        if (!strcmp(MissionInfo->MapTitle, CurrentMapTitle)) {
            MissionInfo->Kills = GlobalKills;
            return 1;
        }
    }
    return 0;
}

void PrintLevelInterMission(void)
{
    char Count;

    for (Count = 0; Count < 6; Count++) {
    }
}

void SetInterMission(void)
{
    struct PLAYER *Player;

    GameWindows[0].CameraRoutine = MoveQuakeCameraInterMission;
    Player = &Players[0];
    Player->Extras.GunSightType = 0;
    GetMissionName();
    MissionRect.h = PreProcessInterMission();
}

short CentreSecretsKills(char *String, short XOffset)
{
    short Length;

    Length = strlen(String);
    if (!(Length & 1))
        Length--;
    Length = (56 - Length * 8) >> 1;
    return Length + XOffset;
}

short PreProcessInterMission(void)
{
    struct MISSIONEND *MissionInfo;
    short Height;
    char Count;

    MissionInfo = MissionEnd;
    Height = 24;
    if (strlen(MissionName_Line2))
        Height = 32;
    Height += 18;
    for (Count = 0; Count < 6; Count++, MissionInfo++) {
        if (strlen(MissionInfo->MapTitle) > 0)
            Height += 10;
    }
    return Height + 14;
}

extern int NewVcounts;
extern int LevelIsMultiPlayer;
extern int NewMission;
extern int Mission;
static char MissionTitle[32];
int MapPadInputToFire(int Player);
char *strcpy();
void *memset();
int sprintf();
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
void SplitMissionName(char *MissionName);

void ClearLevelIntermission(void)
{
    struct SAVEFILEHDR *FileHdr;

    if (!LevelIsMultiPlayer) {
        FileHdr = OpenSaveMemory(*(struct IDENTITYNAME *)"PlayerSave0", 0);
        if (FileHdr)
            memset((char *)FileHdr + 0xe4, 0, 0x96);
    }
}

void GetMissionName(void)
{
    struct IDENTITYNAME MissionLabel;

    memset(&MissionLabel, 0, 12);
    sprintf(MissionLabel.Name, "Unit%dMiss1", Mission);
    SplitMissionName(GetStringFromStringtable(MissionLabel));
    sprintf(MissionTitle, "Mission  %d  -  Complete", Mission);
}

char AddLevelInterMission(char *CurrentMapTitle)
{
    struct MISSIONEND *MissionInfo;
    char Count;

    MissionInfo = MissionEnd;
    if (NewMission)
        NewMission = 0;
    if (!CurrentMapTitle)
        return 0;
    for (Count = 0; Count < 6; Count++, MissionInfo++) {
        if (!strcmp(MissionInfo->MapTitle, CurrentMapTitle))
            return 0;
        if (!strcmp(MissionInfo->MapTitle, "")) {
            strcpy(MissionInfo->MapTitle, CurrentMapTitle);
            MissionInfo->SecretsFound = GlobalSecretsFound;
            MissionInfo->SecretsTotal = GlobalSecretsCount;
            MissionInfo->Kills = GlobalKills;
            MissionInfo->KillsTotal = GlobalKillsCount;
            return 1;
        }
    }
    return 0;
}

char PadEndInterMission(void)
{
    static short DelayPad = 1200;

    DelayPad -= NewVcounts;
    if (DelayPad > 0)
        return 0;
    DelayPad = 0;
    if (MapPadInputToFire(0)) {
        DelayPad = 1200;
        return 4;
    }
    return 0;
}

short CentreText(char *String, short X1, short X2)
{
    char *TString;
    int Length;
    int Spaces;

    Spaces = 0;
    if (String == "")
        return 0;
    TString = String;
    while (*TString++) {
        if (*TString == ' ')
            Spaces++;
    }
    Length = strlen(String) - Spaces;
    Length = Spaces * 4 + Length * 8;
    Length = ((X2 - X1 - Length) >> 1) + X1;
    return Length;
}
