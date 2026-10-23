#include "types.h"

/* defined in this file (Quake.obj), so $gp-relative */
int Paused;

int GameIsPaused(void)
{
    return Paused;
}
