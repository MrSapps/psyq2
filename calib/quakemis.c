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
