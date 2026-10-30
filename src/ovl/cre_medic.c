/* Medic (monster_medic, m_medic.c), with the PS1's resurrection of dead monsters */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table, or with AI_LOCAL
 * LocalAiTable[26 + n] (the medic's own versions of the ai_* functions) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5
#define AI_LOCAL 0x80

/* mframe_t.aifunc_local_index: LocalAiTable, filled in by PreInit_Medic */
enum {
    L_NONE,
    L_IDLE,
    L_RUN,
    L_DEAD,
    L_DUCK_DOWN,
    L_DUCK_HOLD,
    L_DUCK_UP,
    L_FIRE_BLASTER,
    L_CONTINUE,
    L_HOOK_LAUNCH,
    L_CABLE_ATTACK,
    L_HOOK_RETRACT,
};

/* a dead monster (a CREATURE of type DEADBODY) has no edict; the medic aims at it
 * through FakeEdict and keeps the creature in pos1: vx while it is the enemy, vy
 * while it is the goal, vz its Ident */
int abs(int);

#define DEADBODY 47
#define ENEMYCRE(e) (*(struct CREATURE **)&(e)->pos1.vx)
#define GOALCRE(e) (*(struct CREATURE **)&(e)->pos1.vy)
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

void PreInit_Medic(struct GLOBMODHDR *Hdr);
void medic_UpdateTarget(struct edict_s *self);
struct CREATURE *medic_FindDeadMonster(struct edict_s *self);
static void medic_idle(struct edict_s *self);
static void medic_search(struct edict_s *self);
static void medic_run(struct edict_s *self);
static void medic_fire_blaster(struct edict_s *self);
static void medic_Resurrect(struct CREATURE *Cre);
static void medic_cable_attack(struct edict_s *self);
void SP_monster_medic(struct edict_s *self);
static void medic_dead(struct edict_s *self);
static void medic_duck_down(struct edict_s *self);
static void medic_duck_hold(struct edict_s *self);
static void medic_duck_up(struct edict_s *self);
static void medic_continue(struct edict_s *self);
static void medic_hook_launch(struct edict_s *self);
static void medic_hook_retract(struct edict_s *self);
static void medic_ai_run(struct edict_s *self);
static void medic_ai_walk(struct edict_s *self);
static void medic_ai_stand(struct edict_s *self);
static void medic_ai_charge(struct edict_s *self);
static void medic_ai_move(struct edict_s *self);
void medic_InitFake(struct edict_s *ent, struct CREATURE *Cre);
void medic_CheckTarget(struct edict_s *ent, struct CREATURE *Cre, struct edict_s *self);
void medic_HuntTarget(struct edict_s *self, struct CREATURE *Cre);
void medic_ResetFake(void);
int medic_StackVisible(void);
static void medic_sight(struct edict_s *self);
static void medic_stand(struct edict_s *self);
static void medic_walk(struct edict_s *self);
static void medic_pain(struct edict_s *self);
static void medic_die(struct edict_s *self);
static void medic_dodge(struct edict_s *self, struct edict_s *attacker);
static void medic_attack(struct edict_s *self);
static int medic_checkattack(struct edict_s *self);

static struct mframe_t medic_frames_stand[] = {
    { AI_LOCAL | AI_STAND, 0, L_IDLE }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
    { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 }, { AI_LOCAL | AI_STAND, 0, 0 },
};
static struct mmove_t medic_move_stand = { 12, 101, medic_frames_stand, 0 };
static struct mframe_t medic_frames_walk[] = {
    { AI_LOCAL | AI_WALK, 6, 0 }, { AI_LOCAL | AI_WALK, 18, 0 }, { AI_LOCAL | AI_WALK, 1, 0 },
    { AI_LOCAL | AI_WALK, 9, 0 }, { AI_LOCAL | AI_WALK, 10, 0 }, { AI_LOCAL | AI_WALK, 9, 0 },
    { AI_LOCAL | AI_WALK, 11, 0 }, { AI_LOCAL | AI_WALK, 12, 0 }, { AI_LOCAL | AI_WALK, 2, 0 },
    { AI_LOCAL | AI_WALK, 10, 0 }, { AI_LOCAL | AI_WALK, 14, 0 }, { AI_LOCAL | AI_WALK, 9, 0 },
};
static struct mmove_t medic_move_walk = { 0, 11, medic_frames_walk, 0 };
static struct mframe_t medic_frames_run[] = {
    { AI_LOCAL | AI_RUN, 18, 0 }, { AI_LOCAL | AI_RUN, 22, 0 }, { AI_LOCAL | AI_RUN, 25, 0 },
    { AI_LOCAL | AI_RUN, 23, 0 }, { AI_LOCAL | AI_RUN, 24, 0 }, { AI_LOCAL | AI_RUN, 36, 0 },
};
static struct mmove_t medic_move_run = { 102, 107, medic_frames_run, 0 };
static struct mframe_t medic_frames_pain1[] = {
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
};
static struct mmove_t medic_move_pain1 = { 108, 115, medic_frames_pain1, medic_run };
static struct mframe_t medic_frames_pain2[] = {
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
};
static struct mmove_t medic_move_pain2 = { 116, 130, medic_frames_pain2, medic_run };
static struct mframe_t medic_frames_death[] = {
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 },
};
static struct mmove_t medic_move_death = { 147, 176, medic_frames_death, medic_dead };
static struct mframe_t medic_frames_duck[] = {
    { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, L_DUCK_DOWN },
    { AI_LOCAL | AI_MOVE, -1, L_DUCK_HOLD }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 },
    { AI_LOCAL | AI_MOVE, -1, L_DUCK_UP }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 },
    { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 },
    { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 },
    { AI_LOCAL | AI_MOVE, -1, 0 },
};
static struct mmove_t medic_move_duck = { 131, 146, medic_frames_duck, medic_run };
static struct mframe_t medic_frames_attackHyperBlaster[] = {
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 },
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
};
static struct mmove_t medic_move_attackHyperBlaster = { 191, 206, medic_frames_attackHyperBlaster, medic_run };
static struct mframe_t medic_frames_attackBlaster[] = {
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 5, 0 }, { AI_LOCAL | AI_CHARGE, 5, 0 },
    { AI_LOCAL | AI_CHARGE, 3, 0 }, { AI_LOCAL | AI_CHARGE, 2, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 },
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, L_FIRE_BLASTER },
    { AI_LOCAL | AI_CHARGE, 0, 0 }, { AI_LOCAL | AI_CHARGE, 0, L_CONTINUE },
};
static struct mmove_t medic_move_attackBlaster = { 177, 190, medic_frames_attackBlaster, medic_run };
static struct mframe_t medic_frames_attackCable[] = {
    { AI_LOCAL | AI_MOVE, 2, 0 }, { AI_LOCAL | AI_MOVE, 3, 0 }, { AI_LOCAL | AI_MOVE, 5, 0 },
    { AI_LOCAL | AI_MOVE, 4, 0 }, { AI_LOCAL | AI_CHARGE, 4, 0 }, { AI_LOCAL | AI_CHARGE, 5, 0 },
    { AI_LOCAL | AI_CHARGE, 6, 0 }, { AI_LOCAL | AI_CHARGE, 4, 0 }, { AI_LOCAL | AI_CHARGE, 0, 0 },
    { AI_LOCAL | AI_MOVE, 0, L_HOOK_LAUNCH }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK },
    { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK },
    { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK },
    { AI_LOCAL | AI_MOVE, 0, L_CABLE_ATTACK }, { AI_LOCAL | AI_MOVE, -15, L_HOOK_RETRACT },
    { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -1, 0 }, { AI_LOCAL | AI_MOVE, -3, 0 },
    { AI_LOCAL | AI_MOVE, -2, 0 }, { AI_LOCAL | AI_MOVE, 0, 0 }, { AI_LOCAL | AI_MOVE, 1, 0 },
    { AI_LOCAL | AI_MOVE, 1, 0 }, { AI_LOCAL | AI_MOVE, 1, 0 },
};
static struct mmove_t medic_move_attackCable = { 209, 236, medic_frames_attackCable, medic_run };

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_medic, 0, PreInit_Medic, AnimRemap };

static struct edict_s FakeEdict = { 0 };
static struct CREATURE FakeCre = { 0 };

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 11, {"Walk"}, 0, 0 },
    { 102, 107, {"Run"}, 0, 0 },
    { 147, 176, {"Death"}, 0, 0 },
    { 116, 130, {"Pain2"}, 0, 0 },
    { 108, 115, {"Pain1"}, 0, 0 },
    { 12, 51, {"Standa"}, 0, 0 },
    { 52, 101, {"Standb"}, 0, 0 },
    { 131, 146, {"Duck"}, 0, 0 },
    { 177, 190, {"Attblast"}, 0, 0 },
    { 191, 206, {"Atthyper"}, 0, 0 },
    { 209, 236, {"Attcable"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1 };

static int sound_idle;
static int sound_pain1;
static int sound_pain2;
static int sound_die;
static int sound_sight;
static int sound_search;
static int sound_hook_launch;
static int sound_hook_hit;
static int sound_hook_heal;
static int sound_hook_retract;
static int sound_udeath;
/* the Unused* are space the original has that no code uses */
static int Unused0;
static struct MODSUBOFF MuzzleOff;
static int Unused1[2];
static struct ai_local_table_t LocalAiTable[32];
static int Unused2;
/* medic_StackVisible runs visible() on the engine's scratch stack, so it works in statics */
static struct edict_s *VisibleSelf;
static int Unused3;
static unsigned long VisibleSp;
static int VisibleResult;

static const char ClearMsg[16];

void PreInit_Medic(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Medic - ** RESURRECTION ENABLED **\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Medic");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Medic");
        Iface.PrintF("Validated Creature Interface for %s\n", "Medic");
        Hdr->ShadowVerts = ShadowVerts;
        medic_InitFake(&FakeEdict, &FakeCre);
        LocalAiTable[L_NONE].thinkfunc = 0;
        LocalAiTable[L_IDLE].thinkfunc = medic_idle;
        LocalAiTable[L_RUN].thinkfunc = medic_run;
        LocalAiTable[L_DEAD].thinkfunc = medic_dead;
        LocalAiTable[L_DUCK_DOWN].thinkfunc = medic_duck_down;
        LocalAiTable[L_DUCK_HOLD].thinkfunc = medic_duck_hold;
        LocalAiTable[L_DUCK_UP].thinkfunc = medic_duck_up;
        LocalAiTable[L_FIRE_BLASTER].thinkfunc = medic_fire_blaster;
        LocalAiTable[L_CONTINUE].thinkfunc = medic_continue;
        LocalAiTable[L_HOOK_LAUNCH].thinkfunc = medic_hook_launch;
        LocalAiTable[L_CABLE_ATTACK].thinkfunc = medic_cable_attack;
        LocalAiTable[L_HOOK_RETRACT].thinkfunc = medic_hook_retract;
        LocalAiTable[26 + AI_RUN].thinkfunc = medic_ai_run;
        LocalAiTable[26 + AI_WALK].thinkfunc = medic_ai_walk;
        LocalAiTable[26 + AI_STAND].thinkfunc = medic_ai_stand;
        LocalAiTable[26 + AI_CHARGE].thinkfunc = medic_ai_charge;
        LocalAiTable[26 + AI_MOVE].thinkfunc = medic_ai_move;
        Iface.SetMyAiTable(84, LocalAiTable);
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"med_idle"});
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"med_pain1"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"med_pain2"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"med_deth1"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"med_sght1"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"med_srch1"});
        sound_hook_launch = Iface.GetSoundNum((struct IDENTITYNAME){"med_atck2"});
        sound_hook_hit = Iface.GetSoundNum((struct IDENTITYNAME){"med_atck3"});
        sound_hook_heal = Iface.GetSoundNum((struct IDENTITYNAME){"med_atck4"});
        sound_hook_retract = Iface.GetSoundNum((struct IDENTITYNAME){"med_atck5"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!sound_pain2 || !sound_pain1) {
            sound_pain2 |= sound_pain1;
            sound_pain1 |= sound_pain2;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 239, 246, &MuzzleOff))
            Iface.PrintF("Medic Blaster muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF(ClearMsg);
}

/* the two bytes after the string are the original's padding */
static const char ClearMsg[16] = "Clear Medic\r\n\0\001u";

void medic_InitFake(struct edict_s *ent, struct CREATURE *Cre)
{
    Cre->EdictT = ent;
    ent->Extra.Creature = Cre;
    Cre->Health = -1;
    ent->inuse = 1;
}

void medic_ResetFake(void)
{
    medic_InitFake(&FakeEdict, &FakeCre);
}

static void medic_ClearTarget(struct edict_s *self)
{
    if (self->enemy == &FakeEdict) {
        self->enemy = 0;
        ENEMYCRE(self) = 0;
    }
    if (self->goalentity == &FakeEdict) {
        self->goalentity = 0;
        GOALCRE(self) = 0;
    }
    self->pos1.vz = -1;
    self->monsterinfo.aiflags &= ~0x2000;
}

void medic_CheckTarget(struct edict_s *ent, struct CREATURE *Cre, struct edict_s *self)
{
    if (self->pos1.vz == Cre->Ident && Cre->Life > 0 && Cre->TypeIdent == DEADBODY) {
        ent->Extra.Creature = Cre;
        ent->sorigin.vx = Cre->Box.Location.vx;
        ent->sorigin.vy = Cre->Box.Location.vy;
        ent->sorigin.vz = Cre->Box.Location.vz;
        ent->classname = 86;
        ent->owner = self;
        ent->inuse = 1;
    } else
        medic_ClearTarget(self);
}

void medic_UpdateTarget(struct edict_s *self)
{
    if (ENEMYCRE(self) && self->enemy == &FakeEdict)
        medic_CheckTarget(&FakeEdict, ENEMYCRE(self), self);
    if (GOALCRE(self) && self->goalentity == &FakeEdict)
        medic_CheckTarget(&FakeEdict, GOALCRE(self), self);
}

int medic_StackVisible(void)
{
    VisibleSp = Iface.SetSp(*Iface.usp);
    VisibleResult = Iface.visible(VisibleSelf, &FakeEdict);
    Iface.SetSp(VisibleSp);
    return VisibleResult;
}

struct CREATURE *medic_FindDeadMonster(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct CREATURE *best;

    Cre = 0;
    best = Cre;
    while ((Cre = Iface.CreFindRadius(Cre, self->sorigin, 12288))) {
        if (Cre == self->Extra.Creature)
            continue;
        if (Cre->TypeIdent != DEADBODY)
            continue;
        if (Cre->Player)
            continue;
        if (Cre->IdealYaw == 34)
            continue;
        if (Cre->Health > 0)
            continue;
        if (best && Cre->Params[1] <= best->Params[1])
            continue;
        if (Cre->Box.MoveSpace < 0)
            continue;
        FakeEdict.sorigin.vx = Cre->Box.Location.vx;
        FakeEdict.sorigin.vy = Cre->Box.Location.vy;
        FakeEdict.sorigin.vz = Cre->Box.Location.vz;
        VisibleSelf = self;
        if (medic_StackVisible())
            best = Cre;
    }
    return best;
}

void medic_HuntTarget(struct edict_s *self, struct CREATURE *Cre)
{
    self->show_hostile = Iface.level->time + 10;
    self->monsterinfo.last_sighting.vx = Cre->Box.Location.vx;
    self->monsterinfo.last_sighting.vy = Cre->Box.Location.vy;
    self->monsterinfo.last_sighting.vz = Cre->Box.Location.vz;
    self->monsterinfo.trail_time = Iface.level->time;
    Iface.HuntTarget(self);
}

static __inline__ void medic_SetTarget(struct edict_s *self, struct CREATURE *Cre)
{
    if (self->enemy && self->enemy != &FakeEdict)
        self->oldenemy = self->enemy;
    medic_CheckTarget(&FakeEdict, Cre, self);
    self->enemy = &FakeEdict;
    self->monsterinfo.aiflags |= 0x2000;
    medic_HuntTarget(self, Cre);
    ENEMYCRE(self) = Cre;
    self->pos1.vz = Cre->Ident;
    if (self->goalentity == &FakeEdict)
        GOALCRE(self) = Cre;
}

static void medic_idle(struct edict_s *self)
{
    struct CREATURE *Cre;

    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
    Cre = medic_FindDeadMonster(self);
    if (Cre)
        medic_SetTarget(self, Cre);
}

static void medic_search(struct edict_s *self)
{
    struct CREATURE *Cre;

    Iface.SoundByCreature(sound_search, self->Extra.Creature);
    if (!self->oldenemy) {
        Cre = medic_FindDeadMonster(self);
        if (Cre)
            medic_SetTarget(self, Cre);
    }
}

static void medic_run(struct edict_s *self)
{
    struct CREATURE *Cre;

    if (!(self->monsterinfo.aiflags & 0x2000)) {
        Cre = medic_FindDeadMonster(self);
        if (Cre) {
            medic_SetTarget(self, Cre);
            return;
        }
    } else if (Iface.AreaHidden(CRE(self->enemy)->Box.Area)) {
        self->pos1.vz = -1;
        self->monsterinfo.aiflags &= ~0x2000;
        ENEMYCRE(self) = 0;
        self->enemy = 0;
        if (self->oldenemy && self->oldenemy->client)
            self->enemy = self->oldenemy;
    }
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &medic_move_stand;
    else
        self->monsterinfo.currentmove = &medic_move_run;
}

static void medic_fire_blaster(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT end;
    struct VECT dir;
    struct SVECT vec;
    int effect;

    Cre = self->Extra.Creature;
    if (self->s.frame == 185 || self->s.frame == 188)
        effect = 8;
    else if (self->s.frame == 195 || self->s.frame == 198 || self->s.frame == 201 || self->s.frame == 204)
        effect = 64;
    else
        effect = 0;
    vec = Iface.GetSubModXYZ(Cre->ModelHdr, MuzzleOff, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &vec, &vec);
    start.vx = Cre->Draw.Location.vx + vec.vx;
    start.vy = Cre->Draw.Location.vy + vec.vy;
    start.vz = Cre->Draw.Location.vz + vec.vz;
    end.vx = self->enemy->sorigin.vx;
    end.vy = self->enemy->sorigin.vy;
    end.vz = self->enemy->sorigin.vz;
    end.vy += self->enemy->viewheight;
    dir.vx = end.vx - start.vx;
    dir.vy = end.vy - start.vy;
    dir.vz = end.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_blaster(self, &start, &dir, 2, 1000, 0, effect);
}

#define MEDIC(Cre) ((struct edict_s *)(Cre)->Params[3])

/* the dead body's Primary once the medic's cable has done its work */
static void medic_Resurrect(struct CREATURE *Cre)
{
    struct CREDATA Data;
    struct CREATURE *New;

    if (Cre->TypeIdent == DEADBODY)
        Data.TypeIdent = Cre->IdealYaw;
    else
        Data.TypeIdent = Cre->TypeIdent;
    Data.Location.vx = Cre->Box.Location.vx;
    Data.Location.vy = Cre->Box.Location.vy;
    Data.Location.vz = Cre->Box.Location.vz;
    Data.AngY = Cre->Ang.vy;
    Data.TargetIdent = 0;
    Data.SpawnFlags = 0;
    New = Iface.SpawnGeneral(&Data, 0);
    if (New) {
        New->EdictT->spawnflags = 0;
        New->EdictT->monsterinfo.aiflags = 0;
        New->EdictT->target = 0;
        New->EdictT->targetname = 0;
        New->EdictT->combattarget = 0;
        New->EdictT->owner = 0;
        if (New->EdictT->think) {
            New->EdictT->nextthink = Iface.level->time;
            New->EdictT->think(New->EdictT);
        }
        if (Cre->Params[2]) {
            New->EdictT->enemy = (struct edict_s *)Cre->Params[2];
            Iface.FoundTarget(New->EdictT);
        }
        if (MEDIC(Cre) && MEDIC(Cre)->inuse && MEDIC(Cre)->enemy == &FakeEdict && ENEMYCRE(MEDIC(Cre)) == Cre) {
            MEDIC(Cre)->enemy = New->EdictT;
            ENEMYCRE(MEDIC(Cre)) = GOALCRE(MEDIC(Cre)) = 0;
            MEDIC(Cre)->pos1.vz = -1;
        }
    } else if (MEDIC(Cre) && MEDIC(Cre)->inuse && MEDIC(Cre)->enemy == &FakeEdict && ENEMYCRE(MEDIC(Cre)) == Cre) {
        MEDIC(Cre)->enemy = 0;
        ENEMYCRE(MEDIC(Cre)) = GOALCRE(MEDIC(Cre)) = 0;
        MEDIC(Cre)->pos1.vz = -1;
        MEDIC(Cre)->monsterinfo.aiflags &= ~0x2000;
    }
    Cre->Health = Cre->Params[0];
    Cre->Primary = Iface.PrimaryQuakeDeadBody;
}

static void medic_cable_attack(struct edict_s *self)
{
    struct SVECT offset; /* unused, but it takes its stack slot */
    struct VECT dir;
    struct SVECT angles;

    medic_UpdateTarget(self);
    if (!self->enemy || !self->enemy->inuse) {
        ENEMYCRE(self) = 0;
        self->pos1.vz = -1;
        self->monsterinfo.aiflags &= ~0x2000;
        medic_ResetFake();
        return;
    }
    dir.vx = CRE(self)->Box.Location.vx - self->monsterinfo.last_sighting.vx;
    dir.vy = CRE(self)->Box.Location.vy - self->monsterinfo.last_sighting.vy;
    dir.vz = CRE(self)->Box.Location.vz - self->monsterinfo.last_sighting.vz;
    if (Iface.VectorLength(&dir) > 3072) {
        medic_ResetFake();
        return;
    }
    Iface.vectoangles(&dir, &angles);
    if (angles.vx < -2048)
        angles.vx += 4096;
    if (angles.vx > 2048)
        angles.vx -= 4096;
    if (abs(angles.vx) > 512) {
        medic_ResetFake();
        return;
    }
    dir.vx = self->enemy->sorigin.vx;
    dir.vy = self->enemy->sorigin.vy;
    dir.vz = self->enemy->sorigin.vz;
    dir.vy += (572 - CRE(self->enemy)->Box.HitHeight) / 2;
    Iface.gitrace(&CRE(self)->Box.Location, 0, 0, &dir, self, 0x6000003, CRE(self)->Box.MoveSpace);
    if (Iface.Trace->cre == self->enemy->Extra.Creature)
        Iface.Trace->ent = self->enemy;
    if (Iface.Trace->ent != self->enemy) {
        medic_ResetFake();
        return;
    }
    if (self->s.frame == 219) {
        Iface.SoundByCreature(sound_hook_hit, self->Extra.Creature);
        CRE(self->enemy)->Effect.HealTime = 30;
        self->enemy->monsterinfo.aiflags |= 0x4000;
    } else if (self->s.frame == 226) {
        CRE(self->enemy)->Primary = medic_Resurrect;
        if (self->oldenemy && self->oldenemy->client)
            CRE(self->enemy)->Params[2] = (int)self->oldenemy;
        else
            CRE(self->enemy)->Params[2] = 0;
        CRE(self->enemy)->Params[3] = (int)self;
    } else
        Iface.SoundByCreature(sound_hook_heal, self->Extra.Creature);
    Iface.AnyoneFireSplurge(self->Extra.Creature, self->enemy->Extra.Creature, 308, 0);
    medic_ResetFake();
}

void SP_monster_medic(struct edict_s *self)
{
    self->mass = 400;
    self->pain = medic_pain;
    self->die = medic_die;
    self->monsterinfo.stand = medic_stand;
    self->monsterinfo.walk = medic_walk;
    self->monsterinfo.run = medic_run;
    self->monsterinfo.dodge = medic_dodge;
    self->monsterinfo.attack = medic_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = medic_sight;
    self->monsterinfo.idle = medic_idle;
    self->monsterinfo.search = medic_search;
    self->monsterinfo.checkattack = medic_checkattack;
    self->pos1.vz = -1;
    GOALCRE(self) = 0;
    ENEMYCRE(self) = 0;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 129);
    self->monsterinfo.currentmove = &medic_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void medic_dead(struct edict_s *self)
{
    self->nextthink = 0;
    self->movetype = 7;
    self->svflags |= 2;
}

static void medic_duck_down(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 0x800)
        return;
    self->monsterinfo.aiflags |= 0x800;
    self->takedamage = 1;
    self->monsterinfo.pausetime = Iface.level->time + 10;
}

static void medic_duck_hold(struct edict_s *self)
{
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~0x80;
    else
        self->monsterinfo.aiflags |= 0x80;
}

static void medic_duck_up(struct edict_s *self)
{
    self->monsterinfo.aiflags &= ~0x800;
    self->takedamage = 2;
}

static void medic_continue(struct edict_s *self)
{
    if (Iface.visible(self, self->enemy))
        if (Iface.rand() < 31129)
            self->monsterinfo.currentmove = &medic_move_attackHyperBlaster;
}

static void medic_hook_launch(struct edict_s *self)
{
    Iface.SoundByCreature(sound_hook_launch, self->Extra.Creature);
}

static void medic_hook_retract(struct edict_s *self)
{
    Iface.SoundByCreature(sound_hook_retract, self->Extra.Creature);
    if (self->enemy)
        self->enemy->monsterinfo.aiflags &= ~0x4000;
}

#define MEDIC_AI(name, aifunc) \
static void name(struct edict_s *self) \
{ \
    int index; \
 \
    index = self->s.frame - self->monsterinfo.currentmove->firstframe; \
    medic_UpdateTarget(self); \
    if (!(self->monsterinfo.aiflags & 0x80)) \
        Iface.aifunc(self, (signed char)self->monsterinfo.currentmove->frame[index].dist * self->monsterinfo.scale * 12 / 10); \
    else \
        Iface.aifunc(self, 0); \
    medic_ResetFake(); \
}

MEDIC_AI(medic_ai_run, ai_run)
MEDIC_AI(medic_ai_walk, ai_walk)
MEDIC_AI(medic_ai_stand, ai_stand)
MEDIC_AI(medic_ai_charge, ai_charge)
MEDIC_AI(medic_ai_move, ai_move)

static void medic_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void medic_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &medic_move_stand;
}

static void medic_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &medic_move_walk;
}

static void medic_pain(struct edict_s *self)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    if (Iface.rand() < 16384) {
        self->monsterinfo.currentmove = &medic_move_pain1;
        Iface.SoundByCreature(sound_pain1, self->Extra.Creature);
    } else {
        self->monsterinfo.currentmove = &medic_move_pain2;
        Iface.SoundByCreature(sound_pain2, self->Extra.Creature);
    }
}

static void medic_die(struct edict_s *self)
{
    if (self->enemy && self->enemy->owner == self)
        self->enemy->owner = 0;
    if (CRE(self)->Health <= self->gib_health) {
        Iface.SoundByCreature(sound_udeath, self->Extra.Creature);
        self->nextthink = 0;
        self->deadflag = 2;
        self->movetype = 7;
        self->svflags |= 2;
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, self->Extra.Creature);
    self->monsterinfo.currentmove = &medic_move_death;
    self->deadflag = 2;
    self->takedamage = 1;
}

static void medic_dodge(struct edict_s *self, struct edict_s *attacker)
{
    if (Iface.rand() < 8193) {
        if (!self->enemy)
            self->enemy = attacker;
        self->monsterinfo.currentmove = &medic_move_duck;
    }
}

static void medic_attack(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 0x2000)
        self->monsterinfo.currentmove = &medic_move_attackCable;
    else
        self->monsterinfo.currentmove = &medic_move_attackBlaster;
}

static int medic_checkattack(struct edict_s *self)
{
    struct VECT dir;

    if (self->monsterinfo.aiflags & 0x2000) {
        dir.vx = CRE(self)->Box.Location.vx - self->monsterinfo.last_sighting.vx;
        dir.vy = CRE(self)->Box.Location.vy - self->monsterinfo.last_sighting.vy;
        dir.vz = CRE(self)->Box.Location.vz - self->monsterinfo.last_sighting.vz;
        if (Iface.VectorLength(&dir) > 3072)
            return 0;
        medic_attack(self);
        return 1;
    }
    return Iface.M_CheckAttack(self);
}
