#include "types.h"

extern struct FLYPICK FlyPicks[];
extern struct IDENTITYNAME LastObjective;
extern int LevelIsMultiPlayer;
void InitFlyingPickup(struct FLYPICK *FlyPick);

void ProcessFlyingPickups(void)
{
    struct FLYPICK *FlyPick;

    for (FlyPick = FlyPicks; FlyPick < &FlyPicks[4]; FlyPick++)
        if (FlyPick->Flag)
            InitFlyingPickup(FlyPick);
}

int CreCollideAsFatPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage);
void PrimaryQuakePickup(struct CREATURE *Cre);

void PrimaryQuakePickupMoving(struct CREATURE *Cre)
{
    struct SVECT Vel[15];
    struct SVECT Off[15];
    struct SVECT BaseVel;
    struct SVECT Rel;
    int CurVel2;

    if (CreCollideAsFatPoint(Cre, 0, 0, 0, 0)) {
        CurVel2 = Cre->Vel.vx * Cre->Vel.vx + Cre->Vel.vy * Cre->Vel.vy + Cre->Vel.vz * Cre->Vel.vz;
        if (CurVel2 < 800000) {
            Cre->Params[3] = 1;
            Cre->Draw.Location = Cre->Box.Location;
            Cre->Draw.Location.vy += 0x11e;
            Cre->Draw.Location.vy -= Cre->HeightOffset;
            if (!(Cre->Params[0] & 8))
                Cre->Params[0] |= 4;
            Cre->Life = 0x21fc;
            Cre->Primary = PrimaryQuakePickup;
            return;
        }
    }
    Cre->Draw.Location = Cre->Box.Location;
    Cre->Draw.Location.vy += 0x11e;
    Cre->Draw.Location.vy -= Cre->HeightOffset;
}

int SelectAmmoTypeInUse(int Gun, int GunBits)
{
    static int PickupType = 0;

    switch (Gun) {
    case 1:
    case 2:
    case 3:
        if (GunBits & 6)
            PickupType = 0x1b;
        break;
    case 4:
    case 5:
        if (GunBits & 0x18)
            PickupType = 0x17;
        break;
    case 6:
    case 7:
        if (GunBits & 0x60)
            PickupType = 0x17;
        break;
    case 8:
        if (GunBits & 0x80)
            PickupType = 0x17;
        break;
    case 10:
        if (GunBits & 0x200)
            PickupType = 0x17;
        break;
    case 9:
    case 11:
        if (GunBits & 0x500)
            PickupType = 0x18;
        break;
    }
    return PickupType;
}
