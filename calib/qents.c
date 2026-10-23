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
