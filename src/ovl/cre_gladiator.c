/* Gladiator (Quake II m_gladiator.c) */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: this module's AiTable */
#define GLADIATOR_CLEAVER_SWING 1
#define GLADIATOR_MELEE 2
#define GLADIATOR_RUN 3
#define GLADIATOR_GUN 4
#define GLADIATOR_DEAD 5

#define CLASS_GLADIATOR 78

/* The small functions are emitted at the end in this order (see the README); the spawn
 * function is among them, so it is `inline` to be deferred despite its size. The frame
 * tables come before the interface: the original's .data relocations precede its .header ones. */
static void gladiator_cleaver_swing(struct edict_s *self);
static void GaldiatorMelee(struct edict_s *self);
static void gladiator_run(struct edict_s *self);
static void gladiator_dead(struct edict_s *self);
void SP_monster_gladiator(struct edict_s *self);
void PreInit_Gladiator(struct GLOBMODHDR *Hdr);
static struct NEWANIMREMAP AnimRemap[];

static struct mframe_t gladiator_frames_stand[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
};
static struct mmove_t gladiator_move_stand = { 0, 5, gladiator_frames_stand, 0 };

static struct mframe_t gladiator_frames_walk[] = {
    {AI_WALK, 15, 0}, {AI_WALK, 7, 0}, {AI_WALK, 6, 0}, {AI_WALK, 5, 0},
    {AI_WALK, 2, 0}, {AI_WALK, 0, 0}, {AI_WALK, 2, 0}, {AI_WALK, 8, 0},
    {AI_WALK, 12, 0}, {AI_WALK, 8, 0}, {AI_WALK, 5, 0}, {AI_WALK, 5, 0},
    {AI_WALK, 2, 0}, {AI_WALK, 2, 0}, {AI_WALK, 1, 0}, {AI_WALK, 8, 0},
};
static struct mmove_t gladiator_move_walk = { 7, 22, gladiator_frames_walk, 0 };

static struct mframe_t gladiator_frames_run[] = {
    {AI_RUN, 23, 0}, {AI_RUN, 14, 0}, {AI_RUN, 14, 0}, {AI_RUN, 21, 0}, {AI_RUN, 12, 0}, {AI_RUN, 13, 0},
};
static struct mmove_t gladiator_move_run = { 23, 28, gladiator_frames_run, 0 };

static struct mframe_t gladiator_frames_attack_melee[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, GLADIATOR_CLEAVER_SWING}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GLADIATOR_MELEE}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GLADIATOR_CLEAVER_SWING}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GLADIATOR_MELEE}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0},
};
static struct mmove_t gladiator_move_attack_melee = { 29, 45, gladiator_frames_attack_melee, gladiator_run };

static struct mframe_t gladiator_frames_attack_gun[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GLADIATOR_GUN},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0},
};
static struct mmove_t gladiator_move_attack_gun = { 46, 54, gladiator_frames_attack_gun, gladiator_run };

static struct mframe_t gladiator_frames_pain[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t gladiator_move_pain = { 55, 60, gladiator_frames_pain, gladiator_run };

static struct mframe_t gladiator_frames_pain_air[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0},
};
static struct mmove_t gladiator_move_pain_air = { 83, 89, gladiator_frames_pain_air, gladiator_run };

static struct mframe_t gladiator_frames_death[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t gladiator_move_death = { 61, 82, gladiator_frames_death, gladiator_dead };

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_gladiator, 0, PreInit_Gladiator, AnimRemap };

static void GladiatorGun(struct edict_s *self);
static void gladiator_idle(struct edict_s *self);
static void gladiator_sight(struct edict_s *self);
static void gladiator_search(struct edict_s *self);
static void gladiator_stand(struct edict_s *self);
static void gladiator_walk(struct edict_s *self);
static void gladiator_melee(struct edict_s *self);
static void gladiator_attack(struct edict_s *self);
static void gladiator_pain(struct edict_s *self);
static void gladiator_die(struct edict_s *self);

static int sound_pain1;
static int sound_pain2;
static int sound_die;
static int sound_gun;
static int sound_cleaver_swing;
static int sound_cleaver_hit;
static int sound_cleaver_miss;
static int sound_idle;
static int sound_search;
static int sound_sight;
static int sound_gib;

static struct MODSUBOFF RailgunMuzzle;
/* unused; two 8-byte pieces, as a 16-byte one would be 16-aligned */
static int Unused1[2];
static int Unused2[2];
struct ai_local_table_t AiTable[32];   /* a common: it follows the bss word-aligned */

void PreInit_Gladiator(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Glad\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Gladiator");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Gladiator");
        Iface.PrintF("Validated Creature Interface for %s\n", "Gladiator");
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"gld_pain2"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"gld_pain2"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"gld_death"});
        sound_gun = Iface.GetSoundNum((struct IDENTITYNAME){"wep_railgf1a"});
        sound_cleaver_swing = Iface.GetSoundNum((struct IDENTITYNAME){"gld_melee3"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"gld_idle1"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"gld_srch1"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"gld_sight"});
        sound_gib = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        AiTable[0].thinkfunc = 0;
        AiTable[GLADIATOR_CLEAVER_SWING].thinkfunc = gladiator_cleaver_swing;
        AiTable[GLADIATOR_MELEE].thinkfunc = GaldiatorMelee;
        AiTable[GLADIATOR_RUN].thinkfunc = gladiator_run;
        AiTable[GLADIATOR_GUN].thinkfunc = GladiatorGun;
        AiTable[GLADIATOR_DEAD].thinkfunc = gladiator_dead;
        Iface.SetMyAiTable(CLASS_GLADIATOR, AiTable);
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 158, 169, &RailgunMuzzle))
            Iface.PrintF("Gladiator Railgun muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Glad\n");
}

static void GladiatorGun(struct edict_s *self)
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
    Iface.SoundByCreature(sound_gun, self->Extra.Creature);
    Iface.monster_fire_railgun(self, &start, &dir, 50, 100, 0);
}

static void gladiator_cleaver_swing(struct edict_s *self)
{
    Iface.SoundByCreature(sound_cleaver_swing, self->Extra.Creature);
}

static void GaldiatorMelee(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = -48;
    aim.vz = Iface.getminsvx(self);
    Iface.fire_hit(self, aim, Iface.rand() % 5 + 20, 150);
}

static void gladiator_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &gladiator_move_stand;
    else
        self->monsterinfo.currentmove = &gladiator_move_run;
}

static void gladiator_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->nextthink = 0;
    self->svflags |= 2;
}

inline void SP_monster_gladiator(struct edict_s *self)
{
    self->mass = 400;
    self->pain = gladiator_pain;
    self->die = gladiator_die;
    self->monsterinfo.stand = gladiator_stand;
    self->monsterinfo.walk = gladiator_walk;
    self->monsterinfo.run = gladiator_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = gladiator_attack;
    self->monsterinfo.melee = gladiator_melee;
    self->monsterinfo.sight = gladiator_sight;
    self->monsterinfo.idle = gladiator_idle;
    self->monsterinfo.search = gladiator_search;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    self->monsterinfo.currentmove = &gladiator_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void gladiator_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void gladiator_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void gladiator_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search, self->Extra.Creature);
}

static void gladiator_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gladiator_move_stand;
}

static void gladiator_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gladiator_move_walk;
}

static void gladiator_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gladiator_move_attack_melee;
}

static void gladiator_attack(struct edict_s *self)
{
    struct VECT v;

    v.vx = self->sorigin.vx - self->enemy->sorigin.vx;
    v.vy = self->sorigin.vy - self->enemy->sorigin.vy;
    v.vz = self->sorigin.vz - self->enemy->sorigin.vz;
    if (Iface.VectorLength2(&v) > 0x10e310) {
        self->pos1.vx = self->enemy->sorigin.vx;
        self->pos1.vy = self->enemy->sorigin.vy;
        self->pos1.vz = self->enemy->sorigin.vz;
        self->pos1.vy += self->enemy->viewheight;
        self->monsterinfo.currentmove = &gladiator_move_attack_gun;
    }
}

static void gladiator_pain(struct edict_s *self)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time) {
        if (self->velocity.vy > 1200 && self->monsterinfo.currentmove == &gladiator_move_pain)
            self->monsterinfo.currentmove = &gladiator_move_pain_air;
        return;
    }
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain1, self->Extra.Creature);
    if ((*Iface.skill)->value == 3)
        return;
    if (self->velocity.vy > 1200)
        self->monsterinfo.currentmove = &gladiator_move_pain_air;
    else
        self->monsterinfo.currentmove = &gladiator_move_pain;
}

static void gladiator_die(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;

    if (Cre->Health <= self->gib_health) {
        self->deadflag = 2;
        gladiator_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, Cre);
    self->monsterinfo.currentmove = &gladiator_move_death;
    self->deadflag = 2;
    self->takedamage = 1;
}

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 5, {"Stand"}, 0, 0 },
    { 7, 22, {"Walk"}, 0, 0 },
    { 23, 28, {"Run"}, 0, 0 },
    { 29, 45, {"Melee"}, 0, 0 },
    { 46, 54, {"Attack"}, 0, 0 },
    { 55, 60, {"Pain"}, 0, 0 },
    { 83, 89, {"Painup"}, 0, 0 },
    { 61, 82, {"Death"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};
