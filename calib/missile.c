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

extern struct COLLISIONRETURNDATA CollisionReturnData;
extern struct COLLHEADER CollMap;
extern struct SOLIDBOX SolidBoxs[];
extern struct WINDOW GameWindows[];
extern struct CREATURE **ThisBumpList;
extern struct CREATURE **ThisBumpListEnd;
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
void DoSolidBoxEventDamage(struct SOLIDBOX *solidbox, short damage, struct VECT *Location);
void dprintf(struct WINDOW *Window, char *_format);
struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop);
int GenMissileHitPlane(struct VECT *PlaneOrigin, struct COLLPLANE *Plane, struct VECT *Point, struct SVECT *Vel);
void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType, struct VECT *Centre);
void DoNoise(struct CREATURE *Cre, struct VECT *Location, int type);

void GenerateBulletBlood(struct VECT *Location, int Area);
void GenerateBulletGfx(struct VECT *Location, int Area);

/* Space is int in the SYM, but the code loads it as a short at entry */
void AnyoneFireInstantBullet(struct CREATURE *Cre, struct VECT *Start, struct SVECT *Dir, int Damage, short Space, int BulletType)
{
    struct CREATURE *Target;
    struct VECT Stop;
    struct VECT Stop2;
    struct SVECT Rel;
    short StopArea;
    short hitsolid;

    hitsolid = 0;
    Rel = *Dir;
    Stop.vx = Start->vx + Rel.vx;
    Stop.vy = Start->vy + Rel.vy;
    Stop.vz = Start->vz + Rel.vz;
    if (!VisableSolidBoxLine(Start, &Stop, &Stop2) && CollisionReturnData.IdentType == 2) {
        hitsolid = 1;
        Stop = Stop2;
    }
    Visable3d(&CollMap, Start, &Stop, Space);
    Stop = CollMap.AreaIntersectLast->Intersect;
    StopArea = CollMap.ReturnSpace->Area;
    if (hitsolid && Stop.vx == Stop2.vx && Stop.vy == Stop2.vy && Stop.vz == Stop2.vz
        && (SolidBoxs[CollisionReturnData.Ident].Flags & 4)) {
        SolidBoxs[CollisionReturnData.Ident].Flags |= 8;
        DoSolidBoxEventDamage(&SolidBoxs[CollisionReturnData.Ident], Damage, &Stop);
        dprintf(&GameWindows[0], "Hit a solid box\n");
    }
    Target = LineIntersectCreCylinder(Start, &Stop, Cre, &Stop, ThisBumpList, ThisBumpListEnd);
    if (Target) {
        if (Target->Player && Target->Player->Window)
            GenMissileHitPlane(&Target->Player->Window->CamPos, &Target->Player->Window->ScreenPlane, &Stop, &Rel);
        StopArea = Target->Box.Area;
        DoDamage(Cre, Target, Damage, 0x12, &Stop);
        GenerateBulletBlood(&Stop, StopArea);
    } else
        GenerateBulletGfx(&Stop, StopArea);
    if (Cre->Player)
        DoNoise(Cre, &Stop, 2);
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

