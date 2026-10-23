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

extern struct COLLHEADER TrigBoundCollMap;
extern short TelePortData[];
short NumTelePorts;
short Event_GlobalEVOFFSET;

void PSXTELEPORT_T(struct PSXSUBEVENT *subevent)
{
    struct COLLHEADER *Hdr;
    struct COLL3DSPACE *Space;
    short a;    /* never initialised: the original reads it from $a0 */
    short n;    /* not in SYM, but needed so the bound is hoisted past the TelePortData store */

    Hdr = &TrigBoundCollMap;
    Space = Hdr->Coll3dSpace;
    n = Hdr->TotalNumSpaces;
    for (; a < n; a++, Space++) {
        if (*(signed char *)&Space->Area != -1) {
            if (Space->StartJoin == Event_GlobalEVOFFSET) {
                TelePortData[NumTelePorts] = (char *)Space - (char *)Hdr->Coll3dSpace;
                NumTelePorts++;
                return;
            }
        }
    }
}
