/* Jorg (Quake II m_boss31.c), with the Makron he carries (m_boss32.c) and the
 * Makron torso/legs pieces (as in cre_rider3, cre_rider2) */
#include "ovl.h"

#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: JorgAiTable */
#define JORG_MAKRONTOSS 2
#define JORG_DEAD 3
#define JORG_BFG 4
#define JORG_FIREBULLET 6
#define JORG_REATTACK1 7
#define JORG_ATTACK1 8
#define JORG_IDLE 9
#define JORG_STEP_LEFT 10
#define JORG_STEP_RIGHT 11
#define JORG_DEATH_HIT 12
#define JORG_RUN 13
#define JORG_EXPLODE 14

/* mframe_t.aifunc_local_index: MakronAiTable */
#define MAKRON_RAILGUN 1
#define MAKRON_SAVELOC 2
#define MAKRON_HYPERBLASTER 3
#define MAKRON_STEP_LEFT 4
#define MAKRON_STEP_RIGHT 5
#define MAKRON_BFG 6
#define MAKRON_DEAD 7
#define MAKRON_POPUP 8
#define MAKRON_TAUNT 9
#define MAKRON_RUN 10
#define MAKRON_HIT 11
#define MAKRON_BRAINSPLORCH 12
#define MAKRON_PRERAILGUN 13

#define CLASS_JORG 82
#define CLASS_MAKRON 83

void PreInit_Jorg(struct GLOBMODHDR *Hdr);
static void jorgBFG(struct edict_s *self);
static void makronBFG(struct edict_s *self);
static void MakronRailgun(struct edict_s *self);
static void MakronHyperblaster(struct edict_s *self);
static void MakronToss(struct edict_s *self);

/* the small ones, which gcc emits at the end in this order */
static void jorg_dead(struct edict_s *self);
static void jorg_firebullet(struct edict_s *self);
static void jorg_reattack1(struct edict_s *self);
static void jorg_attack1(struct edict_s *self);
static void jorg_idle(struct edict_s *self);
static void jorg_step_left(struct edict_s *self);
static void jorg_step_right(struct edict_s *self);
static void jorg_death_hit(struct edict_s *self);
static void jorg_run(struct edict_s *self);
static void jorg_explode(struct edict_s *self);
void SP_monster_jorg(struct edict_s *self);
static void MakronSaveloc(struct edict_s *self);
static void makron_step_left(struct edict_s *self);
static void makron_step_right(struct edict_s *self);
static void makron_dead(struct edict_s *self);
static void makron_popup(struct edict_s *self);
static void makron_taunt(struct edict_s *self);
static void makron_run(struct edict_s *self);
static void makron_hit(struct edict_s *self);
static void makron_brainsplorch(struct edict_s *self);
static void makron_prerailgun(struct edict_s *self);
void SP_monster_makron(struct edict_s *self);
static void jorg_search(struct edict_s *self);
static void jorg_stand(struct edict_s *self);
static void jorg_walk(struct edict_s *self);
static void jorg_attack(struct edict_s *self);
static void jorg_die(struct edict_s *self);
static void makron_stand(struct edict_s *self);
static void makron_walk(struct edict_s *self);
static void makron_sight(struct edict_s *self);
static void makron_attack(struct edict_s *self);
void Boss32_die(struct edict_s *self);
void Boss32_pain(struct edict_s *self);
void Boss32Top_think(struct edict_s *self);
void Boss32Top_init(struct edict_s *self);
void Boss32Bottom_think(struct edict_s *self);
void Boss32Bottom_init(struct edict_s *self);
static void makron_die(struct edict_s *self);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_jorg, 0, PreInit_Jorg, AnimRemap };

static unsigned char ExplodeCount = 0;

static struct NEWANIMREMAP AnimRemap[] = {
    { 112, 162, {"Stand"}, 0, 0 },
    { 163, 187, {"Walk"}, 0, 0 },
    { 87, 111, {"Pain3"}, 0, 0 },
    { 84, 86, {"Pain2"}, 0, 0 },
    { 81, 83, {"Pain1"}, 0, 0 },
    { 31, 80, {"Death"}, 0, 0 },
    { 18, 30, {"Attack2"}, 0, 0 },
    { 0, 7, {"Startattack"}, 0, 0 },
    { 8, 13, {"Attack"}, 0, 0 },
    { 14, 17, {"Endattack"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };

static struct VECT vec3_origin;
static int Unused0; /* bss the original has */

static int sound_pain1;
static int sound_pain2;
static int sound_pain3;
static int sound_idle;
static int sound_death;
static int sound_search1;
static int sound_search2;
static int sound_search3;
static int sound_attack1;
static int sound_attack2;
static int sound_firegun;
static int sound_step_left;
static int sound_step_right;
static int sound_death_hit;

static int sound_pain4;
static int sound_pain5;
static int sound_pain6;
static int sound_makron_death;
static int sound_makron_step_left;
static int sound_makron_step_right;
static int sound_attack_bfg;
static int sound_brainsplorch;
static int sound_prerailgun;
static int sound_popup;
static int sound_taunt1;
static int sound_taunt2;
static int sound_taunt3;
static int sound_hit;

static struct MODSUBOFF LeftChainGunMuzzle;
static struct MODSUBOFF RightChainGunMuzzle;
static struct MODSUBOFF BFGMuzzle;
static struct MODSUBOFF BlasterMuzzle;
static struct MODSUBOFF RailGunMuzzle;
static int Unused1[2];
static struct ai_local_table_t JorgAiTable[32];
static struct ai_local_table_t MakronAiTable[32];
static int Unused2[2];

void PreInit_Jorg(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Boss31(Jorg)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Boss31");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Boss31");
        Iface.PrintF("Validated Creature Interface for %s\n", "Boss31");
        Hdr->ShadowVerts = ShadowVerts;
        JorgAiTable[0].thinkfunc = 0;
        JorgAiTable[JORG_MAKRONTOSS].thinkfunc = MakronToss;
        JorgAiTable[JORG_DEAD].thinkfunc = jorg_dead;
        JorgAiTable[JORG_BFG].thinkfunc = jorgBFG;
        JorgAiTable[JORG_FIREBULLET].thinkfunc = jorg_firebullet;
        JorgAiTable[JORG_REATTACK1].thinkfunc = jorg_reattack1;
        JorgAiTable[JORG_ATTACK1].thinkfunc = jorg_attack1;
        JorgAiTable[JORG_IDLE].thinkfunc = jorg_idle;
        JorgAiTable[JORG_STEP_LEFT].thinkfunc = jorg_step_left;
        JorgAiTable[JORG_STEP_RIGHT].thinkfunc = jorg_step_right;
        JorgAiTable[JORG_DEATH_HIT].thinkfunc = jorg_death_hit;
        JorgAiTable[JORG_RUN].thinkfunc = jorg_run;
        JorgAiTable[JORG_EXPLODE].thinkfunc = jorg_explode;
        Iface.SetMyAiTable(CLASS_JORG, JorgAiTable);
        MakronAiTable[0].thinkfunc = 0;
        MakronAiTable[MAKRON_RAILGUN].thinkfunc = MakronRailgun;
        MakronAiTable[MAKRON_SAVELOC].thinkfunc = MakronSaveloc;
        MakronAiTable[MAKRON_HYPERBLASTER].thinkfunc = MakronHyperblaster;
        MakronAiTable[MAKRON_STEP_LEFT].thinkfunc = makron_step_left;
        MakronAiTable[MAKRON_STEP_RIGHT].thinkfunc = makron_step_right;
        MakronAiTable[MAKRON_BFG].thinkfunc = makronBFG;
        MakronAiTable[MAKRON_DEAD].thinkfunc = makron_dead;
        MakronAiTable[MAKRON_POPUP].thinkfunc = makron_popup;
        MakronAiTable[MAKRON_TAUNT].thinkfunc = makron_taunt;
        MakronAiTable[MAKRON_RUN].thinkfunc = makron_run;
        MakronAiTable[MAKRON_HIT].thinkfunc = makron_hit;
        MakronAiTable[MAKRON_BRAINSPLORCH].thinkfunc = makron_brainsplorch;
        MakronAiTable[MAKRON_PRERAILGUN].thinkfunc = makron_prerailgun;
        Iface.SetMyAiTable(CLASS_MAKRON, MakronAiTable);
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_pain1"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_pain1"});
        sound_pain3 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_pain1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_idle1"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_deth1"});
        sound_search1 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_srch2"});
        sound_search2 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_srch2"});
        sound_search3 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_srch2"});
        sound_attack1 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_atck1"});
        sound_attack2 = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_atck2"});
        sound_firegun = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_xfire"});
        sound_step_left = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_step1"});
        sound_step_right = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_step2"});
        sound_death_hit = Iface.GetSoundNum((struct IDENTITYNAME){"bs3_dhit"});
        if (!sound_step_left || !sound_step_right) {
            sound_step_left |= sound_step_right;
            sound_step_right |= sound_step_left;
        }
        /* MakronPrecache */
        sound_pain4 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain3"});
        sound_pain5 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain2"});
        sound_pain6 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain1"});
        sound_makron_death = Iface.GetSoundNum((struct IDENTITYNAME){"mak_death"});
        sound_makron_step_left = Iface.GetSoundNum((struct IDENTITYNAME){"mak_step1"});
        sound_makron_step_right = Iface.GetSoundNum((struct IDENTITYNAME){"mak_step2"});
        sound_attack_bfg = Iface.GetSoundNum((struct IDENTITYNAME){"mak_bfg_fire"});
        sound_brainsplorch = Iface.GetSoundNum((struct IDENTITYNAME){"mak_brain1"});
        sound_prerailgun = Iface.GetSoundNum((struct IDENTITYNAME){"mak_rail_up"});
        sound_popup = Iface.GetSoundNum((struct IDENTITYNAME){"mak_popup"});
        sound_taunt1 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_voice4"});
        sound_taunt2 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_voice3"});
        sound_taunt3 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_voice"});
        sound_hit = Iface.GetSoundNum((struct IDENTITYNAME){"mak_bhit"});
        if (!sound_taunt2)
            sound_taunt2 = sound_taunt1;
        if (!sound_taunt3)
            sound_taunt3 = sound_taunt1;
        if (!sound_pain4)
            sound_pain4 = sound_pain5;
        if (!sound_pain6)
            sound_pain6 = sound_pain5;
        if (!sound_attack_bfg)
            sound_attack_bfg = Iface.GetSoundNum((struct IDENTITYNAME){"wep_bfg__f1y"});
        if (!sound_makron_step_left || !sound_makron_step_right) {
            sound_makron_step_left |= sound_makron_step_right;
            sound_makron_step_right |= sound_makron_step_left;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 165, 161, &BFGMuzzle))
            Iface.PrintF("Jorg BFG muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 419, 422, &LeftChainGunMuzzle))
            Iface.PrintF("Jorg left ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 226, 228, &RightChainGunMuzzle))
            Iface.PrintF("Jorg right ChainGun muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Boss31(Jorg)\n");
}

static struct mframe_t jorg_frames_stand[] = {
    {AI_STAND, 0, JORG_IDLE}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, JORG_STEP_LEFT}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, JORG_STEP_RIGHT}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, JORG_STEP_LEFT},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, JORG_STEP_RIGHT},
};
static struct mmove_t jorg_move_stand = { 112, 162, jorg_frames_stand, 0 };

static struct mframe_t jorg_frames_run[] = {
    {AI_RUN, 17, JORG_STEP_LEFT}, {AI_RUN, 0, 0}, {AI_RUN, 0, 0}, {AI_RUN, 0, 0}, {AI_RUN, 12, 0}, {AI_RUN, 8, 0},
    {AI_RUN, 10, 0}, {AI_RUN, 33, 0}, {AI_RUN, 0, JORG_STEP_RIGHT}, {AI_RUN, 0, 0}, {AI_RUN, 0, 0}, {AI_RUN, 9, 0},
    {AI_RUN, 9, 0}, {AI_RUN, 9, 0},
};
static struct mmove_t jorg_move_run = { 168, 181, jorg_frames_run, 0 };

static struct mframe_t jorg_frames_start_walk[] = {
    {AI_WALK, 5, 0}, {AI_WALK, 6, 0}, {AI_WALK, 7, 0}, {AI_WALK, 9, 0}, {AI_WALK, 15, 0},
};
static struct mmove_t jorg_move_start_walk = { 163, 167, jorg_frames_start_walk, 0 };

static struct mframe_t jorg_frames_walk[] = {
    {AI_WALK, 17, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 12, 0}, {AI_WALK, 8, 0},
    {AI_WALK, 10, 0}, {AI_WALK, 33, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 9, 0},
    {AI_WALK, 9, 0}, {AI_WALK, 9, 0},
};
static struct mmove_t jorg_move_walk = { 168, 181, jorg_frames_walk, 0 };

static struct mframe_t jorg_frames_end_walk[] = {
    {AI_WALK, 11, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 8, 0}, {AI_WALK, -8, 0},
};
static struct mmove_t jorg_move_end_walk = { 182, 187, jorg_frames_end_walk, 0 };

static struct mframe_t jorg_frames_pain3[] = {
    {AI_MOVE, -28, 0}, {AI_MOVE, -6, 0}, {AI_MOVE, -3, JORG_STEP_LEFT}, {AI_MOVE, -9, 0}, {AI_MOVE, 0, JORG_STEP_RIGHT}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -7, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, -11, 0}, {AI_MOVE, -4, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 10, 0}, {AI_MOVE, 11, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 10, 0},
    {AI_MOVE, 3, 0}, {AI_MOVE, 10, 0}, {AI_MOVE, 7, JORG_STEP_LEFT}, {AI_MOVE, 17, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, JORG_STEP_RIGHT},
};
static struct mmove_t jorg_move_pain3 = { 87, 111, jorg_frames_pain3, jorg_run };

static struct mframe_t jorg_frames_pain2[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t jorg_move_pain2 = { 84, 86, jorg_frames_pain2, jorg_run };

static struct mframe_t jorg_frames_pain1[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t jorg_move_pain1 = { 81, 83, jorg_frames_pain1, jorg_run };

static struct mframe_t jorg_frames_death1[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, JORG_EXPLODE},
    {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE},
    {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE},
    {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE}, {AI_MOVE, 0, JORG_EXPLODE},
    {AI_MOVE, 0, JORG_MAKRONTOSS}, {AI_MOVE, 0, JORG_EXPLODE},
};
static struct mmove_t jorg_move_death1 = { 31, 80, jorg_frames_death1, jorg_dead };

static struct mframe_t jorg_frames_attack2[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, JORG_BFG}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0},
};
static struct mmove_t jorg_move_attack2 = { 18, 30, jorg_frames_attack2, jorg_run };

static struct mframe_t jorg_frames_start_attack1[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
};
static struct mmove_t jorg_move_start_attack1 = { 0, 7, jorg_frames_start_attack1, jorg_attack1 };

static struct mframe_t jorg_frames_attack1[] = {
    {AI_CHARGE, 0, JORG_FIREBULLET}, {AI_CHARGE, 0, JORG_FIREBULLET}, {AI_CHARGE, 0, JORG_FIREBULLET}, {AI_CHARGE, 0, JORG_FIREBULLET}, {AI_CHARGE, 0, JORG_FIREBULLET}, {AI_CHARGE, 0, JORG_FIREBULLET},
};
static struct mmove_t jorg_move_attack1 = { 8, 13, jorg_frames_attack1, jorg_reattack1 };

static struct mframe_t jorg_frames_end_attack1[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t jorg_move_end_attack1 = { 14, 17, jorg_frames_end_attack1, jorg_run };

static void jorg_pain(struct edict_s *self, struct edict_s *other, int kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    if (damage <= 40)
        if (Iface.rand() < 19961)
            return;
    if (self->s.frame >= 0 && self->s.frame <= 7)
        if (Iface.rand() < 164)
            return;
    if (self->s.frame >= 8 && self->s.frame <= 13)
        if (Iface.rand() < 3)
            return;
    if (self->s.frame >= 18 && self->s.frame <= 25)
        if (Iface.rand() < 164)
            return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 50) {
        Iface.SoundByCreature(sound_pain1, self->Extra.Creature);
        self->monsterinfo.currentmove = &jorg_move_pain1;
    } else if (damage <= 100) {
        Iface.SoundByCreature(sound_pain2, self->Extra.Creature);
        self->monsterinfo.currentmove = &jorg_move_pain2;
    } else {
        if (Iface.rand() < 9831) {
            Iface.SoundByCreature(sound_pain3, self->Extra.Creature);
            self->monsterinfo.currentmove = &jorg_move_pain3;
        }
    }
}

static void jorgBFG(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct VECT vec;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, BFGMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    vec.vx = self->enemy->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz;
    vec.vy += self->enemy->viewheight;
    dir.vx = vec.vx - start.vx;
    dir.vy = vec.vy - start.vy;
    dir.vz = vec.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.SoundByCreature(sound_attack2, self->Extra.Creature);
    Iface.monster_fire_bfg(self, &start, &dir, 50, 300, 100, 200, 132);
}

static void jorg_firebullet_right(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT forward;
    struct VECT target;
    struct VECT start;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RightChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    Iface.VectorMA(&self->enemy->sorigin, -2, &self->enemy->velocity, &target);
    target.vy += self->enemy->viewheight;
    forward.vx = target.vx - start.vx;
    forward.vy = target.vy - start.vy;
    forward.vz = target.vz - start.vz;
    Iface.VectorNormalize(&forward);
    Iface.monster_fire_bullet(self, &start, &forward, 6, 4, 300, 500, 0);
}

static void jorg_firebullet_left(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT forward;
    struct VECT target;
    struct VECT start;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, LeftChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    Iface.VectorMA(&self->enemy->sorigin, -2, &self->enemy->velocity, &target);
    target.vy += self->enemy->viewheight;
    forward.vx = target.vx - start.vx;
    forward.vy = target.vy - start.vy;
    forward.vz = target.vz - start.vz;
    Iface.VectorNormalize(&forward);
    Iface.monster_fire_bullet(self, &start, &forward, 6, 4, 300, 500, 0);
}

static int Jorg_CheckAttack(struct edict_s *self)
{
    struct VECT spot1;
    struct VECT spot2;
    struct VECT temp;
    int chance;
    int enemy_range;

    if (CRE(self->enemy)->Health > 0) {
        spot1.vx = self->sorigin.vx;
        spot1.vy = self->sorigin.vy;
        spot1.vz = self->sorigin.vz;
        spot1.vy += self->viewheight;
        spot2.vx = self->enemy->sorigin.vx;
        spot2.vy = self->enemy->sorigin.vy;
        spot2.vz = self->enemy->sorigin.vz;
        spot2.vy += self->enemy->viewheight;
        Iface.gitrace(&spot1, 0, 0, &spot2, self, 0x2000019, CRE(self)->Box.MoveSpace);
        if (Iface.Trace->ent != self->enemy)
            return 0;
    }
    Iface.infront(self, self->enemy);
    enemy_range = Iface.range(self, self->enemy);
    temp.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    temp.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    temp.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    self->ideal_yaw = Iface.vectoyaw(&temp);
    if (enemy_range == 0) {
        if (self->monsterinfo.melee)
            self->monsterinfo.attack_state = 3;
        else
            self->monsterinfo.attack_state = 4;
        return 1;
    }
    if (!self->monsterinfo.attack)
        return 0;
    if (Iface.level->time < self->monsterinfo.attack_finished)
        return 0;
    if (enemy_range == 3)
        return 0;
    if (self->monsterinfo.aiflags & 1)
        chance = 13106;
    else if (enemy_range == 0)
        chance = 26213;
    else if (enemy_range == 1)
        chance = 13106;
    else if (enemy_range == 2)
        chance = 6553;
    else
        return 0;
    if (Iface.rand() < chance) {
        self->monsterinfo.attack_state = 4;
        self->monsterinfo.attack_finished = Iface.level->time + Iface.rand() * 20 / 32767;
        return 1;
    }
    if (self->flags & 1) {
        if (Iface.rand() < 9830)
            self->monsterinfo.attack_state = 2;
        else
            self->monsterinfo.attack_state = 1;
    }
    return 0;
}

static struct mframe_t makron_frames_stand[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
};
static struct mmove_t makron_move_stand = { 414, 473, makron_frames_stand, 0 };

static struct mframe_t makron_frames_run[] = {
    {AI_RUN, 3, MAKRON_STEP_LEFT}, {AI_RUN, 12, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, MAKRON_STEP_RIGHT}, {AI_RUN, 6, 0},
    {AI_RUN, 12, 0}, {AI_RUN, 9, 0}, {AI_RUN, 6, 0}, {AI_RUN, 12, 0},
};
static struct mmove_t makron_move_run = { 477, 486, makron_frames_run, 0 };

/* unused: makron_move_walk has makron_frames_run, as in Quake II */
static struct mframe_t makron_frames_walk[] = {
    {AI_WALK, 3, MAKRON_STEP_LEFT}, {AI_WALK, 12, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, MAKRON_STEP_RIGHT}, {AI_WALK, 6, 0},
    {AI_WALK, 12, 0}, {AI_WALK, 9, 0}, {AI_WALK, 6, 0}, {AI_WALK, 12, 0},
};
static struct mmove_t makron_move_walk = { 477, 486, makron_frames_run, 0 };

static struct mframe_t makron_frames_pain6[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, MAKRON_POPUP}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, MAKRON_TAUNT},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_pain6 = { 387, 413, makron_frames_pain6, makron_run };

static struct mframe_t makron_frames_pain5[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_pain5 = { 383, 386, makron_frames_pain5, makron_run };

static struct mframe_t makron_frames_pain4[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_pain4 = { 379, 382, makron_frames_pain4, makron_run };

static struct mframe_t makron_frames_death2[] = {
    {AI_MOVE, -15, 0}, {AI_MOVE, 3, 0}, {AI_MOVE, -12, 0}, {AI_MOVE, 0, MAKRON_STEP_LEFT}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 11, 0}, {AI_MOVE, 12, 0}, {AI_MOVE, 11, MAKRON_STEP_RIGHT},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 5, 0}, {AI_MOVE, 7, 0}, {AI_MOVE, 6, MAKRON_STEP_LEFT},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, 2, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -6, 0},
    {AI_MOVE, -4, 0}, {AI_MOVE, -6, MAKRON_STEP_RIGHT}, {AI_MOVE, -4, 0}, {AI_MOVE, -4, MAKRON_STEP_LEFT}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, -5, 0}, {AI_MOVE, -3, MAKRON_STEP_RIGHT}, {AI_MOVE, -8, 0},
    {AI_MOVE, -3, MAKRON_STEP_LEFT}, {AI_MOVE, -7, 0}, {AI_MOVE, -4, 0}, {AI_MOVE, -4, MAKRON_STEP_RIGHT}, {AI_MOVE, -6, 0}, {AI_MOVE, -7, 0},
    {AI_MOVE, 0, MAKRON_STEP_LEFT}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 2, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 27, MAKRON_HIT}, {AI_MOVE, 26, 0}, {AI_MOVE, 0, MAKRON_BRAINSPLORCH}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_death2 = { 251, 345, makron_frames_death2, makron_dead };

static struct mframe_t makron_frames_death3[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_death3 = { 346, 365, makron_frames_death3, 0 };

static struct mframe_t makron_frames_sight[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_sight = { 188, 200, makron_frames_sight, makron_run };

static struct mframe_t makron_frames_attack3[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, MAKRON_BFG}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_attack3 = { 201, 208, makron_frames_attack3, makron_run };

static struct mframe_t makron_frames_attack4[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER},
    {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER},
    {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER},
    {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, MAKRON_HYPERBLASTER}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_attack4 = { 209, 234, makron_frames_attack4, makron_run };

static struct mframe_t makron_frames_attack5[] = {
    {AI_CHARGE, 0, MAKRON_PRERAILGUN}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, MAKRON_SAVELOC}, {AI_MOVE, 0, MAKRON_RAILGUN}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t makron_move_attack5 = { 235, 250, makron_frames_attack5, makron_run };

static void makronBFG(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct VECT vec;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, BFGMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    vec.vx = self->enemy->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz;
    vec.vy += self->enemy->viewheight;
    dir.vx = vec.vx - start.vx;
    dir.vy = vec.vy - start.vy;
    dir.vz = vec.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.SoundByCreature(sound_attack_bfg, self->Extra.Creature);
    Iface.monster_fire_bfg(self, &start, &dir, 50, 300, 100, 300, 101);
}

static void MakronRailgun(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RailGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    dir.vx = self->pos1.vx - start.vx;
    dir.vy = self->pos1.vy - start.vy;
    dir.vz = self->pos1.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_railgun(self, &start, &dir, 50, 100, 119);
    Iface.SoundByCreature(Iface.GetSoundNum((struct IDENTITYNAME){"wep_railgf1a"}), self->Extra.Creature);
}

static void MakronHyperblaster(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT dir;
    struct VECT vec;
    struct SVECT ang;
    struct VECT start;
    struct VECT forward;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, BlasterMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    if (self->enemy) {
        vec.vx = self->enemy->sorigin.vx;
        vec.vy = self->enemy->sorigin.vy;
        vec.vz = self->enemy->sorigin.vz;
        vec.vy += self->enemy->viewheight;
        vec.vx = vec.vx - start.vx;
        vec.vy = vec.vy - start.vy;
        vec.vz = vec.vz - start.vz;
        Iface.vectoangles(&vec, &ang);
        vec.vx = ang.vx;
        vec.vy = ang.vy;
        vec.vz = ang.vz;
        dir.vx = vec.vx;
    } else
        dir.vx = 0;
    if (self->s.frame <= 221)
        dir.vz = self->sangles.vz + (self->s.frame - 217) * 40960 / 360;
    else
        dir.vz = self->sangles.vz - (self->s.frame - 226) * 40960 / 360;
    dir.vy = 0;
    ang.vx = dir.vx;
    ang.vy = dir.vy;
    ang.vz = dir.vz;
    Iface.AngleVectors(&ang, &forward, 0, 0);
    Iface.monster_fire_blaster(self, &start, &forward, 15, 1000, 102, 8);
}

static void makron_pain(struct edict_s *self, struct edict_s *other, int kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    if (damage <= 25)
        if (Iface.rand() < 6553)
            return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 40) {
        Iface.SoundByCreature(sound_pain4, self->Extra.Creature);
        self->monsterinfo.currentmove = &makron_move_pain4;
    } else if (damage <= 110) {
        Iface.SoundByCreature(sound_pain5, self->Extra.Creature);
        self->monsterinfo.currentmove = &makron_move_pain5;
    } else {
        if (damage <= 150)
            if (Iface.rand() < 14746) {
                Iface.SoundByCreature(sound_pain6, self->Extra.Creature);
                self->monsterinfo.currentmove = &makron_move_pain6;
            } else if (Iface.rand() < 11469) {
                Iface.SoundByCreature(sound_pain6, self->Extra.Creature);
                self->monsterinfo.currentmove = &makron_move_pain6;
            }
    }
}

static int Makron_CheckAttack(struct edict_s *self)
{
    struct VECT spot1;
    struct VECT spot2;
    struct VECT temp;
    int chance;
    int enemy_range;

    if (CRE(self->enemy)->Health > 0) {
        spot1.vx = self->sorigin.vx;
        spot1.vy = self->sorigin.vy;
        spot1.vz = self->sorigin.vz;
        spot1.vy += self->viewheight;
        spot2.vx = self->enemy->sorigin.vx;
        spot2.vy = self->enemy->sorigin.vy;
        spot2.vz = self->enemy->sorigin.vz;
        spot2.vy += self->enemy->viewheight;
        Iface.gitrace(&spot1, 0, 0, &spot2, self, 0x2000019, CRE(self)->Box.MoveSpace);
        if (Iface.Trace->ent != self->enemy)
            return 0;
    }
    Iface.infront(self, self->enemy);
    enemy_range = Iface.range(self, self->enemy);
    temp.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    temp.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    temp.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    self->ideal_yaw = Iface.vectoyaw(&temp);
    if (enemy_range == 0) {
        if (self->monsterinfo.melee)
            self->monsterinfo.attack_state = 3;
        else
            self->monsterinfo.attack_state = 4;
        return 1;
    }
    if (!self->monsterinfo.attack)
        return 0;
    if (Iface.level->time < self->monsterinfo.attack_finished)
        return 0;
    if (enemy_range == 3)
        return 0;
    if (self->monsterinfo.aiflags & 1)
        chance = 13106;
    else if (enemy_range == 0)
        chance = 26213;
    else if (enemy_range == 1)
        chance = 13106;
    else if (enemy_range == 2)
        chance = 6553;
    else
        return 0;
    if (Iface.rand() < chance) {
        self->monsterinfo.attack_state = 4;
        self->monsterinfo.attack_finished = Iface.level->time + Iface.rand() * 2 / 32767;
        return 1;
    }
    if (self->flags & 1) {
        if (Iface.rand() < 9830)
            self->monsterinfo.attack_state = 2;
        else
            self->monsterinfo.attack_state = 1;
    }
    return 0;
}

void SP_monster_makron(struct edict_s *self)
{
    self->mass = 500;
    self->pain = makron_pain;
    self->die = makron_die;
    self->monsterinfo.stand = makron_stand;
    self->monsterinfo.walk = makron_walk;
    self->monsterinfo.run = makron_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = makron_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = makron_sight;
    self->monsterinfo.checkattack = Makron_CheckAttack;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &makron_move_sight;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void MakronSpawn(struct edict_s *self)
{
    struct VECT vec;
    struct edict_s *player;

    CRE(self)->WinData[0].NoDraw = 0;
    SP_monster_makron(self);
    player = Iface.level->sight_client;
    if (!player)
        return;
    vec.vx = player->sorigin.vx - self->sorigin.vx;
    vec.vy = player->sorigin.vy - self->sorigin.vy;
    vec.vz = player->sorigin.vz - self->sorigin.vz;
    self->sangles.vz = Iface.vectoyaw(&vec);
    Iface.VectorNormalize(&vec);
    Iface.VectorMA(&vec3_origin, 400, &vec, &self->velocity);
    self->velocity.vy = 200;
    self->groundentity = 0;
}

static void MakronToss(struct edict_s *self)
{
    struct CREDATA Data;
    struct CREATURE *Cre;
    struct edict_s *ent;

    Data.TypeIdent = 14;
    Data.Location.vx = CRE(self)->Box.Location.vx;
    Data.Location.vy = CRE(self)->Box.Location.vy;
    Data.Location.vz = CRE(self)->Box.Location.vz;
    Data.AngY = 0;
    Data.TargetIdent = 0;
    Data.SpawnFlags = 1024;
    do {
        Cre = Iface.SpawnGeneral(&Data, 0);
        if (Cre) {
            if (!Iface.GetModSubOffFrom2Verteces(Cre->ModelHdr, 431, 434, &BFGMuzzle))
                Iface.PrintF("Makron BFG muzzle points are from different submodels\r\n");
            if (!Iface.GetModSubOffFrom2Verteces(Cre->ModelHdr, 419, 422, &BlasterMuzzle))
                Iface.PrintF("Makron Blaster muzzle points are from different submodels\r\n");
            if (!Iface.GetModSubOffFrom2Verteces(Cre->ModelHdr, 225, 227, &RailGunMuzzle))
                Iface.PrintF("Makron RailGun muzzle points are from different submodels\r\n");
            Cre->WinData[0].NoDraw = 1;
            ent = Cre->EdictT;
            ent->nextthink = Iface.level->time + 2;
            ent->think = MakronSpawn;
            ent->target = self->target;
            ent->sorigin.vx = self->sorigin.vx;
            ent->sorigin.vy = self->sorigin.vy;
            ent->sorigin.vz = self->sorigin.vz;
        } else {
            Data.Location.vx = CRE(self)->Box.Location.vx;
            Data.Location.vy = CRE(self)->Box.Location.vy;
            Data.Location.vz = CRE(self)->Box.Location.vz;
            Data.Location.vx += (Iface.rand() - 16384) / 16;
            Data.Location.vz += (Iface.rand() - 16384) / 16;
        }
    } while (!Cre);
}

static void jorg_dead(struct edict_s *self)
{
    CRE(self)->Health = self->gib_health;
    self->nextthink = 0;
    self->takedamage = 0;
    self->movetype = 7;
    self->svflags |= 2;
}

static void jorg_firebullet(struct edict_s *self)
{
    jorg_firebullet_left(self);
    jorg_firebullet_right(self);
}

static void jorg_reattack1(struct edict_s *self)
{
    if (Iface.visible(self, self->enemy))
        if (Iface.rand() < 29490)
            self->monsterinfo.currentmove = &jorg_move_attack1;
        else
            self->monsterinfo.currentmove = &jorg_move_end_attack1;
    else
        self->monsterinfo.currentmove = &jorg_move_end_attack1;
}

static void jorg_attack1(struct edict_s *self)
{
    self->monsterinfo.currentmove = &jorg_move_attack1;
}

static void jorg_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void jorg_step_left(struct edict_s *self)
{
    Iface.SetShockData(4, 14, 4096, &self->sorigin, (struct POWER){ 2048, 8192 });
    Iface.SoundByCreature(sound_step_left, self->Extra.Creature);
}

static void jorg_step_right(struct edict_s *self)
{
    Iface.SetShockData(4, 14, 4096, &self->sorigin, (struct POWER){ 2048, 8192 });
    Iface.SoundByCreature(sound_step_right, self->Extra.Creature);
}

static void jorg_death_hit(struct edict_s *self)
{
    Iface.SoundByCreature(sound_death_hit, self->Extra.Creature);
}

static void jorg_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &jorg_move_stand;
    else
        self->monsterinfo.currentmove = &jorg_move_run;
}

static void jorg_explode(struct edict_s *self)
{
    struct VECT org;
    int area;

    if (++ExplodeCount & 1)
        return;
    area = CRE(self)->Box.Area;
    org.vx = self->sorigin.vx + (Iface.rand() - 16384) / 32;
    org.vy = self->sorigin.vy + (Iface.rand() - 16384) / 64 - 60;
    org.vz = self->sorigin.vz + (Iface.rand() - 16384) / 32;
    Iface.QuakeGenExplosion(&org, area, 4096, 0);
}

void SP_monster_jorg(struct edict_s *self)
{
    self->mass = 1000;
    self->pain = jorg_pain;
    self->die = jorg_die;
    self->monsterinfo.stand = jorg_stand;
    self->monsterinfo.walk = jorg_walk;
    self->monsterinfo.run = jorg_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = jorg_attack;
    self->monsterinfo.search = jorg_search;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = 0;
    self->monsterinfo.checkattack = Jorg_CheckAttack;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &jorg_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void MakronSaveloc(struct edict_s *self)
{
    self->pos1.vx = self->enemy->sorigin.vx;
    self->pos1.vy = self->enemy->sorigin.vy;
    self->pos1.vz = self->enemy->sorigin.vz;
    self->pos1.vy += self->enemy->viewheight;
}

static void makron_step_left(struct edict_s *self)
{
    Iface.SoundByCreature(sound_makron_step_left, self->Extra.Creature);
}

static void makron_step_right(struct edict_s *self)
{
    Iface.SoundByCreature(sound_makron_step_right, self->Extra.Creature);
}

static void makron_dead(struct edict_s *self)
{
    struct edict_s *e;

    self->nextthink = 0;
    self->takedamage = 0;
    self->movetype = 7;
    self->svflags |= 2;
    for (e = self; !e->client; e--)
        ;
    e->pos1 = self->sorigin;
}

static void makron_popup(struct edict_s *self)
{
    Iface.SoundByCreature(sound_popup, self->Extra.Creature);
}

static void makron_taunt(struct edict_s *self)
{
    Iface.SoundByCreature(sound_taunt3, self->Extra.Creature);
}

static void makron_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &makron_move_stand;
    else
        self->monsterinfo.currentmove = &makron_move_run;
}

static void makron_hit(struct edict_s *self)
{
    Iface.SoundByCreature(sound_hit, self->Extra.Creature);
}

static void makron_brainsplorch(struct edict_s *self)
{
    Iface.SoundByCreature(sound_brainsplorch, self->Extra.Creature);
}

static void makron_prerailgun(struct edict_s *self)
{
    Iface.SoundByCreature(sound_prerailgun, self->Extra.Creature);
}

static void jorg_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search1, self->Extra.Creature);
}

static void jorg_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &jorg_move_stand;
}

static void jorg_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &jorg_move_walk;
}

static void jorg_attack(struct edict_s *self)
{
    if (Iface.rand() < 24576)
        self->monsterinfo.currentmove = &jorg_move_start_attack1;
    else {
        Iface.SoundByCreature(sound_attack2, self->Extra.Creature);
        self->monsterinfo.currentmove = &jorg_move_attack2;
    }
}

static void jorg_die(struct edict_s *self)
{
    Iface.SoundByCreature(sound_death, self->Extra.Creature);
    self->deadflag = 2;
    self->takedamage = 0;
    self->count = 0;
    self->monsterinfo.currentmove = &jorg_move_death1;
}

static void makron_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &makron_move_stand;
}

static void makron_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &makron_move_walk;
}

static void makron_sight(struct edict_s *self)
{
    self->monsterinfo.currentmove = &makron_move_sight;
}

static void makron_attack(struct edict_s *self)
{
    struct VECT vec;
    int r;

    r = Iface.rand();
    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    Iface.VectorLength(&vec);
    if (r < 9831)
        self->monsterinfo.currentmove = &makron_move_attack3;
    else if (r < 19961)
        self->monsterinfo.currentmove = &makron_move_attack4;
    else
        self->monsterinfo.currentmove = &makron_move_attack5;
}

/* Makron's torso and legs (cre_rider3, cre_rider2), unused here. Global: gcc emits a
 * deferred static only once something already emitted refers to it, which these
 * (in their original place) were not */
void Boss32_die(struct edict_s *self)
{
}

void Boss32_pain(struct edict_s *self)
{
}

void Boss32Top_think(struct edict_s *self)
{
    if (++self->s.frame >= 20)
        self->s.frame = 0;
    self->nextthink = Iface.level->time + 1;
}

void Boss32Top_init(struct edict_s *self)
{
    self->pain = Boss32_pain;
    self->die = Boss32_die;
    self->think = Boss32Top_think;
    self->s.frame = 0;
    self->movetype = 0;
    self->solid = 0;
    self->nextthink = Iface.level->time + 2;
}

void Boss32Bottom_think(struct edict_s *self)
{
    if (++self->s.frame < 345)
        self->nextthink = Iface.level->time + 1;
    else {
        CRE(self)->Health = 0;
        self->deadflag = 2;
        self->movetype = 7;
        self->nextthink = 0;
        self->svflags |= 2;
    }
}

void Boss32Bottom_init(struct edict_s *self)
{
    self->pain = Boss32_pain;
    self->die = Boss32_die;
    self->s.frame = 251;
    self->think = Boss32Bottom_think;
    self->movetype = 0;
    self->solid = 0;
    self->nextthink = Iface.level->time + 2;
}

/* inline: with makron_dead inlined twice it is over the size limit, and only an
 * explicit inline is deferred to the end of the file regardless of size, as here */
static inline void makron_die(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;

    if (Cre->Health <= self->gib_health) {
        self->deadflag = 2;
        makron_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_makron_death, Cre);
    self->deadflag = 2;
    self->takedamage = 1;
    CRE(self)->Health = self->gib_health;
    makron_dead(self);
}
