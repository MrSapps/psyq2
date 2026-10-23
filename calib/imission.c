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
