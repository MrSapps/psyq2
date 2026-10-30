/* Lab: the laboratory level (mission 4). Two lasers can be switched off for a while, and
 * picking up the head wakes the dead commander. */
#include "ovl.h"

#define Engine LevelInterface.Engine
#define IDNAME(s) ((struct IDENTITYNAME){s})

void InitLevel(void);
struct QLEVELINTERFACE LevelInterface OVL_HEADER = { InitLevel, 0, 0 };

/* small functions: gcc emits them at the end of the file in this order */
struct CREATURE *FindCreature(struct IDENTITYNAME Name);
static void Laser0Off(void);
static void Laser1Off(void);

static void Zone0Load(void);
static void Zone1Load(void);
static void Zone2Load(void);
static void Zone3Load(void);
static void Zone4Load(void);

static struct IDENTITYNAME LiftNames[2] = { { "Lift A" }, { "Lift C" } };

/* the two lasers in zone 1; their ends wobble while they are switched off */
static struct VECT Laser0Start = { -1521, -1665, 4062 };
static struct VECT Laser0End = { -1511, -619, 3441 };
static struct VECT Laser1Start = { 2382, -1670, 5041 };
static struct VECT Laser1End = { 1765, -639, 5022 };
static int Laser0Timer = 0;
static int Laser1Timer = 0;
static short Laser0Space = -1;
static short Laser1Space = -1;
static short Laser0Area = -1;
static short Laser1Area = -1;

static struct CREATURE *FoundCre;
static int DeadCommDone;
static int Unused0, Unused1, Unused2; /* bss the original has but no code uses */
static struct GRAVDATA *MissionData;

/* put the lifts back where the saved game left them */
static void RestoreLifts(void)
{
    int i;
    struct SVECT Unused; /* the original's frame has 8 bytes of unused locals */

    for (i = 0; i < 2; i++) {
        MissionData->LabLifts[i].Object = Engine->GetSceneNum(LiftNames[i]);
        MissionData->LabLifts[i].MoveBuf = 0;
        if (MissionData->LabLifts[i].Object != -1 && Engine->Scene2Data[MissionData->LabLifts[i].Object].MoveBufId)
            MissionData->LabLifts[i].MoveBuf = Engine->BrushMoveBufs + Engine->Scene2Data[MissionData->LabLifts[i].Object].MoveBufId - 1;
        if (MissionData->LabLifts[i].MoveBuf && MissionData->LabLifts[i].ValidData) {
            MissionData->LabLifts[i].MoveBuf->Time = MissionData->LabLifts[i].Time;
            MissionData->LabLifts[i].MoveBuf->Offset.vy = MissionData->LabLifts[i].OffsetY;
            MissionData->LabLifts[i].MoveBuf->OpenFlag = MissionData->LabLifts[i].OpenFlag;
            if (MissionData->LabLifts[i].MoveBuf->Link)
                MissionData->LabLifts[i].MoveBuf->Link->Offset.vy = MissionData->LabLifts[i].OffsetY;
            if (MissionData->LabLifts[i].MoveBuf->SolidBox) {
                MissionData->LabLifts[i].MoveBuf->SolidBox->MinMax = MissionData->LabLifts[i].SolidMinMax;
                MissionData->LabLifts[i].MoveBuf->SolidBox->CMinMax = MissionData->LabLifts[i].SolidCMinMax;
                MissionData->LabLifts[i].MoveBuf->SolidBox->Moved = MissionData->LabLifts[i].SolidMoved;
            }
        }
    }
}

/* remember where the lifts are for the saved game */
static void StoreLifts(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (MissionData->LabLifts[i].MoveBuf) {
            MissionData->LabLifts[i].ValidData = 1;
            MissionData->LabLifts[i].OpenFlag = MissionData->LabLifts[i].MoveBuf->OpenFlag;
            MissionData->LabLifts[i].OffsetY = MissionData->LabLifts[i].MoveBuf->Offset.vy;
            MissionData->LabLifts[i].Time = MissionData->LabLifts[i].MoveBuf->Time;
            if (MissionData->LabLifts[i].MoveBuf->SolidBox) {
                MissionData->LabLifts[i].SolidMinMax = MissionData->LabLifts[i].MoveBuf->SolidBox->MinMax;
                MissionData->LabLifts[i].SolidCMinMax = MissionData->LabLifts[i].MoveBuf->SolidBox->CMinMax;
                MissionData->LabLifts[i].SolidMoved = MissionData->LabLifts[i].MoveBuf->SolidBox->Moved;
            }
        }
    }
}

static void Zone0Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone0"));
    RestoreLifts();
}

static void Zone1Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone1"));
    Laser0Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser0Start, -1, 1);
    Laser1Space = Engine->Find3dSpaceIndxLink(Engine->CollMap, &Laser1Start, -1, 1);
    Laser0Area = Engine->CollMap->Coll3dSpace[Laser0Space].Area;
    Laser1Area = Engine->CollMap->Coll3dSpace[Laser1Space].Area;
    RestoreLifts();
}

static void Zone2Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone2"));
    RestoreLifts();
}

static void Zone3Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone3"));
    RestoreLifts();
}

static void Zone4Load(void)
{
    Engine->ActivateCreatureBatch(IDNAME("Zone4"));
    RestoreLifts();
}

void *ZoneLoadTable[] = { Zone0Load, Zone1Load, Zone2Load, Zone3Load, Zone4Load };

/* NONMATCHING: FindCreature and CheckGotHead (which inlines it) are the original's code (the
 * same problem as level_boss1's FindCreature/CheckNearMakron). In the C below gcc's loop pass
 * hoists %hi(FoundCre) into a copy of the register that set it, which shifts every register in
 * the loop, and reorg fills the loop branch's delay slot with "return 0" instead of filling the
 * found branch with "return FoundCre". Tried (scripted, ~2000 variants): for / for with
 * separate init / while / do loops, goto Found vs return inside the loop, a Cre local, both
 * operand orders, &x->ModelName vs ModelName.Name casts, every declaration order, a goto loop,
 * an unreferenced label before "return 0". */
#ifdef NONMATCHING
struct CREATURE *FindCreature(struct IDENTITYNAME Name)
{
    int i;
    int *a, *b;

    a = (int *)&Name;
    for (i = 0, FoundCre = Engine->Creatures; i < 48; i++, FoundCre++) {
        b = (int *)&FoundCre->ModelHdr->ModelName;
        if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2])
            goto Found;
    }
    return 0;
Found:
    return FoundCre;
}

/* once the head has been picked up, wake the dead commander */
void CheckGotHead(void)
{
    struct CREATURE *Cre;

    if (!DeadCommDone && Engine->Event_GetDisableNamed(IDNAME("E_GotHead"))) {
        DeadCommDone = 1;
        Cre = FindCreature(IDNAME("DeadComm"));
        if (Cre && Cre->EdictT)
            Cre->EdictT->use(Cre->EdictT, 0, 0);
    }
}

#else
struct CREATURE *FindCreature(struct IDENTITYNAME Name);
void CheckGotHead(void);
__asm__(
    "\t.rdata\n"
    "\t.align\t2\n"
    "GotHeadName:\n"
    "\t.ascii\t\"E_GotHead\\000\"\n"
    "\t.space\t2\n"
    "\t.align\t2\n"
    "DeadCommName:\n"
    "\t.ascii\t\"DeadComm\\000\"\n"
    "\t.space\t3\n"
    "\t.text\n"
    "\t.align\t2\n"
    "\t.globl\tCheckGotHead\n"
    "CheckGotHead:\n"
    "\t.set\tnoreorder\n"
    "\t.set\tnomacro\n"
    "\taddu\t$29,$29,-48\n"
    "\tsw\t$17,36($29)\n"
    "\tlui\t$17,%hi(DeadCommDone)\n"
    "\tlw\t$2,%lo(DeadCommDone)($17)\n"
    "\tsw\t$31,40($29)\n"
    "\tbne\t$2,$0,GH_b38\n"
    "\tsw\t$16,32($29)\n"
    "\tlui\t$5,0xff\n"
    "\tlui\t$2,%hi(GotHeadName)\n"
    "\taddu\t$8,$2,%lo(GotHeadName)\n"
    "\tori\t$5,$5,0xffff\n"
    "\tlbu\t$3,1($8)\n"
    "\tlbu\t$7,%lo(GotHeadName)($2)\n"
    "\tlbu\t$2,2($8)\n"
    "\tlbu\t$4,4($8)\n"
    "\tlbu\t$6,8($8)\n"
    "\tsll\t$3,$3,0x8\n"
    "\tor\t$7,$7,$3\n"
    "\tsll\t$2,$2,0x10\n"
    "\tor\t$7,$7,$2\n"
    "\tlbu\t$3,5($8)\n"
    "\tlbu\t$2,6($8)\n"
    "\tsll\t$3,$3,0x8\n"
    "\tor\t$9,$4,$3\n"
    "\tsll\t$2,$2,0x10\n"
    "\tor\t$9,$9,$2\n"
    "\tand\t$9,$9,$5\n"
    "\tlbu\t$3,9($8)\n"
    "\tlbu\t$2,10($8)\n"
    "\tlbu\t$4,3($8)\n"
    "\tsll\t$3,$3,0x8\n"
    "\tor\t$6,$6,$3\n"
    "\tsll\t$2,$2,0x10\n"
    "\tor\t$6,$6,$2\n"
    "\tand\t$6,$6,$5\n"
    "\tsll\t$4,$4,0x18\n"
    "\tor\t$4,$7,$4\n"
    "\tlui\t$2,%hi(LevelInterface)\n"
    "\taddu\t$16,$2,%lo(LevelInterface)\n"
    "\tlbu\t$5,7($8)\n"
    "\tlbu\t$3,11($8)\n"
    "\tlw\t$2,8($16)\n"
    "\tsll\t$5,$5,0x18\n"
    "\tor\t$5,$9,$5\n"
    "\tsll\t$3,$3,0x18\n"
    "\tlw\t$2,164($2)\n"
    "\tnop\n"
    "\tjal\t$31,$2\n"
    "\tor\t$6,$6,$3\n"
    "\tbeq\t$2,$0,GH_b38\n"
    "\taddu\t$7,$29,16\n"
    "\tmove\t$6,$0\n"
    "\tlui\t$2,%hi(FoundCre)\n"
    "\tmove\t$8,$2\n"
    "\tlw\t$3,8($16)\n"
    "\tlui\t$2,%hi(DeadCommName)\n"
    "\taddu\t$13,$2,%lo(DeadCommName)\n"
    "\tlwl\t$10,3($13)\n"
    "\tlwr\t$10,0($13)\n"
    "\tlwl\t$11,7($13)\n"
    "\tlwr\t$11,4($13)\n"
    "\tlwl\t$12,11($13)\n"
    "\tlwr\t$12,8($13)\n"
    "\tswl\t$10,19($29)\n"
    "\tswr\t$10,16($29)\n"
    "\tswl\t$11,23($29)\n"
    "\tswr\t$11,20($29)\n"
    "\tswl\t$12,27($29)\n"
    "\tswr\t$12,24($29)\n"
    "\tlw\t$3,224($3)\n"
    "\tli\t$2,1\n"
    "\tsw\t$2,%lo(DeadCommDone)($17)\n"
    "\tsw\t$3,%lo(FoundCre)($8)\n"
    "GH_aac:\n"
    "\tlw\t$5,%lo(FoundCre)($8)\n"
    "\tnop\n"
    "\tlw\t$2,16($5)\n"
    "\tlw\t$4,0($7)\n"
    "\tlw\t$3,8($2)\n"
    "\tnop\n"
    "\tbne\t$4,$3,GH_af4\n"
    "\taddu\t$2,$2,8\n"
    "\tlw\t$4,4($7)\n"
    "\tlw\t$3,4($2)\n"
    "\tnop\n"
    "\tbne\t$4,$3,GH_af4\n"
    "\tnop\n"
    "\tlw\t$4,8($7)\n"
    "\tlw\t$3,8($2)\n"
    "\tnop\n"
    "\tbeq\t$4,$3,GH_b10\n"
    "\tmove\t$4,$5\n"
    "GH_af4:\n"
    "\tlw\t$2,%lo(FoundCre)($8)\n"
    "\taddu\t$6,$6,1\n"
    "\taddu\t$2,$2,768\n"
    "\tsw\t$2,%lo(FoundCre)($8)\n"
    "\tslt\t$2,$6,48\n"
    "\tbne\t$2,$0,GH_aac\n"
    "\tmove\t$4,$0\n"
    "GH_b10:\n"
    "\tbeq\t$4,$0,GH_b38\n"
    "\tnop\n"
    "\tlw\t$4,748($4)\n"
    "\tnop\n"
    "\tbeq\t$4,$0,GH_b38\n"
    "\tmove\t$5,$0\n"
    "\tlw\t$2,156($4)\n"
    "\tnop\n"
    "\tjal\t$31,$2\n"
    "\tmove\t$6,$5\n"
    "GH_b38:\n"
    "\tlw\t$31,40($29)\n"
    "\tlw\t$17,36($29)\n"
    "\tlw\t$16,32($29)\n"
    "\tj\t$31\n"
    "\taddu\t$29,$29,48\n"
    "\t.set\tmacro\n"
    "\t.set\treorder\n");
#endif

static void LevelSpecific(void)
{
    struct SHORTPAIR Parts;
    struct VECT End;
    struct SVECT Vel[15];
    int Vcount;
    short i;

    Vcount = *Engine->NewVcounts;
    Parts.a = Parts.b = 0;
    for (i = 0; i < 15; i++) {
        Vel[i].vx = (Engine->rand() - 16384) >> 8;
        Vel[i].vz = (Engine->rand() - 16384) >> 8;
        Vel[i].vy = -Engine->rand() >> 13;
    }
    if (Laser0Timer > 0) {
        Laser0Timer -= Vcount * 2;
        if (Laser0Space >= 0) {
            End = Laser0End;
            End.vx += Engine->rcossin_tbl[Laser0Timer & 0xfff].sin * 400 / 4096;
            End.vz += Engine->rcossin_tbl[Laser0Timer * 8 & 0xfff].sin * 100 / 4096;
            if (!Engine->AddLaser(&Laser0Start, &End, Laser0Space, 1, Parts))
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 10, 10000, Laser0Area);
            End.vy -= 50;
            Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                             *&(struct SHORTPAIR){ 1, 3 });
            Engine->AddLight(&Laser0Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                             *&(struct SHORTPAIR){ 1, 3 });
        }
    }
    if (Laser1Timer > 0) {
        Laser1Timer -= Vcount * 2;
        if (Laser1Space >= 0) {
            End = Laser1End;
            End.vz += Engine->rcossin_tbl[Laser1Timer & 0xfff].sin * 400 / 4096;
            End.vx += Engine->rcossin_tbl[Laser1Timer * 8 & 0xfff].sin * 100 / 4096;
            if (!Engine->AddLaser(&Laser1Start, &End, Laser1Space, 1, Parts))
                Engine->AddParticlesSL(&End, Vel, *Engine->ParticleColoursTable, *Engine->ParticleColoursTable, 15, 10, 10000, Laser1Area);
            End.vy -= 50;
            Engine->AddLight(&End, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 100 / 32768 + 250, Engine->rand() * 200 / 32768 + 400 },
                             *&(struct SHORTPAIR){ 1, 3 });
            Engine->AddLight(&Laser1Start, *&(struct COLOUR){ 0x40, 0x20, 0xff, 0 },
                             (struct POWER){ Engine->rand() * 200 / 32768 + 350, Engine->rand() * 200 / 32768 + 600 },
                             *&(struct SHORTPAIR){ 1, 3 });
        }
    }
    CheckGotHead();
    StoreLifts();
}

static struct MISSIONEVENT LevelEvents[] = {
    { { "Laser0" }, Laser0Off },
    { { "Laser1" }, Laser1Off },
    { { "" }, 0 },
};

void InitLevel(void)
{
    if (Engine->SizeOf != sizeof(struct QENGINELEVINTERFACE)) {
        for (;;)
            Engine->PrintF("Please get a programmer to recompile level %s\n",
                Engine->PrintIdentityName(*Engine->ThisLevelName));
        Engine->PrintF("Size In Level %d, Size In Engine %d\n", sizeof(struct QENGINELEVINTERFACE), Engine->SizeOf);
    }
    DeadCommDone = 0;
    Engine->LoadSaveCreatureBatches(&IDNAME("LabBatches"), 0);
    *Engine->OnZoneLoadTable = (void **)ZoneLoadTable;
    *Engine->LevelSpecific = LevelSpecific;
    Engine->LevelEvents = LevelEvents;
    MissionData = Engine->MissionData;
    RestoreLifts();
    Engine->InitialiseQuakePlayer(0, *Engine->MainPlayerStart);
    Engine->QuakeSinglePlayerInit();
    Engine->AddLevelInterMission(Engine->GetStringFromStringtable(IDNAME("MapTitle")));
    Engine->PlayThisXA(1);
}

/* the "Laser0"/"Laser1" events switch a laser off for a while */
static void Laser0Off(void)
{
    if (Laser0Timer <= 0)
        Laser0Timer = 8192;
}

static void Laser1Off(void)
{
    if (Laser1Timer <= 0)
        Laser1Timer = 8192;
}

#ifndef NONMATCHING
/* NONMATCHING: see CheckGotHead */
__asm__(
    "\t.text\n"
    "\t.align\t2\n"
    "\t.globl\tFindCreature\n"
    "FindCreature:\n"
    "\t.set\tnoreorder\n"
    "\t.set\tnomacro\n"
    "\tmove\t$8,$29\n"
    "\tlui\t$2,%hi(LevelInterface+8)\n"
    "\tlw\t$2,%lo(LevelInterface+8)($2)\n"
    "\tmove\t$7,$0\n"
    "\tsw\t$4,0($29)\n"
    "\tsw\t$5,4($29)\n"
    "\tsw\t$6,8($29)\n"
    "\tlw\t$2,224($2)\n"
    "\tlui\t$3,%hi(FoundCre)\n"
    "\tsw\t$2,%lo(FoundCre)($3)\n"
    "FC_15b4:\n"
    "\tlw\t$6,%lo(FoundCre)($3)\n"
    "\tnop\n"
    "\tlw\t$2,16($6)\n"
    "\tlw\t$5,0($8)\n"
    "\tlw\t$4,8($2)\n"
    "\tnop\n"
    "\tbne\t$5,$4,FC_15fc\n"
    "\taddu\t$2,$2,8\n"
    "\tlw\t$5,4($8)\n"
    "\tlw\t$4,4($2)\n"
    "\tnop\n"
    "\tbne\t$5,$4,FC_15fc\n"
    "\tnop\n"
    "\tlw\t$5,8($8)\n"
    "\tlw\t$4,8($2)\n"
    "\tnop\n"
    "\tbeq\t$5,$4,FC_1620\n"
    "\tmove\t$2,$6\n"
    "FC_15fc:\n"
    "\tlw\t$2,%lo(FoundCre)($3)\n"
    "\taddu\t$7,$7,1\n"
    "\taddu\t$2,$2,768\n"
    "\tsw\t$2,%lo(FoundCre)($3)\n"
    "\tslt\t$2,$7,48\n"
    "\tbne\t$2,$0,FC_15b4\n"
    "\tnop\n"
    "\tj\t$31\n"
    "\tmove\t$2,$0\n"
    "FC_1620:\n"
    "\tj\t$31\n"
    "\tnop\n"
    "\t.set\tmacro\n"
    "\t.set\treorder\n");
#endif
