#include "types.h"

extern struct PARTCOLOURS ParticleColours_Fire;
extern struct PARTCOLOURS ParticleColours_White;
extern struct PARTCOLOURS ParticleColours_Blue;
extern struct PARTCOLOURS ParticleColours_BloodRed;
extern struct PARTCOLOURS ParticleColours_DarkBloodRed;
extern struct PICKUPDESC QuakePickupData[];
extern unsigned int LevelTicks;
extern int NewVcounts;

void Cre_HighLightFlame(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Time);
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
