#include "types.h"

/* defined in this file (qents.obj), so $gp-relative */
short QuakeTimer;
short QuakeTimerIdentIndx;
short MaxEdictT;
struct pathcorner_s *PathTList;
short MaxPathT;
/* defined here too, but larger than 8 bytes */
struct level_locals_t level;

extern int LevelIsMultiPlayer;
extern int NewVcounts;
extern unsigned int QuakeLevelFrame;
extern unsigned int QuakeLevelTime;
extern struct CREATURE Creatures[];
extern struct QCREINTERFACE *QCreInterfaces[];
extern struct CRESAVE *SaveMemoryWriteAddress;

void PlayerNoise(struct edict_s *who, struct VECT *where, int type);
void FreeEdictT(struct edict_s *EdictT);
void QuakeThinkGeneral(void);
void InitialiseEdictTList(void);
void PlayerTrail_Init(void);
struct QCREINTERFACE *GetCreInterface(struct CREATURE *Cre);
struct SPAWNDETAILS *GetSpawnDetails(int TypeIdent);
void KillModel(struct CREATURE *Cre);
struct CRESAVE *GeneralRespawnStuffStore(struct CREATURE *Cre, struct CRESAVE *CreSave);
struct CREATURE *GeneralRespawnStuff(struct CRESAVE *CreSave);
struct CRESAVE *QuakeStoreInanimate(struct CREATURE *Cre, struct CRESAVE *CreSave);
struct CREATURE *RespawnQuakeDeadBody(struct CRESAVE *CreSave);

/* defined in this file (qents.obj) */
struct VECT *PlayerOldOrigin;
short MaxGClientT;
int *PlayerOldMoveSpace;
struct edict_s *EdictTList;

extern struct CREATURE *CreFreeList[];
void MyFree(void **Addr);
void *MY_malloc_name(int s, char *name);
void *memset();
int EffectyDeath(struct CREATURE *Cre);
void ProcessQuakeGeneralCreEffects(struct CREATURE *Cre);
void AddCreToNextBumpList(struct CREATURE *Cre);
void QuakeCreatureSmash(struct CREATURE *Cre, struct SVECT *Force);
struct edict_s *G_Spawn_Path(void);
int GetDifficultySetting(void);
struct CREATURE *InitialiseQuakePickup(struct PICKUPENTRY *PickupEntry);
void ProcessQuakeDeath(struct CREATURE *Cre);
void QuakeDeadHitCollision(struct COLLBOX3D *Box);
void PrimaryQuakeDeadBody(struct CREATURE *Cre);

void FinishDying(void)
{
    struct CREATURE *Cre;
    struct edict_s *Edict;

    for (Cre = Creatures; Cre < (struct CREATURE *)CreFreeList; Cre++) {
        Edict = Cre->EdictT;
        if (Edict && Edict->deadflag == 2) {
            ProcessQuakeDeath(Cre);
            Cre->Params[0] = Cre->EdictT->gib_health;
            Cre->Params[1] = Cre->EdictT->max_health;
            FreeEdictT(Cre->EdictT);
            Cre->EdictT = 0;
            Cre->Primary = PrimaryQuakeDeadBody;
            QuakeDeadHitCollision(&Cre->Box);
            Cre->Control |= 0x8000;
            Cre->IdealYaw = Cre->TypeIdent;
            Cre->TypeIdent = 0x2f;
        }
    }
}

void FreeEdictTList(void)
{
    MyFree((void **)&PlayerOldMoveSpace);
    MyFree((void **)&PlayerOldOrigin);
    MyFree((void **)&EdictTList);
    MaxEdictT = 0;
    MaxGClientT = 0;
}

void FreePathTList(void)
{
    MyFree((void **)&PathTList);
    MaxPathT = 0;
}

void MallocPathTList(short NumPaths)
{
    PathTList = MY_malloc_name(NumPaths * sizeof(struct pathcorner_s), "Quakes Paths");
    if (PathTList) {
        memset(PathTList, 0, NumPaths * sizeof(struct pathcorner_s));
        MaxPathT = NumPaths;
    } else
        MaxPathT = 0;
}

void SpawnPathCorners(struct PATHCORNER *PathList)
{
    struct edict_s *Edict;

    while (PathList->Location.vx != -1) {
        Edict = G_Spawn_Path();
        if (Edict) {
            Edict->classname = 0x72;
            Edict->inuse = 1;
            Edict->targetname = PathList->Ident;
            Edict->target = PathList->TargetIdent;
            Edict->wait = PathList->Wait;
            Edict->sorigin = PathList->Location;
            Edict->solid = 1;
            Edict->spawnflags = PathList->Flags;
        }
        PathList++;
    }
}

void SetInitialCreAnimFrame(struct CREATURE *Cre)
{
    struct NEWANIMREMAP *Remap;

    Remap = Cre->EdictT->Extra.NewAnimRemapTable;
    if (Remap) {
        while (Remap->First >= 0) {
            if (Cre->EdictT->s.frame >= Remap->First && Cre->EdictT->s.frame <= Remap->Last) {
                Cre->AnimFrame = Remap->Base + (Cre->EdictT->s.frame - Remap->First) * 30;
                Cre->Draw.TargetAnimFrame = Cre->AnimFrame;
                Cre->Draw.AnimJumpServer = 0;
                Cre->Draw.AnimJump = 0;
                return;
            }
            Remap++;
        }
    }
}

void InitialiseQuakePickups(struct PICKUPENTRY *Pickups)
{
    struct PICKUPENTRY *PickupEntry;

    PickupEntry = Pickups;
    while (PickupEntry->SimStart.Location.vx != -1) {
        switch (GetDifficultySetting()) {
        case 0:
            if (PickupEntry->SimStart.NotEasy) {
                PickupEntry++;
                continue;
            }
            break;
        case 1:
            if (PickupEntry->SimStart.NotMedium) {
                PickupEntry++;
                continue;
            }
            break;
        case 2:
            if (PickupEntry->SimStart.NotHard) {
                PickupEntry++;
                continue;
            }
            break;
        }
        InitialiseQuakePickup(PickupEntry);
        PickupEntry++;
    }
}


struct gclient_s *GClientTList;
extern int MaxWindows;
extern int PalNtsc;
/* static in the original (SYM); left global here only so the relocations carry the name */
short QTimerIdentsPal[7] = { 0, 12, 24, 6, 18, -1, -1 };
short QTimerIdentsNtsc[7] = { 0, 10, 20, 5, 15, 25, -1 };

static short GetQuakeTimerIdent(void)
{
    short *IdentsTable;
    short Ret;

    if (PalNtsc == 5)
        IdentsTable = QTimerIdentsNtsc;
    else
        IdentsTable = QTimerIdentsPal;
    Ret = IdentsTable[QuakeTimerIdentIndx];
    if (Ret == -1) {
        QuakeTimerIdentIndx = 0;
        Ret = IdentsTable[0];
    } else
        QuakeTimerIdentIndx++;
    return Ret;
}

struct edict_s *FindFreeEdictT(void)
{
    int a;

    for (a = MaxWindows + 1; a < MaxEdictT; a++) {
        if (!EdictTList[a].inuse) {
            memset(&EdictTList[a], 0, sizeof(struct edict_s));
            EdictTList[a].Extra.Ident = GetQuakeTimerIdent();
            return &EdictTList[a];
        }
    }
    return 0;
}

void MallocEdictTList(short NumEdicts, short NumClients)
{
    int Size;

    Size = NumEdicts * sizeof(struct edict_s) + NumClients * sizeof(struct gclient_s);
    if (Size)
        EdictTList = MY_malloc_name(Size, "Quakes Edicts");
    else
        EdictTList = 0;
    if (EdictTList) {
        memset(EdictTList, 0, Size);
        MaxEdictT = NumEdicts;
        MaxGClientT = NumClients;
        GClientTList = (struct gclient_s *)(EdictTList + NumEdicts);
    } else {
        MaxEdictT = 0;
        MaxGClientT = 0;
        GClientTList = 0;
    }
    if (NumClients) {
        PlayerOldOrigin = MY_malloc_name(NumClients * sizeof(struct VECT), "Quakes Poldorig");
        PlayerOldMoveSpace = MY_malloc_name(NumClients * sizeof(int), "Quakes Poldmvsp");
    } else {
        PlayerOldOrigin = 0;
        PlayerOldMoveSpace = 0;
    }
}

extern struct WINDOW GameWindows[];

void InitialiseEdictTList(void)
{
    int a;

    for (a = 0; a < MaxEdictT; a++) {
        memset(&EdictTList[a], 0, sizeof(struct edict_s));
        EdictTList[a].Extra.Creature = 0;
    }
    if (MaxGClientT + 1 < MaxEdictT && MaxGClientT >= MaxWindows) {
        for (a = 1; a < MaxWindows + 1; a++) {
            EdictTList[a].Extra.Creature = GameWindows[a - 1].Creature;
            ((struct CREATURE *)GameWindows[a - 1].Creature)->EdictT = &EdictTList[a];
            EdictTList[a].viewheight = -250;
            EdictTList[a].inuse = 1;
            memset(&GClientTList[a - 1], 0, sizeof(struct gclient_s));
            EdictTList[a].client = &GClientTList[a - 1];
            EdictTList[a].takedamage = 1;
        }
    }
}

struct SPAWNDETAILS *GetSpawnDetailsFromIdentityName(struct IDENTITYNAME *Name, struct SPAWNDETAILS *start);
void PrintIdentityName(struct IDENTITYNAME Name);

void DumpCreAi(char *Code)
{
    char *CodeFileStart;
    struct SPAWNDETAILS *Details;
    struct SPAWNDETAILS *current;

    if (Code == 0)
        return;
    CodeFileStart = Code;
    while (*(int *)Code) {
        current = 0;
        while ((Details = GetSpawnDetailsFromIdentityName((struct IDENTITYNAME *)Code, current))) {
            Details->PreInitRoutine = 0;
            Details->NewAnimRemap = 0;
            Details->SpawnRoutine = 0;
            PrintIdentityName(*(struct IDENTITYNAME *)Code);
            current = Details + 1;
        }
        Code = CodeFileStart + ((int *)Code)[3];
    }
}

void GameFreeCreatureStuff(void *CreV)
{
    struct CREATURE *Cre = CreV;

    if (Cre->EdictT)
        FreeEdictT(Cre->EdictT);
}

void DoNoise(struct CREATURE *Cre, struct VECT *Location, int type)
{
    if (!LevelIsMultiPlayer && Cre->Player && Cre->EdictT)
        PlayerNoise(Cre->EdictT, Location, type);
}

void DrawPathCorners(void)
{
}

void FreeEdictT(struct edict_s *EdictT)
{
    EdictT->Extra.Creature = 0;
    EdictT->inuse = 0;
}

void QEntsEveryLevel(void)
{
    QuakeTimer = 0;
    QuakeTimerIdentIndx = 0;
}

void QuakeLogic(void)
{
    QuakeTimer += NewVcounts;
    while (QuakeTimer >= 30) {
        QuakeTimer -= 30;
        if (MaxEdictT)
            QuakeThinkGeneral();
    }
}

void QuakeSinglePlayerInit(void)
{
    InitialiseEdictTList();
    memset(&level, 0, sizeof(level));
    level.time = 0;
    level.framenum = 0;
    QuakeLevelTime = 0;
    QuakeLevelFrame = 0;
    Creatures[0].Player->TargetPitchSeek = 0;
    PlayerTrail_Init();
}

void QuakeDeadHitCollision(struct COLLBOX3D *Box)
{
    Box->HitHeight /= 4;
    Box->HitRadius = (short)(Box->HitRadius * 3) / 2;
}

void QuakeCreaturePostDraw(struct CREATURE *Cre)
{
    struct QCREINTERFACE *Iface;

    if (!(Cre->Control & 0x2000000)) {
        Iface = GetCreInterface(Cre);
        if (Iface && Iface->EveryFrameFn)
            Iface->EveryFrameFn(Cre);
    }
}

struct QCREINTERFACE *GetCreInterface(struct CREATURE *Cre)
{
    if (Cre->TypeIdent < 38)
        return QCreInterfaces[Cre->TypeIdent];
}

struct CRESAVE *LoadStoreQuakeCreGen(void *CreV, int Store, struct CRESAVE *CreSave)
{
    if (Store)
        CreSave = GeneralRespawnStuffStore(CreV, CreSave);
    else
        GeneralRespawnStuff(CreSave);
    return CreSave;
}

struct CRESAVE *LoadStoreFragment(void *CreV, int Store, struct CRESAVE *CreSave)
{
    if (Store)
        KillModel(CreV);
    return CreSave;
}

struct CRESAVE *LoadStoreQuakeDeadBody(void *CreV, int Store, struct CRESAVE *CreSave)
{
    if (Store)
        CreSave = QuakeStoreInanimate(CreV, CreSave);
    else
        RespawnQuakeDeadBody(CreSave);
    return CreSave;
}

void RememberMe(struct CREATURE *Cre)
{
    struct CRESAVE *(*Routine)();

    Routine = GetSpawnDetails(Cre->TypeIdent)->LoadStoreRoutine;
    if (Routine)
        SaveMemoryWriteAddress = Routine(Cre, 1, SaveMemoryWriteAddress);
    KillModel(Cre);
}

void DestroyMe(struct CREATURE *Cre)
{
    KillModel(Cre);
}

struct pathcorner_s *FindFreePathT(void)
{
    int a;

    for (a = 0; a < MaxPathT; a++) {
        if (!PathTList[a].inuse)
            return &PathTList[a];
    }
    return 0;
}
