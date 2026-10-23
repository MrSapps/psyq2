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
