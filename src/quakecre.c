#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void QuakeBlowMeUpMetal(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName, int iNumberIndex, int iStart, int iEnd);
int EffectyDeath(struct CREATURE *Cre);
void GenBloodyBody(struct CREATURE *Cre);
void PrimaryKillFade(struct CREATURE *Cre);
void PrimaryKillBurnFade(struct CREATURE *Cre);
void PrimaryKillPlasmaFade(struct CREATURE *Cre);
struct CRESAVE *LoadStoreQuakePickup(void *CreV, int Store, struct CRESAVE *CreSave);
struct PICKUPDESC *FindPickupData(int Ident);
void Cre_Burn(struct CREATURE *Cre);
void Cre_Plasma(struct CREATURE *Cre);
void Cre_Hyper(struct CREATURE *Cre);
void GenPickupGlint(struct CREATURE *Cre);
void ProcessQuakeBurnEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakePlasmaEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeHyperEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeQuadEffect(struct CREATURE *Cre);
void ProcessQuakeHealEffect(struct CREATURE *Cre, int Decrement);
void ProcessQuakeGeneralCreEffects(struct CREATURE *Cre);
int IsEnergyWeapon(int DamageType);
void BodgeModelDataTransparent(struct GLOBPOL *GlobPoly, int a, int b, int c);
short CheckPowerArmour(struct CREATURE *Cre, int Damage);
short CheckArmour(struct CREATURE *Cre, int Damage, int EnergyWeapon);
void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType, struct VECT *Centre);
void ProcessQuakeBFGEffect(struct CREATURE *Cre, int Decrement);
struct CREATURE *InitSimpleModel(struct SIMSTART *SimStart, struct IDENTITYNAME *Name, int Main, int Drop);
void Cre_HighLightFlame(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Time);
void Cre_Quad(struct CREATURE *Cre);
void Cre_BFGHighLight(struct CREATURE *Cre);
void Cre_BFGHighLightFade(struct CREATURE *Cre);
void Cre_Heal(struct CREATURE *Cre);
int PrimaryQuakePickup(struct CREATURE *Cre);
struct CREATURE *InitialiseQuakePickup(struct PICKUPENTRY *PickupEntry);
struct CRESAVE *QuakeStoreInanimate(struct CREATURE *Cre, struct CRESAVE *CreSave);
void PrimaryQuakeBodyPartMoving(struct CREATURE *Cre);
void GenerateQuakeBodyPart(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name);
void QuakeBlowMeUp(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName);
void PrimaryQuakeExplosion(struct CREATURE *Cre);
void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type);
void PreInitExplosion(struct GLOBMODHDR *Hdr);
void GenModShape(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Blood);
void GenerateQuakeBodyPart2(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name);

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

void PrimaryKillPlasmaFade(struct CREATURE *Cre);
void PrimaryKillBurnFade(struct CREATURE *Cre);
void FreeEdictT(struct edict_s *EdictT);

int rand(void);
void GetFireLocSpace(struct CREATURE *Cre, struct VECT *FirePosition, short *FireSpace);
void GenerateQuakeBodyPart(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name);
void GenerateQuakeBodyPart2(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name);
void GenBloodyBody(struct CREATURE *Cre);

extern int LevelIsMultiPlayer;
extern int MaxWindows;
extern int GlobalKey;
extern short rcossin_tbl[][2];
extern struct PARTCOLOURS ParticleColours_Red;
extern struct PARTCOLOURS ParticleColours_Green;
extern struct PARTCOLOURS ParticleColours_Blue;
extern struct WINDOW GameWindows[];
extern struct QENGINELEVINTERFACE Engine;
extern struct PLAYER Players[];
int rand(void);
void *memset(void *s, int c, unsigned int n);
struct MATRIX *RotMatrixYXZ(struct SVECTOR *r, struct MATRIX *m);
int GetSoundNum(struct IDENTITYNAME Name);
int SoundTestIfRunning(short SoundID, short Channel);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB);
enum PICKUPSTATUS ProcessPickup(struct CREATURE *Cre, unsigned char Item_PickedUp);
void GenPickupGlint(struct CREATURE *Cre);
void KillModel(struct CREATURE *Cre);
void PrimaryKillFade(struct CREATURE *Cre);

static int Sound_Explosion;
int GetSoundNum(struct IDENTITYNAME Name);
void ForEachPolyInModelHdr(struct GLOBMODHDR *Hdr, void *VoidRoutine, int a, int b, int c);
struct CREATURE *InitialiseQuakePickup(struct PICKUPENTRY *PickupEntry);
struct CRESAVE *QuakeStoreInanimate(struct CREATURE *Cre, struct CRESAVE *CreSave);

struct CREATURE *InitSimpleModel(struct SIMSTART *SimStart, struct IDENTITYNAME *Name, int Main, int Drop);
int PrimaryQuakePickup(struct CREATURE *Cre);









void Cre_BFGHighLight(struct CREATURE *Cre);
void Cre_BFGHighLightFade(struct CREATURE *Cre);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life);

#define BFGCOLOUR ((struct COLOUR){0, 255, 0, 0})
#define BFGPOWER ((struct POWER){800, 1300})





extern struct PARTCOLOURS ParticleColours_Quad;
extern struct PARTCOLOURS ParticleColours_GreenBFG;
extern struct PARTCOLOURS ParticleColours_GreenBFGFade;
extern struct PLAYER Players[];
extern int ChrisCount;
int GetNumberVertices(struct GLOBMODHDR *Hdr);
void GetAbsoluteModelVert(struct CREATURE *Cre, short VertexNum, struct VECT *Pos);
struct PARTICLE *AddParticles(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);
void *memset(void *s, int c, unsigned int n);

/* Inlined into the four Cre_* callers (SYM shows only its locals as nested blocks).
   The loop is a guarded do/while: a plain while() gives the wrong delay-slot fill
   for the `if (Cre->Player)` branch. */
static void Cre_HighLight(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Time)
{
    struct PARTICLE *Part;
    struct VECT Pos[15];
    struct SVECT Vel[15];
    int VertexNum;
    int a;
    int ThisPackSize;
    int NumVerts;

    NumVerts = GetNumberVertices(Cre->ModelHdr);
    for (a = 0; a < 15; a++)
        memset(&Vel[a], 0, sizeof(struct SVECT));
    VertexNum = ChrisCount & 1;
    NumVerts -= VertexNum;
    if (NumVerts >= 2) do {
        if (NumVerts < 30)
            ThisPackSize = NumVerts >> 1;
        else
            ThisPackSize = 15;
        NumVerts -= ThisPackSize * 2;
        for (a = 0; a < ThisPackSize; a++) {
            GetAbsoluteModelVert(Cre, VertexNum, &Pos[a]);
            VertexNum += 2;
        }
        Part = AddParticles(Pos, Vel, PColoursA, PColoursB, (short)ThisPackSize, (short)Time, Size, Cre->Box.Area);
        if (Part)
            if (Cre->Player)
                Part->NoDrawWin = 1 << (Cre->Player - Players);
    } while (NumVerts >= 2);
}

extern short rcossin_tbl[][2];
static void NoDraw(struct PARTICLE *Part, int Num)
{
    Part->NoDrawWin = 1 << Num;
}


extern struct QENGINELEVINTERFACE Engine;
extern struct ARMOUR_TYPE Armour_Types[];
extern struct WINDOW GameWindows[];
static char Armour_Fade;
void SetWinFade(int Win, struct COLOUR Colour, short FadeType, int Time, int InitTime);

static inline void UsePowerCells(struct PLAYER *Player, int Save, int Ratio)
{
    Player->Extras.AmmoCount[4] -= Save / Ratio;
}

extern short Console_Flags;
extern struct edict_s *EdictTList;
extern struct CREATURE Creatures[];
extern struct VECT NullVec;
extern struct PLAYER Players[];
extern struct WINDOW GameWindows[];
extern struct QENGINELEVINTERFACE Engine;
void *memset(void *s, int c, unsigned int n);
void VectorNormal(struct VECT *v0, struct VECT *v1);
void PlayerLavaSound(struct CREATURE *Cre);
void PlayerToxicSound(struct CREATURE *Cre);
void T_Damage(struct edict_s *targ, struct edict_s *inflictor, struct edict_s *attacker, struct VECT *dir, struct VECT *point, struct VECT *normal, int damage, int knockback, int dflags, int mod);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
short CheckPowerArmour(struct CREATURE *Cre, int Damage);
short CheckArmour(struct CREATURE *Cre, int Damage, int EnergyWeapon);

/* Inlined. The water test must be a value (xor + beqz) against 8 held in a register
   (KnockBack's s4): a plain `DamageType != 8` gives li/beq. */
static int Differ(int a, int b)
{
    return a != b;
}

struct CREATURE *GetNextCre(int UseExpendable);
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
int GetFloorOffset(struct CREATURE *Cre);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
void KillModel(struct CREATURE *Cre);
struct MATRIX *RotMatrixYXZ(struct SVECTOR *r, struct MATRIX *m);

char *PrintIdentityName(struct IDENTITYNAME Name);
char *PrintBool(int Bool);
int FindAnimSequenceNumFromFrame(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int FrameNum);

/* the original compares with xori: the test goes through a value, as from an inline helper */
static int IsDebugType(struct CREATURE *Cre)
{
    return Cre->TypeIdent == 0x2f;
}

extern struct PARTCOLOURS ParticleColours_BloodRed;
extern struct PARTCOLOURS ParticleColours_DarkBloodRed;
int rand(void);
void *memset(void *s, int c, unsigned int n);
int Move3d_SpinBounceAbout(struct CREATURE *Cre, struct CREATURE *Thrower, struct CREATURE **pHitCre, struct SHORTPAIR BounceDamage);
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);
struct PARTICLE *AddParticlesSL0(struct VECT *Pos, struct SVECT *Off, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

extern short rcossin_tbl[][2];
extern struct COLLHEADER CollMap;
void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ);
void PrimaryQuakeBodyPartMoving(struct CREATURE *Cre);

void *memset(void *s, int c, unsigned int n);
void MyScaleMatrix(struct MATRIX *MatrixOut, struct MATRIX *MatrixIn, int Scale);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);

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

/* Power is assigned a second time (from Save): with a single assignment gcc knows the
   lhu-loaded value is zero-extended and tests `beqz a0` instead of `sll v0,a0,16; beqz v0`.
   (found with decomp-permuter) */
short CheckPowerArmour(struct CREATURE *Cre, int Damage)
{
    struct PLAYER *Player;
    short Power;
    short Save;

    Player = Cre->Player;
    Armour_Fade = 0;
    Power = Player->Extras.AmmoCount[4];
    if (!Damage)
        return 0;
    if (!(Player->Extras.ExtraBits & 0x18000))
        return 0;
    if (!Power)
        return 0;
    Damage = Damage * 2 / 3;
    if (Damage < (short)(Power << 1))
        Save = Damage;
    else
        Save = Power << 1;
    Power = Save;
    if (!Power)
        return 0;
    Armour_Fade = 1;
    SetWinFade(Player->Window - GameWindows, (struct COLOUR){0, 0x80, 0x20, 0}, 2, 10, 10);
    UsePowerCells(Player, Power, 2);
    return Power;
}

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

void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType, struct VECT *Centre)
{
    struct PLAYER *Player;
    struct PTIMERS *PTimers;
    int EnergyWeapon;
    int KnockBack;
    int ShockScale;
    int ShockType;

    Player = Target->Player;
    PTimers = &Player->Extras.PTimers;
    KnockBack = Damage;
    if (!Target)
        return;
    if (LevelIsMultiPlayer) {
        if (!Cre) {
            if (Player)
                Target->HitBy = Player - Players;
        } else if (Cre->Player) {
            Target->HitBy = Cre->Player - Players;
        } else if (*(signed char *)&Cre->HitBy != 4) {
            Target->HitBy = Cre->HitBy;
        } else {
            Engine.PrintF("Multiplayer, can't determine which player hit other player \n");
        }
    } else {
        Target->HitBy = Cre - Creatures;
    }
    Target->HitType = DamageType;
    if (KnockBack && Centre && (DamageType == 15 || DamageType == 13 || DamageType == 3 || DamageType == 18)) {
        struct VECT Dir;
        int Mass;
        int Scale;

        Dir.vx = Target->Box.Location.vx - Centre->vx;
        Dir.vy = Target->Box.Location.vy - Centre->vy;
        Dir.vz = Target->Box.Location.vz - Centre->vz;
        VectorNormal(&Dir, &Dir);
        Mass = 2400;
        if (!Player || *(signed char *)&Cre->HitBy != Player - Players)
            Scale = ((Engine.GameConfig.sBlastScale + 64) * 125) >> 6;
        else
            Scale = ((Engine.GameConfig.sBlastScale + 64) * 25) >> 2;
        Dir.vx = Scale * Dir.vx * KnockBack / Mass >> 4;
        Dir.vy = Scale * Dir.vy * KnockBack / Mass >> 4;
        Dir.vz = Scale * Dir.vz * KnockBack / Mass >> 4;
        if (!LevelIsMultiPlayer) {
            if (Dir.vy < -0xc00)
                Dir.vy = -0xc00;
        }
        if (Dir.vx > 0x7fff)
            Dir.vx = 0x8000;
        if (Dir.vx < -0x8000)
            Dir.vx = -0x8000;
        if (Dir.vy > 0x7fff)
            Dir.vy = 0x8000;
        if (Dir.vy < -0x8000)
            Dir.vy = -0x8000;
        if (Dir.vz > 0x7fff)
            Dir.vz = 0x8000;
        if (Dir.vz < -0x8000)
            Dir.vz = -0x8000;
        if (Target->Health > 0) {
            Target->ForceVel.vx += Dir.vx;
            Target->ForceVel.vy += Dir.vy;
            Target->ForceVel.vz += Dir.vz;
            Target->Control |= 0x4000;
        } else {
            Target->ForceVel.vx = Dir.vx;
            Target->ForceVel.vy = Dir.vy;
            Target->ForceVel.vz = Dir.vz;
        }
    }
    switch (DamageType) {
    case 9:
        if (Player) {
            if (LevelTicks < PTimers->EnviroTimer)
                return;
            if (LevelTicks < PTimers->InvTimer)
                return;
            if (Player->Extras.NextToxicFrame < LevelTicks) {
                Player->Extras.NextToxicFrame = LevelTicks + 400;
                if (Player->Creature)
                    PlayerToxicSound(Player->Creature);
            } else
                return;
        }
        break;
    case 10:
        if (Player) {
            if (LevelTicks > Player->Extras.NextToxicFrame) {
                Player->Extras.NextToxicFrame = LevelTicks + 100;
                if (Player->Creature)
                    PlayerLavaSound(Player->Creature);
            } else
                return;
        }
        break;
    }
    if (Player) {
        if (!Engine.GameConfig.sDifficulty && Cre->EdictT && !Cre->Player)
            Damage = (Damage + 1) >> 1;
        if (LevelTicks < PTimers->InvTimer)
            return;
        EnergyWeapon = IsEnergyWeapon(DamageType);
        KnockBack = 8;
        if (Differ(DamageType, KnockBack)) {
            Damage -= CheckPowerArmour(Target, Damage);
            Damage -= CheckArmour(Target, Damage, EnergyWeapon);
        }
        if ((Console_Flags & 0x80) && DamageType && DamageType != 8 && DamageType != 19 && DamageType != 10 && DamageType != 9) {
            if (Target->Health > 0)
                Target->Health = -Damage;
            else
                Target->Health -= Damage;
        } else
            Target->Health -= Damage;
        if (Console_Flags & 2) {
            if (Target->Health <= 0)
                Target->Health = 200;
        }
    } else if (Target->EdictT) {
        if (Cre && Cre->EdictT)
            T_Damage(Target->EdictT, Cre->EdictT, Cre->EdictT, &NullVec, &NullVec, &NullVec, Damage, 0, 0, 32);
        else
            T_Damage(Target->EdictT, EdictTList + 1, EdictTList + 1, &NullVec, &NullVec, &NullVec, Damage, 0, 0, 32);
    } else
        Target->Health -= Damage;
    if (Target->Player && Target->Player->Window && Target->Health > 0) {
        switch (DamageType) {
        case 1:
            ShockType = 13;
            ShockScale = 0x1000;
            break;
        case 5:
        case 6:
        case 18:
            ShockType = 16;
            ShockScale = 0x800;
            break;
        case 3:
        case 7:
        case 19:
        case 20:
        case 21:
            ShockType = 15;
            ShockScale = 0x1000;
            break;
        default:
            ShockScale = 0;
        }
        if (ShockScale)
            SetShockData(Target->Player->Window - GameWindows, ShockType, ShockScale, 0, (struct POWER){0});
    }
    switch (DamageType) {
    case 1:
        Target->Effect.BFGTime = 3;
        break;
    case 2:
        Target->Effect.BurnTime = 15;
        break;
    case 4:
        Target->Effect.PlasmaTime = 30;
        break;
    case 5:
        Target->Effect.HyperTime = 5;
        break;
    }
}

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

void ProcessQuakeHealEffect(struct CREATURE *Cre, int Decrement)
{
    if (Cre->Effect.HealTime) {
        Cre_Heal(Cre);
        Cre->Effect.HealTime -= Decrement;
    }
}

void ProcessQuakeQuadEffect(struct CREATURE *Cre)
{
    if (Cre->Player && Cre->Player->Extras.PTimers.QuadTimer > LevelTicks)
        Cre_Quad(Cre);
}

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

void Cre_HighLightFlame(struct CREATURE *Cre, struct PARTCOLOURS *PColoursA, struct PARTCOLOURS *PColoursB, int Size, int Time)
{
    int NumVerts;
    int ThisPackSize;
    int a;
    int VertexNum;
    int Ang;
    int Speed;
    struct VECT Pos[15];
    struct SVECT Vel[15];
    struct PARTICLE *Part;

    if (Cre->ModelHdr) {
        NumVerts = GetNumberVertices(Cre->ModelHdr);
        for (a = 0; a < 15; a++) {
            Ang = rand() & 0xfff;
            Speed = ((rand() - 0x4000) * 12 >> 13) + 32;
            Vel[a].vx = rcossin_tbl[Ang][0] * Speed / 12000;
            Vel[a].vy = (rand() - 0x4000) >> 10;
            Vel[a].vz = rcossin_tbl[Ang][1] * Speed / 12000;
        }
        VertexNum = ChrisCount & 7;
        NumVerts -= VertexNum;
        /* guarded do/while, as in Cre_HighLight */
        if (NumVerts >= 8) do {
            if (NumVerts < 120)
                ThisPackSize = NumVerts >> 3;
            else
                ThisPackSize = 15;
            NumVerts -= ThisPackSize * 8;
            for (a = 0; a < ThisPackSize; a++) {
                GetAbsoluteModelVert(Cre, VertexNum, &Pos[a]);
                VertexNum += 8;
            }
            Part = AddParticles(Pos, Vel, PColoursA, PColoursB, (short)ThisPackSize, (short)Time, Size, Cre->Box.Area);
            if (Part)
                if (Cre->Player)
                    NoDraw(Part, Cre->Player - Players);
        } while (NumVerts >= 8);
    }
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

void Cre_Quad(struct CREATURE *Cre)
{
    Cre_HighLight(Cre, &ParticleColours_Quad, &ParticleColours_Quad, 0x7fff, 1);
}

void Cre_BFGHighLight(struct CREATURE *Cre)
{
    Cre_HighLight(Cre, &ParticleColours_GreenBFG, &ParticleColours_GreenBFG, 0x7fff, 1);
}

void Cre_BFGHighLightFade(struct CREATURE *Cre)
{
    Cre_HighLight(Cre, &ParticleColours_GreenBFGFade, &ParticleColours_GreenBFGFade, 0x7fff, 4);
}

void Cre_Heal(struct CREATURE *Cre)
{
    Cre_HighLight(Cre, &ParticleColours_Blue, &ParticleColours_Blue, 0x7fff, 4);
}

int PrimaryQuakePickup(struct CREATURE *Cre)
{
    int Flags;
    int a;
    int b;
    struct CREATURE *Target;
    static struct SVECT Vel[15];
    static int Sound_FadeIn = 0;
    struct VECT Loc;
    struct COLOUR Colour;
    struct PARTCOLOURS *Party;
    struct POWER Power;
    enum PICKUPSTATUS PickupStatus;

    memset(&Colour, 0, sizeof(struct COLOUR));
    if (LevelIsMultiPlayer && Cre->WinData[0].NoDraw) {
        if ((Cre->Params[2] -= NewVcounts) <= 0) {
            for (a = 0; a < MaxWindows; a++)
                Cre->WinData[a].NoDraw = 0;
        }
        return 0;
    }
    Flags = Cre->Params[0];
    if (Flags) {
        if (!(Flags & 0x80)) {
            if (Cre->ModelHdr->NumFrames) {
                Cre->AnimFrame += NewVcounts;
                while (Cre->AnimFrame >= Cre->ModelHdr->NumFrames)
                    Cre->AnimFrame -= Cre->ModelHdr->NumFrames;
            }
        }
        if (Flags & 1) {
            Cre->Ang.vy -= NewVcounts * 3;
            RotMatrixYXZ((struct SVECTOR *)&Cre->Ang, &Cre->Matrix);
        }
        if (Flags & 2) {
            if (Cre->ModelOpaque != 0x1000) {
                if (!Cre->ModelOpaque) {
                    Sound_FadeIn = GetSoundNum((struct IDENTITYNAME){"msc_tele1"});
                    Cre->SoundID = 0;
                    Cre->SoundChannel = 0;
                }
                if ((rcossin_tbl[(LevelTicks << 4) & 0xff0][0] + 0x1000) >> 4 > 0xf0)
                    if (!SoundTestIfRunning(Cre->SoundID, Cre->SoundChannel))
                        SoundByCreature(Sound_FadeIn, Cre);
                Cre->ModelOpaque += NewVcounts * 2;
                if (Cre->ModelOpaque > 0x1000) {
                    Cre->ModelOpaque = 0x1000;
                    Cre->Ambience.r = Cre->Ambience.g = Cre->Ambience.b = 0x7f;
                    Cre->DefaultAmbience = Cre->Ambience;
                }
                for (a = 0; a < 15; a++) {
                    Vel[a].vx = (rand() - 0x4000) >> 9;
                    Vel[a].vy = (rand() - 0x4000) >> 9;
                    Vel[a].vz = (rand() - 0x4000) >> 9;
                }
                if (Flags & 0x10)
                    Party = &ParticleColours_Red;
                else if (Flags & 0x20)
                    Party = &ParticleColours_Green;
                else if (Flags & 0x40)
                    Party = &ParticleColours_Blue;
                AddParticlesSL(&Cre->Draw.Location, Vel, Party, Party, 15, 10, 10000, Cre->Box.Area);
            }
        }
        if (Flags & 0x70) {
            short c; /* not in SYM; a short temp keeps gcc from folding (c >> 2) into one shift */
            c = (rcossin_tbl[(LevelTicks << 4) & 0xff0][0] + 0x1000) >> 4;
            Cre->Ambience.r = Cre->Ambience.g = Cre->Ambience.b = 0;
            if (Flags & 0x10) {
                Colour.r = 0xff;
                Cre->Ambience.r = c >> 2;
            }
            if (Flags & 0x20) {
                Colour.g = 0xff;
                Cre->Ambience.g = c >> 2;
            }
            if (Flags & 0x40) {
                Colour.b = 0xff;
                Cre->Ambience.b = c >> 2;
            }
            Loc.vx = Cre->Draw.Location.vx - 50;
            Loc.vy = Cre->Draw.Location.vy - 500;
            Loc.vz = Cre->Draw.Location.vz;
            c += 100;
            Power.Full = c;
            Power.Fall = c + (c >> 1);
            AddLight(&Loc, Colour, Power, (struct SHORTPAIR){0, 0});
        }
        if (Flags & 4) {
            Cre->Life -= NewVcounts;
            if (Cre->Life <= 0) {
                Cre->Life = 2;
                Cre->Primary = PrimaryKillFade;
                Cre->ModelOpaque = 0x1000;
            }
        }
    }
    for (a = 0; a < MaxWindows; a++) {
        Target = GameWindows[a].Creature;
        if (Target && Target->Health > 0 && VectBoxOverlap(&Cre->Box.VectBox, &Target->Box.VectBox)) {
            PickupStatus = ProcessPickup(Target, Cre->Params[1]);
            if (PickupStatus == PICKUP_LEFT_GEN_GLINT && ((Cre->Params[0] & 4) || !Engine.GameConfig.bWeaponStay || !LevelIsMultiPlayer))
                PickupStatus = PICKUP_TAKEN_KILL_MODEL;
            switch (PickupStatus) {
            case PICKUP_TAKEN_KILL_MODEL:
                GenPickupGlint(Cre);
                if (LevelIsMultiPlayer && !(Cre->Params[0] & 4)) {
                    if (Cre->Params[1] == 32)
                        Cre->Params[2] = 32400;
                    else
                        Cre->Params[2] = 9000;
                    for (b = 0; b < MaxWindows; b++)
                        Cre->WinData[b].NoDraw = 1;
                    return 1;
                }
                KillModel(Cre);
                if (!LevelIsMultiPlayer)
                    GlobalKey = Players[0].Extras.ExtraBits & 0xfff;
                return 1;
            case PICKUP_LEFT_GEN_GLINT:
                GenPickupGlint(Cre);
                return 1;
            }
        }
    }
    return 0;
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

/* Space and Life are int in SYM; short gives the original's register allocation */
/* SYM says int Space/Life, but the body sign-extends them: short like GenerateQuakeBodyPart2 */
void GenerateQuakeBodyPart(struct CREATURE *Parent, int Angle, struct SVECT *Force, struct VECT *Location, short Space, short Life, struct IDENTITYNAME Name)
{
    struct CREATURE *Cre;
    int Speed;

    Cre = GetNextCre(0);
    if (Cre) {
        Cre->Primary = PrimaryQuakeBodyPartMoving;
        Cre->ModelHdr = GetModelHdr(Name);
        Cre->HeightOffset = GetFloorOffset(Cre);
        Cre->Control = 0x1800000;
        Cre->Draw.Location = *Location;
        Cre->Box.Location = *Location;
        Cre->DefaultAmbience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Cre->Ambience = *&(struct COLOUR){0x40, 0x40, 0x40, 0};
        Speed = rand() * 768 / 32768 + 256;
        Cre->Vel.vx = Force->vx + rcossin_tbl[Angle & 0xfff][1] * Speed / 4096;
        Cre->Vel.vy = -rand() * 1536 / 32768 - 0xc00 + Force->vy;
        Cre->Vel.vz = Force->vz + rcossin_tbl[Angle & 0xfff][0] * Speed / 4096;
        Cre->Ang = (struct SVECT){rand(), rand(), rand()};
        Cre->Params[0] = (rand() - 0x4000) >> 6;
        RotMatrixYXZ((struct SVECTOR *)&Cre->Ang, &Cre->Matrix);
        Cre->Ambience = Parent->Ambience;
        Cre->DefaultAmbience = Parent->DefaultAmbience;
        Cre->Box.MoveSpace = -1;
        Cre->Life = Life;
        Cre->Box.PrimarySpace = Space;
        Cre->Box.Area = CollMap.Coll3dSpace[Space].Area;
        SetupCollBoxMinMax(&Cre->Box, (struct SVECT){20, 20, 20});
    }
}

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

void BodgeModelDataTransparent(struct GLOBPOL *GlobPoly, int a, int b, int c)
{
    GlobPoly->Tp = (GlobPoly->Tp & 0x1f) | 0x40;
}

void PreInitExplosion(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        ForEachPolyInModelHdr(Hdr, BodgeModelDataTransparent, 0, 0, 0);
        Sound_Explosion = GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
    }
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

void GenBloodyBody(struct CREATURE *Cre)
{
    GenModShape(Cre, &ParticleColours_BloodRed, &ParticleColours_DarkBloodRed, 0x1800, 1);
}

void GenPickupGlint(struct CREATURE *Cre)
{
    GenModShape(Cre, &ParticleColours_White, &ParticleColours_Blue, 0x1800, 0);
}

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
