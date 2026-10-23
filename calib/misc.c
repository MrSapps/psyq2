#include "types.h"

void SeekL(long *a, short b, short c)
{
    if (*a < b) {
        if (*a + c >= b)
            *a = b;
        else
            *a += c;
    } else {
        if (*a - c <= b)
            *a = b;
        else
            *a -= c;
    }
}

int mystrlen(char *String)
{
    char *tc;

    if (String == 0)
        return 0;
    tc = String;
    while (*tc++)
        ;
    return tc - String - 1;
}
