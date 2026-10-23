#include "types.h"
extern int MaxWindows;
extern struct WINDOW GameWindows[];
extern unsigned short tptranstable[];
extern unsigned short allpals[];
extern unsigned short tppalx[20];
extern unsigned short tppaly[];
extern int MYSFXInit(void);

void SetUpSemiTrans(void)
{
    int a;

    for (a = 0; a < 4; a++)
        tptranstable[a + 1] = a << 5;
    tptranstable[0] = 32;
}

void InitCluts(void)
{
    int a;

    for (a = 0; a < sizeof(tppalx) / sizeof(tppalx[0]); a++)
        allpals[a] = (tppaly[a] << 6) | ((tppalx[a] >> 4) & 0x3f);
}

void InitHeaps(void)
{
    MYSFXInit();
}

void ProcessAllPanels(void)
{
    int a;

    for (a = 0; a < MaxWindows; a++)
        if (GameWindows[a].PanelRoutine)
            GameWindows[a].PanelRoutine(&GameWindows[a]);
}

void SetWinFade(int Win, struct COLOUR Colour, short FadeType, int Time, int InitTime)
{
    struct WINDOW *Window;

    Window = &GameWindows[Win];
    Window->FadeTime = Time;
    Window->FadeInitTime = InitTime;
    Window->FadeColour = Colour;
    Window->FadeType = FadeType;
}

void AlterFocalSubDiv(int Win, int Focal, int SubDivDist)
{
    GameWindows[Win].Focal = Focal;
    GameWindows[Win].SubDivDist = SubDivDist;
}
