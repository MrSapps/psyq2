#include "types.h"

int toupper(int c);

char *strupr(char *s)
{
    char *s2 = s;

    while (*s) {
        *s = toupper(*s);
        s++;
    }
    return s2;
}

/* Same code as the original; only differs because splat labelled the static seed1
   (right after freepol in .sdata) as freepol+2, so the reloc names can't agree. */
#ifdef CALIB_SPLAT_NAMES
unsigned short QuickRand(void)
{
    static unsigned short temp;
    static unsigned short seed1 = 0x5a4a;
    static unsigned short seed2 = 0x0248;
    static unsigned short seed3 = 0xb753;

    temp = seed1 + seed2;
    seed1 = seed2;
    seed2 = seed3;
    seed3 = temp + seed2;
    return temp;
}
#endif
