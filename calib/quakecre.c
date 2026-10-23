#include "types.h"

extern struct PARTCOLOURS ParticleColours_Fire;
extern struct PARTCOLOURS ParticleColours_White;
extern struct PARTCOLOURS ParticleColours_Blue;
extern struct PARTCOLOURS ParticleColours_BloodRed;
extern struct PARTCOLOURS ParticleColours_DarkBloodRed;
extern struct PICKUPDESC QuakePickupData[];
extern unsigned int LevelTicks;
extern int NewVcounts;

void Cre_HighLightFlame(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, unsigned char Time);
void GenModShape(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Blood);
void TendToDefaultAmbience(struct CREATURE *Cre, int InverseRate);
void ProcessQuakeBFGEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeBurnEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakePlasmaEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeHyperEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeQuadEffect(struct CREATURE *Cre);
void ProcessQuakeHealEffect(struct CREATURE *Cre, int Decrement);
void Cre_Burn(struct CREATURE *Cre);
void Cre_Plasma(struct CREATURE *Cre);
void Cre_Hyper(struct CREATURE *Cre);
void Cre_Quad(struct CREATURE *Cre);
void Cre_Heal(struct CREATURE *Cre);
void KillModel(struct CREATURE *Cre);

void ProcessQuakeGeneralCreEffects(struct CREATURE *Cre)
{
    int Alive;

    TendToDefaultAmbience(Cre, 7);
    Alive = Cre->Health > 0;
    ProcessQuakeBFGEffect(Cre, 1);
    ProcessQuakeBurnEffect(Cre, Alive);
    ProcessQuakePlasmaEffect(Cre, Alive);
    ProcessQuakeHyperEffect(Cre, 1);
    ProcessQuakeQuadEffect(Cre);
    ProcessQuakeHealEffect(Cre, 1);
}

void PrimaryKillPlasmaFade(struct CREATURE *Cre);
void PrimaryKillBurnFade(struct CREATURE *Cre);
void FreeEdictT(struct edict_s *EdictT);

int EffectyDeath(struct CREATURE *Cre)
{
    int Return = 0;

    if (Cre->Effect.PlasmaTime) {
        Return = 1;
        Cre->Primary = PrimaryKillPlasmaFade;
        Cre->Life = 0x1000;
    } else if (Cre->Effect.BurnTime) {
        Return = 1;
        Cre->Primary = PrimaryKillBurnFade;
        Cre->Life = 0x1000;
    }
    if (Return)
        FreeEdictT(Cre->EdictT);
    return Return;
}

void PrimaryKillFade(struct CREATURE *Cre)
{
    Cre->ModelOpaque -= NewVcounts * 16;
    if (Cre->ModelOpaque <= 0)
        KillModel(Cre);
}

void PrimaryKillBurnFade(struct CREATURE *Cre)
{
    ProcessQuakeGeneralCreEffects(Cre);
    Cre->Life -= NewVcounts * 64;
    if (Cre->Life <= 0)
        KillModel(Cre);
}

void PrimaryKillPlasmaFade(struct CREATURE *Cre)
{
    ProcessQuakeGeneralCreEffects(Cre);
    Cre->Life -= NewVcounts * 64;
    if (Cre->Life <= 0)
        KillModel(Cre);
}

void ProcessQuakeBurnEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.BurnTime) {
        Cre_Burn(Cre);
        Cre->Effect.BurnTime -= Decrement;
    }
}

void ProcessQuakePlasmaEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.PlasmaTime) {
        Cre_Plasma(Cre);
        Cre->Effect.PlasmaTime -= Decrement;
    }
}

void ProcessQuakeHyperEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.HyperTime) {
        Cre_Hyper(Cre);
        Cre->Effect.HyperTime -= Decrement;
    }
}

void ProcessQuakeQuadEffect(struct CREATURE *Cre)
{
    if (Cre->Player && Cre->Player->Extras.PTimers.QuadTimer > LevelTicks)
        Cre_Quad(Cre);
}

void ProcessQuakeHealEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.HealTime) {
        Cre_Heal(Cre);
        Cre->Effect.HealTime -= Decrement;
    }
}

void BodgeModelDataTransparent(struct GLOBPOL *GlobPoly, int a, int b, int c)
{
    GlobPoly->Tp = (GlobPoly->Tp & 0x1f) | 0x40;
}

struct PICKUPDESC *FindPickupData(int Ident)
{
    struct PICKUPDESC *PickUpDesc;

    for (PickUpDesc = QuakePickupData; (signed char)PickUpDesc->Ident != -1; PickUpDesc++) {
        if ((signed char)PickUpDesc->Ident == Ident)
            return PickUpDesc;
    }
    return 0;
}

void GenBloodyBody(struct CREATURE *Cre)
{
    GenModShape(Cre, &ParticleColours_BloodRed, &ParticleColours_DarkBloodRed, 0x1800, 1);
}

void Cre_Burn(struct CREATURE *Cre)
{
    Cre_HighLightFlame(Cre, &ParticleColours_Fire, &ParticleColours_Fire, 0x7fff, 4);
}

void Cre_Plasma(struct CREATURE *Cre)
{
    Cre_HighLightFlame(Cre, &ParticleColours_Fire, &ParticleColours_Fire, 0x7fff, 4);
}

void Cre_Hyper(struct CREATURE *Cre)
{
    Cre_HighLightFlame(Cre, &ParticleColours_Fire, &ParticleColours_Fire, 0x7fff, 4);
}

void GenPickupGlint(struct CREATURE *Cre)
{
    GenModShape(Cre, &ParticleColours_White, &ParticleColours_Blue, 0x1800, 0);
}

static int Sound_Explosion;
int GetSoundNum(struct IDENTITYNAME Name);
void ForEachPolyInModelHdr(struct GLOBMODHDR *Hdr, void *VoidRoutine, int a, int b, int c);
struct CREATURE *InitialiseQuakePickup(struct PICKUPENTRY *PickupEntry);
struct CRESAVE *QuakeStoreInanimate(struct CREATURE *Cre, struct CRESAVE *CreSave);

void PreInitExplosion(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        ForEachPolyInModelHdr(Hdr, BodgeModelDataTransparent, 0, 0, 0);
        Sound_Explosion = GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
    }
}

struct CREATURE *InitSimpleModel(struct SIMSTART *SimStart, struct IDENTITYNAME *Name, int Main, int Drop);
int PrimaryQuakePickup(struct CREATURE *Cre);

struct CREATURE *InitialiseQuakePickup(struct PICKUPENTRY *PickupEntry)
{
    struct CREATURE *Pickup;
    struct PICKUPDESC *PickupData;

    PickupData = FindPickupData(PickupEntry->SimStart.Ident);
    if (PickupData) {
        Pickup = InitSimpleModel(&PickupEntry->SimStart, &PickupData->ModelName, -1, !(PickupData->Flags & 0x100) << 10);
        if (Pickup) {
            Pickup->TypeIdent = 0x2e;
            Pickup->IdealYaw = PickupEntry->SimStart.Ident;
            Pickup->Primary = PrimaryQuakePickup;
            Pickup->Params[0] = PickupData->Flags;
            Pickup->Params[1] = PickupData->Icon;
            if (PickupData->Flags & 2)
                Pickup->ModelOpaque = 0;
            else
                Pickup->ModelOpaque = 0x1000;
            Pickup->Life = 1500;
            Pickup->ModelHdr->ShadowVerts = (short *)&PickupData->ShadowVerts;
            if (!(Pickup->Params[0] & 1))
                Pickup->Control &= ~1;
            return Pickup;
        }
    }
    return 0;
}









void Cre_BFGHighLight(struct CREATURE *Cre);
void Cre_BFGHighLightFade(struct CREATURE *Cre);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life);

#define BFGCOLOUR ((struct COLOUR){0, 255, 0, 0})
#define BFGPOWER ((struct POWER){800, 1300})
void ProcessQuakeBFGEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.BFGTime) {
        if (Cre->Effect.BFGTime > 2) {
            Cre_BFGHighLight(Cre);
            AddLight(&Cre->Box.Location, BFGCOLOUR, BFGPOWER, (struct SHORTPAIR){0, 0});
            Cre->Ambience = *&BFGCOLOUR;
        } else {
            Cre_BFGHighLightFade(Cre);
            if (Cre->Effect.BFGTime == 2)
                AddTimedLight(&Cre->Box.Location, BFGCOLOUR, BFGPOWER, 4);
        }
        Cre->Effect.BFGTime -= Decrement;
    }
}





extern struct PARTCOLOURS ParticleColours_Quad;
extern struct PARTCOLOURS ParticleColours_GreenBFG;
extern struct PARTCOLOURS ParticleColours_GreenBFGFade;
extern struct PLAYER Players[];
extern int ChrisCount;
int GetNumberVertices(struct GLOBMODHDR *Hdr);
void GetAbsoluteModelVert(struct CREATURE *Cre, short VertexNum, struct VECT *Pos);
struct PARTICLE *AddParticles(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

extern struct QENGINELEVINTERFACE Engine;
extern struct ARMOUR_TYPE Armour_Types[];
extern struct WINDOW GameWindows[];
static char Armour_Fade;
void SetWinFade(int Win, struct COLOUR Colour, short FadeType, int Time, int InitTime);

short CheckArmour(struct CREATURE *Cre, int Damage, int EnergyWeapon)
{
    struct PLAYER *Player;
    char Armour_Index;
    short Save;
    short SaveHalf;

    Player = Cre->Player;
    Armour_Index = Player->Extras.Max_ArmourType;
    if (!Damage)
        return 0;
    if (!Engine.GameConfig.sDifficulty)
        SaveHalf = 0xfff;
    else
        SaveHalf = 0x800;
    if (!Player->Extras.Armour_Count)
        return 0;
    if (EnergyWeapon)
        Save = (unsigned)(Armour_Types[Armour_Index].Energy_Protection * Damage + SaveHalf) >> 12;
    else
        Save = (unsigned)(Armour_Types[Armour_Index].Normal_Protection * Damage + SaveHalf) >> 12;
    if (Save >= Player->Extras.Armour_Count)
        Save = Player->Extras.Armour_Count;
    if (!Save)
        return 0;
    if (!Armour_Fade)
        SetWinFade(Player->Window - GameWindows, (struct COLOUR){0x80, 0x80, 0x80, 0}, 2, 20, 20);
    Player->Extras.Armour_Count -= Save;
    return Save;
}

void PrimaryQuakeExplosion(struct CREATURE *Cre)
{
    if (Cre->ModelHdr->NumFrames * 10 <= (Cre->AnimFrame += NewVcounts * 2))
        KillModel(Cre);
    {
        int Bright;

        Bright = 320 - Cre->AnimFrame;
        Bright *= 25;
        if (Bright > 0x1000)
            Bright = 0x1000;
        if (Bright < 0)
            Bright = 0;
        Cre->ModelBright = Bright;
    }
    {
        int Light;
        struct POWER Power;

        Light = 320 - Cre->AnimFrame;
        Light *= 51;
        if (Light > 0x1000)
            Light = 0x1000;
        if (Light < 0)
            Light = 0;
        Power.Fall = Light * 1300 / 4096;
        Power.Full = Power.Fall * 3 / 4;
        AddLight(&Cre->Draw.Location, (struct COLOUR){0xc0, 0x40, 0x31, 0}, Power, (struct SHORTPAIR){0, 0});
    }
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}


int IsEnergyWeapon(int DamageType)
{
    switch (DamageType) {
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 11:
    case 12:
    case 14:
    case 16:
        return 1;
    }
    return 0;
}
