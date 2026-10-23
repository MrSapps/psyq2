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

extern struct PARTCOLOURS ParticleColours_BloodRed;
extern struct PARTCOLOURS ParticleColours_DarkBloodRed;
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

void GenerateBulletBlood(struct VECT *Location, int Area)
{
    int a;
    int b;
    struct SVECT Vel[15];
    struct PARTICLE *Particle;

    for (b = 0; b < 2; b++) {
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) >> 10;
            Vel[a].vy = (rand() - 0x4000) >> 10;
            Vel[a].vz = (rand() - 0x4000) >> 10;
        }
        Particle = AddParticlesSL(Location, Vel, &ParticleColours_BloodRed, &ParticleColours_DarkBloodRed, 15, 15, 0x1800, Area);
        if (Particle)
            Particle->Drift.vy = 2;
    }
}

extern struct PARTCOLOURS ParticleColours_Grey;
extern struct PARTCOLOURS ParticleColours_SolidGrey;
struct PARTICLE *AddParticlesSL0(struct VECT *Pos, struct SVECT *Off, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

void GenerateBulletGfx(struct VECT *Location, int Area)
{
    struct POWER Power;
    struct SVECT Vel[15];
    struct SVECT Off[15];
    int a;

    for (a = 0; a < 15; a++) {
        Off[a].vx = (rand() - 0x4000) >> 7;
        Off[a].vy = (rand() - 0x4000) >> 7;
        Off[a].vz = (rand() - 0x4000) >> 7;
        Vel[a].vx = (rand() - 0x4000) >> 11;
        Vel[a].vy = (rand() - 0x4000) >> 11;
        Vel[a].vz = (rand() - 0x4000) >> 11;
    }
    AddParticlesSL0(Location, Off, Vel, &ParticleColours_Grey, &ParticleColours_SolidGrey, 15, 32, 0x1000, Area);
}
