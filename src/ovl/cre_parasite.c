/* Parasite (m_parasite.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum {
    L_NONE, L_STAND, L_START_RUN, L_RUN, L_WALK, L_START_WALK, L_END_FIDGET, L_DO_FIDGET, L_REFIDGET,
    L_SCRATCH, L_TAP, L_LAUNCH, L_DRAIN_ATTACK, L_REEL_IN, L_DEAD
};

#define CLASS_PARASITE 86
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* gcc emits the small functions last, in this order */
void SP_monster_parasite(struct edict_s *self);
void parasite_stand(struct edict_s *self);
void parasite_start_run(struct edict_s *self);
void parasite_run(struct edict_s *self);
void parasite_walk(struct edict_s *self);
void parasite_start_walk(struct edict_s *self);
void parasite_end_fidget(struct edict_s *self);
void parasite_do_fidget(struct edict_s *self);
void parasite_refidget(struct edict_s *self);
void parasite_scratch(struct edict_s *self);
void parasite_tap(struct edict_s *self);
void parasite_launch(struct edict_s *self);
void parasite_reel_in(struct edict_s *self);
void parasite_dead(struct edict_s *self);
void parasite_sight(struct edict_s *self);
void parasite_search(struct edict_s *self);
void parasite_idle(struct edict_s *self);
void parasite_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
void parasite_attack(struct edict_s *self);
void parasite_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);
void PreInit_Parasite(struct GLOBMODHDR *Hdr);
void parasite_drain_attack(struct edict_s *self);
static struct NEWANIMREMAP AnimRemap[];
static struct mmove_t parasite_move_start_fidget, parasite_move_fidget, parasite_move_end_fidget;
static struct mmove_t parasite_move_stand, parasite_move_run, parasite_move_start_run;
static struct mmove_t parasite_move_walk, parasite_move_start_walk, parasite_move_pain1;
static struct mmove_t parasite_move_drain, parasite_move_death;

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_parasite, 0, PreInit_Parasite, AnimRemap };

static int sound_pain1;
static int sound_pain2;
static int sound_die;
static int sound_launch;
static int sound_impact;
static int sound_suck;
static int sound_reelin;
static int sound_sight;
static int sound_tap;
static int sound_scratch;
static int sound_search;
static int Unused[2];
struct ai_local_table_t AiTable[32];

void PreInit_Parasite(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Parasite\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Parasite");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Parasite");
        Iface.PrintF("Validated Creature Interface for %s\n", "Parasite");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_STAND].thinkfunc = parasite_stand;
        AiTable[L_START_RUN].thinkfunc = parasite_start_run;
        AiTable[L_RUN].thinkfunc = parasite_run;
        AiTable[L_WALK].thinkfunc = parasite_walk;
        AiTable[L_START_WALK].thinkfunc = parasite_start_walk;
        AiTable[L_END_FIDGET].thinkfunc = parasite_end_fidget;
        AiTable[L_DO_FIDGET].thinkfunc = parasite_do_fidget;
        AiTable[L_REFIDGET].thinkfunc = parasite_refidget;
        AiTable[L_SCRATCH].thinkfunc = parasite_scratch;
        AiTable[L_TAP].thinkfunc = parasite_tap;
        AiTable[L_LAUNCH].thinkfunc = parasite_launch;
        AiTable[L_DRAIN_ATTACK].thinkfunc = parasite_drain_attack;
        AiTable[L_REEL_IN].thinkfunc = parasite_reel_in;
        AiTable[L_DEAD].thinkfunc = parasite_dead;
        Iface.SetMyAiTable(CLASS_PARASITE, AiTable);
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"par_pain2"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"par_pain2"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"par_deth1"});
        sound_launch = Iface.GetSoundNum((struct IDENTITYNAME){"par_atck4"});
        sound_impact = Iface.GetSoundNum((struct IDENTITYNAME){"par_atck2"});
        sound_suck = Iface.GetSoundNum((struct IDENTITYNAME){"par_atck3"});
        sound_reelin = Iface.GetSoundNum((struct IDENTITYNAME){"par_atck4"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"par_sght1"});
        sound_tap = Iface.GetSoundNum((struct IDENTITYNAME){"par_idle1"});
        sound_scratch = Iface.GetSoundNum((struct IDENTITYNAME){"par_idle2"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"par_idle1"});
    } else
        Iface.PrintF("Clear Parasite\n");
}

static struct NEWANIMREMAP AnimRemap[] = {
    { 68, 69, {"Run1"}, 0, 0 },
    { 70, 76, {"Run2"}, 0, 0 },
    { 77, 82, {"Run3"}, 0, 0 },
    { 39, 56, {"Drain"}, 0, 0 },
    { 57, 67, {"Pain"}, 0, 0 },
    { 83, 99, {"Stand"}, 0, 0 },
    { 100, 103, {"Stfidget"}, 0, 0 },
    { 104, 109, {"Fidget"}, 0, 0 },
    { 110, 117, {"Endfidget"}, 0, 0 },
    { 32, 38, {"Death"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static struct mframe_t parasite_frames_start_fidget[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t parasite_move_start_fidget = { 100, 103, parasite_frames_start_fidget, parasite_do_fidget };
static struct mframe_t parasite_frames_fidget[] = {
    { AI_STAND, 0, L_SCRATCH }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, L_SCRATCH },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t parasite_move_fidget = { 104, 109, parasite_frames_fidget, parasite_refidget };
static struct mframe_t parasite_frames_end_fidget[] = {
    { AI_STAND, 0, L_SCRATCH }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t parasite_move_end_fidget = { 110, 117, parasite_frames_end_fidget, parasite_stand };
static struct mframe_t parasite_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, L_TAP }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, L_TAP }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, L_TAP }, { AI_STAND, 0, 0 }, { AI_STAND, 0, L_TAP }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, L_TAP }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, L_TAP },
};
static struct mmove_t parasite_move_stand = { 83, 99, parasite_frames_stand, parasite_stand };
static struct mframe_t parasite_frames_run[] = {
    { AI_RUN, 30, 0 }, { AI_RUN, 30, 0 }, { AI_RUN, 22, 0 }, { AI_RUN, 19, 0 },
    { AI_RUN, 24, 0 }, { AI_RUN, 28, 0 }, { AI_RUN, 25, 0 },
};
static struct mmove_t parasite_move_run = { 70, 76, parasite_frames_run, 0 };
static struct mframe_t parasite_frames_start_run[] = {
    { AI_RUN, 0, 0 }, { AI_RUN, 30, 0 },
};
static struct mmove_t parasite_move_start_run = { 68, 69, parasite_frames_start_run, parasite_run };
static struct mframe_t parasite_frames_stop_run[] = {
    { AI_RUN, 20, 0 }, { AI_RUN, 20, 0 }, { AI_RUN, 12, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 0, 0 }, { AI_RUN, 0, 0 },
};
static struct mmove_t parasite_move_stop_run = { 77, 82, parasite_frames_stop_run, 0 };
static struct mframe_t parasite_frames_walk[] = {
    { AI_WALK, 30, 0 }, { AI_WALK, 30, 0 }, { AI_WALK, 22, 0 }, { AI_WALK, 19, 0 },
    { AI_WALK, 24, 0 }, { AI_WALK, 28, 0 }, { AI_WALK, 25, 0 },
};
static struct mmove_t parasite_move_walk = { 70, 76, parasite_frames_walk, parasite_walk };
static struct mframe_t parasite_frames_start_walk[] = {
    { AI_WALK, 0, 0 }, { AI_WALK, 30, L_WALK },
};
static struct mmove_t parasite_move_start_walk = { 68, 69, parasite_frames_start_walk, 0 };
static struct mframe_t parasite_frames_stop_walk[] = {
    { AI_WALK, 20, 0 }, { AI_WALK, 20, 0 }, { AI_WALK, 12, 0 }, { AI_WALK, 10, 0 },
    { AI_WALK, 0, 0 }, { AI_WALK, 0, 0 },
};
static struct mmove_t parasite_move_stop_walk = { 77, 82, parasite_frames_stop_walk, 0 };
static struct mframe_t parasite_frames_pain1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 6, 0 }, { AI_MOVE, 16, 0 },
    { AI_MOVE, -6, 0 }, { AI_MOVE, -7, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t parasite_move_pain1 = { 57, 67, parasite_frames_pain1, parasite_start_run };
static struct mframe_t parasite_frames_drain[] = {
    { AI_CHARGE, 0, L_LAUNCH }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 15, L_DRAIN_ATTACK }, { AI_CHARGE, 0, L_DRAIN_ATTACK },
    { AI_CHARGE, 0, L_DRAIN_ATTACK }, { AI_CHARGE, 0, L_DRAIN_ATTACK }, { AI_CHARGE, 0, L_DRAIN_ATTACK }, { AI_CHARGE, -2, L_DRAIN_ATTACK },
    { AI_CHARGE, -2, L_DRAIN_ATTACK }, { AI_CHARGE, -3, L_DRAIN_ATTACK }, { AI_CHARGE, -2, L_DRAIN_ATTACK }, { AI_CHARGE, 0, L_DRAIN_ATTACK },
    { AI_CHARGE, -1, L_DRAIN_ATTACK }, { AI_CHARGE, 0, L_REEL_IN }, { AI_CHARGE, -2, 0 }, { AI_CHARGE, -2, 0 },
    { AI_CHARGE, -3, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t parasite_move_drain = { 39, 56, parasite_frames_drain, parasite_start_run };
static struct mframe_t parasite_frames_death[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t parasite_move_death = { 32, 38, parasite_frames_death, parasite_dead };

static int parasite_drain_attack_ok(struct VECT start, struct VECT end)
{
    struct VECT dir;
    struct SVECT angles;

    dir.vx = start.vx - end.vx;
    dir.vy = start.vy - end.vy;
    dir.vz = start.vz - end.vz;
    if (Iface.VectorLength(&dir) > 3072)
        return 0;
    Iface.vectoangles(&dir, &angles);
    if (angles.vx < -2048)
        angles.vx += 4096;
    if (angles.vx > 341)
        return 0;
    return 1;
}

void parasite_drain_attack(struct edict_s *self)
{
    struct SVECT offset;
    struct VECT start;
    struct VECT f;
    struct VECT r;
    struct VECT end;
    struct VECT dir;
    int damage;
    int type;

    Iface.AngleVectors(&self->sangles, &f, &r, 0);
    offset.vx = 288;
    offset.vy = 72;
    offset.vz = 0;
    Iface.G_ProjectSourceNM(&self->sorigin, &offset, &f, &r, &start);
    end.vx = self->enemy->sorigin.vx;
    end.vy = self->enemy->sorigin.vy;
    end.vz = self->enemy->sorigin.vz;
    if (!parasite_drain_attack_ok(start, end)) {
        end.vy = self->enemy->sorigin.vy + Iface.getmaxsvy(self->enemy) - 96;
        if (!parasite_drain_attack_ok(start, end)) {
            end.vy = self->enemy->sorigin.vy + Iface.getminsvy(self->enemy) + 96;
            if (!parasite_drain_attack_ok(start, end))
                return;
        }
    }
    end.vx = self->enemy->sorigin.vx;
    end.vy = self->enemy->sorigin.vy;
    end.vz = self->enemy->sorigin.vz;
    Iface.gitrace(&start, 0, 0, &end, self, 0x6000003, CRE(self)->Box.MoveSpace);
    if (Iface.Trace->ent != self->enemy)
        return;
    if (self->s.frame == 41) {
        Iface.SoundByCreature(sound_impact, CRE(self));
        damage = 5;
        type = 21;
    } else {
        Iface.SoundByCreature(sound_suck, CRE(self));
        damage = 2;
        type = 0;
    }
    dir.vx = start.vx - end.vx;
    dir.vy = start.vy - end.vy;
    dir.vz = start.vz - end.vz;
    Iface.DoDamage(CRE(self), CRE(self->enemy), damage, type, 0);
    Iface.AnyoneFireSplurge(CRE(self), CRE(self->enemy), 97, damage);
}

void SP_monster_parasite(struct edict_s *self)
{
    self->mass = 250;
    self->pain = parasite_pain;
    self->die = parasite_die;
    self->monsterinfo.stand = parasite_stand;
    self->monsterinfo.walk = parasite_start_walk;
    self->monsterinfo.run = parasite_start_run;
    self->monsterinfo.attack = parasite_attack;
    self->monsterinfo.sight = parasite_sight;
    self->monsterinfo.idle = parasite_idle;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    self->monsterinfo.currentmove = &parasite_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

void parasite_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_stand;
}

void parasite_start_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &parasite_move_stand;
    else
        self->monsterinfo.currentmove = &parasite_move_start_run;
}

void parasite_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &parasite_move_stand;
    else
        self->monsterinfo.currentmove = &parasite_move_run;
}

void parasite_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_walk;
}

void parasite_start_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_start_walk;
}

void parasite_end_fidget(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_end_fidget;
}

void parasite_do_fidget(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_fidget;
}

void parasite_refidget(struct edict_s *self)
{
    if (Iface.rand() < 26215)
        self->monsterinfo.currentmove = &parasite_move_fidget;
    else
        self->monsterinfo.currentmove = &parasite_move_end_fidget;
}

void parasite_scratch(struct edict_s *self)
{
    Iface.SoundByCreature(sound_scratch, CRE(self));
}

void parasite_tap(struct edict_s *self)
{
    Iface.SoundByCreature(sound_tap, CRE(self));
}

void parasite_launch(struct edict_s *self)
{
    Iface.SoundByCreature(sound_launch, CRE(self));
}

void parasite_reel_in(struct edict_s *self)
{
    Iface.SoundByCreature(sound_reelin, CRE(self));
}

void parasite_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->svflags |= 2;
    self->nextthink = 0;
}

void parasite_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, CRE(self));
}

void parasite_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search, CRE(self));
}

void parasite_idle(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_start_fidget;
}

void parasite_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    Iface.SoundByCreature(sound_pain2, CRE(self));
    self->monsterinfo.currentmove = &parasite_move_pain1;
}

void parasite_attack(struct edict_s *self)
{
    self->monsterinfo.currentmove = &parasite_move_drain;
}

void parasite_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    if (CRE(self)->Health <= self->gib_health) {
        self->deadflag = 2;
        parasite_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, CRE(self));
    self->deadflag = 2;
    self->takedamage = 1;
    self->monsterinfo.currentmove = &parasite_move_death;
}
