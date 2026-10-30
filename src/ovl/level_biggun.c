/* BigGun: the Big Gun level (mission 4). The gun fires lightning beams at its emitters and
 * cracks the glass around it once the reactor is destroyed. */
#include "ovl.h"
#include "bss.h"
void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* the gun's ammunition: lightning beams drawn as strips of POLY_FT4s */
struct BEAMTEX {
    short Type;    /* 0: textured with the default clut */
    short Phase;   /* offset into the wobble table */
};
struct BEAMSHAPE {
    short Size;    /* segment length and half width */
    short Amp;     /* wobble amplitude */
};
struct BEAM {                   /* size 0x50 */
    struct VECT Start;          /* 0x00 */
    struct VECT End;            /* 0x0c */
    struct BEAMTEX Tex;         /* 0x18 */
    struct BEAMSHAPE Shape;     /* 0x1c */
    short Area;                 /* 0x20 */
    struct VECTBOX VectBox;     /* 0x24 */
    struct SORT Sort;           /* 0x3c */
};
struct BEAMEDGE {
    short x0, y0, x1, y1;
};
struct BEAMWAVE {
    int Phase;
    int Value;
};
struct UVPAIR {
    unsigned char u, v;
};

/* defined below; gcc defers these to the end of the file in this order */
void ResetBeams(void);
int AddBeam(struct VECT *Start, struct VECT *End, struct BEAMTEX Tex, struct BEAMSHAPE Shape, short Area);
void SetGlassHidden(int n, int Hidden);
int InSweep(int n);
struct CREATURE *FindGun(void);
static void DrawExtra(void);
int rand(void);
void LinkedLibraries(void);

static void DrawBeam(struct BEAM *Beam);

static int Unused0 = 0;     /* UnusedN: space in the original that no code uses */
static struct UVPAIR BeamUV[8] = {
    { 0x00, 0x9f }, { 0x40, 0x9f }, { 0x00, 0xb7 }, { 0x40, 0xb7 },
    { 0x00, 0xcf }, { 0x40, 0xcf }, { 0x00, 0xe7 }, { 0x40, 0xe7 },
};
static int SweepHigh = 0;
static int GunPower = 4096;
static int GunPowerTimer = 3600;
static int Firing = 0;
static int GlassBroken = 0;
static int GunOpened = 0;
static int HurtTimer = 300;
static int GunDead = 0;
static struct SVECT Shake = { 0, 0, 0 };
static short ShakeTimer = 0;
static struct VECT BoltTargets[7] = {
    { -15244, 1600, 13600 },
    { -15244, 1600, 10720 },
    { -15244, 100, 12160 },
    { -18676, 1600, 13600 },
    { -18676, 1600, 10720 },
    { -18676, 100, 12160 },
    { -16963, 1624, 15080 },
};
static struct SVECT BoltAngles[7] = {
    { 0, 0x239, 0 },
    { 0, 0x5c7, 0 },
    { 0x1c7, 0x400, 0 },
    { 0, -0x239, 0 },
    { 0, -0x5c7, 0 },
    { 0x1c7, -0x400, 0 },
    { 0, 0x800, 0 },
};

/* bss: definition order, word aligned (the BSS attribute; .lcomm would 16-align the arrays) */
static int Unused1 BSS;
static struct CREATURE *GunCre BSS;
static struct POLY_FT4 *BeamPolys BSS;
static int BeamArea BSS;
static int Unused2 BSS;
static struct BEAM *BeamsEnd BSS;
static struct BEAM *BeamPtr BSS;
static int Unused3 BSS;
static struct GRAVDATA *MissionData BSS;
static int Unused4 BSS;
static short GlassIdx[22] BSS;
static int SweepLow BSS;
static short PowerSourceIdx BSS;
static int Unused4b BSS;
static struct BEAM Beams[8] BSS;
static struct BEAMWAVE Waves[8] BSS;
static struct POLY_FT4 *BeamPolysEnd BSS;
static int Unused5 BSS;
static struct POLY_FT4 BeamPolyBuf[2][32] BSS;

void ResetBeams(void)
{
    BeamPtr = Beams;
    BeamsEnd = &Beams[8];
    BeamPolys = BeamPolyBuf[*LevelInterface.Engine->DbFrame];
    BeamPolysEnd = &BeamPolyBuf[*LevelInterface.Engine->DbFrame][32];
}

int AddBeam(struct VECT *Start, struct VECT *End, struct BEAMTEX Tex, struct BEAMSHAPE Shape, short Area)
{
    if (BeamPtr < BeamsEnd) {
        BeamPtr->Start = *Start;
        BeamPtr->End = *End;
        BeamPtr->Tex.Type = Tex.Type;
        BeamPtr->Tex.Phase = Tex.Phase;
        BeamPtr->Shape.Size = Shape.Size;
        BeamPtr->Shape.Amp = Shape.Amp;
        BeamPtr->Area = Area;
        BeamPtr++;
        return 1;
    }
    return 0;
}

void SetGlassHidden(int n, int Hidden)
{
    short Idx;

    Idx = GlassIdx[n - 1];
    if (Idx != -1)
        LevelInterface.Engine->Scene2Data[Idx].NoDraw = Hidden;
}

int InSweep(int n)
{
    return n >= SweepLow && n < SweepHigh;
}

struct CREATURE *FindGun(void)
{
    struct CREATURE *Cre;
    int i;

    Cre = LevelInterface.Engine->Creatures;
    for (i = 0; i < 48; i++, Cre++)
        if (Cre->Life && Cre->TypeIdent == 46 && Cre->IdealYaw == 20)
            return Cre;
    /* no return here: the original returns whatever is left in $2 (0 out of line) */
}

static void DrawBeams(void)
{
    struct BEAM *Beam;
    int i;
    int Unused[8];  /* the original's frame has 32 bytes of locals no code uses */

    if (BeamPtr == Beams)
        return;
    LevelInterface.Engine->SetGeomStuffWin(0);
    if (LevelInterface.Engine->NewVcounts) {
        Waves[0].Phase += (LevelInterface.Engine->rand() - 24000) << 15;
        Waves[1].Phase += (LevelInterface.Engine->rand() - 20000) << 15;
        Waves[2].Phase += (LevelInterface.Engine->rand() - 8000) << 15;
        Waves[3].Phase += (LevelInterface.Engine->rand() - 16384) << 15;
        Waves[4].Phase += (LevelInterface.Engine->rand() - 12000) << 15;
        Waves[5].Phase += (LevelInterface.Engine->rand() - 6000) << 15;
        Waves[6].Phase += (LevelInterface.Engine->rand() - 24000) << 15;
        Waves[7].Phase += (LevelInterface.Engine->rand() - 27000) << 15;
        for (i = 0; i < 8; i++)
            Waves[i].Value = (Waves[i].Phase >= 0 ? Waves[i].Phase / 131072 : -(Waves[i].Phase / 131072)) - 8192;
    }
    for (Beam = Beams; Beam < BeamPtr; Beam++) {
        LevelInterface.Engine->SetupScreenGivenArea(0, Beam->Area);
        DrawBeam(Beam);
    }
}

static void DrawBeam(struct BEAM *Beam)
{
    struct SVECTOR Scr[36];
    int DX[34];
    int DY[34];
    struct VECT *Points;
    struct SVECTOR *Rel;
    struct BEAMEDGE *Edges;
    struct UVPAIR *UV;
    short Clut;
    short TPage;
    int Len;
    struct POLY_FT4 *First;
    int Segs;
    int i;
    int dx, dy, dz;
    int W;
    int ox, oy;
    /* a and b are each one variable for several jobs, as in the original (it decides the
     * registers): the running x and y offsets, then the wobble, then (a) the u width */
    int a, b, z;

    Points = (struct VECT *)LevelInterface.Engine->DummyMemory;
    Edges = (struct BEAMEDGE *)&Points[33];
    Rel = (struct SVECTOR *)&Edges[33];
    if (!LevelInterface.Engine->AreaValid(0, Beam->Area))
        return;
    LevelInterface.Engine->SetupScreenGivenArea(0, Beam->Area);
    UV = &BeamUV[(*LevelInterface.Engine->ChrisCount / 2) & 7];
    TPage = LevelInterface.Engine->GetTPage(0, 1, LevelInterface.Engine->tpscreenx[12], LevelInterface.Engine->tpscreenx[12]);
    if (!Beam->Tex.Type)
        Clut = LevelInterface.Engine->palette[77];
    dx = Beam->End.vx - Beam->Start.vx;
    dy = Beam->End.vy - Beam->Start.vy;
    dz = Beam->End.vz - Beam->Start.vz;
    First = BeamPolys;
    Len = LevelInterface.Engine->SquareRootNR(dx * dx + dy * dy + dz * dz);
    Segs = (Len + Beam->Shape.Size - 1) / Beam->Shape.Size;
    if (Segs > 32)
        Segs = 32;
    if (!Segs)
        goto Done;
    i = (Beam->Shape.Size << 16) / Len;  /* the step, in i like the original */
    dx *= i;
    dy *= i;
    dz *= i;
    z = b = a = 0;
    for (i = 1; i < Segs; i++) {
        a += dx;
        b += dy;
        z += dz;
        Points[i].vx = Beam->Start.vx + a / 65536;
        Points[i].vy = Beam->Start.vy + b / 65536;
        Points[i].vz = Beam->Start.vz + z / 65536;
    }
    Points[0] = Beam->Start;
    Points[Segs] = Beam->End;
    for (i = 0; i < Segs + 1; i++) {
        Rel[i].vx = Points[i].vx - LevelInterface.Engine->GameWindows->CamPos.vx;
        Rel[i].vy = Points[i].vy - LevelInterface.Engine->GameWindows->CamPos.vy;
        Rel[i].vz = Points[i].vz - LevelInterface.Engine->GameWindows->CamPos.vz;
    }
    for (i = 0; i < Segs + 1; i += 3)
        LevelInterface.Engine->RemoveMeRoutine(&Rel[i], &Scr[i]);
    for (i = 0; i < Segs; i++) {
        DX[i] = Scr[i].vx - Scr[i + 1].vx;
        DY[i] = Scr[i].vy - Scr[i + 1].vy;
    }
    DX[i] = DX[i - 1];
    DY[i] = DY[i - 1];

    W = LevelInterface.Engine->SquareRootNR(DX[0] * DX[0] + DY[0] * DY[0]) * Scr[0].vz;
    if (W) {
        ox = DY[0] * (Beam->Shape.Size << 6) / W;
        oy = DX[0] * (Beam->Shape.Size << 6) / W;
        Edges[0].x0 = Scr[0].vx - ox;
        Edges[0].y0 = Scr[0].vy + oy;
        Edges[0].x1 = Scr[0].vx + ox;
        Edges[0].y1 = Scr[0].vy - oy;
    } else {
        Edges[0].x0 = Edges[0].x1 = Scr[0].vx;
        Edges[0].y0 = Edges[0].y1 = Scr[0].vy;
    }
    for (i = 1; i < Segs; i++) {
        W = LevelInterface.Engine->SquareRootNR(DX[i] * DX[i] + DY[i] * DY[i]) * Scr[i].vz;
        if (W) {
            oy = (DX[i] * Beam->Shape.Size << 6) / W;
            ox = (DY[i] * Beam->Shape.Size << 6) / W;
            a = Beam->Shape.Amp * DY[i] * Waves[(i + Beam->Tex.Phase) & 7].Value / W;
            b = Beam->Shape.Amp * DX[i] * Waves[(i + 4 + Beam->Tex.Phase) & 7].Value / W;
            Edges[i].x0 = Scr[i].vx - ox + a;
            Edges[i].y0 = Scr[i].vy + oy + b;
            Edges[i].x1 = Scr[i].vx + ox + a;
            Edges[i].y1 = Scr[i].vy - oy + b;
        } else {
            Edges[i].x0 = Edges[i].x1 = Scr[i].vx;
            Edges[i].y0 = Edges[i].y1 = Scr[i].vy;
        }
    }
    W = LevelInterface.Engine->SquareRootNR(DX[Segs] * DX[Segs] + DY[Segs] * DY[Segs]) * Scr[Segs].vz;
    if (W) {
        ox = (DY[Segs] * Beam->Shape.Size << 6) / W;
        oy = (DX[Segs] * Beam->Shape.Size << 6) / W;
        Edges[Segs].x0 = Scr[Segs].vx - ox;
        Edges[Segs].y0 = Scr[Segs].vy + oy;
        Edges[Segs].x1 = Scr[Segs].vx + ox;
        Edges[Segs].y1 = Scr[Segs].vy - oy;
    } else {
        Edges[Segs].x0 = Edges[Segs].x1 = Scr[Segs].vx;
        Edges[Segs].y0 = Edges[Segs].y1 = Scr[Segs].vy;
    }

    a = 63;
    for (i = 0; i < Len / Beam->Shape.Size; i++) {
        if (BeamPolys >= BeamPolysEnd)
            goto Done;
        if (Scr[i].vz >= 192 && Scr[i + 1].vz >= 192) {
            BeamPolys->x0 = Edges[i].x0;
            BeamPolys->y0 = Edges[i].y0;
            BeamPolys->x2 = Edges[i].x1;
            BeamPolys->y2 = Edges[i].y1;
            BeamPolys->x1 = Edges[i + 1].x0;
            BeamPolys->y1 = Edges[i + 1].y0;
            BeamPolys->x3 = Edges[i + 1].x1;
            BeamPolys->y3 = Edges[i + 1].y1;
            BeamPolys->tpage = TPage;
            BeamPolys->clut = Clut;
            BeamPolys->r0 = 128;
            BeamPolys->g0 = 128;
            BeamPolys->b0 = 128;
            BeamPolys->u0 = UV->u;
            BeamPolys->v0 = UV->v;
            BeamPolys->u1 = UV->u + a;
            BeamPolys->v1 = UV->v;
            BeamPolys->u2 = UV->u;
            BeamPolys->v2 = UV->v + 23;
            BeamPolys->u3 = UV->u + a;
            BeamPolys->v3 = UV->v + 23;
            BeamPolys->code = 46;
            BeamPolys->tag = 0x09000000;
            LevelInterface.Engine->AddPrim_(&Beam->Sort.FirstLast[0], BeamPolys++);
        }
    }
    a = Len & (Beam->Shape.Size - 1);
    a <<= 6;
    a /= Beam->Shape.Size;
    if (a && BeamPolys < BeamPolysEnd && Scr[i].vz >= 192 && Scr[i + 1].vz >= 192) {
        BeamPolys->x0 = Edges[i].x0;
        BeamPolys->y0 = Edges[i].y0;
        BeamPolys->x2 = Edges[i].x1;
        BeamPolys->y2 = Edges[i].y1;
        BeamPolys->x1 = Edges[i + 1].x0;
        BeamPolys->y1 = Edges[i + 1].y0;
        BeamPolys->x3 = Edges[i + 1].x1;
        BeamPolys->y3 = Edges[i + 1].y1;
        BeamPolys->tpage = TPage;
        BeamPolys->clut = Clut;
        BeamPolys->r0 = 128;
        BeamPolys->g0 = 128;
        BeamPolys->b0 = 128;
        BeamPolys->u0 = UV->u;
        BeamPolys->v0 = UV->v;
        BeamPolys->u1 = UV->u + a;
        BeamPolys->v1 = UV->v;
        BeamPolys->u2 = UV->u;
        BeamPolys->v2 = UV->v + 23;
        BeamPolys->u3 = UV->u + a;
        BeamPolys->v3 = UV->v + 23;
        BeamPolys->code = 46;
        BeamPolys->tag = 0x09000000;
        LevelInterface.Engine->AddPrim_(&Beam->Sort.FirstLast[0], BeamPolys++);
    }
Done:
    if (First != BeamPolys) {
        Beam->Sort.FirstLast[1] = (struct POLY_GT4 *)First;
        Beam->Sort.VectBox = &Beam->VectBox;
        LevelInterface.Engine->GenVectBox(&Beam->VectBox, &Beam->Start, &Beam->End);
        Beam->Sort.Flags = 0;
        LevelInterface.Engine->OrderedSort(&Beam->Sort, &LevelInterface.Engine->AreaData[Beam->Area].Data[0].SortLink);
    }
}

/* "Destroy Grav" event */
static void DestroyGrav(void)
{
    MissionData->ReactorDestroyed = 1;
    LevelInterface.Engine->HelpComputer_NewMessage((struct IDENTITYNAME){"Unit4Curr8"}, (struct IDENTITYNAME){"Unit4Miss2"}, 150);
}

static void DestroyGlass(void);

static void GunPrimary(struct CREATURE *Cre)
{
    int Opaque;
    int r;
    int t;
    struct ANIMHDR *Hdr;

    if (!Cre->SequenceHdr)
        return;
    Cre->AnimFrame += *LevelInterface.Engine->NewVcounts;
    if (Cre->AnimFrame > Cre->SequenceHdr->AnimDetails.Stop) {
        if (Firing)
            Cre->AnimFrame = Cre->SequenceHdr->AnimDetails.Stop;
        else
            Cre->AnimFrame = Cre->SequenceHdr->AnimDetails.Start + (Cre->AnimFrame - Cre->SequenceHdr->AnimDetails.Stop);
    }
    if (Firing) {
        t = 1390 - (Cre->AnimFrame - Cre->SequenceHdr->AnimDetails.Start);
        t = (t << 12) / 200;
        if (t <= 0) {
            Opaque = 0;
            LevelInterface.Engine->KillModel(Cre);
            GunDead = 1;
        } else if (t > 4096) {
            Opaque = 4096;
        } else {
            Opaque = t;
            GunOpened = 1;
            GunCre->Control &= ~0x4000000;
            if (!GlassBroken) {
                DestroyGlass();
                GlassBroken = 1;
                LevelInterface.Engine->SetWinFade(0, (struct COLOUR){ 255, 255, 255, 0 }, 2, 10, 10);
                LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"GravTeleport"}, 0);
            }
        }
        Cre->ModelOpaque = Opaque;
    }
    ShakeTimer -= *LevelInterface.Engine->NewVcounts;
    if (ShakeTimer <= 0) {
        if (!GunPower) {
            Hdr = LevelInterface.Engine->FindAnimHdr(GunCre->ModelHdr, (struct IDENTITYNAME){"Death"});
            Firing = 1;
            GunCre->SequenceHdr = Hdr;
            GunCre->AnimFrame = GunCre->SequenceHdr->AnimDetails.Start;
            ShakeTimer = 30000;
            if (!SweepHigh)
                SweepHigh = SweepLow = 1600;
        } else {
            r = rand() * 96 / 32768;
            if (r < 48)
                r = -r - 48;
            r = r * 3072 / (GunPower / 2 + 1024);
            t = rand() * 300 / 32768;
            ShakeTimer = (t + 140) * (GunPower + 4096) / 8192 + 10;
            if (Shake.vy) {
                Shake.vy = 0;
                ShakeTimer /= 2;
            } else
                Shake.vy = r;
        }
    }
    Cre->Ang.vx += Shake.vx;
    Cre->Ang.vy += Shake.vy;
    Cre->Ang.vz += Shake.vz;
    LevelInterface.Engine->RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
}

static int PowerSourceOn(void)
{
    if (PowerSourceIdx != -1)
        return !LevelInterface.Engine->Scene2Data[PowerSourceIdx].NoDraw;
    return 0;
}

static void BigGunProcess(void)
{
    struct SVECT Radius;
    struct MATRIX S;
    struct VECT Pos;
    struct CREATURE *Cre;
    int i;
    int Bob;
    int j;

    if (!GunCre) {
        Cre = FindGun();
        GunCre = Cre;
        if (Cre) {
            BeamArea = Cre->Box.Area;
            GunCre->SequenceHdr = LevelInterface.Engine->FindAnimHdr(Cre->ModelHdr, (struct IDENTITYNAME){"Activate"});
            Firing = 0;
            GunCre->Control |= 0x4000000;
            GunCre->ShieldColour.Pad = 6;
        }
    }
    if (GunCre) {
        Radius = *&(struct SVECT){ 0x300, 0x300, 0x300 };
        if (MissionData->ReactorDestroyed) {
            GunPowerTimer -= *LevelInterface.Engine->NewVcounts;
            if (GunPowerTimer < 0)
                GunPowerTimer = 0;
        }
        GunPower = (GunPowerTimer << 12) / 3600;
        if (GunPower < 0)
            GunPower = 0;
        GunPrimary(GunCre);
        Bob = (LevelInterface.Engine->rcossin_tbl[(*LevelInterface.Engine->LevelTicks << 12) / 18756 & 4095].sin + 8192) * 2;
        GunCre->Draw.Location.vy = LevelInterface.Engine->rcossin_tbl[(*LevelInterface.Engine->LevelTicks << 12) / 1000 & 4095].sin * Bob / 409600 + 1500;
        if (GunCre->Control & 0x4000000) {
            GunCre->ShieldTimer = 4;
            for (i = 0; i < GunCre->ShieldColour.Pad; i++) {
                LevelInterface.Engine->ReactorShieldGlints[i].Ang.vx = BoltAngles[i].vx - GunCre->Ang.vx + (rand() - 16384) / 256;
                LevelInterface.Engine->ReactorShieldGlints[i].Ang.vy = BoltAngles[i].vy - GunCre->Ang.vy + (rand() - 16384) / 256;
                LevelInterface.Engine->ReactorShieldGlints[i].Ang.vz = BoltAngles[i].vz - GunCre->Ang.vz + (rand() - 16384) / 256;
                LevelInterface.Engine->GenShieldMatrix(GunCre, &LevelInterface.Engine->ReactorShieldGlints[i].Ang, &Radius, &S);
                LevelInterface.Engine->Rotate(&S, &(struct SVECT){ 0, 0, 0x200 }, &Pos);
                Pos.vx += GunCre->Draw.Location.vx;
                Pos.vy += GunCre->Draw.Location.vy;
                Pos.vz += GunCre->Draw.Location.vz;
                /* the all-zero BEAMTEX literal is built by a call to memset */
                AddBeam(&Pos, &BoltTargets[i], (struct BEAMTEX){ 0, 0 }, (struct BEAMSHAPE){ 256, 2 }, BeamArea);
                LevelInterface.Engine->ReactorShieldGlints[i].Timer = 4;
                LevelInterface.Engine->ReactorShieldGlints[i].Colour.r = rand();
                LevelInterface.Engine->ReactorShieldGlints[i].Colour.g = rand();
                LevelInterface.Engine->ReactorShieldGlints[i].Colour.b = rand();
            }
        }
        if (!GunOpened && PowerSourceOn()) {
            struct SVECT Vel[15];
            int k;

            for (k = 0; k < 15; k++) {
                Vel[k].vx = (LevelInterface.Engine->rand() - 16384) >> 12;
                Vel[k].vy = (LevelInterface.Engine->rand() - 16384) >> 12;
                Vel[k].vz = -LevelInterface.Engine->rand() >> 8;
            }
            LevelInterface.Engine->AddParticlesSL(&BoltTargets[6], Vel, *LevelInterface.Engine->ParticleColoursTable,
                *LevelInterface.Engine->ParticleColoursTable, 15, 23, 20000, BeamArea);
        }
    }
    if (MissionData->ReactorDestroyed && GunCre) {
        if (InSweep(1300))
            SetGlassHidden(1, 0);
        if (InSweep(1000)) {
            SetGlassHidden(2, 0);
            SetGlassHidden(3, 0);
            SetGlassHidden(4, 0);
        }
        if (InSweep(850))
            SetGlassHidden(5, 0);
        if (InSweep(800)) {
            SetGlassHidden(6, 0);
            SetGlassHidden(7, 0);
        }
        if (InSweep(700))
            SetGlassHidden(8, 0);
        if (InSweep(640))
            SetGlassHidden(10, 0);
        if (InSweep(630))
            SetGlassHidden(12, 0);
        if (InSweep(620)) {
            SetGlassHidden(15, 0);
            SetGlassHidden(21, 0);
        }
        if (InSweep(600))
            for (j = 1; j < 23; j++)
                SetGlassHidden(j, 0);
        if (SweepLow < 0)
            SweepLow = 0;
        if (SweepHigh) {
            SweepHigh = SweepLow;
            SweepLow -= *LevelInterface.Engine->NewVcounts;
        }
    }
}

static void DestroyGlass(void)
{
    int i;

    LevelInterface.Engine->Event_DoNamed((struct IDENTITYNAME){"DestroyGlass"}, 0);
    for (i = 1; i < 23; i++)
        SetGlassHidden(i, 1);
}

static void Zone0Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone0"});
    GunCre = 0;
}

static void Zone1Load(void)
{
    struct IDENTITYNAME Name;
    int i;

    LevelInterface.Engine->Event_SetDisableNamed((struct IDENTITYNAME){"GravTeleport"}, !MissionData->ReactorDestroyed);
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone1"});
    GunCre = 0;
    Name = (struct IDENTITYNAME){"Crack"};
    for (i = 1; i < 23; i++) {
        if (i < 10)
            Name.Name[5] = '0' + i;
        else if (i < 20) {
            Name.Name[5] = '1';
            Name.Name[6] = '0' + i - 10;
        } else {
            Name.Name[5] = '2';
            Name.Name[6] = '0' + i - 20;
        }
        GlassIdx[i - 1] = LevelInterface.Engine->GetSceneNum(Name);
        SetGlassHidden(i, 1);
    }
    Name = (struct IDENTITYNAME){"Power Source"};
    PowerSourceIdx = LevelInterface.Engine->GetSceneNum(Name);
}

static void Zone2Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone2"});
    GunCre = 0;
}

static void Zone3Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone3"});
    GunCre = 0;
}

static void Zone4Load(void)
{
    LevelInterface.Engine->ActivateCreatureBatch((struct IDENTITYNAME){"Zone4"});
    GunCre = 0;
}

void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load };

static void LevelSpecific(void)
{
    struct CREATURE *Cre;

    if (!GunDead) {
        ResetBeams();
        if (LevelInterface.Engine->ZoneName->Name[4] == '1')
            BigGunProcess();
    }
    if (LevelInterface.Engine->ZoneName->Name[4] == '1' && GunDead && !LevelInterface.Engine->Creatures->Box.InTeleporter) {
        HurtTimer -= *LevelInterface.Engine->NewVcounts;
        if (HurtTimer <= 0) {
            HurtTimer += 300;
            for (Cre = LevelInterface.Engine->Creatures; Cre < LevelInterface.Engine->Creatures + 48; Cre++)
                if (Cre->Life > 0 && (Cre->Player || Cre->TypeIdent < 38 || Cre->TypeIdent == 47))
                    LevelInterface.Engine->DoDamage(0, Cre, 5, 9, &Cre->Box.Location);
        }
    }
}

/* "STOPPLATFORM" event */
static void StopPlatform(void)
{
    int Base;
    int Offset;
    short Obj;
    struct BRUSHMOVEBUF *Buf;

    Base = (int)*LevelInterface.Engine->eventlistbase;
    Offset = LevelInterface.Engine->Event_GetNamedOffset((struct IDENTITYNAME){"PLATFORM"});
    if (Offset > 0) {
        /* the platform's brush */
        Obj = *(short *)(Offset + Base + 24);
        if (Obj >= 0) {
            Buf = LevelInterface.Engine->BrushMoveBufs;
            Buf += Obj;
            Buf->Time = 0xffff;
        }
    }
}

struct MISSIONEVENT LevelEvents[] = {
    { {"STOPPLATFORM"}, StopPlatform },
    { {"Destroy Grav"}, DestroyGrav },
    { {""}, 0 },
};

void InitLevel(void)
{
    if (LevelInterface.Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            LevelInterface.Engine->PrintF("Please get a programmer to recompile level %s\n",
                LevelInterface.Engine->PrintIdentityName(*LevelInterface.Engine->ThisLevelName));
        LevelInterface.Engine->PrintF("Size In Level %d, Size In Engine %d\n",
            sizeof(struct QENGINELEVINTERFACE), LevelInterface.Engine->SizeOf);
    }
    *LevelInterface.Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *LevelInterface.Engine->LevelSpecific = LevelSpecific;
    *LevelInterface.Engine->LevelExtraDraw = DrawExtra;
    LevelInterface.Engine->LoadSaveCreatureBatches(&(struct IDENTITYNAME){"BigGBatches"}, 0);
    LevelInterface.Engine->InitialiseQuakePlayer(0, *LevelInterface.Engine->MainPlayerStart);
    LevelInterface.Engine->QuakeSinglePlayerInit();
    LevelInterface.Engine->LevelEvents = LevelEvents;
    LevelInterface.Engine->AddLevelInterMission(LevelInterface.Engine->GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    MissionData = LevelInterface.Engine->MissionData;
}

static void DrawExtra(void)
{
    DrawBeams();
}

/* The library code the original linked after the module's own object: libc2 MEMSET (memset)
 * and RAND (rand, srand). It was built with other flags (the stores to next go through $at),
 * so it is reproduced here byte for byte: one asm block in the last function the compiler
 * emits, whose own epilogue goes to a section the linker ignores. (Not ovl-libs: RAND.OBJ of
 * PSY-Q 4.3-4.5 has 0x40 bytes of text, the original's 0x3c.) RAND's seed is next, the
 * last of the module's bss. */
static unsigned long next BSS;
static int Unused6[3] BSS;

void LinkedLibraries(void)
{
    __asm__(
        ".set noreorder\n"
        ".set noat\n"
        " .globl memset\n"
        " .globl rand\n"
        " .globl srand\n"
        "memset:\n"
        " .word 0x10800009,0x00001021,0x1cc00003,0x00801021\n"
        " j memset_t28\n"
        " addu $2,$0,$0\n"
        " .word 0xa0850000,0x24c6ffff,0x1cc0fffd,0x24840001\n"
        "memset_t28:\n"
        " .word 0x03e00008,0x00000000\n"
        "rand:\n"
        " lui $3,0x41c6\n"
        " lui $2,%hi(next)\n"
        " lw $2,%lo(next)($2)\n"
        " ori $3,$3,0x4e6d\n"
        " mult $2,$3\n"
        " mflo $4\n"
        " addiu $2,$4,12345\n"
        " lui $1,%hi(next)\n"
        " sw $2,%lo(next)($1)\n"
        " srl $2,$2,16\n"
        " jr $31\n"
        " andi $2,$2,0x7fff\n"
        "srand:\n"
        " lui $1,%hi(next)\n"
        " jr $31\n"
        " sw $4,%lo(next)($1)\n"
        ".set at\n"
        ".set reorder\n"
        ".section .discard\n");
}
