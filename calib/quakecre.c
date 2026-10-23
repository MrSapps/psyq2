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

int rand(void);
void GetFireLocSpace(struct CREATURE *Cre, struct VECT *FirePosition, short *FireSpace);
struct CREATURE *GenerateQuakeBodyPart(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, int Space, int Life, struct IDENTITYNAME Name);
void GenerateQuakeBodyPart2(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name);
void GenBloodyBody(struct CREATURE *Cre);

void QuakeBlowMeUp(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName)
{
    int a;
    int Angle;
    int AngleStep;
    struct VECT Position;
    short Space;

    Angle = rand();
    AngleStep = 4096 / NumBits;
    GenBloodyBody(Cre);
    for (a = 0; a < NumBits; a++) {
        Position.vx = Cre->Box.Location.vx + (rand() - 0x4000) * 286 / 16384;
        Position.vy = Cre->Box.Location.vy + (rand() - 0x4000) * 286 / 16384;
        Position.vz = Cre->Box.Location.vz + (rand() - 0x4000) * 286 / 16384;
        GetFireLocSpace(Cre, &Position, &Space);
        GenerateQuakeBodyPart(Cre, Angle, Force, &Position, Space, 0x200, (struct IDENTITYNAME){"Gib meat"});
        Angle += AngleStep;
    }
    if (BigBitName)
        GenerateQuakeBodyPart(Cre, Angle, Force, &Cre->Draw.Location, Space, 0xa00, *BigBitName);
}

void QuakeBlowMeUpMetal(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName, int iNumberIndex, int iStart, int iEnd)
{
    int a;
    int Angle;
    int AngleStep;
    int num;
    struct VECT Position;
    short Space;

    num = iStart;
    Angle = rand();
    AngleStep = 4096 / NumBits;
    for (a = 0; a < NumBits; a++) {
        Position.vx = Cre->Box.Location.vx + (rand() - 0x4000) * 286 / 16384;
        Position.vy = Cre->Box.Location.vy + (rand() - 0x4000) * 286 / 16384;
        Position.vz = Cre->Box.Location.vz + (rand() - 0x4000) * 286 / 16384;
        GetFireLocSpace(Cre, &Position, &Space);
        BigBitName->Name[iNumberIndex] = num + '1';
        num++;
        if (num > iEnd)
            num = iStart;
        GenerateQuakeBodyPart2(Cre, Angle, Force, &Position, Space, 0x200, *BigBitName);
        Angle += AngleStep;
    }
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

struct CRESAVE *LoadStoreQuakePickup(void *CreV, int Store, struct CRESAVE *CreSave)
{
    if (Store) {
        if (((struct CREATURE *)CreV)->Params[0] & 4)
            return CreSave;
        CreSave = QuakeStoreInanimate(CreV, CreSave);
    } else {
        /* the Ident store is written in both branches: the call after the join stays unscheduled */
        struct PICKUPENTRY PickupEntry;
        if (CreSave->u.Packed.Packed) {
            PickupEntry.SimStart.Location.vx = CreSave->u.Packed.Location.vx;
            PickupEntry.SimStart.Location.vy = CreSave->u.Packed.Location.vy;
            PickupEntry.SimStart.Location.vz = CreSave->u.Packed.Location.vz;
            PickupEntry.SimStart.AngY = CreSave->u.Packed.Ang << 6;
            PickupEntry.SimStart.Ident = CreSave->u.Packed.Target;
        } else {
            PickupEntry.SimStart.AngY = CreSave->u.UnPacked.Ang << 6;
            PickupEntry.SimStart.Location = CreSave->u.UnPacked.Location;
            PickupEntry.SimStart.Ident = CreSave->u.UnPacked.Target;
        }
        InitialiseQuakePickup(&PickupEntry);
    }
    return CreSave;
}

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

void GenModShape(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Blood)
{
    int NumVerts;
    int NumPackets;
    int a;
    int VertexNum;
    short Life; /* int in SYM; short gives the original's register allocation */
    struct VECT Pos[15];
    struct SVECT Vel[15];
    struct PARTICLE *Particle;

    VertexNum = 0;
    NumVerts = GetNumberVertices(Cre->ModelHdr);
    NumPackets = NumVerts / 15;
    if (Blood) {
        Life = 10;
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) >> 10;
            Vel[a].vy = (rand() - 0x4000) >> 10;
            Vel[a].vz = (rand() - 0x4000) >> 10;
        }
    } else {
        Life = 32;
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) * 3 / 16384;
            Vel[a].vy = (rand() - 0x4000) * 3 / 16384;
            Vel[a].vz = (rand() - 0x4000) * 3 / 16384;
        }
    }
    while (NumPackets--) {
        for (a = 0; a < 15; a++)
            GetAbsoluteModelVert(Cre, VertexNum++, &Pos[a]);
        Particle = AddParticles(Pos, Vel, PColoursA, PColoursB, 15, Life, Size, Cre->Box.Area);
        if (Blood && Particle)
            Particle->Drift.vy = 3;
    }
}

struct CREATURE *GetNextCre(int UseExpendable);
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
int GetFloorOffset(struct CREATURE *Cre);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
void KillModel(struct CREATURE *Cre);
struct MATRIX *RotMatrixYXZ(struct SVECTOR *r, struct MATRIX *m);

struct CREATURE *InitSimpleModel(struct SIMSTART *SimStart, struct IDENTITYNAME *Name, int Main, int Drop)
{
    struct CREATURE *Cre;

    Cre = GetNextCre(1);
    if (Cre) {
        Cre->Control = 0x8000001;
        Cre->Primary = 0;
        Cre->ModelHdr = GetModelHdr(*Name);
        if (!Cre->ModelHdr)
            return 0;
        Cre->HeightOffset = GetFloorOffset(Cre);
        if (!SetupBoxDrop(&Cre->Box, &SimStart->Location, (struct SCRPNT){0x11e, 0x23c}, (struct SHORTPAIR){-1, Drop}, (struct SHORTPAIR){0x80, 0x80})) {
            KillModel(Cre);
            return 0;
        }
        Cre->Draw.Location = Cre->Box.Location;
        Cre->Draw.Location.vy += 0x11e;
        Cre->Draw.Location.vy -= Cre->HeightOffset;
        Cre->ShadowPos = Cre->Box.Location;
        Cre->ShadowPos.vy = Cre->Box.Location.vy + 0x11e;
        Cre->Ang.vy = SimStart->AngY;
        Cre->Ambience = *&(struct COLOUR){0x30, 0x30, 0x30, 0};
        Cre->DefaultAmbience = *&(struct COLOUR){0x30, 0x30, 0x30, 0};
        RotMatrixYXZ((struct SVECTOR *)&Cre->Ang, &Cre->Matrix);
    }
    return Cre;
}

char *PrintIdentityName(struct IDENTITYNAME Name);
char *PrintBool(int Bool);
int FindAnimSequenceNumFromFrame(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int FrameNum);

/* the original compares with xori: the test goes through a value, as from an inline helper */
static int IsDebugType(struct CREATURE *Cre)
{
    return Cre->TypeIdent == 0x2f;
}

struct CRESAVE *QuakeStoreInanimate(struct CREATURE *Cre, struct CRESAVE *CreSave)
{
    int Packed;
    int Param0;
    int Param1;

    Packed = 0;
    if (!((Cre->Box.Location.vx + 0x8000) & 0xffff0000))
        if (!((Cre->Box.Location.vy + 0x8000) & 0xffff0000))
            if (!((Cre->Box.Location.vz + 0x8000) & 0xffff0000))
                if (!((Cre->Health / 4 + 0x80) & 0xff00))
                    Packed = 1;
    if (IsDebugType(Cre)) {
        Param0 = FindAnimSequenceNumFromFrame(Cre->ModelHdr->AnimData, Cre->AnimFrame);
        PrintIdentityName(Cre->ModelHdr->ModelName);
        PrintBool(Packed);
    }
    CreSave->u.Packed.Packed = Packed;
    if (Packed) {
        CreSave->u.Packed.Location.vx = Cre->Box.Location.vx;
        CreSave->u.Packed.Location.vy = Cre->Box.Location.vy + 0x11e;
        CreSave->u.Packed.Location.vz = Cre->Box.Location.vz;
        CreSave->u.Packed.Ang = (Cre->Ang.vy & 0xfff) >> 6;
        CreSave->u.Packed.Target = Cre->IdealYaw;
        CreSave->u.Packed.TypeIdent = Cre->TypeIdent;
        CreSave->u.Packed.Health = Cre->Health / 4;
        CreSave->u.Packed.Param = Param0;
    } else {
        CreSave->u.UnPacked.Location.vx = Cre->Box.Location.vx;
        CreSave->u.UnPacked.Location.vy = Cre->Box.Location.vy + 0x11e;
        CreSave->u.UnPacked.Location.vz = Cre->Box.Location.vz;
        CreSave->u.UnPacked.Ang = (Cre->Ang.vy & 0xfff) >> 6;
        CreSave->u.UnPacked.Target = Cre->IdealYaw;
        CreSave->u.UnPacked.TypeIdent = Cre->TypeIdent;
        CreSave->u.UnPacked.Health = Cre->Health;
        CreSave->u.UnPacked.Param0 = Param0;
        CreSave->u.UnPacked.Param1 = Param1;
    }
    {
        char *Ret = (char *)CreSave;
        if (Packed)
            Ret += 12;
        else
            Ret += 20;
        return (struct CRESAVE *)Ret;
    }
}

extern struct PARTCOLOURS ParticleColours_BloodRed;
extern struct PARTCOLOURS ParticleColours_DarkBloodRed;
int rand(void);
void *memset(void *s, int c, unsigned int n);
int Move3d_SpinBounceAbout(struct CREATURE *Cre, struct CREATURE *Thrower, struct CREATURE **pHitCre, struct SHORTPAIR BounceDamage);
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);
struct PARTICLE *AddParticlesSL0(struct VECT *Pos, struct SVECT *Off, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

void PrimaryQuakeBodyPartMoving(struct CREATURE *Cre)
{
    struct SVECT Vel[15];
    struct SVECT Off[15];
    struct SVECT BaseVel;
    struct SVECT Rel;
    int a;

    Move3d_SpinBounceAbout(Cre, 0, 0, (struct SHORTPAIR){0x900, 0});
    if (Cre->Life <= NewVcounts) {
        Cre->Primary = PrimaryKillFade;
        Cre->Life = NewVcounts + 1;
    }
    Cre->Life -= NewVcounts;
    Cre->Draw.Location = Cre->Box.Location;
    if (!Cre->Box.OnFloor) {
        BaseVel.vx = (Cre->Vel.vx << 2) - 0x2000;
        BaseVel.vy = (Cre->Vel.vy << 2) - 0x2000;
        BaseVel.vz = (Cre->Vel.vz << 2) - 0x2000;
        for (a = 0; a < 15; a++)
            Vel[a] = (struct SVECT){(rand() + BaseVel.vx) >> 11, (rand() + BaseVel.vy) >> 11, (rand() + BaseVel.vz) >> 11};
        Rel.vx = Cre->Vel.vx / 15;
        Rel.vy = Cre->Vel.vy / 15;
        Rel.vz = Cre->Vel.vz / 15;
        memset(Off, 0, sizeof(struct SVECT));
        for (a = 1; a < 15; a++) {
            Off[a].vx = Off[a - 1].vx + Rel.vx;
            Off[a].vy = Off[a - 1].vy + Rel.vy;
            Off[a].vz = Off[a - 1].vz + Rel.vz;
        }
        AddParticlesSL0(&Cre->Draw.Location, Off, Vel, &ParticleColours_BloodRed, &ParticleColours_DarkBloodRed, 15, 3, 0x1800, Cre->Box.Area);
    }
    UpdateBoxStuff(&Cre->Box, 2);
}

extern short rcossin_tbl[][2];
extern struct COLLHEADER CollMap;
void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ);
void PrimaryQuakeBodyPartMoving(struct CREATURE *Cre);

/* Space and Life are int in SYM; short gives the original's register allocation */
void GenerateQuakeBodyPart2(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name)
{
    struct CREATURE *Cre;
    int Speed;

    Cre = GetNextCre(0);
    if (Cre) {
        Cre->TypeIdent = 48;
        Cre->Primary = PrimaryQuakeBodyPartMoving;
        Cre->ModelHdr = GetModelHdr(Name);
        Cre->HeightOffset = GetFloorOffset(Cre);
        Cre->Control = 0x1800000;
        Cre->Draw.Location = *Location;
        Cre->Box.Location = *Location;
        Cre->DefaultAmbience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Cre->Ambience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Speed = rand() / 64 + 0x300;
        Cre->Vel.vx = rcossin_tbl[Angle & 0xfff][1] * Speed / 4096;
        Cre->Vel.vy = -rand() * 2000 / 32768 - 3000;
        Cre->Vel.vz = rcossin_tbl[Angle & 0xfff][0] * Speed / 4096;
        Cre->Ang = (struct SVECT){rand(), rand(), rand()};
        Cre->Params[0] = (rand() - 0x4000) >> 6;
        RotMatrixYXZ((struct SVECTOR *)&Cre->Ang, &Cre->Matrix);
        Cre->Ambience = Parent->Ambience;
        Cre->DefaultAmbience = Parent->DefaultAmbience;
        Cre->Life = Life + rand() / 32;
        Cre->Box.MoveSpace = -1;
        Cre->Box.PrimarySpace = Space;
        Cre->Box.Area = CollMap.Coll3dSpace[Space].Area;
        SetupCollBoxMinMax(&Cre->Box, (struct SVECT){20, 20, 20});
    }
}

void *memset(void *s, int c, unsigned int n);
void MyScaleMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, int Scale);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);

void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type)
{
    struct CREATURE *Cre;

    Cre = GetNextCre(1);
    if (Cre) {
        switch (Type) {
        case 0:
            Cre->ModelHdr = GetModelHdr((struct IDENTITYNAME){"Explosion"});
            Cre->Primary = PrimaryQuakeExplosion;
            break;
        case 1:
            Cre->ModelHdr = GetModelHdr((struct IDENTITYNAME){"Hexplosion"});
            Cre->Primary = PrimaryQuakeExplosion;
            break;
        }
        if (Cre->ModelHdr) {
            Cre->Primary = PrimaryQuakeExplosion;
            Cre->HeightOffset = GetFloorOffset(Cre);
            Cre->Draw.Location = *Location;
            Cre->Box.Location = *Location;
            Cre->DefaultAmbience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
            Cre->Ambience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
            Cre->Ang = (struct SVECT){0, rand() & 0xfff, 0};
            memset(&Cre->Ang, 0, sizeof(struct SVECT));
            RotMatrixYXZ((struct SVECTOR *)&Cre->Ang, &Cre->Matrix);
            MyScaleMatrix(&Cre->Matrix, &Cre->Matrix, Scale);
            Cre->Box.Min = (struct SVECT){-0x100, Cre->HeightOffset - 0x200, -0x100};
            Cre->Box.Max = (struct SVECT){0x100, Cre->HeightOffset, 0x100};
            Cre->Box.Radius = 0x80;
            Cre->Box.VectBox.Min.vx = Cre->Box.Location.vx + Cre->Box.Min.vx;
            Cre->Box.VectBox.Min.vy = Cre->Box.Location.vy + Cre->Box.Min.vy;
            Cre->Box.VectBox.Min.vz = Cre->Box.Location.vz + Cre->Box.Min.vz;
            Cre->Box.VectBox.Max.vx = Cre->Box.Location.vx + Cre->Box.Max.vx;
            Cre->Box.VectBox.Max.vy = Cre->Box.Location.vy + Cre->Box.Max.vy;
            Cre->Box.VectBox.Max.vz = Cre->Box.Location.vz + Cre->Box.Max.vz;
            Cre->Box.MoveSpace = -1;
            Cre->Box.Area = Area;
            Cre->Box.PrimarySpace = -1;
            Cre->TypeIdent = 0x28;
            SoundByCreature(Sound_Explosion, Cre);
            SetShockData(4, 13, Scale, Location, (struct POWER){Scale / 2, Scale});
        }
    }
}
