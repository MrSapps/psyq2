#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
int Q2CreDropItemOnDeath(struct CREATURE *Cre);
void ProcessFlyingPickups(void);
int SelectAmmoTypeInUse(int Gun, int GunBits);
int rand(void); void InitFlyingPickup(struct FLYPICK *FlyPick);
int GenerateQuakeFlyingPickup(int PickupType, int Angle, struct SVECT *Force, struct VECT *Location, short MoveSpace);
void PrimaryQuakePickupMoving(struct CREATURE *Cre);

extern struct FLYPICK FlyPicks[];
extern struct IDENTITYNAME LastObjective;
extern int LevelIsMultiPlayer;
void InitFlyingPickup(struct FLYPICK *FlyPick);

int CreCollideAsFatPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage);
void PrimaryQuakePickup(struct CREATURE *Cre);

extern struct PLAYER Players[];
extern char WeaponTypeMap[];
extern struct CREATURE FlyPickCre;
int GenerateQuakeFlyingPickup(int PickupType, int Angle, struct SVECT *Force, struct VECT *Location, short MoveSpace);

extern struct COLLHEADER MoveCollMap;
extern struct CSTBL rcossin_tbl[];
struct PICKUPDESC *FindPickupData(int Ident);
struct CREATURE *GetNextCre(int UseExpendable);
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
void KillModel(struct CREATURE *Cre);
int GetFloorOffset(struct CREATURE *Cre);
void RotMatrixYXZ(struct SVECT *r, struct MATRIX *m);

extern struct FLYPICK FlyPicks[];
extern int LevelIsMultiPlayer;

int Q2CreDropItemOnDeath(struct CREATURE *Cre)
{
    struct FLYPICK *FlyPick;
    struct PLAYER *Player;

    Player = Cre->Player;
    for (FlyPick = FlyPicks; FlyPick < &FlyPicks[4]; FlyPick++)
        if (!FlyPick->Flag)
            break;
    /* &FlyPicks[4]: splat names that address LastObjective; cmpfunc compares it by address */
    if (FlyPick < &FlyPicks[4]) {
        FlyPick->Flag = 1;
        FlyPick->TypeIdent = Cre->TypeIdent;
        if (LevelIsMultiPlayer) {
            if (Player)
                FlyPick->Weapon = Player->Extras.FastWeaponSelect;
        } else
            FlyPick->Weapon = 0;
        FlyPick->Box = Cre->Box;
    }
}

void ProcessFlyingPickups(void)
{
    struct FLYPICK *FlyPick;

    for (FlyPick = FlyPicks; FlyPick < &FlyPicks[4]; FlyPick++)
        if (FlyPick->Flag)
            InitFlyingPickup(FlyPick);
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

int rand(void);

void InitFlyingPickup(struct FLYPICK *FlyPick)
{
    int PickupType;
    int Gun;
    int GunBits;
    struct VECT Position;
    struct SVECT Force;

    Gun = Players[0].Extras.FastWeaponSelect;
    GunBits = Players[0].Extras.WeaponsAvailable;
    PickupType = 0;
    FlyPick->Flag = 0;
    if (LevelIsMultiPlayer) {
        PickupType = WeaponTypeMap[FlyPick->Weapon];
    } else {
        switch (FlyPick->TypeIdent) {
        case 18:
            PickupType = 0x1b;
            break;
        case 5:
        case 29:
            PickupType = 0x1a;
            break;
        case 8:
        case 11:
        case 15:
        case 19:
        case 28:
            PickupType = 0x18;
            break;
        case 9:
            PickupType = 0x1c;
            break;
        case 10:
            PickupType = 0x17;
            if (!(GunBits & 0x40) || (Gun >= 4 && Gun <= 5))
                break;
            /* fall through */
        case 30:
            PickupType = 0x19;
            break;
        case 25:
        case 26:
            PickupType = SelectAmmoTypeInUse(Gun, GunBits & 0x91);
            if (PickupType)
                break;
            /* fall through */
        case 12:
        case 20:
            PickupType = 0x17;
            break;
        case 31:
            PickupType = 0x1d;
            break;
        case 32:
            PickupType = 0x15;
            break;
        case 34:
            PickupType = 0x9;
            break;
        case 24:
            PickupType = 0x26;
            break;
        }
    }
    if (PickupType) {
        memset(&Force, 0, 6);
        *&FlyPickCre.Box = FlyPick->Box;
        GenerateQuakeFlyingPickup(PickupType, rand() & 0xfff, &Force, &FlyPickCre.Box.Location, FlyPickCre.Box.MoveSpace);
    }
}

/* MoveSpace is int in the SYM; as a short param it is kept in a register from entry, as in the original */
int GenerateQuakeFlyingPickup(int PickupType, int Angle, struct SVECT *Force, struct VECT *Location, short MoveSpace)
{
    struct CREATURE *Cre;
    int Speed;
    struct PICKUPDESC *PickupData;

    PickupData = FindPickupData(PickupType);
    if (!PickupData)
        return 0;
    Cre = GetNextCre(1);
    if (Cre) {
        Cre->Primary = PrimaryQuakePickupMoving;
        Cre->TypeIdent = 0x2e;
        Cre->ModelHdr = GetModelHdr(PickupData->ModelName);
        Cre->ModelHdr->ShadowVerts = (short *)&PickupData->ShadowVerts;
        if (!Cre->ModelHdr) {
            KillModel(Cre);
            return 0;
        }
        Cre->HeightOffset = GetFloorOffset(Cre);
        Cre->Draw.Location = *Location;
        Cre->Draw.Location.vy -= 0x11e;
        Cre->Draw.Location.vy -= Cre->HeightOffset;
        Cre->Box.Location = *Location;
        Cre->DefaultAmbience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Cre->Ambience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Speed = rand() * 0x300 / 0x8000 + 0x100;
        Cre->Vel.vx = rcossin_tbl[Angle & 0xfff].cos * Speed / 4096;
        Cre->Vel.vy = (short)(-rand() * 0x600 / 0x8000 - 0xc00) / 2;
        Cre->Vel.vz = rcossin_tbl[Angle & 0xfff].sin * Speed / 4096;
        memset(&Cre->Ang, 0, 6);
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        Cre->Life = 0x60;
        Cre->Box.MoveSpace = MoveSpace;
        Cre->Box.PrimarySpace = -1;
        Cre->Box.Area = MoveCollMap.Coll3dSpace[Cre->Box.MoveSpace].Area;
        if (PickupType == 0x15)
            Cre->Params[0] = PickupData->Flags;
        else
            Cre->Params[0] = (short)(PickupData->Flags | 4);
        Cre->Params[1] = PickupData->Icon;
        Cre->Params[3] = 0;
        Cre->Box.Min = (struct SVECT){-0x100, Cre->HeightOffset - 0x200, -0x100};
        Cre->Box.Max = (struct SVECT){0x100, Cre->HeightOffset, 0x100};
        Cre->Box.Radius = 0x80;
        Cre->Box.VectBox.Min.vx = Cre->Box.Location.vx + Cre->Box.Min.vx;
        Cre->Box.VectBox.Min.vy = Cre->Box.Location.vy + Cre->Box.Min.vy;
        Cre->Box.VectBox.Min.vz = Cre->Box.Location.vz + Cre->Box.Min.vz;
        Cre->Box.VectBox.Max.vx = Cre->Box.Location.vx + Cre->Box.Max.vx;
        Cre->Box.VectBox.Max.vy = Cre->Box.Location.vy + Cre->Box.Max.vy;
        Cre->Box.VectBox.Max.vz = Cre->Box.Location.vz + Cre->Box.Max.vz;
        return 1;
    }
    return 0;
}

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
