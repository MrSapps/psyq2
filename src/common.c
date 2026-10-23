#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
unsigned short QuickRand(void);
char *strupr(char *s);

int toupper(int c);

/* freepol and QuickRand's seeds are adjacent in .sdata (common.obj). splat labelled seed1 as
   freepol+2 (and seed2/seed3 as an unnamed D_ label), so seed1 is reached through freepol
   here to give the same relocation; seed2/seed3 are file-scope statics (anonymous, $gp). */
unsigned short freepol = 1;
static unsigned short seed1 = 0x5a4a;
static unsigned short seed2 = 0x0248;
static unsigned short seed3 = 0xb753;

char *strupr(char *s)
{
    char *s2 = s;

    while (*s) {
        *s = toupper(*s);
        s++;
    }
    return s2;
}

unsigned short QuickRand(void)
{
    static unsigned short temp;

    temp = (&freepol)[1] + seed2;
    (&freepol)[1] = seed2;
    seed2 = seed3;
    seed3 = temp + seed2;
    return temp;
}
