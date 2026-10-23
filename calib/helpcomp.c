#include "types.h"

extern struct WINDOW GameWindows[];
extern unsigned int LevelTicks;
extern int Paused;
static int StopTime;
int HelpComputerOn;
struct IDENTITYNAME LastObjective;
struct IDENTITYNAME LastOrders;

extern int Pauser;
extern int PalNtscRate;
extern struct CREATURE Creatures[48];
int ActivationCountdown;
static int NextDisplayPeriod;
static int AllowButtonExit;
void HelpComputer_SetMessages(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective);
void HelpComputer_StartDisplay(int FramesB4Start, int SecondsToStay);
int GetSoundNum(struct IDENTITYNAME Name);
int SoundAtLocation(int SFXNumber, struct VECT *loc);


char *CurrentOrders;
char *MissionObjective;
/* static in HelpComp.obj; declared global here so the assembler keeps the symbol
   in the relocation instead of rewriting it to .data+offset */
char NoOrders[17] = "Awaiting Orders.";
char NoMission[28] = "Awaiting Mission Objective.";
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
char *PrintIdentityName(struct IDENTITYNAME Name);

void HelpComputer_SetMessages(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective)
{
    LastOrders = Orders;
    LastObjective = Objective;
    CurrentOrders = GetStringFromStringtable(Orders);
    if (!CurrentOrders)
        CurrentOrders = NoOrders;
    MissionObjective = GetStringFromStringtable(Objective);
    if (!MissionObjective)
        MissionObjective = NoMission;
    PrintIdentityName(Orders);
    PrintIdentityName(Objective);
}

void HelpComputer_NewMessage(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective, int FrameDelay)
{
    if (!GameWindows[0].Input.SelfPlayMode) {
        HelpComputer_SetMessages(Orders, Objective);
        if (FrameDelay < 5)
            FrameDelay = 5;
        HelpComputer_StartDisplay(FrameDelay, 15);
    }
}

void HelpComputer_GetMessages(struct IDENTITYNAME *Orders, struct IDENTITYNAME *Objective)
{
    *Orders = LastOrders;
    *Objective = LastObjective;
}

void HelpComputer_StopDisplay(int DoPad)
{
    HelpComputerOn = 0;
    Paused = 0;
    StopTime = LevelTicks;
    if (DoPad) {
        GameWindows[0].Input.Padd &= ~0x4000;
        GameWindows[0].Input.Original &= ~0x4000;
    }
}

int HelpComputer_Reset(void)
{
    LastOrders = *(struct IDENTITYNAME *)"";
    LastObjective = *(struct IDENTITYNAME *)"";
    HelpComputer_StopDisplay(0);
}

void HelpComputer_StartDisplay(int FramesB4Start, int SecondsToStay)
{
    SoundAtLocation(GetSoundNum((struct IDENTITYNAME){"msc_comp_up"}), &Creatures[0].Draw.Location);
    if (FramesB4Start) {
        NextDisplayPeriod = SecondsToStay;
        ActivationCountdown = FramesB4Start * PalNtscRate * 2;
    } else {
        HelpComputerOn = 1;
        Paused = 1;
        Pauser = 0;
        if (SecondsToStay <= 0)
            SecondsToStay = 0x127500;
        GameWindows[0].Input.Padd = 0;
        GameWindows[0].Input.OPadd = 0;
        GameWindows[0].Input.Original = 0;
        AllowButtonExit = 0;
        StopTime = SecondsToStay * 300 + LevelTicks;
    }
}

extern int LevelIsMultiPlayer;
extern int LevelChange;
extern int NewVcounts;
void SetCursorPos(struct WINDOW *Window, struct SCRPNT Pos);
void SpriText();
void DrawDarkenedBox(struct RECT Rect);

void HelpComputer_DoDisplay(void)
{
    static int ThisLevel = -2;
    struct SCRPNT Pos;
    static struct RECT Rect = {0x60, 0x20, 0x150, 0x64};
    static struct COLOUR Colour1 = {0x50, 0x8c, 0x78, 0};
    static struct COLOUR Colour2 = {0x78, 0x8c, 0x50, 0};
    struct WINDOW *Window;

    Window = &GameWindows[4];
    if (GameWindows[0].Input.SelfPlayMode || LevelIsMultiPlayer)
        return;
    if (ThisLevel != LevelChange) {
        ThisLevel = LevelChange;
        StopTime = LevelTicks;
    }
    if (HelpComputerOn) {
        if (StopTime > LevelTicks) {
            Pos.sx = 0x100;
            Pos.sy = Rect.y;
            SetCursorPos(Window, Pos);
            SpriText(Window, "#06A196");
            SpriText(Window, "|0\n");
            Window->Messages.TextColour = Colour1;
            SpriText(Window, "~4Location:\n");
            Window->Messages.TextColour = Colour2;
            SpriText(Window, "%s\n\n", GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
            Window->Messages.TextColour = Colour1;
            SpriText(Window, "Current Orders:\n");
            Window->Messages.TextColour = Colour2;
            SpriText(Window, "%s\n", CurrentOrders);
            Window->Messages.TextColour = Colour1;
            SpriText(Window, "\nMission Objective:\n");
            Window->Messages.TextColour = Colour2;
            SpriText(Window, "%s\n", MissionObjective);
            SpriText(Window, "#000000");
            DrawDarkenedBox(Rect);
        } else
            HelpComputer_StopDisplay(0);
        if (AllowButtonExit) {
            if ((GameWindows[0].Input.OPadd & 0x4000) && !(GameWindows[0].Input.Original & 0x4000))
                HelpComputer_StopDisplay(0);
        }
        if (!(GameWindows[0].Input.Padd & 0x4000))
            AllowButtonExit = 1;
    }
    if (ActivationCountdown) {
        ActivationCountdown -= NewVcounts;
        if (ActivationCountdown < 0)
            ActivationCountdown = 0;
        if (!ActivationCountdown) {
            HelpComputer_StartDisplay(0, NextDisplayPeriod);
            GameWindows[0].Input.Padd = 0;
            GameWindows[0].Input.OPadd = 0;
            GameWindows[0].Input.Original = 0;
            AllowButtonExit = 0;
        }
    }
}
