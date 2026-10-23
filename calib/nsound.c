#include "types.h"

unsigned long Big2Little(unsigned long v)
{
    return (v >> 24) | ((v >> 8) & 0xff00) | ((v & 0xff00) << 8) | (v << 24);
}
