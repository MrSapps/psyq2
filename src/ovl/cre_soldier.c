/* Soldier (monster_soldier_light, monster_soldier, monster_soldier_ss, m_soldier.c).
 * cre_soldier_magdemo.c builds the MAGDEMO copy of this module from this file. */
#include "ovl.h"

#ifndef SOLDIER_MUZZLE_MSG
#define SOLDIER_MUZZLE_MSG "Soldier muzzle points are from different submodels\r\n"
/* the byte after the "Unknown soldier type" string: the original's padding */
#define SOLDIER_RDATA_PAD "h"
#endif

/* mframe_t.aifunc_global_index: the engine's ai_global_table */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: LocalAiTable, filled in by PreInit_Soldier */
enum {
    L_NONE,
    L_STAND,
    L_IDLE,
    L_COCK,
    L_WALK1_RANDOM,
    L_RUN,
    L_FIRE1,
    L_ATTACK1_REFIRE1,
    L_ATTACK1_REFIRE2,
    L_FIRE2,
    L_ATTACK2_REFIRE1,
    L_ATTACK2_REFIRE2,
    L_DUCK_DOWN,
    L_DUCK_UP,
    L_FIRE3,
    L_ATTACK3_REFIRE,
    L_FIRE4,
    L_FIRE8,
    L_ATTACK6_REFIRE,
    L_DUCK_HOLD,
    L_FIRE6,
    L_FIRE7,
    L_DEAD,
};

void PreInit_Soldier(struct GLOBMODHDR *Hdr);
void SP_Soldier(struct edict_s *self);
static void soldier_stand(struct edict_s *self);
static void soldier_idle(struct edict_s *self);
static void soldier_cock(struct edict_s *self);
static void soldier_walk1_random(struct edict_s *self);
static void soldier_run(struct edict_s *self);
static void soldier_fire1(struct edict_s *self);
static void soldier_attack1_refire1(struct edict_s *self);
static void soldier_attack1_refire2(struct edict_s *self);
static void soldier_fire2(struct edict_s *self);
static void soldier_attack2_refire1(struct edict_s *self);
static void soldier_attack2_refire2(struct edict_s *self);
static void soldier_duck_down(struct edict_s *self);
static void soldier_duck_up(struct edict_s *self);
static void soldier_fire3(struct edict_s *self);
static void soldier_attack3_refire(struct edict_s *self);
static void soldier_fire4(struct edict_s *self);
static void soldier_fire8(struct edict_s *self);
static void soldier_attack6_refire(struct edict_s *self);
static void soldier_duck_hold(struct edict_s *self);
static void soldier_fire6(struct edict_s *self);
static void soldier_fire7(struct edict_s *self);
static void soldier_dead(struct edict_s *self);
void Soldier_EveryFrame(struct CREATURE *Cre);
static void soldier_walk(struct edict_s *self);
static void soldier_attack(struct edict_s *self);
static void soldier_sight(struct edict_s *self);
static void soldier_dodge(struct edict_s *self);
static void soldier_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point);
void SP_monster_soldier_x(struct edict_s *self);
void SP_monster_soldier_light(struct edict_s *self);
void SP_monster_soldier(struct edict_s *self);
void SP_monster_soldier_ss(struct edict_s *self);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_Soldier, Soldier_EveryFrame, PreInit_Soldier, AnimRemap };

static struct NEWANIMREMAP AnimRemap[] = {
    { 97, 98, {"Start Run"}, 0, 0 },
    { 99, 104, {"Run"}, 0, 0 },
    { 0, 0, {"Fire 1 Ready"}, 0, 0 },
    { 1, 5, {"Fire 1 Aim"}, 0, 0 },
    { 6, 8, {"Fire 1 Shoot"}, 0, 0 },
    { 9, 11, {"Fire 1 Done"}, 0, 0 },
    { 12, 14, {"Fire 2 Ready"}, 0, 0 },
    { 15, 19, {"Fire 2 Aim"}, 0, 0 },
    { 20, 26, {"Fire 2 Shoot"}, 0, 0 },
    { 27, 29, {"Fire 2 Done"}, 0, 0 },
    { 215, 224, {"Walk 1 Loop"}, 0, 0 },
    { 225, 247, {"Walk 1 Look"}, 0, 0 },
    { 256, 265, {"Walk"}, 0, 0 },
    { 50, 54, {"Pain1"}, 0, 0 },
    { 55, 61, {"Pain2"}, 0, 0 },
    { 62, 79, {"Pain3"}, 0, 0 },
    { 80, 96, {"Pain4"}, 0, 0 },
    { 272, 307, {"Death1"}, 0, 0 },
    { 308, 342, {"Death2"}, 0, 0 },
    { 343, 387, {"Death3"}, 0, 0 },
    { 388, 440, {"Death4"}, 0, 0 },
    { 441, 464, {"Death5"}, 0, 0 },
    { 465, 474, {"Death6"}, 0, 0 },
    { 30, 31, {"Fire 3 Aim"}, 0, 0 },
    { 32, 35, {"Fire 3 Shoot"}, 0, 0 },
    { 36, 38, {"Fire 3 Done"}, 0, 0 },
    { 39, 44, {"Attack4"}, 0, 0 },
    { 45, 49, {"Duck"}, 0, 0 },
    { 146, 175, {"Stand1"}, 0, 0 },
    { 176, 214, {"Stand3"}, 0, 0 },
    { 109, 122, {"Attack6"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { 0x3b, 0x8a, 0x72, -1 };

static int sound_idle;
static int sound_sight1;
static int sound_sight2;
static int sound_pain_light;
static int sound_pain;
static int sound_pain_ss;
static int sound_death_light;
static int sound_death;
static int sound_death_ss;
static int sound_cock;
static int sound_udeath;
static int sound_machinegun;
static int sound_shotgun;
/* the Unused* are space the original has that no code uses */
static int Unused0;
static struct MODSUBOFF MuzzleOff;
static int Unused1[2];
/* commons, which the linker puts after .bss (ASPSX lists them in an order of its own) */
struct ai_local_table_t LocalAiTable[32];
int Unused3;

void PreInit_Soldier(struct GLOBMODHDR *Hdr)
{
    int n;

    if (Hdr) {
        Iface.PrintF("Pre Init Soldier mfb=%x\n", Iface.monster_fire_blaster);
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Soldier");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Soldier");
        Iface.PrintF("Validated Creature Interface for %s\n", "Soldier");
        LocalAiTable[L_NONE].thinkfunc = 0;
        LocalAiTable[L_STAND].thinkfunc = soldier_stand;
        LocalAiTable[L_IDLE].thinkfunc = soldier_idle;
        LocalAiTable[L_COCK].thinkfunc = soldier_cock;
        LocalAiTable[L_WALK1_RANDOM].thinkfunc = soldier_walk1_random;
        LocalAiTable[L_RUN].thinkfunc = soldier_run;
        LocalAiTable[L_FIRE1].thinkfunc = soldier_fire1;
        LocalAiTable[L_ATTACK1_REFIRE1].thinkfunc = soldier_attack1_refire1;
        LocalAiTable[L_ATTACK1_REFIRE2].thinkfunc = soldier_attack1_refire2;
        LocalAiTable[L_FIRE2].thinkfunc = soldier_fire2;
        LocalAiTable[L_ATTACK2_REFIRE1].thinkfunc = soldier_attack2_refire1;
        LocalAiTable[L_ATTACK2_REFIRE2].thinkfunc = soldier_attack2_refire2;
        LocalAiTable[L_DUCK_DOWN].thinkfunc = soldier_duck_down;
        LocalAiTable[L_DUCK_UP].thinkfunc = soldier_duck_up;
        LocalAiTable[L_FIRE3].thinkfunc = soldier_fire3;
        LocalAiTable[L_ATTACK3_REFIRE].thinkfunc = soldier_attack3_refire;
        LocalAiTable[L_FIRE4].thinkfunc = soldier_fire4;
        LocalAiTable[L_FIRE8].thinkfunc = soldier_fire8;
        LocalAiTable[L_ATTACK6_REFIRE].thinkfunc = soldier_attack6_refire;
        LocalAiTable[L_DUCK_HOLD].thinkfunc = soldier_duck_hold;
        LocalAiTable[L_FIRE6].thinkfunc = soldier_fire6;
        LocalAiTable[L_FIRE7].thinkfunc = soldier_fire7;
        LocalAiTable[L_DEAD].thinkfunc = soldier_dead;
        Iface.SetMyAiTable(87, LocalAiTable);
        Iface.SetMyAiTable(89, LocalAiTable);
        Iface.SetMyAiTable(88, LocalAiTable);
        Hdr->ShadowVerts = ShadowVerts;
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"sol_idle1"});
        sound_sight1 = Iface.GetSoundNum((struct IDENTITYNAME){"sol_sght1"});
        sound_sight2 = Iface.GetSoundNum((struct IDENTITYNAME){"sol_sght1"});
        sound_pain_light = Iface.GetSoundNum((struct IDENTITYNAME){"sol_pain1"});
        sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"sol_pain2"});
        sound_pain_ss = Iface.GetSoundNum((struct IDENTITYNAME){"sol_pain3"});
        sound_death_light = Iface.GetSoundNum((struct IDENTITYNAME){"sol_deth1"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"sol_deth2"});
        sound_death_ss = Iface.GetSoundNum((struct IDENTITYNAME){"sol_deth3"});
        sound_machinegun = Iface.GetSoundNum((struct IDENTITYNAME){"wep_machgf1b"});
        sound_shotgun = Iface.GetSoundNum((struct IDENTITYNAME){"wep_shotgf1b"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!(sound_cock = Iface.GetSoundNum((struct IDENTITYNAME){"wep_shotgr1b"})))
            sound_cock = Iface.GetSoundNum((struct IDENTITYNAME){"wep_sshotr1b"});
        n = sound_pain;
        if (!n)
            n = sound_pain_light;
        if (!n)
            n = sound_pain_ss;
        if (!sound_pain_light)
            sound_pain_light = n;
        if (!sound_pain)
            sound_pain = n;
        if (!sound_pain_ss)
            sound_pain_ss = n;
        n = sound_death;
        if (!n)
            n = sound_death_light;
        if (!n)
            n = sound_death_ss;
        if (!sound_death_light)
            sound_death_light = n;
        if (!sound_death)
            sound_death = n;
        if (!sound_death_ss)
            sound_death_ss = n;
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 228, 234, &MuzzleOff))
            Iface.PrintF(SOLDIER_MUZZLE_MSG);
    } else
        Iface.PrintF("Clear Soldier\n");
}

static struct mframe_t soldier_frames_stand1[] = {
    { AI_STAND, 0, L_IDLE }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t soldier_move_stand1 = { 146, 175, soldier_frames_stand1, soldier_stand };
static struct mframe_t soldier_frames_stand3[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, L_COCK }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t soldier_move_stand3 = { 176, 214, soldier_frames_stand3, soldier_stand };
static struct mframe_t soldier_frames_walk1[] = {
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 2, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 3, L_WALK1_RANDOM },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
};
static struct mmove_t soldier_move_walk1 = { 215, 247, soldier_frames_walk1, 0 };
static struct mframe_t soldier_frames_walk2[] = {
    { AI_WALK, 6, 0 }, { AI_WALK, 6, 0 }, { AI_WALK, 6, 0 }, { AI_WALK, 6, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 3, 0 }, { AI_WALK, 7, 0 }, { AI_WALK, 7, 0 }, { AI_WALK, 6, 0 }, { AI_WALK, 4, 0 },
};
static struct mmove_t soldier_move_walk2 = { 256, 265, soldier_frames_walk2, 0 };
static struct mframe_t soldier_frames_start_run[] = {
    { AI_RUN, 4, 0 }, { AI_RUN, 7, 0 },
};
static struct mmove_t soldier_move_start_run = { 97, 98, soldier_frames_start_run, soldier_run };
static struct mframe_t soldier_frames_run[] = {
    { AI_RUN, 14, 0 }, { AI_RUN, 16, 0 }, { AI_RUN, 13, 0 }, { AI_RUN, 11, 0 }, { AI_RUN, 18, 0 },
    { AI_RUN, 16, 0 },
};
static struct mmove_t soldier_move_run = { 99, 104, soldier_frames_run, 0 };
static struct mframe_t soldier_frames_pain1[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 },
};
static struct mmove_t soldier_move_pain1 = { 50, 54, soldier_frames_pain1, soldier_run };
static struct mframe_t soldier_frames_pain2[] = {
    { AI_MOVE, -13, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 },
};
static struct mmove_t soldier_move_pain2 = { 55, 61, soldier_frames_pain2, soldier_run };
static struct mframe_t soldier_frames_pain3[] = {
    { AI_MOVE, -1, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -1, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 },
};
static struct mmove_t soldier_move_pain3 = { 62, 79, soldier_frames_pain3, soldier_run };
static struct mframe_t soldier_frames_pain4[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, -8, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 },
};
static struct mmove_t soldier_move_pain4 = { 80, 96, soldier_frames_pain4, soldier_run };
static struct mframe_t soldier_frames_attack1[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE1 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_ATTACK1_REFIRE1 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_COCK },
    { AI_CHARGE, 0, L_ATTACK1_REFIRE2 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t soldier_move_attack1 = { 0, 11, soldier_frames_attack1, soldier_run };
static struct mframe_t soldier_frames_attack2[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE2 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_ATTACK2_REFIRE1 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_COCK },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_ATTACK2_REFIRE2 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 },
};
static struct mmove_t soldier_move_attack2 = { 12, 29, soldier_frames_attack2, soldier_run };
static struct mframe_t soldier_frames_attack3[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE3 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_ATTACK3_REFIRE }, { AI_CHARGE, 0, L_DUCK_UP }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 },
};
static struct mmove_t soldier_move_attack3 = { 30, 38, soldier_frames_attack3, soldier_run };
static struct mframe_t soldier_frames_attack4[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE4 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t soldier_move_attack4 = { 39, 44, soldier_frames_attack4, soldier_run };
static struct mframe_t soldier_frames_attack6[] = {
    { AI_CHARGE, 15, 0 }, { AI_CHARGE, 15, 0 }, { AI_CHARGE, 8, 0 }, { AI_CHARGE, 12, L_FIRE8 },
    { AI_CHARGE, 18, 0 }, { AI_CHARGE, 8, 0 }, { AI_CHARGE, 12, 0 }, { AI_CHARGE, 17, 0 }, { AI_CHARGE, 8, 0 },
    { AI_CHARGE, 12, 0 }, { AI_CHARGE, 15, 0 }, { AI_CHARGE, 8, 0 }, { AI_CHARGE, 15, 0 }, { AI_CHARGE, 20, L_ATTACK6_REFIRE },
};
static struct mmove_t soldier_move_attack6 = { 109, 122, soldier_frames_attack6, soldier_run };
static struct mframe_t soldier_frames_duck[] = {
    { AI_MOVE, 0, L_DUCK_DOWN }, { AI_MOVE, 0, L_DUCK_HOLD }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, L_DUCK_UP },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_duck = { 45, 49, soldier_frames_duck, soldier_run };
static struct mframe_t soldier_frames_death1[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, -10, 0 }, { AI_MOVE, -9, 0 }, { AI_MOVE, -9, 0 }, { AI_MOVE, -8, 0 },
    { AI_MOVE, -7, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -3, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, L_FIRE6 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, L_FIRE7 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death1 = { 272, 307, soldier_frames_death1, soldier_dead };
static struct mframe_t soldier_frames_death2[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, -8, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death2 = { 308, 342, soldier_frames_death2, soldier_dead };
static struct mframe_t soldier_frames_death3[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, -8, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death3 = { 343, 387, soldier_frames_death3, soldier_dead };
static struct mframe_t soldier_frames_death4[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death4 = { 388, 440, soldier_frames_death4, soldier_dead };
static struct mframe_t soldier_frames_death5[] = {
    { AI_MOVE, -10, 0 }, { AI_MOVE, -8, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death5 = { 441, 464, soldier_frames_death5, soldier_dead };
static struct mframe_t soldier_frames_death6[] = {
    { AI_MOVE, -5, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t soldier_move_death6 = { 465, 474, soldier_frames_death6, soldier_dead };

/* the MZ2_SOLDIER_* muzzle flash of each soldier_fireN */
static int blaster_flash[] = { 39, 40, 83, 86, 89, 92, 95, 98 };
static int shotgun_flash[] = { 41, 42, 84, 87, 90, 93, 96, 99 };
static int machinegun_flash[] = { 43, 44, 85, 88, 91, 94, 97, 100 };

#ifdef SOLDIER_BSS_ALIGN
/* the original linker started .bss on an 8-byte boundary; tools/ovllink.py uses 4 */
static int BssAlign = 0;
#endif

static void soldier_pain(struct edict_s *self)
{
    int r;

    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum |= 1;
    if (Iface.level->time < self->pain_debounce_time) {
        if (self->velocity.vy < -1200
            && (self->monsterinfo.currentmove == &soldier_move_pain1
                || self->monsterinfo.currentmove == &soldier_move_pain2
                || self->monsterinfo.currentmove == &soldier_move_pain3))
            self->monsterinfo.currentmove = &soldier_move_pain4;
        return;
    }
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain_ss, self->Extra.Creature);
    if (self->velocity.vy < -1200) {
        self->monsterinfo.currentmove = &soldier_move_pain4;
        return;
    }
    if ((*Iface.skill)->value == 3)
        return;
    r = Iface.rand();
    if (r < 10813)
        self->monsterinfo.currentmove = &soldier_move_pain1;
    else if (r < 21626)
        self->monsterinfo.currentmove = &soldier_move_pain2;
    else
        self->monsterinfo.currentmove = &soldier_move_pain3;
}

static void soldier_fire(struct edict_s *self, int flash_number)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT forward;
    struct VECT right;
    struct VECT up;
    struct VECT aim;
    struct SVECT dir;
    struct VECT end;
    struct SVECT vec;
    long r;
    long u;
    int flash_index;

    Cre = self->Extra.Creature;
    if (self->s.skinnum < 2)
        flash_index = blaster_flash[flash_number];
    else if (self->s.skinnum < 4)
        flash_index = shotgun_flash[flash_number];
    else
        flash_index = machinegun_flash[flash_number];
    Iface.AngleVectors(&self->sangles, &forward, &right, 0);
    vec = Iface.GetSubModXYZ(Cre->ModelHdr, MuzzleOff, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &vec, &vec);
    start.vx = Cre->Draw.Location.vx + vec.vx;
    start.vy = Cre->Draw.Location.vy + vec.vy;
    start.vz = Cre->Draw.Location.vz + vec.vz;
    if (flash_number == 5 || flash_number == 6) {
        aim.vx = forward.vx;
        aim.vy = forward.vy;
        aim.vz = forward.vz;
    } else {
        end.vx = self->enemy->sorigin.vx;
        end.vy = self->enemy->sorigin.vy;
        end.vz = self->enemy->sorigin.vz;
        end.vy += self->enemy->viewheight;
        aim.vx = end.vx - start.vx;
        aim.vy = end.vy - start.vy;
        aim.vz = end.vz - start.vz;
        Iface.vectoangles(&aim, &dir);
        Iface.AngleVectors(&dir, &forward, &right, &up);
        r = (Iface.rand() - 16384) * 2000 / 32768;
        u = (Iface.rand() - 16384) * 1000 / 32768;
        Iface.VectorMA(&start, 8192, &forward, &end);
        Iface.VectorMA(&end, r, &right, &end);
        Iface.VectorMA(&end, u, &up, &end);
        aim.vx = end.vx - start.vx;
        aim.vy = end.vy - start.vy;
        aim.vz = end.vz - start.vz;
        Iface.VectorNormalize(&aim);
    }
    if (self->s.skinnum < 2)
        Iface.monster_fire_blaster(self, &start, &aim, 5, 600, flash_index, 8);
    else if (self->s.skinnum < 4) {
        Iface.SoundByCreature(sound_shotgun, self->Extra.Creature);
        Iface.monster_fire_shotgun(self, &start, &aim, 2, 1, 1000, 500, 12, flash_index);
    } else {
        if (!(self->monsterinfo.aiflags & 0x80))
            self->monsterinfo.pausetime = Iface.level->time + (3 + Iface.rand() % 8);
        Iface.SoundByCreature(sound_machinegun, self->Extra.Creature);
        Iface.monster_fire_bullet(self, &start, &aim, 2, 4, 300, 500, flash_index);
        if (Iface.level->time >= self->monsterinfo.pausetime)
            self->monsterinfo.aiflags &= ~0x80;
        else
            self->monsterinfo.aiflags |= 0x80;
    }
}

static void soldier_stand(struct edict_s *self)
{
    if (self->monsterinfo.currentmove == &soldier_move_stand3 || Iface.rand() < 26214)
        self->monsterinfo.currentmove = &soldier_move_stand1;
    else
        self->monsterinfo.currentmove = &soldier_move_stand3;
}

static void soldier_idle(struct edict_s *self)
{
    if (Iface.rand() > 26214)
        Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void soldier_cock(struct edict_s *self)
{
    Iface.SoundByCreature(sound_cock, self->Extra.Creature);
}

static void soldier_walk1_random(struct edict_s *self)
{
    if (Iface.rand() > 3277)
        self->monsterinfo.nextframe = 215;
}

static void soldier_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1) {
        self->monsterinfo.currentmove = &soldier_move_stand1;
        return;
    }
    if (self->monsterinfo.currentmove == &soldier_move_walk1
        || self->monsterinfo.currentmove == &soldier_move_walk2
        || self->monsterinfo.currentmove == &soldier_move_start_run)
        self->monsterinfo.currentmove = &soldier_move_run;
    else
        self->monsterinfo.currentmove = &soldier_move_start_run;
}

static void soldier_fire1(struct edict_s *self)
{
    soldier_fire(self, 0);
}

static void soldier_attack1_refire1(struct edict_s *self)
{
    if (self->s.skinnum > 1)
        return;
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
        return;
    if (((*Iface.skill)->value == 3 && Iface.rand() < 16384) || Iface.range(self, self->enemy) == 0)
        self->monsterinfo.nextframe = 1;
    else
        self->monsterinfo.nextframe = 9;
}

static void soldier_attack1_refire2(struct edict_s *self)
{
    if (self->s.skinnum < 2)
        return;
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
        return;
    if (((*Iface.skill)->value == 3 && Iface.rand() < 16384) || Iface.range(self, self->enemy) == 0)
        self->monsterinfo.nextframe = 1;
}

static void soldier_fire2(struct edict_s *self)
{
    soldier_fire(self, 1);
}

static void soldier_attack2_refire1(struct edict_s *self)
{
    if (self->s.skinnum > 1)
        return;
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
        return;
    if (((*Iface.skill)->value == 3 && Iface.rand() < 16384) || Iface.range(self, self->enemy) == 0)
        self->monsterinfo.nextframe = 15;
    else
        self->monsterinfo.nextframe = 27;
}

static void soldier_attack2_refire2(struct edict_s *self)
{
    if (self->s.skinnum < 2)
        return;
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
        return;
    if (((*Iface.skill)->value == 3 && Iface.rand() < 16384) || Iface.range(self, self->enemy) == 0)
        self->monsterinfo.nextframe = 15;
}

static void soldier_duck_down(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 0x800)
        return;
    self->monsterinfo.aiflags |= 0x800;
    self->takedamage = 1;
    self->monsterinfo.pausetime = Iface.level->time + 10;
}

static void soldier_duck_up(struct edict_s *self)
{
    self->monsterinfo.aiflags &= ~0x800;
    self->takedamage = 2;
}

static void soldier_fire3(struct edict_s *self)
{
    soldier_duck_down(self);
    soldier_fire(self, 2);
}

static void soldier_attack3_refire(struct edict_s *self)
{
    if (Iface.level->time + 4 < self->monsterinfo.pausetime)
        self->monsterinfo.nextframe = 32;
}

static void soldier_fire4(struct edict_s *self)
{
    soldier_fire(self, 3);
}

static void soldier_fire8(struct edict_s *self)
{
    soldier_fire(self, 7);
}

static void soldier_attack6_refire(struct edict_s *self)
{
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
        return;
    if (Iface.range(self, self->enemy) < 2)
        return;
    if ((*Iface.skill)->value == 3)
        self->monsterinfo.nextframe = 111;
}

static void soldier_duck_hold(struct edict_s *self)
{
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~0x80;
    else
        self->monsterinfo.aiflags |= 0x80;
}

static void soldier_fire6(struct edict_s *self)
{
    soldier_fire(self, 5);
}

static void soldier_fire7(struct edict_s *self)
{
    soldier_fire(self, 6);
}

static void soldier_dead(struct edict_s *self)
{
    self->nextthink = 0;
    self->movetype = 7;
    self->svflags |= 2;
}

void Soldier_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->Control < 0)
        Iface.MuzzleFlash(Cre, MuzzleOff, 120);
}

static void soldier_walk(struct edict_s *self)
{
    if (Iface.rand() < 16384)
        self->monsterinfo.currentmove = &soldier_move_walk1;
    else
        self->monsterinfo.currentmove = &soldier_move_walk2;
}

static void soldier_attack(struct edict_s *self)
{
    if (self->s.skinnum < 4) {
        if (Iface.rand() < 16384)
            self->monsterinfo.currentmove = &soldier_move_attack1;
        else
            self->monsterinfo.currentmove = &soldier_move_attack2;
    } else
        self->monsterinfo.currentmove = &soldier_move_attack4;
}

static void soldier_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight1, self->Extra.Creature);
    if ((*Iface.skill)->value > 0 && Iface.range(self, self->enemy) >= 2) {
        if (Iface.rand() > 16384)
            self->monsterinfo.currentmove = &soldier_move_attack6;
    }
}

static void soldier_dodge(struct edict_s *self)
{
}

static void soldier_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point)
{
    int n;

    if (((struct CREATURE *)self->Extra.Creature)->Health <= self->gib_health) {
        self->nextthink = 0;
        self->deadflag = 2;
        self->movetype = 7;
        self->svflags |= 2;
        return;
    }
    if (self->deadflag == 2)
        return;
    self->deadflag = 2;
    self->takedamage = 1;
    self->s.skinnum |= 1;
    Iface.SoundByCreature(sound_death_ss, self->Extra.Creature);
    n = Iface.rand() % 4;
    if (n == 0)
        self->monsterinfo.currentmove = &soldier_move_death1;
    else if (n == 1)
        self->monsterinfo.currentmove = &soldier_move_death2;
    else if (n == 2)
        self->monsterinfo.currentmove = &soldier_move_death5;
    else
        self->monsterinfo.currentmove = &soldier_move_death6;
}

void SP_monster_soldier_x(struct edict_s *self)
{
    self->monsterinfo.scale = 12;
    self->mass = 100;
    self->pain = soldier_pain;
    self->die = soldier_die;
    self->monsterinfo.stand = soldier_stand;
    self->monsterinfo.walk = soldier_walk;
    self->monsterinfo.run = soldier_run;
    self->monsterinfo.dodge = soldier_dodge;
    self->monsterinfo.attack = soldier_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = soldier_sight;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 133);
    self->monsterinfo.stand(self);
    Iface.walkmonster_start(self);
}

void SP_monster_soldier_light(struct edict_s *self)
{
    SP_monster_soldier_x(self);
    self->s.skinnum = 0;
    ((struct CREATURE *)self->Extra.Creature)->Health = 20;
    self->gib_health = -30;
}

void SP_monster_soldier(struct edict_s *self)
{
    SP_monster_soldier_x(self);
    self->s.skinnum = 2;
    ((struct CREATURE *)self->Extra.Creature)->Health = 30;
    self->gib_health = -30;
}

void SP_monster_soldier_ss(struct edict_s *self)
{
    SP_monster_soldier_x(self);
    self->s.skinnum = 4;
    ((struct CREATURE *)self->Extra.Creature)->Health = 40;
    self->gib_health = -30;
}

static const char UnknownType[24] = "Unknown soldier type\r\n\0" SOLDIER_RDATA_PAD;

void SP_Soldier(struct edict_s *self)
{
    switch (((struct CREATURE *)self->Extra.Creature)->TypeIdent) {
    case 18:
        SP_monster_soldier(self);
        break;
    case 19:
        SP_monster_soldier_light(self);
        break;
    case 20:
        SP_monster_soldier_ss(self);
        break;
    default:
        Iface.PrintF(UnknownType);
    }
}
