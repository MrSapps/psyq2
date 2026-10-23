#include "types.h"

short MaxParticleFt4;
short MaxParticle;
struct PARTICLE *Particles;
extern int MaxWindows;
extern struct DB db[];
extern short palette[];
void *MY_malloc_name(int s, char *name);

void MallocParticles(int NumParticles)
{
    struct POLY_FT4 *Ft40;
    struct POLY_FT4 *Ft41;
    int a;
    int Size;

    if (NumParticles < 0)
        MaxParticle = 0;
    else if (NumParticles)
        MaxParticle = NumParticles;
    else
        MaxParticle = 32;
    if (MaxWindows == 1)
        MaxParticleFt4 = MaxParticle * 15;
    else
        MaxParticleFt4 = MaxParticle / 2 * MaxWindows * 15;
    Size = MaxParticleFt4 * sizeof(struct POLY_FT4) * 2 + MaxParticle * sizeof(struct PARTICLE);
    if (Size) {
        db[0].PartFt4 = MY_malloc_name(Size, "Some Particles");
        db[1].PartFt4 = db[0].PartFt4 + MaxParticleFt4;
        Particles = (struct PARTICLE *)(db[1].PartFt4 + MaxParticleFt4);
    } else
        db[0].PartFt4 = 0;
    Ft40 = db[0].PartFt4;
    Ft41 = db[1].PartFt4;
    for (a = 0; a < MaxParticleFt4; a++) {
        Ft40->clut = Ft41->clut = palette[75];
        Ft40->u0 = 0xF0;
        Ft40->v0 = 0xF0;
        Ft40->u1 = 0xFF;
        Ft40->v1 = 0xF0;
        Ft40->u2 = 0xF0;
        Ft40->v2 = 0xFF;
        Ft40->u3 = 0xFF;
        Ft40->v3 = 0xFF;
        Ft41->u0 = 0xF0;
        Ft41->v0 = 0xF0;
        Ft41->u1 = 0xFF;
        Ft41->v1 = 0xF0;
        Ft41->u2 = 0xF0;
        Ft41->v2 = 0xFF;
        Ft41->u3 = 0xFF;
        Ft41->v3 = 0xFF;
        Ft41->tag = 0x9000000;
        Ft41++;
        Ft40->tag = 0x9000000;
        Ft40++;
    }
    for (a = 0; a < MaxParticle; a++)
        Particles[a].Timer = 0;
}

extern struct SPARK SparkList[];
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);

void ProcessSparks(void)
{
    struct SPARK *Spark;
    struct COLOUR Colour;
    struct POWER Power;

    for (Spark = SparkList; Spark < &SparkList[16]; Spark++) {
        if (Spark->Timer) {
            switch (Spark->Timer) {
            case 8:
                Colour = *&(struct COLOUR){0xf5, 0x57, 0, 0};
                break;
            case 7:
                Colour = *&(struct COLOUR){0xd6, 0x49, 0, 0};
                break;
            case 6:
                Colour = *&(struct COLOUR){0xb8, 0x41, 0, 0};
                break;
            case 5:
                Colour = *&(struct COLOUR){0x99, 0x36, 0, 0};
                break;
            case 4:
                Colour = *&(struct COLOUR){0x7a, 0x2b, 0, 0};
                break;
            case 3:
                Colour = *&(struct COLOUR){0x5b, 0x21, 0, 0};
                break;
            case 2:
                Colour = *&(struct COLOUR){0x3d, 0x16, 0, 0};
                break;
            case 1:
                Colour = *&(struct COLOUR){0x1f, 0x0b, 0, 0};
                break;
            }
            Power.Full = 10;
            Power.Fall = 20;
            AddLight(&Spark->Location, Colour, Power, (struct SHORTPAIR){0, 0});
            Spark->Timer--;
        }
    }
}

extern struct WINDOW GameWindows[];
void *memset(void *, int, int);
int FindAreaNum(int Area, struct VECT *Pos);

static struct PARTICLE *FindFreeParticle(void)
{
    {
        struct PARTICLE *Particle;

        for (Particle = Particles; Particle < &Particles[MaxParticle]; Particle++)
            if (!Particle->Timer)
                return Particle;
    }
    return 0;
}

struct PARTICLE *AddParticles(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area)
{
    struct PARTICLE *Particle;
    struct VECT Origin;
    int a;

    Particle = FindFreeParticle();
    if (Particle) {
        Particle->Origin = *Pos;
        Origin = Particle->Origin;
        Particle->Vel[0] = Vel[0];
        Particle->Area = FindAreaNum(Area, Pos);
        for (a = 0; a < MaxWindows; a++) {
            Particle->Sort[a].VectBox = (struct VECTBOX *)Particle;
            Particle->Sort[a].Flags = 1;
        }
        for (a = 1; a < NumToDraw; a++) {
            Particle->RelPos[a - 1].vx = Pos[a].vx - Origin.vx;
            Particle->RelPos[a - 1].vy = Pos[a].vy - Origin.vy;
            Particle->RelPos[a - 1].vz = Pos[a].vz - Origin.vz;
            Particle->Vel[a].vx = Vel[a].vx - Vel[0].vx;
            Particle->Vel[a].vy = Vel[a].vy - Vel[0].vy;
            Particle->Vel[a].vz = Vel[a].vz - Vel[0].vz;
        }
        memset(&Particle->Drift, 0, sizeof(struct SVECT));
        Particle->PColours[0] = PColour0;
        Particle->PColours[1] = PColour1;
        Particle->Size = Size * GameWindows[0].StandardSize.sx / 512;
        Particle->Control = 0;
        Particle->NoDrawWin = 0;
        Particle->NumToDraw = NumToDraw;
        Particle->Timer = Timer;
    }
    return Particle;
}

struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area)
{
    struct PARTICLE *Particle;
    int a;

    Particle = FindFreeParticle();
    if (Particle) {
        Particle->Origin = *Pos;
        Particle->Vel[0] = Vel[0];
        Particle->Area = FindAreaNum(Area, Pos);
        for (a = 0; a < MaxWindows; a++) {
            Particle->Sort[a].VectBox = (struct VECTBOX *)Particle;
            Particle->Sort[a].Flags = 1;
        }
        for (a = 1; a < NumToDraw; a++) {
            memset((char *)Particle + (a - 1) * 6 + 12, 0, sizeof(struct SVECT));
            Particle->Vel[a].vx = Vel[a].vx - Vel[0].vx;
            Particle->Vel[a].vy = Vel[a].vy - Vel[0].vy;
            Particle->Vel[a].vz = Vel[a].vz - Vel[0].vz;
        }
        memset(&Particle->Drift, 0, sizeof(struct SVECT));
        Particle->PColours[0] = PColour0;
        Particle->PColours[1] = PColour1;
        Particle->Size = Size * GameWindows[0].StandardSize.sx / 512;
        Particle->Control = 0;
        Particle->NoDrawWin = 0;
        Particle->NumToDraw = NumToDraw;
        Particle->Timer = Timer;
    }
    return Particle;
}

struct PARTICLE *AddParticlesSL0(struct VECT *Pos, struct SVECT *Off, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area)
{
    struct PARTICLE *Particle;
    int a;

    Particle = FindFreeParticle();
    if (Particle) {
        Particle->Origin = *Pos;
        Particle->Vel[0] = Vel[0];
        Particle->Area = FindAreaNum(Area, Pos);
        for (a = 0; a < MaxWindows; a++) {
            Particle->Sort[a].VectBox = (struct VECTBOX *)Particle;
            Particle->Sort[a].Flags = 1;
        }
        for (a = 1; a < NumToDraw; a++) {
            Particle->RelPos[a - 1].vx = Off[a].vx >> 4;
            Particle->RelPos[a - 1].vy = Off[a].vy >> 4;
            Particle->RelPos[a - 1].vz = Off[a].vz >> 4;
            Particle->Vel[a].vx = Vel[a].vx - Vel[0].vx;
            Particle->Vel[a].vy = Vel[a].vy - Vel[0].vy;
            Particle->Vel[a].vz = Vel[a].vz - Vel[0].vz;
        }
        memset(&Particle->Drift, 0, sizeof(struct SVECT));
        Particle->PColours[0] = PColour0;
        Particle->PColours[1] = PColour1;
        Particle->Size = Size * GameWindows[0].StandardSize.sx / 512;
        Particle->Control = 0;
        Particle->NoDrawWin = 0;
        Particle->NumToDraw = NumToDraw;
        Particle->Timer = Timer;
    }
    return Particle;
}
