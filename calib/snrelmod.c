#include "types.h"
void EnterCriticalSection(void);
void FlushCache(void);
void ExitCriticalSection(void);

/* SNRELMOD.OBJ (SN Systems' relocation stub) was built unoptimised: it matches at -O0
   (cc1psx 4.3, 4.4 and 4.5 alike; 4.0 doesn't), not with the game's -O3.
   calib-flags: -O0 -G8 */
void __SN_RELOCATE(unsigned int baseaddr, unsigned int *relocs)
{
    unsigned int *rel_addr;

    while (*relocs != 0xffffffff) {
        rel_addr = (unsigned int *)(baseaddr + (*relocs & ~3));
        switch (*relocs++ & 3) {
        case 0:
            *rel_addr = *rel_addr + baseaddr;
            break;
        case 1:
            *rel_addr = (*rel_addr & 0xffff0000) | (((*relocs++ + baseaddr + 0x8000) >> 16) & 0xffff);
            break;
        case 2:
            *rel_addr = (*rel_addr & 0xffff0000) | ((*rel_addr + baseaddr) & 0xffff);
            break;
        case 3:
            *rel_addr = (*rel_addr & 0xfc000000) | ((((*rel_addr & 0x3ffffff) << 2) + baseaddr) >> 2 & 0x3ffffff);
            break;
        }
    }
    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
}

