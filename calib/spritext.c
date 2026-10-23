#include "types.h"

extern char MikeStringBuffer[];
extern int MaxWindows;
extern struct WINDOW GameWindows[];

/* defined in spritext.obj, so $gp-relative */
int SpritextPal;
int SpaceMode;
short CharX;
short CharY;

int sprintf();
void DoSpriText(struct WINDOW *Window);
void DoAddSpriText(struct WINDOW *Window);
void DoAddCentredSpriText(struct WINDOW *Window);
void UpdateMessageText(struct WINDOW *Window);

void ProcessMessageText(void)
{
    int a;

    for (a = 0; a < MaxWindows; a++)
        UpdateMessageText(&GameWindows[a]);
}

void SetCursorPos(struct WINDOW *Window, struct SCRPNT Pos)
{
    CharX = Pos.sx;
    CharY = Pos.sy;
    Window->Messages.TextPos.sx = CharX;
    Window->Messages.TextPos.sy = CharY;
}

void bprintf(struct WINDOW *Window, char *_format, ...)
{
    sprintf(MikeStringBuffer, _format);
    SpritextPal = 0x48;
    SpaceMode = 0;
    if (MikeStringBuffer[0])
        DoAddSpriText(Window);
}

void SpriText(struct WINDOW *Window, char *_format, ...)
{
    sprintf(MikeStringBuffer, _format);
    if (MikeStringBuffer[0])
        DoSpriText(Window);
}

void centreprintf(struct WINDOW *Window, char *_format, ...)
{
    sprintf(MikeStringBuffer, _format);
    SpritextPal = 0x48;
    SpaceMode = 0;
    if (MikeStringBuffer[0])
        DoAddCentredSpriText(Window);
}

void dprintf(struct WINDOW *Window, char *_format, ...)
{
}

char *strncpy();

void DoAddSpriText(struct WINDOW *Window)
{
    static short MaxRows[5] = {0, 4, 2, 1, 1};

    if (!Window->Messages.RowsUsed)
        Window->Messages.ScrollTimer = 0;
    if (++Window->Messages.RowsUsed > MaxRows[MaxWindows]) {
        Window->Messages.ScrollTimer = 0;
        if (Window->Messages.RowsUsed)
            Window->Messages.RowsUsed--;
    }
    strncpy(Window->Messages.Strings[Window->Messages.FreeRow], MikeStringBuffer, 63);
    Window->Messages.Strings[Window->Messages.FreeRow][63] = 0;
    Window->Messages.RowTimer[Window->Messages.FreeRow] = 0;
    if (++Window->Messages.FreeRow == 4)
        Window->Messages.FreeRow = 0;
}

char *strcpy();

void InitWindowText(void)
{
    int i;
    int j;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            strcpy(GameWindows[i].Messages.Strings[j], " : No message");
            GameWindows[i].Messages.Strings[j][0] = j + '0';
            GameWindows[i].Messages.RowTimer[j] = 1200;
        }
        GameWindows[i].Messages.TextPos.sx = 24;
        GameWindows[i].Messages.TextPos.sy = 16;
        GameWindows[i].Messages.FreeRow = 0;
        GameWindows[i].Messages.RowsUsed = 0;
        GameWindows[i].Messages.ScrollTimer = 0;
        GameWindows[i].Messages.TextColour.r = 127;
        GameWindows[i].Messages.TextColour.g = 127;
        GameWindows[i].Messages.TextColour.b = 127;
    }
}

extern struct DB *currdb;
extern short tpscreenx[];
extern short tpscreeny[];
unsigned short GetTPage(int tp, int abr, int x, int y);
void SetDrawTPage(struct DR_TPAGE *p, int dfe, int dtd, int tpage);
void AddPrim_(void *ot, void *p);

void SetSpriteTpage(void)
{
    int a;
    unsigned char Tpage;
    long *Ot;

    Tpage = 15;
    for (a = 0; a < MaxWindows; a++) {
        Ot = (long *)&currdb->Ot[a][49];
        SetDrawTPage(&currdb->SpriteTpages[a], 1, 1, GetTPage(0, 1, tpscreenx[Tpage], tpscreeny[Tpage]));
        AddPrim_(Ot, &currdb->SpriteTpages[a]);
    }
    Ot = (long *)&currdb->BigOt[8];
    SetDrawTPage(&currdb->SpriteTpages[4], 1, 1, GetTPage(0, 1, tpscreenx[Tpage], tpscreeny[Tpage]) | 0x20);
    AddPrim_(Ot, &currdb->SpriteTpages[4]);
}

#define HEXVAL(c) ((c) >= 'a' ? (c) - 'a' + 10 : (c) > '9' ? (c) - 'A' + 10 : (c) - '0')

char *GetColourFromString(char *SubString, struct COLOUR *Colour)
{
    int Lo;
    int Hi;

    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    Colour->r = Lo | (Hi << 4);
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    Colour->g = Lo | (Hi << 4);
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    Colour->b = Lo | (Hi << 4);
    return SubString;
}

char *GetCoordsFromString(char *SubString)
{
    short Lo;
    short Hi;
    short Top;

    Top = HEXVAL(*SubString);
    SubString++;
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    CharX = Lo | (Hi << 4) | (Top << 8);
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    CharY = Lo | (Hi << 4);
    return SubString;
}

int LeftMargin;
int RightMargin;
int MarginsOn;

char *GetMarginsFromString(char *SubString)
{
    short Lo;
    short Hi;
    short Top;

    Top = HEXVAL(*SubString);
    SubString++;
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    LeftMargin = Lo | (Hi << 4) | (Top << 8);
    Top = HEXVAL(*SubString);
    SubString++;
    Hi = HEXVAL(*SubString);
    SubString++;
    Lo = HEXVAL(*SubString);
    SubString++;
    RightMargin = Lo | (Hi << 4) | (Top << 8);
    if (LeftMargin | RightMargin)
        MarginsOn = 1;
    else
        MarginsOn = 0;
    return SubString;
}

extern int NewVcounts;
int SpriTextError;

void UpdateMessageText(struct WINDOW *Window)
{
    int i;
    int t;
    static char buf[8];
    char *String;
    struct COLOUR ColourSave;
    struct COLOUR MessageColour = {0x78, 0x78, 0x78, 0};

    ColourSave = Window->Messages.TextColour;
    Window->Messages.TextColour = MessageColour;
    if (Window->Messages.RowTimer[4] < 1200) {
        Window->Messages.RowTimer[4] += NewVcounts;
        SpriText(Window, Window->Messages.Strings[4]);
        if (Window->Messages.Strings[4][0] == '*' && Window->Messages.Strings[4][1] > '0')
            Window->Messages.Strings[4][1]--;
    }
    if (Window->Messages.ScrollTimer++ >= 60) {
        Window->Messages.ScrollTimer = 0;
        if (Window->Messages.RowsUsed)
            Window->Messages.RowsUsed--;
    }
    i = Window->Messages.RowsUsed;
    t = Window->Messages.FreeRow;
    while (i--) {
        t = (t - 1) & 3;
        Window->Messages.TextPos.sx = 24;
        Window->Messages.TextPos.sy = i * 9 + (Window->Size.sy >> 5) + 16;
        SpriText(Window, Window->Messages.Strings[t]);
        if (SpriTextError == 1 && Window->Messages.RowsUsed >= 2) {
            Window->Messages.ScrollTimer = 0;
            if (Window->Messages.RowsUsed)
                Window->Messages.RowsUsed--;
        }
        String = Window->Messages.Strings[t];
        if (*String++ == '*' && *String > '0')
            (*String)--;
    }
    Window->Messages.TextColour = ColourSave;
}

extern short Event_MuteLevFlag;
int mystrlen(char *String);
char *strchr();
int GetLengthOfToken(char Token);
char BaseFormatThing[14] = "*7@%03X%02X%s";

void DoAddCentredSpriText(struct WINDOW *Window)
{
    int LenForPos;
    int LenCheck;
    char *FormatThing;
    int AdditionalLength;
    char *tc;
    int x;

    LenForPos = mystrlen(MikeStringBuffer);
    FormatThing = BaseFormatThing;
    if (Event_MuteLevFlag)
        return;
    if (MikeStringBuffer[0] == '|' && MikeStringBuffer[1] != '0')
        FormatThing += 2;
    AdditionalLength = mystrlen(FormatThing) + 5;
    tc = strchr(MikeStringBuffer, '&');
    if (tc)
        LenForPos += GetLengthOfToken(tc[1]) - 2;
    if (MikeStringBuffer[0] == '^')
        LenForPos -= 7;
    LenForPos *= 4;
    Window->Messages.RowTimer[4] = 0;
    x = Window->Size.sx / 2 - LenForPos;
    LenForPos = Window->Size.sy / 2 + 20;
    LenCheck = mystrlen(MikeStringBuffer) + AdditionalLength;
    if (LenCheck < 63) {
        sprintf(Window->Messages.Strings[4], FormatThing, (short)x, (short)LenForPos, MikeStringBuffer);
    } else {
        sprintf(Window->Messages.Strings[4], FormatThing, (short)x, (short)LenForPos, "String too long!");
        strcpy(MikeStringBuffer + 16, "...");
    }
}
