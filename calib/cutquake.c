#include "types.h"

/* defined in this file (cutquake.obj) */
char FinishedCutScene;
char WaitingCutScene;

extern struct WINDOW GameWindows[];

void QuakeLoadingSpecific(void)
{
    if (WaitingCutScene)
        FinishedCutScene = 1;
}

void QuakeCameraLoading(int Win)
{
    static struct VECT Pos;
    struct WINDOW *Window;

    Window = &GameWindows[Win];
    Window->ControlBits = 6;
    memset(&Window->CamPos, 0, sizeof(struct VECT));
    memset(&Window->CamAng, 0, sizeof(struct SVECT));
}
