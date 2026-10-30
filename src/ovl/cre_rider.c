/* Makron (Quake II m_boss32.c), with the torso and legs pieces of rider2/rider3 */
#include "ovl.h"

#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: this module's AiTable */
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

#define CLASS_MAKRON 83

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
static void makron_stand(struct edict_s *self);
static void makron_walk(struct edict_s *self);
void makron_sight(struct edict_s *self); /* unreferenced, so public */
static void makron_attack(struct edict_s *self);
/* the torso and legs pieces (as in cre_rider3/cre_rider2); nothing here references
   them, so the original had them public */
void Boss32Top_die(struct edict_s *self);
void Boss32Top_pain(struct edict_s *self);
void Boss32Top_think(struct edict_s *self);
void Boss32Top_init(struct edict_s *self);
void Boss32Bottom_think(struct edict_s *self);
void Boss32Bottom_init(struct edict_s *self);
static void makron_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point);

void PreInit_Makron(struct GLOBMODHDR *Hdr);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_makron, 0, PreInit_Makron, AnimRemap };

static void makronBFG(struct edict_s *self);
static void MakronRailgun(struct edict_s *self);
static void MakronHyperblaster(struct edict_s *self);
static void makron_pain(struct edict_s *self, struct edict_s *other, int kick, int damage);
static int Makron_CheckAttack(struct edict_s *self);

static struct mmove_t makron_move_stand;
static struct mmove_t makron_move_run;
static struct mmove_t makron_move_walk;
static struct mmove_t makron_move_pain6;
static struct mmove_t makron_move_pain5;
static struct mmove_t makron_move_pain4;
static struct mmove_t makron_move_death2;
static struct mmove_t makron_move_death3;
static struct mmove_t makron_move_sight;
static struct mmove_t makron_move_attack3;
static struct mmove_t makron_move_attack4;
static struct mmove_t makron_move_attack5;

static struct NEWANIMREMAP AnimRemap[] = {
    { 414, 473, {"Stand"}, 0, 0 },
    { 477, 486, {"Walk"}, 0, 0 },
    { 387, 413, {"Pain6"}, 0, 0 },
    { 383, 386, {"Pain5"}, 0, 0 },
    { 379, 382, {"Pain4"}, 0, 0 },
    { 188, 200, {"Sight"}, 0, 0 },
    { 201, 208, {"Attack3"}, 0, 0 },
    { 209, 234, {"Attack4"}, 0, 0 },
    { 235, 250, {"Attack5"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };



static int sound_pain4;
static int sound_pain5;
static int sound_pain6;
static int sound_death;
static int sound_step_left;
static int sound_step_right;
static int sound_attack_bfg;
static int sound_brainsplorch;
static int sound_prerailgun;
static int sound_popup;
static int sound_taunt1;
static int sound_taunt2;
static int sound_taunt3;
static int sound_hit;
static struct MODSUBOFF BlasterMuzzle;
static struct MODSUBOFF BFGMuzzle;
static struct MODSUBOFF RailgunMuzzle;
static int Unused[2];                  /* unused */
struct ai_local_table_t AiTable[34];   /* a common (a static of this size would be 16-aligned) */

void PreInit_Makron(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Boss32(Makron)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Boss32");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Boss32");
        Iface.PrintF("Validated Creature Interface for %s\n", "Boss32");
        Hdr->ShadowVerts = ShadowVerts;
        AiTable[0].thinkfunc = 0;
        AiTable[MAKRON_RAILGUN].thinkfunc = MakronRailgun;
        AiTable[MAKRON_SAVELOC].thinkfunc = MakronSaveloc;
        AiTable[MAKRON_HYPERBLASTER].thinkfunc = MakronHyperblaster;
        AiTable[MAKRON_STEP_LEFT].thinkfunc = makron_step_left;
        AiTable[MAKRON_STEP_RIGHT].thinkfunc = makron_step_right;
        AiTable[MAKRON_BFG].thinkfunc = makronBFG;
        AiTable[MAKRON_DEAD].thinkfunc = makron_dead;
        AiTable[MAKRON_POPUP].thinkfunc = makron_popup;
        AiTable[MAKRON_TAUNT].thinkfunc = makron_taunt;
        AiTable[MAKRON_RUN].thinkfunc = makron_run;
        AiTable[MAKRON_HIT].thinkfunc = makron_hit;
        AiTable[MAKRON_BRAINSPLORCH].thinkfunc = makron_brainsplorch;
        AiTable[MAKRON_PRERAILGUN].thinkfunc = makron_prerailgun;
        Iface.SetMyAiTable(CLASS_MAKRON, AiTable);
        sound_pain4 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain3"});
        sound_pain5 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain2"});
        sound_pain6 = Iface.GetSoundNum((struct IDENTITYNAME){"mak_pain1"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"mak_death"});
        sound_step_left = Iface.GetSoundNum((struct IDENTITYNAME){"mak_step1"});
        sound_step_right = Iface.GetSoundNum((struct IDENTITYNAME){"mak_step2"});
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
        if (!sound_step_left || !sound_step_right) {
            sound_step_left |= sound_step_right;
            sound_step_right |= sound_step_left;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 431, 434, &BFGMuzzle))
            Iface.PrintF("Makron BFG muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 419, 422, &BlasterMuzzle))
            Iface.PrintF("Makron Blaster muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 225, 227, &RailgunMuzzle))
            Iface.PrintF("Makron RailGun muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Boss32(Makron)\n");
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

/* unused: makron_move_walk uses the run frames, as in Quake II */
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

/* initialised, so it ends .data rather than starting .bss */
static struct VECT vec3_origin = { 0, 0, 0 };

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

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RailgunMuzzle, Cre->AnimFrame,
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
        vec.vx -= start.vx;
        vec.vy -= start.vy;
        vec.vz -= start.vz;
        Iface.vectoangles(&vec, &ang);
        vec.vx = ang.vx;
        vec.vy = ang.vy;
        vec.vz = ang.vz;
        dir.vx = vec.vx;
    } else
        dir.vx = 0;
    if (self->s.frame <= 221)
        dir.vz = self->sangles.vz + (self->s.frame - 217) * 10 * 4096 / 360;
    else
        dir.vz = self->sangles.vz - (self->s.frame - 226) * 10 * 4096 / 360;
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
    /* Lessen the chance of him going into his pain frames */
    if (damage <= 25)
        if (Iface.rand() < 6553)
            return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return; /* no pain anims in nightmare */
    if (damage <= 40) {
        Iface.SoundByCreature(sound_pain4, self->Extra.Creature);
        self->monsterinfo.currentmove = &makron_move_pain4;
    } else if (damage <= 110) {
        Iface.SoundByCreature(sound_pain5, self->Extra.Creature);
        self->monsterinfo.currentmove = &makron_move_pain5;
    } else if (damage <= 150) {
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
        /* see if any entities are in the way of the shot */
        spot1.vx = self->sorigin.vx;
        spot1.vy = self->sorigin.vy;
        spot1.vz = self->sorigin.vz;
        spot1.vy += self->viewheight;
        spot2.vx = self->enemy->sorigin.vx;
        spot2.vy = self->enemy->sorigin.vy;
        spot2.vz = self->enemy->sorigin.vz;
        spot2.vy += self->enemy->viewheight;
        Iface.gitrace(&spot1, 0, 0, &spot2, self, 0x2000019, CRE(self)->Box.MoveSpace);
        /* do we have a clear shot? */
        if (Iface.Trace->ent != self->enemy)
            return 0;
    }

    Iface.infront(self, self->enemy);
    enemy_range = Iface.range(self, self->enemy);
    temp.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    temp.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    temp.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    self->ideal_yaw = Iface.vectoyaw(&temp);

    /* melee attack */
    if (enemy_range == 0) {
        if (self->monsterinfo.melee)
            self->monsterinfo.attack_state = 3;
        else
            self->monsterinfo.attack_state = 4;
        return 1;
    }

    /* missile attack */
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
        self->monsterinfo.attack_finished = Iface.level->time + 2 * Iface.rand() / 32767;
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

static void MakronSaveloc(struct edict_s *self)
{
    self->pos1.vx = self->enemy->sorigin.vx;
    self->pos1.vy = self->enemy->sorigin.vy;
    self->pos1.vz = self->enemy->sorigin.vz;
    self->pos1.vy += self->enemy->viewheight;
}

static void makron_step_left(struct edict_s *self)
{
    Iface.SoundByCreature(sound_step_left, self->Extra.Creature);
}

static void makron_step_right(struct edict_s *self)
{
    Iface.SoundByCreature(sound_step_right, self->Extra.Creature);
}

/* also hands the player its position (pos1), found by walking back to the client edict */
static void makron_dead(struct edict_s *self)
{
    struct edict_s *player;

    self->takedamage = 0;
    self->movetype = 7;
    self->svflags |= 2;
    self->nextthink = 0;
    player = self;
    while (!player->client)
        player--;
    player->pos1 = self->sorigin;
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

static void makron_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &makron_move_stand;
}

static void makron_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &makron_move_walk;
}

void makron_sight(struct edict_s *self)
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
    if (r <= 9830)
        self->monsterinfo.currentmove = &makron_move_attack3;
    else if (r <= 19960)
        self->monsterinfo.currentmove = &makron_move_attack4;
    else
        self->monsterinfo.currentmove = &makron_move_attack5;
}

void Boss32Top_die(struct edict_s *self)
{
}

void Boss32Top_pain(struct edict_s *self)
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
    self->pain = Boss32Top_pain;
    self->die = Boss32Top_die;
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
    self->pain = Boss32Top_pain;
    self->die = Boss32Top_die;
    self->s.frame = 251;
    self->think = Boss32Bottom_think;
    self->movetype = 0;
    self->solid = 0;
    self->nextthink = Iface.level->time + 2;
}

static void makron_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point)
{
    struct CREATURE *Cre = self->Extra.Creature;

    /* check for gib */
    if (Cre->Health <= self->gib_health) {
        self->deadflag = 2;
        makron_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    /* regular death */
    Iface.SoundByCreature(sound_death, Cre);
    self->deadflag = 2;
    self->takedamage = 1;
    CRE(self)->Health = self->gib_health;
    makron_dead(self);
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
    self->monsterinfo.sight = 0;
    self->monsterinfo.checkattack = Makron_CheckAttack;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &makron_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

/* Jorg's death spawns the Makron, which jumps at the player */
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
