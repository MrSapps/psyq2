#include "types.h"

int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2)
{
    int a, b;

    b = ((int *)Name2)[0];
    if (b == 0)
        return 2;
    a = ((int *)Name1)[0];
    if (a != b)
        return 0;
    a = ((int *)Name1)[1];
    b = ((int *)Name2)[1];
    if (a != b)
        return 0;
    a = ((int *)Name1)[2];
    b = ((int *)Name2)[2];
    return a == b;
}
