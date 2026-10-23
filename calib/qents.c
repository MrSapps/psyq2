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

int BodgySleep(struct CREATURE *Cre);
void UpdateEntity(struct edict_s *ent, int flags);
void G_TouchTriggers(struct edict_s *ent);
struct MATRIX *RotMatrixYXZ(struct SVECT *r, struct MATRIX *m);
void NewTrueMove3d(struct CREATURE *Cre, int ThirdPersonStuff, int FinalCall, int AlwaysStep);
void AnimEdictCre(struct CREATURE *Cre, int NewFrameBool, int Time);

/* inlined twice into PrimaryQuakeCreature (SYM shows its locals ToProcess,
   AlwaysStep and DeltaYaw as nested blocks there) */
static void QuakeCreMove(struct CREATURE *Cre, int ToProcess, int FinalCall)
{
    int AlwaysStep;
    int DeltaYaw;

    AlwaysStep = 1;
    Cre->MoveVel.vx = (Cre->IdealLocation.vx - Cre->Box.Location.vx) * ToProcess / 4096;
    Cre->MoveVel.vy = (Cre->IdealLocation.vy - Cre->Box.Location.vy) * ToProcess / 4096;
    Cre->MoveVel.vz = (Cre->IdealLocation.vz - Cre->Box.Location.vz) * ToProcess / 4096;
    DeltaYaw = ((Cre->IdealYaw - Cre->Ang.vy + 0x800) & 0xfff) - 0x800;
    Cre->Ang.vy += DeltaYaw * ToProcess / 4096;
    if (Cre->EdictT && (Cre->EdictT->flags & 3))
        AlwaysStep = 0;
    NewTrueMove3d(Cre, 0, FinalCall, AlwaysStep);
}

void PrimaryQuakeCreature(struct CREATURE *Cre)
{
    int StartQuakeTimer;
    int StopQuakeTimer;
    int NewQuakeFrame;
    int OldAnimJump;

    NewQuakeFrame = 0;
    if (Cre->Health <= 0)
        Cre->Control |= 0x8000;
    ProcessQuakeGeneralCreEffects(Cre);
    if (!(Cre->EdictT->svflags & 2)) {
        if (BodgySleep(Cre)) {
            Cre->EdictT->Extra.LevelTime = QuakeLevelTime;
            Cre->EdictT->Extra.LevelFrame = QuakeLevelFrame;
        } else {
        StartQuakeTimer = Cre->EdictT->Extra.QuakeTimer - Cre->EdictT->Extra.Ident;
        if (StartQuakeTimer < 0)
            StartQuakeTimer += 30;
        StopQuakeTimer = StartQuakeTimer + NewVcounts;
        while (StopQuakeTimer >= 30) {
            Cre->EdictT->Extra.LevelFrame++;
            Cre->EdictT->Extra.LevelTime = Cre->EdictT->Extra.LevelFrame;
            level.time = Cre->EdictT->Extra.LevelTime;
            level.framenum = Cre->EdictT->Extra.LevelFrame;
            QuakeCreMove(Cre, ((30 - StartQuakeTimer) << 12) / 30, 0);
            StartQuakeTimer = 0;
            Cre->Control &= 0x7fffffff;
            StopQuakeTimer -= 30;
            {
                long thinktime;
                struct edict_s *Self;

                Self = Cre->EdictT;
                if (Self->inuse) {
                    level.current_entity = Self;
                    UpdateEntity(Self, 1);
                    Cre->IdealLocation = Cre->Box.Location;
                    if (Self->movetype == 5)
                        G_TouchTriggers(Self);
                    thinktime = Self->nextthink;
                    if (thinktime > 0 && thinktime <= level.time) {
                        Self->nextthink = 0;
                        if (Self->think)
                            Self->think(Self);
                    }
                }
            }
            NewQuakeFrame = 1;
        }
        QuakeCreMove(Cre, ((StopQuakeTimer - StartQuakeTimer) << 12) / 30, 1);
        Cre->EdictT->Extra.QuakeTimer = Cre->EdictT->Extra.Ident + StopQuakeTimer;
        if (Cre->EdictT->Extra.QuakeTimer >= 30)
            Cre->EdictT->Extra.QuakeTimer -= 30;
        OldAnimJump = Cre->Draw.AnimJumpServer;
        AnimEdictCre(Cre, NewQuakeFrame, StopQuakeTimer);
        if (NewQuakeFrame && OldAnimJump && Cre->SequenceData2) {
            Cre->IdealYaw += Cre->SequenceData2;
            Cre->EdictT->sangles.vz += Cre->SequenceData2;
            Cre->Ang.vy += Cre->SequenceData2;
            Cre->SequenceData2 = 0;
        }
        RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
        }
    } else {
        struct edict_s *EdictT;

        ProcessQuakeDeath(Cre);
        Cre->Params[0] = Cre->EdictT->gib_health;
        Cre->Params[1] = Cre->EdictT->max_health;
        EdictT = Cre->EdictT;
        EdictT->Extra.Creature = 0;
        EdictT->inuse = 0;
        Cre->EdictT = 0;
        Cre->Primary = PrimaryQuakeDeadBody;
        QuakeDeadHitCollision(&Cre->Box);
        Cre->Control |= 0x8000;
        Cre->IdealYaw = Cre->TypeIdent;
        Cre->TypeIdent = 0x2f;
    }
}

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

void AnimEdictCre(struct CREATURE *Cre, int NewFrameBool, int Time)
{
    short NewTargetFrame;
    struct NEWANIMREMAP *Remap;

    Remap = Cre->EdictT->Extra.NewAnimRemapTable;
    if (Remap) {
        if (NewFrameBool) {
            while (Remap->First >= 0) {
                if (Cre->EdictT->s.frame >= Remap->First && Cre->EdictT->s.frame <= Remap->Last) {
                    short OldTargetFrame;
                    short First;
                    short Last;
                    int SameSequence;
                    int Wrap;

                    OldTargetFrame = Cre->Draw.TargetAnimFrame;
                    NewTargetFrame = Remap->Base + (Cre->EdictT->s.frame - Remap->First) * 30;
                    if (OldTargetFrame == NewTargetFrame)
                        Cre->Control |= 0x40;
                    else
                        Cre->Control &= ~0x40;
                    First = Remap->Base;
                    Last = First + (Remap->Last - Remap->First) * 30;
                    SameSequence = OldTargetFrame >= First && OldTargetFrame <= Last;
                    if (SameSequence)
                        Wrap = NewTargetFrame < OldTargetFrame;
                    else
                        Wrap = 0;
                    Cre->Draw.AnimJumpServer = !SameSequence || Wrap;
                    Cre->AnimFrame = OldTargetFrame;
                    Cre->Draw.TargetAnimFrame = NewTargetFrame;
                    Cre->EdictT->Extra.QuakeFrame = OldTargetFrame;
                    break;
                }
                Remap++;
            }
        }
        if (!(Cre->Control & 0x40))
            Cre->AnimFrame = Cre->EdictT->Extra.QuakeFrame + Time;
        Cre->Draw.AnimJump = Cre->Draw.AnimJumpServer && Time >= 20;
    }
}

extern int DebugWatch;
struct edict_s *G_Spawn_NoCre(void);
struct SHORTPAIR GetCreHitSize(struct CREATURE *Cre);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
void PrimaryQuakeCreature(struct CREATURE *Cre);
void QuakeCreaturePostDraw(struct CREATURE *Cre);
void CalcCreShadowPos(struct CREATURE *Cre);

int GameSpawn(struct CREATURE *Cre, struct CREDATA *CreData, struct SPAWNDETAILS *Details, struct RESPAWNDATA *RespawnData)
{
    struct edict_s *Edict;
    int Drop;

    Edict = G_Spawn_NoCre();
    if (!Edict && !LevelIsMultiPlayer) {
        KillModel(Cre);
        return 0;
    }
    if (CreData->TypeIdent == 35 || CreData->TypeIdent == 36)
        CreData->Location = EdictTList[1].pos1;
    switch (CreData->TypeIdent) {
    case 2:
    case 7:
    case 8:
    case 11:
        Drop = 0;
        break;
    default:
        Drop = 0x400;
        break;
    }
    if (!SetupBoxDrop(&Cre->Box, &CreData->Location, *&(struct SCRPNT){0x11e, 0x23c}, (struct SHORTPAIR){-1, Drop}, GetCreHitSize(Cre))) {
        DebugWatch = 0;
        KillModel(Cre);
        return 0;
    }
    Cre->Box.UseCreatureBlockers = 1;
    Cre->Draw.Location = Cre->Box.Location;
    Cre->Primary = PrimaryQuakeCreature;
    Cre->PostDraw = QuakeCreaturePostDraw;
    Cre->Control = 1;
    Cre->EdictT = Edict;
    Cre->Draw.Location.vy += 0x11e;
    Edict->Extra.LevelTime = QuakeLevelTime;
    Edict->Extra.LevelFrame = QuakeLevelFrame;
    Edict->Extra.Creature = Cre;
    Edict->inuse = 1;
    Edict->target = CreData->TargetIdent;
    Edict->spawnflags = Cre->SpawnFlags;
    Edict->sangles.vz = Cre->Ang.vy;
    Edict->ideal_yaw = Cre->Ang.vy;
    Edict->sorigin = Cre->Box.Location;
    Edict->classname = Details->ClassName;
    Edict->Extra.NewAnimRemapTable = Details->NewAnimRemap;
    if (RespawnData)
        Cre->Health = RespawnData->Health;
    else
        Cre->Health = Details->MaxHealth;
    Edict->gib_health = Details->GibHealth;
    if (Details->SpawnRoutine)
        Details->SpawnRoutine(Edict);
    CalcCreShadowPos(Cre);
    return 1;
}

extern struct SPAWNDETAILS SpawnTables[];
struct GLOBMODHDR *GetModelHdr(struct IDENTITYNAME Name);
struct ANIMHDR *FindAnimHdr(struct GLOBMODHDR *Hdr, struct IDENTITYNAME Name);

void RemapModels(void)
{
    struct SPAWNDETAILS *Details;
    struct GLOBMODHDR *Hdr;
    struct NEWANIMREMAP *Remap;
    struct ANIMHDR *AnimHdr;

    for (Details = SpawnTables; Details->TypeIdent; Details++) {
        Hdr = GetModelHdr(Details->ModelName);
        Details->Hdr = Hdr;
        if (Hdr) {
            Remap = Details->NewAnimRemap;
            if (Remap) {
                while (Remap->First != -1) {
                    AnimHdr = FindAnimHdr(Hdr, Remap->AnimName);
                    if (AnimHdr) {
                        if (Remap->Offset == -1)
                            Remap->Base = AnimHdr->AnimDetails.Wrap;
                        else
                            Remap->Base = AnimHdr->AnimDetails.Start + Remap->Offset;
                    } else {
                        Remap->Base = 0;
                        PrintIdentityName(Remap->AnimName);
                    }
                    Remap++;
                }
            }
            if (Details->PreInitRoutine)
                Details->PreInitRoutine(Hdr);
        } else if (Details->PreInitRoutine)
            Details->PreInitRoutine(0);
    }
}

/* defined in this file (qents.obj), so $gp-relative */
extern struct cvar_s skillstore;
struct cvar_s *skill = &skillstore;
extern struct trace_t Trace;
extern unsigned int usp;
void __SN_RELOCATE(unsigned int baseaddr, unsigned int *relocs);
int printf();
int rand();
void linkentity();
int SoundByCreature();
int GetSoundNum();
void RotateS();
struct SVECT GetSubModXYZ();
struct MODSUBOFF GetModSubOffFromVertex();
int GetModSubOffFrom2Verteces();
void GetAbsoluteModelVert();
int GetSoundClone();
int SoundTestIfRunning();
void AlterSampleAttrs();
void ai_run();
void ai_walk();
void ai_stand();
void ai_move();
void ai_charge();
struct CREATURE *SpawnGeneral();
int VertexAttack();
int AddBlurPoints();
int AnyoneFireSplurge();
void monster_fire_blaster();
void monster_fire_bullet();
void monster_fire_shotgun();
void monster_fire_railgun();
void monster_fire_bfg();
void monster_fire_grenade();
void monster_fire_rocket();
void monster_fire_laser();
void MuzzleFlash();
struct CREATURE *CreFindRadius();
int AreaHidden();
void FoundTarget();
void HuntTarget();
int range();
long VectorLength();
long VectorLength2();
void VectorMA();
long VectorNormalize();
void vectoangles();
long vectoyaw();
int infront();
long anglemod();
void AngleVectors();
void G_ProjectSourceNM();
void CalcAbsFromCreBox();
int visible();
int M_CheckAttack();
int fire_hit();
void DoDamage();
void gitrace();
void walkmonster_start();
void flymonster_start();
void swimmonster_start();
int getmaxsvx();
int getmaxsvy();
int getminsvy();
int getminsvx();
void SetMyAiTable();
void QuakeGenExplosion();
unsigned long SetSp();
void SetShockData();

void LoadRelocateCreAi(char *Code, char *Rel)
{
    struct QCREINTERFACE *Iface;
    struct SPAWNDETAILS *Details;
    char *CodeFileStart;
    char *RelFileStart;
    char *RelocData;

    CodeFileStart = Code;
    RelFileStart = Rel;
    while (*(int *)Code) {
        Details = GetSpawnDetailsFromIdentityName((struct IDENTITYNAME *)Code, 0);
        if (Details) {
            Iface = (struct QCREINTERFACE *)(Code + 16);
            RelocData = Rel + 16;
            Rel = RelFileStart + ((int *)Rel)[3];
            __SN_RELOCATE((unsigned int)Iface, (unsigned int *)RelocData);
            Iface->StructSize = sizeof(struct QCREINTERFACE);
            Iface->Version = 1;
            Iface->rand = rand;
            /* PrintF is set twice; flow drops the first store, and the two uses of
               printf are what make loop.c hoist its address out of the loop */
            Iface->PrintF = printf;
            Iface->GetSoundNum = GetSoundNum;
            Iface->linkentity = linkentity;
            Iface->SoundByCreature = SoundByCreature;
            Iface->RotateS = RotateS;
            Iface->GetSubModXYZ = GetSubModXYZ;
            Iface->GetModSubOffFromVertex = GetModSubOffFromVertex;
            Iface->GetModSubOffFrom2Verteces = GetModSubOffFrom2Verteces;
            Iface->GetAbsoluteModelVert = GetAbsoluteModelVert;
            Iface->GetSoundClone = GetSoundClone;
            Iface->SoundTestIfRunning = SoundTestIfRunning;
            Iface->AlterSampleAttrs = AlterSampleAttrs;
            Iface->level = &level;
            Iface->skill = &skill;
            Iface->Creatures = Creatures;
            Iface->ai_run = ai_run;
            Iface->ai_walk = ai_walk;
            Iface->ai_stand = ai_stand;
            Iface->ai_charge = ai_charge;
            Iface->ai_move = ai_move;
            Iface->SpawnGeneral = SpawnGeneral;
            Iface->KillModel = KillModel;
            Iface->VertexAttack = VertexAttack;
            Iface->FindAnimHdr = FindAnimHdr;
            Iface->AddBlurPoints = AddBlurPoints;
            Iface->AnyoneFireSplurge = AnyoneFireSplurge;
            Iface->monster_fire_blaster = monster_fire_blaster;
            Iface->monster_fire_bullet = monster_fire_bullet;
            Iface->monster_fire_shotgun = monster_fire_shotgun;
            Iface->monster_fire_railgun = monster_fire_railgun;
            Iface->monster_fire_bfg = monster_fire_bfg;
            Iface->PrintF = printf;
            Iface->monster_fire_grenade = monster_fire_grenade;
            Iface->monster_fire_rocket = monster_fire_rocket;
            Iface->monster_fire_laser = monster_fire_laser;
            Iface->MuzzleFlash = MuzzleFlash;
            Iface->CreFindRadius = CreFindRadius;
            Iface->AreaHidden = AreaHidden;
            Iface->FoundTarget = FoundTarget;
            Iface->HuntTarget = HuntTarget;
            Iface->range = range;
            Iface->VectorLength = VectorLength;
            Iface->VectorLength2 = VectorLength2;
            Iface->VectorMA = VectorMA;
            Iface->VectorNormalize = VectorNormalize;
            Iface->vectoangles = vectoangles;
            Iface->vectoyaw = vectoyaw;
            Iface->infront = infront;
            Iface->anglemod = anglemod;
            Iface->AngleVectors = AngleVectors;
            Iface->G_ProjectSourceNM = G_ProjectSourceNM;
            Iface->CalcAbsFromCreBox = CalcAbsFromCreBox;
            Iface->visible = visible;
            Iface->M_CheckAttack = M_CheckAttack;
            Iface->fire_hit = fire_hit;
            Iface->DoDamage = DoDamage;
            Iface->gitrace = gitrace;
            Iface->Trace = &Trace;
            Iface->walkmonster_start = walkmonster_start;
            Iface->flymonster_start = flymonster_start;
            Iface->swimmonster_start = swimmonster_start;
            Iface->getmaxsvx = getmaxsvx;
            Iface->getmaxsvy = getmaxsvy;
            Iface->getminsvy = getminsvy;
            Iface->getminsvx = getminsvx;
            Iface->SetMyAiTable = SetMyAiTable;
            Iface->PrimaryQuakeDeadBody = PrimaryQuakeDeadBody;
            Iface->QuakeGenExplosion = QuakeGenExplosion;
            Iface->SetSp = SetSp;
            Iface->usp = &usp;
            Iface->SetShockData = SetShockData;
            do {
                QCreInterfaces[Details->TypeIdent] = Iface;
                Details->PreInitRoutine = Iface->PreInitFn;
                Details->NewAnimRemap = Iface->NewAnimRemap;
                Details->SpawnRoutine = Iface->SpawnFn;
                Details = GetSpawnDetailsFromIdentityName((struct IDENTITYNAME *)Code, Details + 1);
            } while (Details);
            Code = CodeFileStart + ((int *)Code)[3];
        } else {
            Code = CodeFileStart + ((int *)Code)[3];
            PrintIdentityName(*(struct IDENTITYNAME *)Code);
            while (1)
                ;
        }
    }
}

/* splat names the zero SVECT this passes (an anonymous constant in the original,
   laid out in .sdata right after coop) coop+4. coop is declared as an array of
   unknown size so that gcc splits the %hi/%lo itself, as for a local label */
extern char coop[];

void PrimaryQuakeDeadBody(struct CREATURE *Cre)
{
    if (EffectyDeath(Cre))
        return;
    ProcessQuakeGeneralCreEffects(Cre);
    AddCreToNextBumpList(Cre);
    if (Cre->Health <= Cre->Params[0])
        QuakeCreatureSmash(Cre, (struct SVECT *)(coop + 4));
}

extern int GlobalKillsCount;
extern int GlobalKills;
extern char GameDeathWatchA;
extern char GameDeathWatchB;
extern char GameDeathWatchC;
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
void AddKillInterMission(char *String);

void ProcessQuakeDeath(struct CREATURE *Cre)
{
    short InformDeathFlags;
    short Type;

    InformDeathFlags = Cre->SpawnFlags & 0x600;
    Type = Cre->TypeIdent;
    if (Type == 0x2f)
        Type = Cre->IdealYaw;
    if (Type != 0x22 && GlobalKills < GlobalKillsCount)
        GlobalKills++;
    AddKillInterMission(GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    if (InformDeathFlags == 0x200 && GameDeathWatchA)
        GameDeathWatchA--;
    if (InformDeathFlags == 0x400 && GameDeathWatchB)
        GameDeathWatchB--;
    if (InformDeathFlags == 0x600 && GameDeathWatchC)
        GameDeathWatchC--;
}

struct CREATURE *SpawnGeneral(struct CREDATA *CreData, struct RESPAWNDATA *RespawnData);

/* SYM shows its RespawnData pointer as a nested block in GeneralRespawnStuff;
   too big for -O3's automatic inlining, so it was declared inline */
static __inline__ void UnpackCreSave(struct CRESAVE *CreSave, struct RESPAWNDATA *RespawnData)
{
    if (CreSave->u.Packed.Packed) {
        RespawnData->SimStart.Location.vx = CreSave->u.Packed.Location.vx;
        RespawnData->SimStart.Location.vy = CreSave->u.Packed.Location.vy;
        RespawnData->SimStart.Location.vz = CreSave->u.Packed.Location.vz;
        RespawnData->SimStart.AngY = CreSave->u.Packed.Ang << 6;
        RespawnData->SimStart.Ident = CreSave->u.Packed.TypeIdent;
        RespawnData->Target = CreSave->u.Packed.Target;
        RespawnData->Health = (CreSave->u.Packed.Health << 24) >> 22;
        RespawnData->SpawnFlags = CreSave->u.Packed.SpawnFlags;
        RespawnData->Param0 = CreSave->u.Packed.Param;
        RespawnData->Param1 = 0;
    } else {
        RespawnData->SimStart.AngY = CreSave->u.UnPacked.Ang << 6;
        RespawnData->SimStart.Location = CreSave->u.UnPacked.Location;
        RespawnData->SimStart.Ident = CreSave->u.UnPacked.TypeIdent;
        RespawnData->Target = CreSave->u.UnPacked.Target;
        RespawnData->Health = CreSave->u.UnPacked.Health;
        RespawnData->SpawnFlags = CreSave->u.UnPacked.SpawnFlags;
        RespawnData->Param0 = CreSave->u.UnPacked.Param0;
        RespawnData->Param1 = CreSave->u.UnPacked.Param1;
    }
}

struct CREATURE *GeneralRespawnStuff(struct CRESAVE *CreSave)
{
    struct CREATURE *Cre;
    struct RESPAWNDATA RespawnData;
    struct CREDATA CreData;

    UnpackCreSave(CreSave, &RespawnData);
    CreData.TypeIdent = RespawnData.SimStart.Ident;
    CreData.AngY = RespawnData.SimStart.AngY;
    CreData.TargetIdent = RespawnData.Target;
    CreData.SpawnFlags = RespawnData.SpawnFlags;
    CreData.Location = RespawnData.SimStart.Location;
    Cre = SpawnGeneral(&CreData, &RespawnData);
    if (Cre)
        Cre->Health = RespawnData.Health;
    return Cre;
}

struct CREATURE *GetNextCre(int UseExpendable);
int GetFloorOffset(struct CREATURE *Cre);
int GetCeilingOffset(struct CREATURE *Cre);
int LastFrameFromSequenceNum(struct ANIMFRAMEHDRHDR *FrameHdrHdr, int SequenceNum);

struct CREATURE *RespawnQuakeDeadBody(struct CRESAVE *CreSave)
{
    struct RESPAWNDATA RespawnData;

    UnpackCreSave(CreSave, &RespawnData);
    {
        struct CREATURE *Cre;
        struct SPAWNDETAILS *Details;

        Cre = GetNextCre(1);
        if (Cre) {
            Details = GetSpawnDetails(RespawnData.Target);
            if (!Details)
                return 0;
            Cre->Primary = PrimaryQuakeDeadBody;
            Cre->ModelHdr = Details->Hdr;
            Cre->IdealYaw = RespawnData.Target;
            Cre->TypeIdent = 0x2f;
            Cre->Control = 0x8001;
            Cre->HeightOffset = GetFloorOffset(Cre);
            Cre->TotalHeight = Cre->HeightOffset + GetCeilingOffset(Cre);
            Cre->Ang.vy = RespawnData.SimStart.AngY;
            Cre->AnimFrame = LastFrameFromSequenceNum(Cre->ModelHdr->AnimData, RespawnData.Param0);
            RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
            if (!SetupBoxDrop(&Cre->Box, &RespawnData.SimStart.Location, *&(struct SCRPNT){0x11e, 0x23c}, *&(struct SHORTPAIR){-1, 0}, GetCreHitSize(Cre))) {
                KillModel(Cre);
                return 0;
            }
            QuakeDeadHitCollision(&Cre->Box);
            Cre->Draw.Location = Cre->Box.Location;
            Cre->Draw.Location.vy += 0x11e;
            Cre->Draw.Location.vy -= Cre->HeightOffset;
            Cre->Health = RespawnData.Health;
            Cre->Params[0] = Details->GibHealth;
            Cre->Params[1] = Details->MaxHealth;
            CalcCreShadowPos(Cre);
        }
    }
    return 0;
}

extern int MaxWindows;
void AI_SetSightClient(void);
struct edict_s *PlayerTrail_LastSpot(void);
int visible(struct edict_s *self, struct edict_s *other);
void PlayerTrail_Add(struct VECT spot, int space);
struct VECTBOX *PointInCreatureBlocker(struct VECT *point, int space);
int SnapPointToBoxFace(struct VECT *point, struct VECTBOX *Box, struct VECT *snapped_point, int space);

/* SYM shows Self and Box as nested blocks in QuakeThinkGeneral */
static __inline__ void PlayerEndFrame(struct edict_s *Self)
{
    struct VECTBOX *Box;

    if (!visible(Self, PlayerTrail_LastSpot()))
        PlayerTrail_Add(PlayerOldOrigin[Self - EdictTList - 1], PlayerOldMoveSpace[Self - EdictTList - 1]);
    Box = PointInCreatureBlocker(&Self->sorigin, ((struct CREATURE *)Self->Extra.Creature)->Box.MoveSpace);
    if (Box)
        SnapPointToBoxFace(&Self->sorigin, Box, &Self->client->corrected_sorigin, ((struct CREATURE *)Self->Extra.Creature)->Box.MoveSpace);
    else {
        Self->client->corrected_sorigin.vx = Self->sorigin.vx;
        Self->client->corrected_sorigin.vy = Self->sorigin.vy;
        Self->client->corrected_sorigin.vz = Self->sorigin.vz;
    }
    PlayerOldOrigin[Self - EdictTList - 1].vx = Self->sorigin.vx;
    PlayerOldOrigin[Self - EdictTList - 1].vy = Self->sorigin.vy;
    PlayerOldOrigin[Self - EdictTList - 1].vz = Self->sorigin.vz;
    PlayerOldMoveSpace[Self - EdictTList - 1] = ((struct CREATURE *)Self->Extra.Creature)->Box.MoveSpace;
}

void QuakeThinkGeneral(void)
{
    int a;
    struct edict_s *Player;

    QuakeLevelFrame++;
    QuakeLevelTime = QuakeLevelFrame;
    level.framenum = QuakeLevelFrame;
    level.time = QuakeLevelFrame;
    AI_SetSightClient();
    for (a = 0, Player = EdictTList + 1; a < MaxWindows; a++, Player++) {
        if (Player->inuse) {
            level.current_entity = Player;
            UpdateEntity(Player, 1);
            PlayerEndFrame(Player);
        }
    }
}

/* inlined twice into GeneralRespawnStuffStore (its two `self` blocks) */
static int GetEdictTarget(struct edict_s *self)
{
    if (!self->target) {
        if (self->movetarget && self->movetarget->classname == 0x72)
            return self->movetarget->targetname;
        return 0;
    }
    return self->target;
}

struct CRESAVE *GeneralRespawnStuffStore(struct CREATURE *Cre, struct CRESAVE *CreSave)
{
    struct edict_s *Edict;
    int Packed;
    short SpawnFlags;

    Edict = Cre->EdictT;
    if (!Edict)
        return CreSave;
    Packed = 0;
    if (!((Cre->Box.Location.vx + 0x8000) & 0xffff0000) && !((Cre->Box.Location.vy + 0x8000) & 0xffff0000)
        && !((Cre->Box.Location.vz + 0x8000) & 0xffff0000) && !((Cre->Health / 4 + 0x80) & 0xff00))
        Packed = 1;
    CreSave->u.Packed.Packed = Packed;
    SpawnFlags = Edict->spawnflags | (Cre->SpawnFlags & 0x600);
    if (Packed) {
        CreSave->u.Packed.Location.vx = Cre->Box.Location.vx;
        CreSave->u.Packed.Location.vy = Cre->Box.Location.vy + 0x11e;
        CreSave->u.Packed.Location.vz = Cre->Box.Location.vz;
        CreSave->u.Packed.Ang = (Cre->Ang.vy & 0xfff) >> 6;
        CreSave->u.Packed.TypeIdent = Cre->TypeIdent;
        CreSave->u.Packed.Health = Cre->Health / 4;
        CreSave->u.Packed.Target = GetEdictTarget(Edict);
        CreSave->u.Packed.SpawnFlags = SpawnFlags;
    } else {
        CreSave->u.UnPacked.Location.vx = Cre->Box.Location.vx;
        CreSave->u.UnPacked.Location.vy = Cre->Box.Location.vy + 0x11e;
        CreSave->u.UnPacked.Location.vz = Cre->Box.Location.vz;
        CreSave->u.UnPacked.Ang = (Cre->Ang.vy & 0xfff) >> 6;
        CreSave->u.UnPacked.TypeIdent = Cre->TypeIdent;
        CreSave->u.UnPacked.Health = Cre->Health;
        CreSave->u.UnPacked.Target = GetEdictTarget(Edict);
        CreSave->u.UnPacked.SpawnFlags = SpawnFlags;
    }
    {
        unsigned char *Addr;

        Addr = (unsigned char *)CreSave;
        if (Packed)
            Addr += sizeof(struct CRESAVEPACKED);
        else
            Addr += sizeof(struct CRESAVEUNPACKED);
        return (struct CRESAVE *)Addr;
    }
}

void QuakeBlowMeUp(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName);
void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type);
void QuakeBlowMeUpMetal(struct CREATURE *Cre, int NumBits, struct SVECT *Force, struct IDENTITYNAME *BigBitName, int iNumberIndex, int iStart, int iEnd);

void QuakeCreatureSmash(struct CREATURE *Cre, struct SVECT *Force)
{
    short Type;
    struct SVECT Vel;

    Type = Cre->TypeIdent;
    if (Type == 0x2f)
        Type = Cre->IdealYaw;
    switch (Type) {
    case 2:
    case 8:
    case 11:
        QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x800, 0);
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeBlowMeUpMetal(Cre, 16, &Vel, (struct IDENTITYNAME *)"Bos2Gib1", 7, 1, 8);
        break;
    case 24:
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeBlowMeUpMetal(Cre, 16, &Vel, (struct IDENTITYNAME *)"Bos1Gib1", 7, 1, 8);
        break;
    case 32:
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeGenExplosion(&Cre->Box.Location, Cre->Box.Area, 0x800, 0);
        QuakeBlowMeUpMetal(Cre, 16, &Vel, (struct IDENTITYNAME *)"StridGib1", 8, 1, 7);
        break;
    case 13:
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeBlowMeUpMetal(Cre, 16, &Vel, (struct IDENTITYNAME *)"JorgGib1", 7, 1, 9);
        break;
    case 14:
    case 37:
        break;
    case 17:
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeBlowMeUp(Cre, 5, &Vel, 0);
        break;
    default:
        Vel.vx = Cre->Vel.vx + Cre->ForceVel.vx;
        Vel.vy = Cre->Vel.vy + Cre->ForceVel.vy;
        Vel.vz = Cre->Vel.vz + Cre->ForceVel.vz;
        QuakeBlowMeUp(Cre, 5, &Vel, (struct IDENTITYNAME *)"Chest");
        break;
    }
    KillModel(Cre);
}

/* defined last so that RespawnQuakeDeadBody calls it rather than inlining it */
void QuakeDeadHitCollision(struct COLLBOX3D *Box)
{
    Box->HitHeight /= 4;
    Box->HitRadius = (short)(Box->HitRadius * 3) / 2;
}
