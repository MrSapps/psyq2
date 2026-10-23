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
