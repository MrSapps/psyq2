#include "types.h"

/* defined in this file (quakemis.obj) */
int SoundNoAmmo;
int SoundBlasterFire;
int SoundShotgunFire;
int SoundSuperShotgunFire;
int SoundRailgunFire;
int SoundMachinegunFire;
int SoundRocketFire;
int SoundGrenadeLFire;
int SoundIonRipperFire;
int SoundPlasmaFire;
int SoundFlame;
int SoundBFGFire;
int SoundHyperblasterFire1;
int SoundHyperblasterFire2;
struct SPLURGE AllSplurges[12];

extern unsigned int LevelTicks;
extern short Console_Flags;
extern int LevelIsMultiPlayer;
extern struct PLAYER Players[];
extern int NewVcounts;
extern struct WINDOW GameWindows[];
extern struct COLLHEADER CollMap;
extern struct SIMPOL ParasiteModelData[];
extern struct SIMPOL ParasiteEndModelDataA[];
extern struct SIMPOL ParasiteEndModelDataB[];

int GetSoundNum(struct IDENTITYNAME Name);
void SetRandomPitch(int SoundNum, int MaxPlus, int MaxMinus);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);
int AnyoneFireBFG(struct CREATURE *Aggressor, int Damage);
int AnyoneFireGrenade(struct CREATURE *Aggressor, int Damage, int Life);
void PreInitWeaponExtras(void);
void RotateS(struct MATRIX *m, struct SVECT *v1, struct SVECT *o2);
void PlayerNoise(struct edict_s *who, struct VECT *where, int type);
int AnyoneFireGrenadeL(struct CREATURE *Aggressor, struct VECT *Start, struct SVECT *Dir, int Damage, int Life);
int AnyoneFireBlaster(struct CREATURE *Self, struct VECT *FirePosition, struct SVECT *Dir, int Damage, int Flags, int DamageType, int FireSpace);
int AnyoneFireInstantRail(struct CREATURE *Cre, struct VECT *Start, struct SVECT *Dir, int Damage);
int AnyoneFireRocketL(struct CREATURE *Aggressor, struct VECT *Start, struct SVECT *Dir, int Damage);
struct CREATURE *GetNextCre(int UseExpendable);
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
int GetFloorOffset(struct CREATURE *Cre);
void PrimaryBFG(struct CREATURE *Cre);
void PrimaryGrenadeActive(struct CREATURE *Cre);
void GetAbsoluteModelVert(struct CREATURE *Cre, short VertexNum, struct VECT *Pos);
int AddThickLine(struct VECT *Start, struct VECT *Stop, short Width, short Space, struct SIMPOL *ModelData, struct SIMPOL *EndModelDataA, struct SIMPOL *EndModelDataB);
void GetFireLocSpace(struct CREATURE *Cre, struct VECT *FirePosition, short *FireSpace);
int CreSeesCre(struct CREATURE *CreA, struct CREATURE *CreB);

/* inlined: the nested Off block of the QPlayerFire* functions */
static void GetFirePosition(struct CREATURE *Cre, struct VECT *FirePosition, struct SVECT *Base)
{
    {
        struct SVECT Off;

        Off.vx = Base->vx;
        Off.vy = -Base->vy;
        Off.vz = Base->vz;
        RotateS(&Cre->Matrix, &Off, &Off);
        FirePosition->vx = Cre->Box.Location.vx + Off.vx;
        FirePosition->vy = Cre->Box.Location.vy - Off.vy - (Cre->HeadHeight - 0x11e);
        FirePosition->vz = Cre->Box.Location.vz + Off.vz;
    }
}

void ProcessSplurges(void)
{
    struct SPLURGE *Splurge;
    struct VECT Start;
    struct VECT Stop;
    short Space;

    for (Splurge = AllSplurges; Splurge < &AllSplurges[12]; Splurge++) {
        if (Splurge->Time) {
            Splurge->Time -= NewVcounts;
            if (Splurge->Time < 0)
                Splurge->Time = 0;
            GetAbsoluteModelVert(Splurge->Aggressor, Splurge->Vertex, &Start);
            GetFireLocSpace(Splurge->Aggressor, &Start, &Space);
            Stop = Splurge->Target->Box.Location;
            Stop.vy += (0x23c - Splurge->Target->Box.HitHeight) / 2;
            AddThickLine(&Start, &Stop, Splurge->Width, Space, Splurge->Model, Splurge->EndModelA, Splurge->EndModelB);
        }
    }
}

/* sound names are unterminated 12-char IDENTITYNAME constants: GNU constructor expressions */
void PreInitBlaster(void)
{
    SoundBlasterFire = GetSoundNum((struct IDENTITYNAME){"wep_blastf1a"});
    PreInitWeaponExtras();
}

void PreInitShotgun(void)
{
    SoundShotgunFire = GetSoundNum((struct IDENTITYNAME){"wep_shotgf1b"});
}

void PreInitSuperShotgun(void)
{
    SoundSuperShotgunFire = GetSoundNum((struct IDENTITYNAME){"wep_sshotf1b"});
}

void PreInitRailgun(void)
{
    SoundRailgunFire = GetSoundNum((struct IDENTITYNAME){"wep_railgf1a"});
}

void PreInitMachinegun(void)
{
    SoundMachinegunFire = GetSoundNum((struct IDENTITYNAME){"wep_machgf1b"});
    SetRandomPitch(SoundMachinegunFire, 8, 0);
}

void PreInitHyperblaster(void)
{
    SoundHyperblasterFire1 = GetSoundNum((struct IDENTITYNAME){"wep_hyprbf1a"});
    SoundHyperblasterFire2 = GetSoundNum((struct IDENTITYNAME){"wep_hyprbd1a"});
}

void PreInitRocketLauncher(void)
{
    SoundRocketFire = GetSoundNum((struct IDENTITYNAME){"wep_rocklf1a"});
}

void PreInitGrenadeLauncher(void)
{
    SoundGrenadeLFire = GetSoundNum((struct IDENTITYNAME){"wep_grenlf1a"});
}

void PreInitIonRipper(void)
{
    SoundIonRipperFire = GetSoundNum((struct IDENTITYNAME){"wep_hyprbf1a"});
}

void PreInitPlasma(void)
{
    SoundPlasmaFire = GetSoundNum((struct IDENTITYNAME){"pla_burn1"});
}

void PreInitFlamegun(void)
{
    SoundFlame = GetSoundNum((struct IDENTITYNAME){"amb_spark5"});
}

void PreInitBFG(void)
{
    SoundBFGFire = GetSoundNum((struct IDENTITYNAME){"wep_bfg__f1y"});
}

void GetFireLocSpace(struct CREATURE *Cre, struct VECT *FirePosition, short *FireSpace)
{
    struct SVECT Off;

    *FireSpace = Find3dSpaceGivenSecondary(FirePosition, &Cre->Box.Location, Cre->Box.MoveSpace);
    *FirePosition = CollMap.AreaIntersectLast->Intersect;
    *FireSpace = CollMap.ReturnSpaceNum;
}

enum Fire_Status QPlayerFireBFG(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    short Damage;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    Gun = Player->Extras.Weapon;
    if (Player->Extras.AmmoCount[4] < 50) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (Is_Quad)
        Damage = 800;
    else
        Damage = 200;
    if (AnyoneFireBFG(Cre, Damage)) {
        SetShockData(Cre->Player->Window - GameWindows, 12, 0x1000, 0, (struct POWER){0, 0});
        return 1;
    }
    return 0;
}

enum Fire_Status QPlayerFireGrenade(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    short Damage;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[2]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (Is_Quad)
        Damage = 480;
    else
        Damage = 120;
    if (AnyoneFireGrenade(Cre, Damage, 0x672)) {
        SetShockData(Cre->Player->Window - GameWindows, 7, 0x1000, 0, (struct POWER){0, 0});
        return 1;
    }
    return 0;
}

enum Fire_Status QPlayerFireRocket(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    short Damage;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    PTimers = &Player->Extras.PTimers;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[3]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[3]--;
    Player->Extras.Kick_Angles.vx = -11;
    PTimers->Kick_Timer = LevelTicks + 30;
    Damage = rand() % 20 + 100;
    if (Is_Quad)
        Damage <<= 2;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){40, 80, 100});
    Dir.vx = Player->TargetDir.vx * 2;
    Dir.vy = Player->TargetDir.vy * 2;
    Dir.vz = Player->TargetDir.vz * 2;
    if (AnyoneFireRocketL(Cre, &FirePosition, &Dir, Damage)) {
        SoundByCreature(SoundRocketFire, Gun);
        PlayerNoise(Cre->EdictT, &FirePosition, 1);
        SetShockData(Cre->Player->Window - GameWindows, 10, 0x1000, 0, (struct POWER){0, 0});
        return 1;
    }
    return 0;
}

enum Fire_Status QPlayerFireGrenadeL(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    static struct SVECT BasicDir = {0, 0x800, 0x1800};
    short Damage;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    PTimers = &Player->Extras.PTimers;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[2]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[2]--;
    Player->Extras.Kick_Angles.vx = -11;
    PTimers->Kick_Timer = LevelTicks + 30;
    if (Is_Quad)
        Damage = 480;
    else
        Damage = 120;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){80, 40, 200});
    GetFireLocSpace(Cre, &FirePosition, 0);
    RotateS(&Cre->Matrix, &BasicDir, &Dir);
    Dir.vy = -Dir.vy;
    if (AnyoneFireGrenadeL(Cre, &FirePosition, &Dir, Damage, 900)) {
        SoundByCreature(SoundGrenadeLFire, Gun);
        PlayerNoise(Cre->EdictT, &FirePosition, 1);
        SetShockData(Cre->Player->Window - GameWindows, 9, 0x1000, 0, (struct POWER){0, 0});
        return 1;
    }
    return 0;
}

enum Fire_Status QPlayerFireBlaster(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct SVECT Dir;
    struct VECT FirePosition;
    short Damage;
    short FireSpace;
    int Is_Quad; /* not in SYM, but the compare is done before the calls */

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    Player->Extras.Kick_Angles.vx = -11;
    Player->Extras.PTimers.Kick_Timer = LevelTicks + 30;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){80, 56, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    if (Is_Quad)
        Damage = 32;
    else
        Damage = 8;
    Dir.vx = Player->TargetDir.vx / 64;
    Dir.vy = Player->TargetDir.vy / 64;
    Dir.vz = Player->TargetDir.vz / 64;
    if (AnyoneFireBlaster(Cre, &FirePosition, &Dir, Damage, 11, 6, FireSpace)) {
        PlayerNoise(Cre->EdictT, &FirePosition, 1);
        SetShockData(Cre->Player->Window - GameWindows, 1, 0x1000, 0, (struct POWER){0, 0});
    } else
        return 0;
    return 1;
}

enum Fire_Status QPlayerFireHyperblaster(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct SVECT Dir;
    struct SVECT Start;
    struct VECT FirePosition;
    struct MODSUBOFF ModSubOff;
    short Damage;
    short FireSpace;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[4]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[4]--;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){90, 140, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    if (Is_Quad)
        Damage = 80;
    else
        Damage = 20;
    Dir.vx = Player->TargetDir.vx / 128;
    Dir.vy = Player->TargetDir.vy / 128;
    Dir.vz = Player->TargetDir.vz / 128;
    if (AnyoneFireBlaster(Cre, &FirePosition, &Dir, Damage, 14, 5, FireSpace)) {
        SoundByCreature(SoundHyperblasterFire1, Gun);
        SetShockData(Cre->Player->Window - GameWindows, 2, 0x1000, 0, (struct POWER){0, 0});
        PlayerNoise(Cre->EdictT, &FirePosition, 1);
        return 1;
    }
    return 0;
}

enum Fire_Status QPlayerFireRailGun(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    int Is_Quad;
    struct VECT FirePosition;
    short Damage;
    struct SVECT Dir;
    int GunSound;

    Player = Cre->Player;
    PTimers = &Player->Extras.PTimers;
    Is_Quad = LevelTicks < PTimers->QuadTimer;
    Gun = Player->Extras.Weapon;
    GunSound = GetSoundNum((struct IDENTITYNAME){"wep_railgf1a"});
    if (!Player->Extras.AmmoCount[5]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[5]--;
    Player->Extras.Kick_Angles.vx = -34;
    PTimers->Kick_Timer = LevelTicks + 30;
    SoundByCreature(GunSound, Cre);
    if (LevelIsMultiPlayer)
        Damage = 100;
    else
        Damage = 150;
    if (Is_Quad)
        Damage <<= 2;
    Dir = Player->TargetDir;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){40, 96, 256});
    AnyoneFireInstantRail(Cre, &FirePosition, &Dir, Damage);
    SetShockData(Cre->Player->Window - GameWindows, 11, 0x1000, 0, (struct POWER){0, 0});
    PlayerNoise(Cre->EdictT, &FirePosition, 1);
    return 1;
}

enum Fire_Status QPlayerFireNothing(struct CREATURE *Cre)
{
}

void PreInitDischarge(void)
{
}

int AnyoneFireGrenade(struct CREATURE *Aggressor, int Damage, int Life)
{
    struct CREATURE *Cre;
    char Win;

    Cre = GetNextCre(1);
    if (Cre) {
        Cre->Primary = PrimaryGrenadeActive;
        if ((Cre->ModelHdr = GetModelHdr((struct IDENTITYNAME){"Grenade3"})))
            Cre->HeightOffset = GetFloorOffset(Cre);
        else
            Cre->WinData[0].NoDraw = 1;
        for (Win = 0; Win < 4; Win++)
            Cre->WinData[Win].NoDraw = 1;
        if (Aggressor->Player)
            Cre->HitBy = Aggressor->Player - Players;
        else
            Cre->HitBy = 4;
        Cre->Params[1] = 1;
        Cre->Params[2] = 0x1000;
        Cre->SequenceData = (int)Aggressor;
        Cre->Params[0] = Damage;
        Cre->Params[3] = -1;
        Cre->Life = Life;
    } else
        return 0;
    return 1;
}

int AnyoneFireBFG(struct CREATURE *Aggressor, int Damage)
{
    struct CREATURE *Cre;
    char Win;

    Cre = GetNextCre(1);
    if (Cre) {
        Cre->Primary = PrimaryBFG;
        Cre->ModelHdr = GetModelHdr((struct IDENTITYNAME){"BFGBlast"});
        for (Win = 0; Win < 4; Win++)
            Cre->WinData[Win].NoDraw = 1;
        Cre->HeightOffset = GetFloorOffset(Cre);
        Cre->SequenceData = (int)Aggressor;
        Cre->Life = 2400;
        if (Aggressor->Player) {
            Cre->HitBy = Aggressor->Player - Players;
            Cre->Params[1] = 1;
        } else {
            Cre->HitBy = 4;
            Cre->Params[1] = 2;
        }
        Cre->Params[0] = Damage;
    } else
        return 0;
    return 1;
}

static struct SPLURGE *FindSplurge(struct CREATURE *Aggressor, struct CREATURE *Target, short Vertex)
{
    struct SPLURGE *Splurge;

    for (Splurge = AllSplurges; Splurge < &AllSplurges[12]; Splurge++) {
        if (Splurge->Time && Splurge->Aggressor == Aggressor && Splurge->Target == Target && Splurge->Vertex == Vertex)
            return Splurge;
    }
    for (Splurge = AllSplurges; Splurge < &AllSplurges[12]; Splurge++) {
        if (Splurge->Time == 0)
            return Splurge;
    }
    return 0;
}

static int AddSplurge(struct CREATURE *Aggressor, struct CREATURE *Target, int Vertex, int Width,
                      struct SIMPOL *Model, struct SIMPOL *EndModelA, struct SIMPOL *EndModelB)
{
    struct SPLURGE *Splurge;

    Splurge = FindSplurge(Aggressor, Target, Vertex);
    if (Splurge) {
        Splurge->Time = 45;
        Splurge->Target = Target;
        Splurge->Aggressor = Aggressor;
        Splurge->Vertex = Vertex;
        Splurge->Width = Width;
        Splurge->Model = Model;
        Splurge->EndModelA = EndModelA;
        Splurge->EndModelB = EndModelB;
        return 1;
    }
    return 0;
}

int AnyoneFireSplurge(struct CREATURE *Aggressor, struct CREATURE *Target, int Vertex, int Damage)
{
    struct SVECT Rel;
    struct VECT Start;

    GetAbsoluteModelVert(Aggressor, Vertex, &Start);
    if (!CreSeesCre(Aggressor, Target))
        return 0;
    return AddSplurge(Aggressor, Target, Vertex, 10, ParasiteModelData, ParasiteEndModelDataA, ParasiteEndModelDataB);
}

/* static in quakemis.obj (initialised data) */
extern struct SIMPOL LaserModelDataRed[];
extern struct SIMPOL LaserEndModelDataARed[];
extern struct SIMPOL LaserEndModelDataBRed[];
extern struct CREATURE **ThisBumpList;
extern struct CREATURE **ThisBumpListEnd;
extern struct SOLIDBOX SolidBoxs[];
extern struct COLLISIONRETURNDATA CollisionReturnData;
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop);
void DoSolidBoxEventDamage(); /* called unprototyped: damage is not truncated */
void DoDamage();

int AnyoneFireInstantLaser(struct CREATURE *Cre, struct VECT *Start, struct SVECT *Dir, int Damage)
{
    struct CREATURE *Target;
    struct VECT Stop;
    struct VECT ISect;
    short FireSpace;

    GetFireLocSpace(Cre, Start, &FireSpace);
    Stop.vx = Start->vx + Dir->vx * 8;
    Stop.vy = Start->vy + Dir->vy * 8;
    Stop.vz = Start->vz + Dir->vz * 8;
    if (!Visable3d(&CollMap, Start, &Stop, FireSpace))
        Stop = CollMap.AreaIntersectLast->Intersect;
    Target = LineIntersectCreCylinder(Start, &Stop, Cre, &ISect, ThisBumpList, ThisBumpListEnd);
    if (!VisableSolidBoxLine(Start, &Stop, &Stop)) {
        if (CollisionReturnData.IdentType) {
            if (SolidBoxs[CollisionReturnData.Ident].Flags & 4) {
                SolidBoxs[CollisionReturnData.Ident].Flags |= 8;
                DoSolidBoxEventDamage(&SolidBoxs[CollisionReturnData.Ident], Damage, &Stop);
            }
        }
    }
    if (Target)
        DoDamage(Cre, Target, Damage, 11, &ISect);
    AddThickLine(Start, &Stop, 32, FireSpace, LaserModelDataRed, LaserEndModelDataARed, LaserEndModelDataBRed);
    return 1;
}

/* defined in this file (quakemis.obj) */
int SoundGrenadeBounce;
int AddBlurPoints(struct CREATURE *Cre, short *PointList, struct COLOUR Colour, int Time);
void ExpandVectBox(struct VECTBOX *PQBox, struct VECTBOX *PQExp, short Width, struct SHORTPAIR MinMaxY);
int InVectBox(struct VECTBOX *VectBox, struct VECT *Vect);
int Move3d_SpinBounceAbout(struct CREATURE *Cre, struct CREATURE *Thrower, struct CREATURE **pHitCre, struct SHORTPAIR BounceDamage);
void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type);
int PointHit(struct CREATURE *Aggressor, int PrimarySpace, struct CREATURE *Ignore, struct VECT *Centre, struct SHORTPAIR DamRad, enum DAMAGETYPES DamType, int IgnoreAggressor);
void KillModel(struct CREATURE *Cre);
void UpdateBoxStuff(struct COLLBOX3D *Box, int AreaSource);

void PrimaryGrenadeLActive(struct CREATURE *Cre)
{
    struct SVECT Rel;
    struct SVECT Offset;
    struct SVECT BaseVel;
    struct VECT Pos;
    struct CREATURE *pHitCre;
    struct CREATURE *SequenceData;

    SequenceData = (struct CREATURE *)Cre->SequenceData;
    pHitCre = 0;
    Cre->Draw.Location = Cre->Box.Location;
    AddBlurPoints(Cre, (short []){0, 7, -1, 0}, *&(struct COLOUR){0x20, 0x20, 0x20, 0}, 6);
    if (Cre->Params[3] & Cre->SequenceData) {
        struct VECTBOX ExpVectBox;

        ExpandVectBox(&((struct CREATURE *)Cre->SequenceData)->Box.VectBox, &ExpVectBox, 40, *&(struct SHORTPAIR){-64, 64});
        if (!InVectBox(&ExpVectBox, &Cre->Box.Location))
            Cre->Params[3] = 0;
    }
    if (Move3d_SpinBounceAbout(Cre, (struct CREATURE *)(Cre->SequenceData & Cre->Params[3]), &pHitCre, *&(struct SHORTPAIR){0x900, 3})) {
        if (pHitCre) {
            QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x1000, 0);
            if (pHitCre != SequenceData) {
                DoDamage(SequenceData, pHitCre, Cre->Params[0], 13, &Cre->Box.Location);
                PointHit(SequenceData, Cre->Box.PrimarySpace, pHitCre, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
            } else
                PointHit(SequenceData, Cre->Box.PrimarySpace, 0, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
            KillModel(Cre);
            return;
        }
        SoundByCreature(SoundGrenadeBounce, Cre);
    }
    Cre->Life -= NewVcounts;
    if (Cre->Life <= 0) {
        QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x1000, 0);
        PointHit(SequenceData, Cre->Box.PrimarySpace, 0, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
        KillModel(Cre);
        return;
    }
    UpdateBoxStuff(&Cre->Box, 2);
}

int SoundShotgunFire;
int rand(void);
void AnyoneFireInstantBullet(struct CREATURE *Cre, struct VECT *Start, struct SVECT *Dir, int Damage, int Space, int BulletType);
int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life);

enum Fire_Status QPlayerFireShotGun(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    int a;
    short Damage;
    short FireSpace;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    PTimers = &Player->Extras.PTimers;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[0]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[0]--;
    Player->Extras.Kick_Angles.vx = -22;
    PTimers->Kick_Timer = LevelTicks + 30;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){76, 80, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    Damage = 6;
    if (Is_Quad)
        Damage = 24;
    for (a = 0; a < 5; a++) {
        Dir.vx = Player->TargetDir.vx * 4 + ((rand() - 0x4000) >> 4);
        Dir.vy = Player->TargetDir.vy * 4 + ((rand() - 0x4000) >> 4);
        Dir.vz = Player->TargetDir.vz * 4 + ((rand() - 0x4000) >> 4);
        AnyoneFireInstantBullet(Cre, &FirePosition, &Dir, Damage, FireSpace, 1);
    }
    PlayerNoise(Cre->EdictT, &FirePosition, 1);
    AddTimedLight(&Cre->Draw.Location, *&(struct COLOUR){200, 100, 100, 0}, *&(struct POWER){600, 1200}, 3);
    SoundByCreature(SoundShotgunFire, Gun);
    SetShockData(Cre->Player->Window - GameWindows, 3, 0x1000, 0, (struct POWER){0, 0});
    Gun->ShieldColour.Pad = 1;
    return 1;
}

int SoundSuperShotgunFire;

enum Fire_Status QPlayerFireSuperShotGun(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    int a;
    short Damage;
    short FireSpace;
    int Is_Quad;

    Player = Cre->Player;
    PTimers = &Player->Extras.PTimers;
    Is_Quad = LevelTicks < PTimers->QuadTimer;
    Gun = Player->Extras.Weapon;
    if (Player->Extras.AmmoCount[0] < 2) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[0] -= 2;
    Player->Extras.Kick_Angles.vx = -22;
    PTimers->Kick_Timer = LevelTicks + 30;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){96, 72, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    Damage = 8;
    if (Is_Quad)
        Damage = 32;
    for (a = 0; a < 10; a++) {
        Dir.vx = Player->TargetDir.vx * 4 + ((rand() - 0x4000) >> 3);
        Dir.vy = Player->TargetDir.vy * 4 + ((rand() - 0x4000) >> 4);
        Dir.vz = Player->TargetDir.vz * 4 + ((rand() - 0x4000) >> 4);
        AnyoneFireInstantBullet(Cre, &FirePosition, &Dir, Damage, FireSpace, 2);
    }
    PlayerNoise(Cre->EdictT, &FirePosition, 1);
    AddTimedLight(&Cre->Box.Location, *&(struct COLOUR){200, 100, 100, 0}, *&(struct POWER){800, 1600}, 4);
    SoundByCreature(SoundSuperShotgunFire, Gun);
    SetShockData(Cre->Player->Window - GameWindows, 4, 0x1000, 0, (struct POWER){0, 0});
    return 1;
}

int SoundChaingunFire;
int SoundChaingunFire1;
int SoundChaingunFire2;
int SoundChaingunFire3;
int GetSoundClone(int OriginalSoundNum);
void SoundSetRelativeVolume(int SampleNum, int Volume);

void PreInitChaingun(void)
{
    SoundChaingunFire = GetSoundNum((struct IDENTITYNAME){"wep_machgf1b"});
    SoundChaingunFire = GetSoundClone(SoundChaingunFire);
    SetRandomPitch(SoundChaingunFire, 24, 16);
    SoundChaingunFire1 = GetSoundNum((struct IDENTITYNAME){"wep_chngnu1a"});
    SoundChaingunFire2 = GetSoundNum((struct IDENTITYNAME){"wep_chngnl1a"});
    SoundChaingunFire3 = GetSoundNum((struct IDENTITYNAME){"wep_chngnd1a"});
    SoundSetRelativeVolume(SoundChaingunFire, -40);
    SoundSetRelativeVolume(SoundChaingunFire1, 40);
    SoundSetRelativeVolume(SoundChaingunFire2, 40);
    SoundSetRelativeVolume(SoundChaingunFire3, 40);
}

extern struct SIMPOL BFGModelData[];
extern struct SIMPOL BFGEndModelDataA[];
extern struct SIMPOL BFGEndModelDataB[];
int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB);

void BFGSpecial(struct CREATURE *Aggressor, struct CREATURE *Cre, int Damage)
{
    struct CREATURE **Bump;
    struct VECT Centre;
    struct VECTBOX VectBox;
    int Dx;
    int Dy;
    int Dz;
    int Dist2;
    struct SHORTPAIR *SPair;

    VectBox.Min.vx = Cre->Box.Location.vx - 0xc00;
    VectBox.Min.vy = Cre->Box.Location.vy - 0xc00;
    VectBox.Min.vz = Cre->Box.Location.vz - 0xc00;
    VectBox.Max.vx = Cre->Box.Location.vx + 0xc00;
    VectBox.Max.vy = Cre->Box.Location.vy + 0xc00;
    VectBox.Max.vz = Cre->Box.Location.vz + 0xc00;
    SPair = (struct SHORTPAIR *)&Cre->Params[2];
    for (Bump = ThisBumpList; Bump < ThisBumpListEnd; Bump++) {
        if (VectBoxOverlap(&VectBox, &(*Bump)->Box.VectBox) && *Bump != Aggressor) {
            Dx = Cre->Box.Location.vx - (*Bump)->Box.Location.vx;
            Dy = Cre->Box.Location.vy - (*Bump)->Box.Location.vy;
            Dz = Cre->Box.Location.vz - (*Bump)->Box.Location.vz;
            Dist2 = Dx * Dx + Dy * Dy + Dz * Dz;
            if (Dist2 <= 0x900000 && (!(*Bump)->EdictT || (*Bump)->EdictT->takedamage) && CreSeesCre(Cre, *Bump)) {
                AddSplurge(Cre, *Bump, -1, 64, BFGModelData, BFGEndModelDataA, BFGEndModelDataB);
                if (LevelIsMultiPlayer) {
                    if (SPair->a >= 30)
                        DoDamage(Cre, *Bump, 5, 1, 0);
                } else {
                    if (SPair->a >= 30)
                        DoDamage(Cre, *Bump, 10, 1, 0);
                }
            }
        }
    }
}

int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
int CreCollideAsPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage);
extern short rcossin_tbl[][2];

void PrimaryRocketActive(struct CREATURE *Cre)
{
    struct CREATURE *pHitCre;

    pHitCre = 0;
    AddLight(&Cre->Box.Location, (struct COLOUR){(rand() & 0x1f) + 0x70, (rand() & 0x1f) + 0x50, (rand() & 0x1f) + 0x21},
             *&(struct POWER){2000, 1000}, *&(struct SHORTPAIR){0, 0});
    Cre->Params[1] = (Cre->Params[1] + NewVcounts * 32) & 0xfff;
    Cre->Ang.vx = Cre->Params[3] + (rcossin_tbl[Cre->Params[1]][0] >> 6);
    Cre->Ang.vy = Cre->Params[2] + (rcossin_tbl[Cre->Params[1]][1] >> 6);
    Cre->Life -= NewVcounts;
    if (Cre->Life <= 0)
        KillModel(Cre);
    RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
    if (CreCollideAsPoint(Cre, (struct CREATURE *)Cre->SequenceData, 0, &pHitCre, 10)) {
        QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x1000, 0);
        if (pHitCre) {
            DoDamage(Cre->SequenceData, pHitCre, Cre->Params[0], 15, &Cre->Box.Location);
            PointHit((struct CREATURE *)Cre->SequenceData, Cre->Box.PrimarySpace, pHitCre, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1300}, 15, 0);
        } else
            PointHit((struct CREATURE *)Cre->SequenceData, Cre->Box.PrimarySpace, 0, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1300}, 15, 0);
        KillModel(Cre);
    } else {
        UpdateBoxStuff(&Cre->Box, 2);
        AddBlurPoints(Cre, (short []){15, 21, -1, 0}, *&(struct COLOUR){0x20, 0x20, 0x20, 0}, 7);
        AddBlurPoints(Cre, (short []){38, 30, -1, 0}, *&(struct COLOUR){0x20, 0x20, 0x20, 0}, 7);
        Cre->Draw.Location = Cre->Box.Location;
    }
}

int GetPrimarySpaceFromSecondary(int Indx, struct VECT *Location);
int FindNewSpaceFromOld(struct COLLHEADER *Map, struct VECT *New, struct VECT *Old, int Space);
void SetupCollBoxMinMax(struct COLLBOX3D *Box, struct SVECT RadiusXYZ);

int AnyoneFireGrenadeL(struct CREATURE *Aggressor, struct VECT *Start, struct SVECT *Dir, int Damage, int Life)
{
    struct CREATURE *Cre;
    short Space;

    Cre = GetNextCre(1);
    if (Cre) {
        Space = FindNewSpaceFromOld(&CollMap, Start, &Aggressor->Box.Location, GetPrimarySpaceFromSecondary(Aggressor->Box.MoveSpace, &Aggressor->Box.Location));
        Cre->Primary = PrimaryGrenadeLActive;
        if ((Cre->ModelHdr = GetModelHdr((struct IDENTITYNAME){"Grenade2"})))
            Cre->HeightOffset = GetFloorOffset(Cre);
        else
            Cre->WinData[0].NoDraw = 1;
        if (Aggressor->Player)
            Cre->HitBy = Aggressor->Player - Players;
        else
            Cre->HitBy = 4;
        Cre->Draw.Location = *Start;
        Cre->Box.Location = Cre->Draw.Location;
        Cre->Vel = *Dir;
        Cre->Box.PrimarySpace = Space;
        Cre->Box.Area = CollMap.Coll3dSpace[Space].Area;
        Cre->SequenceData = (int)Aggressor;
        Cre->Params[0] = Damage;
        Cre->Params[3] = -1;
        SetupCollBoxMinMax(&Cre->Box, *&(struct SVECT){20, 20, 20});
        Cre->Ang = (struct SVECT){rand(), rand(), rand()};
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        Cre->Life = Life;
        if (Aggressor->Player)
            SoundByCreature(SoundGrenadeLFire, Aggressor->Player->Extras.Weapon);
        else
            SoundByCreature(SoundGrenadeLFire, Aggressor);
    } else
        return 0;
    return 1;
}

enum Fire_Status QPlayerFireMachineGun(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    int a;
    int Full;
    int Fall;
    short Damage;
    short FireSpace;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    PTimers = &Player->Extras.PTimers;
    Gun = Player->Extras.Weapon;
    if (!Player->Extras.AmmoCount[1]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[1]--;
    /* srl in the original: the kick offset is computed unsigned */
    Player->Extras.Kick_Angles.vx = ((rand() & 0x7fff) - 0x4000u) >> 11;
    Player->Extras.Kick_Angles.vy = ((rand() & 0x7fff) - 0x4000u) >> 12;
    Player->Extras.Kick_Angles.vz = ((rand() & 0x7fff) - 0x4000u) >> 12;
    PTimers->Kick_Timer = LevelTicks + 30;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){72, 80, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    Damage = 8;
    if (Is_Quad)
        Damage = 32;
    for (a = 0; a < 1; a++) {
        Dir.vx = Player->TargetDir.vx * 4 + ((rand() - 0x4000) * 512 >> 14);
        Dir.vy = Player->TargetDir.vy * 4 + ((rand() - 0x4000) * 512 >> 14);
        Dir.vz = Player->TargetDir.vz * 4 + ((rand() - 0x4000) * 512 >> 14);
        AnyoneFireInstantBullet(Cre, &FirePosition, &Dir, Damage, FireSpace, 3);
    }
    PlayerNoise(Cre->EdictT, &FirePosition, 1);
    Full = (rand() * 100 >> 15) + 250;
    Fall = (rand() * 200 >> 15) + 700;
    AddLight(&Cre->Box.Location, *&(struct COLOUR){200, 100, 100, 0}, (struct POWER){Full, Fall}, *&(struct SHORTPAIR){0, 0});
    SoundByCreature(SoundMachinegunFire, Gun);
    SetShockData(Cre->Player->Window - GameWindows, 5, 0x1000, 0, (struct POWER){0, 0});
    return 1;
}

int SoundAtLocation(int SFXNumber, struct VECT *loc);

enum Fire_Status QPlayerFireChainGun(struct CREATURE *Cre)
{
    struct PLAYER *Player;
    struct CREATURE *Gun;
    struct PTIMERS *PTimers;
    struct SVECT Dir;
    struct VECT FirePosition;
    int a;
    int Full;
    int Fall;
    short Damage;
    short FireSpace;
    short Shots;
    int Is_Quad;

    Player = Cre->Player;
    Is_Quad = LevelTicks < Player->Extras.PTimers.QuadTimer;
    PTimers = &Player->Extras.PTimers;
    Gun = Player->Extras.Weapon;
    Shots = Gun->ShadowPos.vx;
    if (!Player->Extras.AmmoCount[1]) {
        SoundByCreature(SoundNoAmmo, Gun);
        return 2;
    }
    if (!(Console_Flags & 1))
        Player->Extras.AmmoCount[1] -= Shots;
    /* srl in the original: the kick offset is computed unsigned */
    Player->Extras.Kick_Angles.vy = ((rand() & 0x7fff) - 0x4000u) >> 11;
    Player->Extras.Kick_Angles.vz = ((rand() & 0x7fff) - 0x4000u) >> 11;
    PTimers->Kick_Timer = LevelTicks + 30;
    GetFirePosition(Cre, &FirePosition, &(struct SVECT){104, 128, 250});
    GetFireLocSpace(Cre, &FirePosition, &FireSpace);
    Damage = 8;
    if (Is_Quad)
        Damage = 32;
    for (a = 0; a < Shots; a++) {
        Dir.vx = Player->TargetDir.vx * 4 + ((rand() - 0x4000) * 512 >> 14);
        Dir.vy = Player->TargetDir.vy * 4 + ((rand() - 0x4000) * 512 >> 14);
        Dir.vz = Player->TargetDir.vz * 4 + ((rand() - 0x4000) * 512 >> 14);
        AnyoneFireInstantBullet(Cre, &FirePosition, &Dir, Damage, FireSpace, 4);
        SoundAtLocation(SoundChaingunFire, &Cre->Box.Location);
    }
    if (Shots) {
        SoundByCreature(SoundMachinegunFire, Gun);
        SetShockData(Cre->Player->Window - GameWindows, 6, 0x1000, 0, (struct POWER){0, 0});
    }
    PlayerNoise(Cre->EdictT, &FirePosition, 1);
    Full = (rand() * 200 >> 15) + 350;
    Fall = (rand() * 300 >> 15) + 900;
    AddLight(&Cre->Box.Location, *&(struct COLOUR){200, 100, 100, 0}, (struct POWER){Full, Fall}, *&(struct SHORTPAIR){0, 0});
    return 1;
}

extern struct SIMPOL LaserModelDataBlue[];
extern struct SIMPOL LaserEndModelDataABlue[];
extern struct SIMPOL LaserEndModelDataBBlue[];
extern struct SIMPOL LaserModelDataYellow[];
extern struct SIMPOL LaserEndModelDataAYellow[];
extern struct SIMPOL LaserEndModelDataBYellow[];
extern struct PARTCOLOURS ParticleColours_Red;
extern struct PARTCOLOURS ParticleColours_Blue;
extern struct PARTCOLOURS ParticleColours_Yellow;
void GenerateBulletBlood(struct VECT *Location, int Area);
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);

static void LaserSparks(struct VECT *Location, int Area, struct PARTCOLOURS *PColour)
{
    struct PARTICLE *Particle;
    struct SVECT Vel[15];
    int b;
    int a;

    for (b = 0; b < 4; b++) {
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) >> 10;
            Vel[a].vy = (rand() - 0x4000) >> 10;
            Vel[a].vz = (rand() - 0x4000) >> 10;
        }
        Particle = AddParticlesSL(Location, Vel, PColour, PColour, 15, 15, 0x1800, Area);
        if (Particle)
            Particle->Drift.vy = 2;
    }
}

struct CREATURE *AddLaser(struct VECT *Start, struct VECT *Stop, short Space, short LaserType, struct SHORTPAIR Parts)
{
    struct PARTCOLOURS *PColour;
    short Area;
    short Damage;
    short DamageType;
    struct CREATURE *Target;

    Target = 0;
    switch (LaserType) {
    case 0:
        AddThickLine(Start, Stop, 16, Space, LaserModelDataRed, LaserEndModelDataARed, LaserEndModelDataBRed);
        Damage = 0x200;
        DamageType = 11;
        PColour = &ParticleColours_Red;
        break;
    case 1:
        AddThickLine(Start, Stop, 16, Space, LaserModelDataBlue, LaserEndModelDataABlue, LaserEndModelDataBBlue);
        Damage = 1;
        DamageType = 16;
        PColour = &ParticleColours_Blue;
        break;
    case 2:
        AddThickLine(Start, Stop, 16, Space, LaserModelDataYellow, LaserEndModelDataAYellow, LaserEndModelDataBYellow);
        Damage = 0x200;
        DamageType = 12;
        PColour = &ParticleColours_Yellow;
        break;
    case 4:
        AddThickLine(Start, Stop, 64, Space, LaserModelDataRed, LaserEndModelDataARed, LaserEndModelDataBRed);
        Damage = 0x200;
        DamageType = 11;
        PColour = &ParticleColours_Red;
        break;
    case 3:
        AddThickLine(Start, Stop, 64, Space, LaserModelDataBlue, LaserEndModelDataABlue, LaserEndModelDataBBlue);
        Damage = 1;
        DamageType = 16;
        PColour = &ParticleColours_Blue;
        break;
    case 5:
        AddThickLine(Start, Stop, 64, Space, LaserModelDataYellow, LaserEndModelDataAYellow, LaserEndModelDataBYellow);
        Damage = 0x200;
        DamageType = 12;
        PColour = &ParticleColours_Yellow;
        break;
    }
    if (Damage) {
        struct VECT ISect;

        /* assigned in the condition: a separate statement lays out the second Vel[] differently */
        if ((Target = LineIntersectCreCylinder(Start, Stop, 0, &ISect, ThisBumpList, ThisBumpListEnd))) {
            DoDamage(0, Target, Damage, DamageType, &ISect);
            GenerateBulletBlood(&ISect, Target->Box.Area);
        }
    }
    Area = CollMap.Coll3dSpace[Space].Area;
    if (Parts.a)
        LaserSparks(Start, Area, PColour);
    if (Parts.b)
        LaserSparks(Stop, Area, PColour);
    return Target;
}

int SoundGrenadeTick;
int SoundGrenadeThrow;
int SoundReload;
int SoundGrenadeExplodes;

void PreInitWeaponExtras(void)
{
    SoundNoAmmo = GetSoundNum((struct IDENTITYNAME){"wep_noammo"});
    SoundGrenadeBounce = GetSoundNum((struct IDENTITYNAME){"wep_grenlb1b"});
    SoundGrenadeTick = GetSoundNum((struct IDENTITYNAME){"wep_hgrenc1b"});
    SoundGrenadeThrow = GetSoundNum((struct IDENTITYNAME){"wep_hgrent1a"});
    SoundReload = GetSoundNum((struct IDENTITYNAME){"wep_shotgr1b"});
    if (!SoundReload)
        SoundReload = GetSoundNum((struct IDENTITYNAME){"wep_sshotr1b"});
    SoundGrenadeExplodes = GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
}

int SoundStop(struct SOUNDINFO *SoundInfo);

void PrimaryGrenadeActive(struct CREATURE *Cre)
{
    struct CREATURE *pHitCre;
    struct CREATURE *Gun;
    struct CREATURE *SequenceData;
    struct PLAYER *Player;
    struct SOUNDINFO SInfo;
    struct SVECT Dir;
    struct SVECT Velocity;
    struct VECT FirePosition;
    int Space;
    char Win;
    int Init_Grenade;

    SequenceData = (struct CREATURE *)Cre->SequenceData;
    pHitCre = 0;
    Init_Grenade = 0;
    Player = SequenceData->Player;
    if (SequenceData->Health <= 0 || !SequenceData->Life)
        SequenceData = 0;
    else
        Gun = Player->Extras.Weapon;
    if (!SequenceData && Cre->Params[1] != 3) {
        KillModel(Cre);
        return;
    }
    Cre->Life -= NewVcounts;
GAHold:
    switch (Cre->Params[1]) {
    case 1:
        Cre->Draw.Location = Gun->Draw.Location;
        if (Gun->AnimFrame == 0x17c) {
            Cre->Params[2] += NewVcounts * 6;
            if (Cre->Life <= 0) {
                Init_Grenade = 1;
                Cre->Params[1] = 2;
                goto GAHold;
            }
        }
        if (Gun->AnimFrame >= 0x105 && Gun->SequenceData < 0x105)
            SoundByCreature(SoundGrenadeTick, Cre);
        if (Gun->AnimFrame >= 0x19b && Gun->SequenceData < 0x19b) {
            Cre->Params[1] = 2;
            if (SequenceData->Player && SequenceData->Player->Window)
                SetShockData(SequenceData->Player->Window - GameWindows, 8, 0x1000, 0, (struct POWER){0, 0});
        }
        Gun->SequenceData = Gun->AnimFrame - 1;
        return;
    case 2:
        for (Win = 0; Win < 4; Win++)
            Cre->WinData[Win].NoDraw = 0;
        GetFirePosition(SequenceData, &FirePosition, &(struct SVECT){80, -50, 200});
        Space = FindNewSpaceFromOld(&CollMap, &FirePosition, &SequenceData->Box.Location, GetPrimarySpaceFromSecondary(SequenceData->Box.MoveSpace, &SequenceData->Box.Location));
        Cre->Draw.Location = FirePosition;
        Cre->Box.Location = Cre->Draw.Location;
        Cre->Box.PrimarySpace = Space;
        Cre->Box.Area = CollMap.Coll3dSpace[Cre->Box.PrimarySpace].Area;
        SetupCollBoxMinMax(&Cre->Box, *&(struct SVECT){20, 20, 20});
        {
            struct MATRIX Matrix;

            Velocity = (struct SVECT){0, 0x800, Cre->Params[2]};
            RotMatrixYXZ(&SequenceData->Ang, &Matrix);
            RotateS(&Matrix, &Velocity, &Dir);
            Dir.vy = -Dir.vy;
        }
        Cre->Ang = (struct SVECT){-(Player->Window->CamAng.vx + 0x500), -Player->Window->CamAng.vy, Player->Window->CamAng.vz};
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        Cre->Vel = Dir;
        Cre->Box.OnFloor = 0;
        SoundByCreature(SoundGrenadeThrow, Gun);
        Player = SequenceData->Player;
        if (!(Console_Flags & 1))
            Player->Extras.AmmoCount[2]--;
        Cre->Params[1] = 3;
    default:
        Cre->Draw.Location = Cre->Box.Location;
        AddBlurPoints(Cre, (short []){2, 39, -1, 0}, *&(struct COLOUR){0x20, 0x20, 0x20, 0}, 6);
        if (Cre->Params[3] & Cre->SequenceData) {
            struct VECTBOX ExpVectBox;

            ExpandVectBox(&((struct CREATURE *)Cre->SequenceData)->Box.VectBox, &ExpVectBox, 40, *&(struct SHORTPAIR){-64, 64});
            if (!InVectBox(&ExpVectBox, &Cre->Box.Location))
                Cre->Params[3] = 0;
        }
        if (Move3d_SpinBounceAbout(Cre, (struct CREATURE *)(Cre->SequenceData & Cre->Params[3]), &pHitCre, *&(struct SHORTPAIR){0x900, 3})) {
            if (pHitCre) {
                QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x1000, 0);
                if (pHitCre != SequenceData) {
                    DoDamage(SequenceData, pHitCre, Cre->Params[0], 13, &Cre->Box.Location);
                    PointHit(SequenceData, Cre->Box.PrimarySpace, pHitCre, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
                } else
                    PointHit(SequenceData, Cre->Box.PrimarySpace, 0, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
                SInfo.Channel = Cre->SoundChannel;
                SInfo.SoundID = Cre->SoundID;
                SoundStop(&SInfo);
                KillModel(Cre);
                return;
            }
            SoundByCreature(SoundGrenadeBounce, Cre);
        }
        UpdateBoxStuff(&Cre->Box, 2);
        if (Cre->Life <= 0) {
            QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x1000, 0);
            PointHit(SequenceData, Cre->Box.PrimarySpace, 0, &Cre->Box.Location, (struct SHORTPAIR){Cre->Params[0], 1000}, 13, 0);
            SInfo.Channel = Cre->SoundChannel;
            SInfo.SoundID = Cre->SoundID;
            SoundStop(&SInfo);
            KillModel(Cre);
            if (Init_Grenade && SequenceData) {
                Gun->Params[2] = 2;
                Gun->SequenceData = 0;
                Gun->AnimFrame = 0;
                Gun->Params[3] = 150;
                Gun->SequenceHdr = 0;
                Gun->AnimFlags = (Gun->AnimFlags | 1) & ~2;
            }
        }
    }
}
