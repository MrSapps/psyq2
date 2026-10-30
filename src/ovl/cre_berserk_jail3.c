/* Berserker, the build in jail3's level file: the same code, but zeroes in the
   .rdata padding where cre_berserk has stray bytes. */
#define RDATA_PADDING ""
#include "cre_berserk.c"
