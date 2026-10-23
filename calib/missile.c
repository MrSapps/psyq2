#include "types.h"

/* defined in this file (Missile.obj) */
struct MISSILE Missiles[64];

void ProcessMissiles_Many(void);

struct MISSILE *FindFreeMissile(void)
{
    struct MISSILE *Missile;

    for (Missile = Missiles; Missile < &Missiles[64]; Missile++) {
        if (!Missile->Life) {
            memset(Missile, 0, sizeof(struct MISSILE));
            return Missile;
        }
    }
    return 0;
}

void FreeMissile(struct MISSILE *Missile)
{
    Missile->Life = 0;
}

void InitialiseMissiles(void)
{
    struct MISSILE *Missile;

    for (Missile = Missiles; Missile < &Missiles[64]; Missile++)
        Missile->Life = 0;
}

void ProcessMissiles(void)
{
    ProcessMissiles_Many();
}
