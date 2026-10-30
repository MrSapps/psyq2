/* Gunner (Quake II m_gunner.c) */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: this module's AiTable */
#define GUNNER_GRENADE 1
#define GUNNER_FIRE 2
#define GUNNER_FIRE_CHAIN 3
#define GUNNER_REFIRE_CHAIN 4
#define GUNNER_STAND 5
#define GUNNER_IDLESOUND 6
#define GUNNER_FIDGET 7
#define GUNNER_RUN 8
#define GUNNER_DEAD 9
#define GUNNER_DUCK_DOWN 10
#define GUNNER_DUCK_HOLD 11
#define GUNNER_DUCK_UP 12
#define GUNNER_OPENGUN 13

#define CLASS_GUNNER 79

/* aiflags */
#define AI_STAND_GROUND 0x1
#define AI_HOLD_FRAME 0x80
#define AI_DUCKED 0x800

/* the small functions, emitted at the end in this order (see the README) */
static void GunnerFire(struct edict_s *self);
static void GunnerGrenade(struct edict_s *self);
static void gunner_fire_chain(struct edict_s *self);
static void gunner_refire_chain(struct edict_s *self);
static void gunner_stand(struct edict_s *self);
static void gunner_idlesound(struct edict_s *self);
static void gunner_fidget(struct edict_s *self);
static void gunner_run(struct edict_s *self);
static void gunner_dead(struct edict_s *self);
static void gunner_duck_down(struct edict_s *self);
static void gunner_duck_hold(struct edict_s *self);
static void gunner_duck_up(struct edict_s *self);
static void gunner_opengun(struct edict_s *self);
void Gunner_PostDraw(struct CREATURE *Cre);
static void gunner_sight(struct edict_s *self);
static void gunner_search(struct edict_s *self);
static void gunner_walk(struct edict_s *self);
void gunner_runandshoot(struct edict_s *self);
static void gunner_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void gunner_die(struct edict_s *self);
static void gunner_dodge(struct edict_s *self, struct edict_s *attacker);
static void gunner_attack(struct edict_s *self);

void SP_monster_gunner(struct edict_s *self);
void PreInit_Gunner(struct GLOBMODHDR *Hdr);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_gunner, Gunner_PostDraw, PreInit_Gunner, AnimRemap };

static struct NEWANIMREMAP AnimRemap[] = {
    { 94, 101, {"Run"}, 0, 0 },
    { 102, 107, {"Runshoot"}, 0, 0 },
    { 108, 128, {"Attack 1"}, 0, 0 },
    { 137, 143, {"Attack Chain"}, 0, 0 },
    { 144, 151, {"Fire Chain"}, 0, 0 },
    { 152, 158, {"End Chain"}, 0, 0 },
    { 159, 176, {"Pain1"}, 0, 0 },
    { 177, 184, {"Pain2"}, 0, 0 },
    { 185, 189, {"Pain3"}, 0, 0 },
    { 76, 88, {"Walk"}, 0, 0 },
    { 190, 200, {"Death"}, 0, 0 },
    { 201, 208, {"Duck"}, 0, 0 },
    { 0, 29, {"Stand"}, 0, 0 },
    { 30, 69, {"Fidget"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };

static int sound_pain;
static int sound_pain2;
static int sound_death;
static int sound_idle;
static int sound_open;
static int sound_search;
static int sound_sight;
static int sound_gib;
static struct MODSUBOFF ChainGunMuzzle;
static struct MODSUBOFF LauncherMuzzle;
static int Unused[2];
struct ai_local_table_t AiTable[32];   /* a common: it follows the bss word-aligned */
int UnusedCommon[2];   /* unused: the original's bss goes 8 bytes past AiTable */

void PreInit_Gunner(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Gunner\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Gunner");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Gunner");
        Iface.PrintF("Validated Creature Interface for %s\n", "Gunner");
        Hdr->ShadowVerts = ShadowVerts;
        sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"gun_pain1"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"gun_pain1"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"gun_death1"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"gun_sight1"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"gun_srch1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"gun_idle1"});
        sound_open = 0;
        sound_gib = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 227, 231, &ChainGunMuzzle))
            Iface.PrintF("Gunner ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 273, 281, &LauncherMuzzle))
            Iface.PrintF("Gunner Launcher muzzle points are from different submodels\r\n");
        AiTable[0].thinkfunc = 0;
        AiTable[GUNNER_GRENADE].thinkfunc = GunnerGrenade;
        AiTable[GUNNER_FIRE].thinkfunc = GunnerFire;
        AiTable[GUNNER_FIRE_CHAIN].thinkfunc = gunner_fire_chain;
        AiTable[GUNNER_REFIRE_CHAIN].thinkfunc = gunner_refire_chain;
        AiTable[GUNNER_STAND].thinkfunc = gunner_stand;
        AiTable[GUNNER_IDLESOUND].thinkfunc = gunner_idlesound;
        AiTable[GUNNER_FIDGET].thinkfunc = gunner_fidget;
        AiTable[GUNNER_RUN].thinkfunc = gunner_run;
        AiTable[GUNNER_DEAD].thinkfunc = gunner_dead;
        AiTable[GUNNER_DUCK_DOWN].thinkfunc = gunner_duck_down;
        AiTable[GUNNER_DUCK_HOLD].thinkfunc = gunner_duck_hold;
        AiTable[GUNNER_DUCK_UP].thinkfunc = gunner_duck_up;
        AiTable[GUNNER_OPENGUN].thinkfunc = gunner_opengun;
        Iface.SetMyAiTable(CLASS_GUNNER, AiTable);
    } else
        Iface.PrintF("Clear Gunner\n");
}

static struct mframe_t gunner_frames_fidget[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, GUNNER_IDLESOUND},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0},
};
static struct mmove_t gunner_move_fidget = { 30, 69, gunner_frames_fidget, gunner_stand };

static struct mframe_t gunner_frames_stand[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, GUNNER_FIDGET},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, GUNNER_FIDGET},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, GUNNER_FIDGET},
};
static struct mmove_t gunner_move_stand = { 0, 29, gunner_frames_stand, 0 };

static struct mframe_t gunner_frames_walk[] = {
    {AI_WALK, 0, 0}, {AI_WALK, 3, 0}, {AI_WALK, 4, 0}, {AI_WALK, 5, 0}, {AI_WALK, 7, 0},
    {AI_WALK, 2, 0}, {AI_WALK, 6, 0}, {AI_WALK, 4, 0}, {AI_WALK, 2, 0}, {AI_WALK, 7, 0},
    {AI_WALK, 5, 0}, {AI_WALK, 7, 0}, {AI_WALK, 4, 0},
};
static struct mmove_t gunner_move_walk = { 76, 88, gunner_frames_walk, 0 };

static struct mframe_t gunner_frames_run[] = {
    {AI_RUN, 4, 0}, {AI_RUN, 13, 0}, {AI_RUN, 13, 0}, {AI_RUN, 8, 0},
    {AI_RUN, 4, 0}, {AI_RUN, 13, 0}, {AI_RUN, 13, 0}, {AI_RUN, 8, 0},
};
static struct mmove_t gunner_move_run = { 94, 101, gunner_frames_run, 0 };

static struct mframe_t gunner_frames_runandshoot[] = {
    {AI_RUN, 32, 0}, {AI_RUN, 15, 0}, {AI_RUN, 10, 0}, {AI_RUN, 18, 0}, {AI_RUN, 8, 0}, {AI_RUN, 20, 0},
};
static struct mmove_t gunner_move_runandshoot = { 102, 107, gunner_frames_runandshoot, 0 };

static struct mframe_t gunner_frames_pain3[] = {
    {AI_MOVE, -3, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 1, 0},
};
static struct mmove_t gunner_move_pain3 = { 185, 189, gunner_frames_pain3, gunner_run };

static struct mframe_t gunner_frames_pain2[] = {
    {AI_MOVE, -2, 0}, {AI_MOVE, 11, 0}, {AI_MOVE, 6, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, -1, 0}, {AI_MOVE, -7, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, -7, 0},
};
static struct mmove_t gunner_move_pain2 = { 177, 184, gunner_frames_pain2, gunner_run };

static struct mframe_t gunner_frames_pain1[] = {
    {AI_MOVE, 2, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -5, 0}, {AI_MOVE, 3, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, 1, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t gunner_move_pain1 = { 159, 176, gunner_frames_pain1, gunner_run };

static struct mframe_t gunner_frames_death[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, -7, 0}, {AI_MOVE, -3, 0}, {AI_MOVE, -5, 0},
    {AI_MOVE, 8, 0}, {AI_MOVE, 6, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t gunner_move_death = { 190, 200, gunner_frames_death, gunner_dead };

static struct mframe_t gunner_frames_duck[] = {
    {AI_MOVE, 1, GUNNER_DUCK_DOWN}, {AI_MOVE, 1, 0}, {AI_MOVE, 1, GUNNER_DUCK_HOLD}, {AI_MOVE, 0, 0},
    {AI_MOVE, -1, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, 0, GUNNER_DUCK_UP}, {AI_MOVE, -1, 0},
};
static struct mmove_t gunner_move_duck = { 201, 208, gunner_frames_duck, gunner_run };

static struct mframe_t gunner_frames_attack_chain[] = {
    {AI_CHARGE, 0, GUNNER_OPENGUN}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
};
static struct mmove_t gunner_move_attack_chain = { 137, 143, gunner_frames_attack_chain, gunner_fire_chain };

static struct mframe_t gunner_frames_fire_chain[] = {
    {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE},
    {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE}, {AI_CHARGE, 0, GUNNER_FIRE},
};
static struct mmove_t gunner_move_fire_chain = { 144, 151, gunner_frames_fire_chain, gunner_refire_chain };

static struct mframe_t gunner_frames_endfire_chain[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
};
static struct mmove_t gunner_move_endfire_chain = { 152, 158, gunner_frames_endfire_chain, gunner_run };

static struct mframe_t gunner_frames_attack_grenade[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, GUNNER_GRENADE}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GUNNER_GRENADE},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GUNNER_GRENADE}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, GUNNER_GRENADE}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0},
};
static struct mmove_t gunner_move_attack_grenade = { 108, 128, gunner_frames_attack_grenade, gunner_run };
/* the original's .bss starts 8-aligned and tools/ovllink.py aligns it to 4: this word is the gap */
static int BssAlign = 0;

static void GunnerFire(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT target;
    struct VECT aim;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, ChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    target.vx = self->enemy->sorigin.vx;
    target.vy = self->enemy->sorigin.vy;
    target.vz = self->enemy->sorigin.vz;
    Iface.VectorMA(&target, -819, &self->enemy->velocity, &target);
    target.vy += self->enemy->viewheight;
    aim.vx = target.vx - start.vx;
    aim.vy = target.vy - start.vy;
    aim.vz = target.vz - start.vz;
    Iface.VectorNormalize(&aim);
    Iface.monster_fire_bullet(self, &start, &aim, 3, 4, 300, 500, 0);
}

void SP_monster_gunner(struct edict_s *self)
{
    self->mass = 200;
    self->pain = gunner_pain;
    self->die = gunner_die;
    self->monsterinfo.stand = gunner_stand;
    self->monsterinfo.walk = gunner_walk;
    self->monsterinfo.run = gunner_run;
    self->monsterinfo.dodge = gunner_dodge;
    self->monsterinfo.attack = gunner_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = gunner_sight;
    self->monsterinfo.search = gunner_search;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &gunner_move_stand;
    self->monsterinfo.scale = 12;
    Iface.walkmonster_start(self);
    sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"gun_pain1"});
    sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"gun_pain1"});
    sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"gun_death1"});
    sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"gun_idle1"});
    sound_open = Iface.GetSoundNum((struct IDENTITYNAME){"par_atck2"});
    sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"gun_srch1"});
    sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"gun_sight1"});
}

static void gunner_fire_chain(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gunner_move_fire_chain;
}

static void gunner_refire_chain(struct edict_s *self)
{
    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health > 0)
        if (Iface.visible(self, self->enemy))
            if (Iface.rand() <= 16384) {
                self->monsterinfo.currentmove = &gunner_move_fire_chain;
                return;
            }
    self->monsterinfo.currentmove = &gunner_move_endfire_chain;
}

static void gunner_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gunner_move_stand;
}

static void gunner_idlesound(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void gunner_fidget(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & AI_STAND_GROUND)
        return;
    if (Iface.rand() <= 1638)
        self->monsterinfo.currentmove = &gunner_move_fidget;
}

static void gunner_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & AI_STAND_GROUND)
        self->monsterinfo.currentmove = &gunner_move_stand;
    else
        self->monsterinfo.currentmove = &gunner_move_run;
}

static void gunner_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->nextthink = 0;
    self->svflags |= 2;
}

static void gunner_duck_down(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & AI_DUCKED)
        return;
    self->monsterinfo.aiflags |= AI_DUCKED;
    if ((*Iface.skill)->value >= 2) {
        if (Iface.rand() & 1)
            GunnerGrenade(self);
    }
    self->takedamage = 1;
    self->monsterinfo.pausetime = Iface.level->time + 10;
}

static void gunner_duck_hold(struct edict_s *self)
{
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
    else
        self->monsterinfo.aiflags |= AI_HOLD_FRAME;
}

static void gunner_duck_up(struct edict_s *self)
{
    self->monsterinfo.aiflags &= ~AI_DUCKED;
    self->takedamage = 2;
}

static void gunner_opengun(struct edict_s *self)
{
}

void Gunner_PostDraw(struct CREATURE *Cre)
{
    if (Cre->Control < 0)
        Iface.MuzzleFlash(Cre, ChainGunMuzzle, 120);
}

static void gunner_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void gunner_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search, self->Extra.Creature);
}

static void gunner_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gunner_move_walk;
}

/* nothing uses it, but the original has it */
void gunner_runandshoot(struct edict_s *self)
{
    self->monsterinfo.currentmove = &gunner_move_runandshoot;
}

static void gunner_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain, self->Extra.Creature);
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 10)
        self->monsterinfo.currentmove = &gunner_move_pain3;
    else if (damage <= 25)
        self->monsterinfo.currentmove = &gunner_move_pain2;
    else
        self->monsterinfo.currentmove = &gunner_move_pain1;
}

static void gunner_die(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;

    if (Cre->Health <= self->gib_health) {
        Iface.SoundByCreature(sound_gib, Cre);
        self->deadflag = 2;
        gunner_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_death, Cre);
    self->monsterinfo.currentmove = &gunner_move_death;
    self->deadflag = 2;
    self->takedamage = 1;
}

static void gunner_dodge(struct edict_s *self, struct edict_s *attacker)
{
    if (Iface.rand() > 8192)
        return;
    if (!self->enemy)
        self->enemy = attacker;
    self->monsterinfo.currentmove = &gunner_move_duck;
}

static void gunner_attack(struct edict_s *self)
{
    if (Iface.range(self, self->enemy) == 0)
        self->monsterinfo.currentmove = &gunner_move_attack_chain;
    else if (Iface.rand() & 1)
        self->monsterinfo.currentmove = &gunner_move_attack_grenade;
    else
        self->monsterinfo.currentmove = &gunner_move_attack_chain;
}

/* defined last so that gunner_duck_down calls it rather than inlining it */
static void GunnerGrenade(struct edict_s *self)
{
    struct VECT start;
    struct VECT forward;
    struct VECT right;
    struct VECT aim;

    Iface.AngleVectors(&self->sangles, &forward, &right, 0);
    aim.vx = forward.vx;
    aim.vy = forward.vy;
    aim.vz = forward.vz;
    Iface.GetAbsoluteModelVert(self->Extra.Creature, 273, &start);
    Iface.monster_fire_grenade(self, &start, &aim, 50, 600, 0);
}
