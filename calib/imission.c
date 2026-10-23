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

extern unsigned int LevelTicks;
static char MissionTitle[32];
static char MissionName_Line1[42];
void DrawDarkenedBox(struct RECT Rect);
void SpriText(struct WINDOW *Window, char *String);
int sprintf();
short CentreText(char *String, short X1, short X2);
short CentreSecretsKills(char *String, short XOffset);

void DBInterMission(void)
{
    struct MISSIONEND *MissionInfo = MissionEnd;
    struct SCRPNT Pos = {MissionRect.x + 20, 36};
    static char String[48];
    char CString[12];
    char Count;
    short Overall_SecretsFound;
    short Overall_SecretsTotal;
    short Overall_Kills;
    short Overall_KillsTotal;
    struct WINDOW *Window;

    Overall_SecretsFound = 0;
    Overall_SecretsTotal = 0;
    Overall_Kills = 0;
    Overall_KillsTotal = 0;
    LevelTicks = 0;
    Window = &GameWindows[4];
    DrawDarkenedBox(MissionRect);
    sprintf(String, "@%03X%2X^BEF0E6|0~4%s", CentreText(MissionTitle, MissionRect.x, MissionRect.x + MissionRect.w), Pos.sy, MissionTitle);
    SpriText(Window, String);
    Pos.sy += 10;
    sprintf(String, "@%03X%2X^DCF082|0~4%s", CentreText(MissionName_Line1, MissionRect.x, MissionRect.x + MissionRect.w), Pos.sy, MissionName_Line1);
    SpriText(Window, String);
    Pos.sy += 8;
    if (strlen(MissionName_Line2)) {
        sprintf(String, "@%03X%2X^DCF082|0~4%s", CentreText(MissionName_Line2, MissionRect.x, MissionRect.x + MissionRect.w), Pos.sy, MissionName_Line2);
        SpriText(Window, String);
        Pos.sy += 8;
    }
    Pos.sy += 6;
    sprintf(String, "@%03X%2X^BEF0E6|0Location", Pos.sx, Pos.sy);
    SpriText(Window, String);
    sprintf(String, "@%03X%2X^BEF0E6|0Secrets", Pos.sx + 176, Pos.sy);
    SpriText(Window, String);
    sprintf(String, "@%03X%2X^BEF0E6|0Kills", Pos.sx + 256, Pos.sy);
    SpriText(Window, String);
    Pos.sy += 12;
    for (Count = 0; Count < 6; MissionInfo++, Count++) {
        if (strlen(MissionInfo->MapTitle) > 0) {
            sprintf(String, "@%3X%2X^DCF082|0~8%s", Pos.sx, Pos.sy, MissionInfo->MapTitle);
            SpriText(Window, String);
            sprintf(CString, "%d/%d", MissionInfo->SecretsFound, MissionInfo->SecretsTotal);
            sprintf(String, "@%3X%2X^DCF082|0%s", CentreSecretsKills(CString, Pos.sx + 176), Pos.sy, CString);
            SpriText(Window, String);
            sprintf(CString, "%d/%d", MissionInfo->Kills, MissionInfo->KillsTotal);
            sprintf(String, "@%3X%2X^DCF082|0%s", CentreSecretsKills(CString, Pos.sx + 248), Pos.sy, CString);
            SpriText(Window, String);
            Overall_SecretsFound += MissionInfo->SecretsFound;
            Overall_SecretsTotal += MissionInfo->SecretsTotal;
            Overall_Kills += MissionInfo->Kills;
            Overall_KillsTotal += MissionInfo->KillsTotal;
            Pos.sy += 10;
        }
    }
    sprintf(String, "@%03X%2X^BEF0E6|0Totals:", Pos.sx + 112, Pos.sy);
    SpriText(Window, String);
    sprintf(CString, "%d/%d", Overall_SecretsFound, Overall_SecretsTotal);
    sprintf(String, "@%3X%2X^C8F000|0%s", CentreSecretsKills(CString, Pos.sx + 176), Pos.sy, CString);
    SpriText(Window, String);
    sprintf(CString, "%d/%d", Overall_Kills, Overall_KillsTotal);
    sprintf(String, "@%3X%2X^C8F000|0%s~4", CentreSecretsKills(CString, Pos.sx + 248), Pos.sy, CString);
    SpriText(Window, String);
}

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

char *strrchr();
char *strncpy();

void SplitMissionName(char *MissionName)
{
    char *LastWord;
    char TString[82] = "\0";
    short Len;

    memset(MissionName_Line1, 0, 42);
    memset(MissionName_Line2, 0, 42);
    strcpy(MissionName_Line1, "\0");
    strcpy(MissionName_Line2, "\0");
    if (strlen(MissionName) < 36) {
        strcpy(MissionName_Line1, MissionName);
        strcpy(MissionName_Line2, "\0");
    } else {
        strcpy(TString, MissionName);
        while (1) {
            LastWord = strrchr(TString, ' ');
            Len = strlen(TString) - strlen(LastWord);
            TString[Len] = 0;
            if (Len < 36) {
                strncpy(MissionName_Line1, MissionName, Len);
                strncpy(MissionName_Line2, MissionName + Len, strlen(MissionName) - Len);
                break;
            }
        }
    }
}
