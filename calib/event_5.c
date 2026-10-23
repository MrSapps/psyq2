#include "types.h"
extern int GlobalSecretsCount;
void DoDamage();

void PSXINACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        DoDamage(0, cre, 1, 9);
}

void PSXUNDERACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre) {
        cre->Box.InWater = 1;
        cre->Box.InAcid = 1;
        DoDamage(0, cre, 1, 9);
    }
}

void PSXINLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InAcid = 1;
    if (cre)
        DoDamage(0, cre, 20, 10);
}

void PSXUNDERLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre) {
        cre->Box.InWater = 1;
        cre->Box.InAcid = 1;
        DoDamage(0, cre, 20, 10);
    }
}

void PSXINWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        cre->Box.InShallowWater = 1;
}

void PSXUNDERWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        cre->Box.InWater = 1;
}

void PSXINCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InCrouch = 1;
}

void PSXINSECRET_T(struct PSXSUBEVENT *subevent)
{
    GlobalSecretsCount++;
}
