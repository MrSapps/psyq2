#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
struct PARTICLE *AddParticles(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area);
struct PARTICLE *AddParticlesSL0(struct VECT *Pos, struct SVECT *Off, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area);
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, short NumToDraw, short Timer, int Size, int Area);
void ProcessParticles(void);
void MallocParticles(int NumParticles);
void ProcessSparks(void);

short MaxParticleFt4;
short MaxParticle;
struct PARTICLE *Particles;
extern int MaxWindows;
extern struct DB db[];
extern short palette[];
void *MY_malloc_name(int s, char *name);

extern struct SPARK SparkList[];
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);

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

extern struct DB *currdb;
extern struct WINDOW *CurrWin;
extern short alltpages[32];
extern struct VECT NullVec;
extern struct AREA AreaData[96];
void SetGeomOffset(long ofx, long ofy);
void SetGeomScreen(long h);
void SetRotMatrix(struct MATRIX *m);
int AreaValid(int Win, int Area);
void SetupScreenGivenArea(int Win, int Area);
void OrderedSort(struct SORT *Sort, struct SORT **Link);
void AddPrim_(void *ot, void *p);

#define gte_SetTransMatrix(r0) __asm__ volatile ( \
	"move	$12,%0\n" \
	"lw	$13,20($12)\n" \
	"lw	$14,24($12)\n" \
	"ctc2	$13,$5\n" \
	"lw	$15,28($12)\n" \
	"ctc2	$14,$6\n" \
	"ctc2	$15,$7" \
	: \
	: "r"( r0 ) \
	: "$12", "$13", "$14", "$15")
#define gte_ldv0(r0) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)"); }
#define gte_rtv0() __asm__ volatile("nop\nnop\n.word 0x4A486012")
#define gte_stlvnl(r0) __asm__ volatile("move $12,%0\nswc2 $25,0($12)\nswc2 $26,4($12)\nswc2 $27,8($12)" : : "r"(r0) : "$12", "memory")
#define gte_mv3(r0, r1, r2) { \
    __asm__ volatile("move $12,%0" : : "r"(r0) : "$12"); \
    __asm__ volatile("move $13,%0" : : "r"(r1) : "$13"); \
    __asm__ volatile("move $14,%0" : : "r"(r2) : "$14"); }
#define gte_ldv3(r0, r1, r2) { gte_mv3(r0, r1, r2); \
    __asm__ volatile("lwc2 $0,0($12)\nlwc2 $1,4($12)\nlwc2 $2,0($13)\nlwc2 $3,4($13)\nlwc2 $4,0($14)\nlwc2 $5,4($14)"); }
#define gte_rtpt() __asm__ volatile("nop\nnop\n.word 0x4A280030")
#define gte_stsxy3(r0, r1, r2) { gte_mv3(r0, r1, r2); \
    __asm__ volatile("swc2 $12,0($12)\nswc2 $13,0($13)\nswc2 $14,0($14)" : : : "memory"); }

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

void ProcessParticles(void)
{
    struct POLY_FT4 *PartFt4;
    struct PARTICLE *Particle;
    struct VECT Relative;
    struct VECT tran;
    struct VECT CamPos;
    struct SVECTOR rel[15];
    struct SVECTOR TempCoord[15];
    int LeftToDraw;
    unsigned char a;
    unsigned char b;
    unsigned char sz;
    unsigned char c;
    unsigned char Win;
    unsigned char HalfSize;
    struct COLOUR Colour0;
    struct COLOUR Colour1;
    struct COLOUR ColourT;
    short TPage0;
    short TPage1;
    short TPageT;
    int Draw;
    struct SCRPNT Coords[3];
    int CurrentArea;

    PartFt4 = currdb->PartFt4;
    LeftToDraw = MaxParticleFt4;
    for (Win = 0; Win < MaxWindows; Win++) {
        CurrentArea = -1;
        CurrWin = &GameWindows[Win];
        CamPos = CurrWin->CamPos;
        SetGeomOffset(CurrWin->Geom.sx, CurrWin->Geom.sy);
        SetGeomScreen(CurrWin->Focal);
        SetRotMatrix(&CurrWin->CamMat);
        gte_SetTransMatrix((struct MATRIX *)((char *)&NullVec - 20));
        for (Particle = Particles; Particle < &Particles[MaxParticle]; Particle++) {
            if (!Particle->Timer || ((Particle->NoDrawWin >> Win) & 1))
                continue;
            if (!AreaValid(Win, Particle->Area))
                continue;
            if (LeftToDraw < Particle->NumToDraw)
                continue;
            Draw = 1;
            Relative.vx = Particle->Origin.vx - CamPos.vx;
            Relative.vy = Particle->Origin.vy - CamPos.vy;
            Relative.vz = Particle->Origin.vz - CamPos.vz;
            if (Relative.vx < -0x7fff || Relative.vx > 0x7fff || Relative.vz > 0x7fff || Relative.vz < -0x7fff)
                continue;
            rel[0].vx = Relative.vx;
            rel[0].vy = Relative.vy;
            rel[0].vz = Relative.vz;
            gte_ldv0(&rel[0]);
            gte_rtv0();
            gte_stlvnl(&tran);
            if (tran.vz < 128)
                continue;
            for (c = Draw; c < Particle->NumToDraw; c++) {
                rel[c].vx = Relative.vx + Particle->RelPos[c - 1].vx;
                rel[c].vy = Relative.vy + Particle->RelPos[c - 1].vy;
                rel[c].vz = Relative.vz + Particle->RelPos[c - 1].vz;
            }
            Colour0 = Particle->PColours[0]->Colours[32 - Particle->Timer];
            Colour1 = Particle->PColours[1]->Colours[32 - Particle->Timer];
            TPage0 = alltpages[15] | Particle->PColours[0]->Trans;
            TPage1 = alltpages[15] | Particle->PColours[1]->Trans;
            sz = Particle->Size / tran.vz;
            if (sz < 2)
                sz = 2;
            HalfSize = sz >> 1;
            if (Particle->Area != CurrentArea) {
                CurrentArea = Particle->Area;
                SetupScreenGivenArea(Win, CurrentArea);
            }
            Particle->Sort[Win].FirstLast[1] = (struct POLY_GT4 *)PartFt4;
            OrderedSort(&Particle->Sort[Win], &AreaData[Particle->Area].Data[Win].QuickSortLink);
            for (c = 0; c < Particle->NumToDraw; c += 3) {
                gte_ldv3(&rel[c], &rel[c] + 1, &rel[c] + 2);  /* +8/+16 from one base, as the original */
                gte_rtpt();
                gte_stsxy3(&Coords[0], &Coords[1], &Coords[2]);
                for (b = 0, a = c; b < 3; b++, a++) {
                    Coords[b].sx -= HalfSize;
                    Coords[b].sy -= HalfSize;
                    PartFt4[a].tpage = TPage0;
                    *(struct COLOUR *)&PartFt4[a].r0 = Colour0;
                    *(struct SCRPNT *)&PartFt4[a].x0 = Coords[b];
                    PartFt4[a].x2 = Coords[b].sx;
                    PartFt4[a].y1 = Coords[b].sy;
                    PartFt4[a].x1 = PartFt4[a].x3 = Coords[b].sx + sz;
                    PartFt4[a].y2 = PartFt4[a].y3 = Coords[b].sy + sz;
                    AddPrim_(&Particle->Sort[Win].FirstLast[0], &PartFt4[a]);
                }
                ColourT = Colour0;
                Colour0 = Colour1;
                Colour1 = ColourT;
                TPageT = TPage0;
                TPage0 = TPage1;
                TPage1 = TPageT;
            }
            PartFt4 += c;
            LeftToDraw -= c;
        }
    }
    for (Particle = Particles; Particle < &Particles[MaxParticle]; Particle++) {
        if (Particle->Timer) {
            Particle->Timer--;
            Particle->Origin.vx += Particle->Vel[0].vx;
            Particle->Origin.vy += Particle->Vel[0].vy;
            Particle->Origin.vz += Particle->Vel[0].vz;
            Particle->Vel[0].vx += Particle->Drift.vx;
            Particle->Vel[0].vy += Particle->Drift.vy;
            Particle->Vel[0].vz += Particle->Drift.vz;
            /* the original reuses HalfSize as this loop's counter (both are $s2 in SYM) */
            for (HalfSize = 1; HalfSize < Particle->NumToDraw; HalfSize++) {
                Particle->RelPos[HalfSize - 1].vx += Particle->Vel[HalfSize].vx;
                Particle->RelPos[HalfSize - 1].vy += Particle->Vel[HalfSize].vy;
                Particle->RelPos[HalfSize - 1].vz += Particle->Vel[HalfSize].vz;
            }
        }
    }
}

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
