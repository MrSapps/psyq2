/* Berserker (m_berserk.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum { L_NONE, L_FIDGET, L_STAND, L_SWING, L_SPIKE, L_RUN, L_CLUB, L_STRIKE, L_DEAD };

#define CLASS_BERSERK 70

/* gcc emits the small functions last, in this order */
void PreInit_Berserk(struct GLOBMODHDR *Hdr);
void Berserk_EveryFrame(struct CREATURE *Cre);
static void berserk_fidget(struct edict_s *self);
static void berserk_stand(struct edict_s *self);
static void berserk_swing(struct edict_s *self);
static void berserk_attack_spike(struct edict_s *self);
static void berserk_run(struct edict_s *self);
static void berserk_attack_club(struct edict_s *self);
static void berserk_strike(struct edict_s *self);
static void berserk_dead(struct edict_s *self);
void SP_monster_berserk(struct edict_s *self);
static void berserk_sight(struct edict_s *self);
static void berserk_search(struct edict_s *self);
static void berserk_walk(struct edict_s *self);
static void berserk_melee(struct edict_s *self);
static void berserk_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void berserk_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_berserk, Berserk_EveryFrame, PreInit_Berserk, AnimRemap };

static struct NEWANIMREMAP AnimRemap[] = {
    { 36, 41, {"Run"}, 0, 0 },
    { 25, 35, {"Walk"}, 0, 0 },
    { 76, 83, {"Attack1"}, 0, 0 },
    { 84, 95, {"Attack2"}, 0, 0 },
    { 96, 109, {"Attack3"}, 0, 0 },
    { 199, 202, {"Pain1"}, 0, 0 },
    { 203, 222, {"Pain2"}, 0, 0 },
    { 223, 235, {"Death"}, 0, 0 },
    { 236, 243, {"Death2"}, 0, 0 },
    { 0, 4, {"Stand"}, 0, 0 },
    { 5, 24, {"Fidget"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static int sound_pain;
static int sound_die;
static int sound_idle;
static int sound_punch;
static int sound_sight;
static int sound_search;
static int sound_udeath;
/* the frames of the attacks that leave a blur */
static short Attack1BlurStart, Attack1BlurEnd;
static short Attack2BlurStart, Attack2BlurEnd;
static short Attack3BlurStart, Attack3BlurEnd;
static int Unused[2]; /* space the original has that no code uses */
static struct ai_local_table_t AiTable[32];
static int Unused2;

void PreInit_Berserk(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *Anim;

    if (Hdr) {
        Iface.PrintF("Pre Init Berserk\r\n");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_FIDGET].thinkfunc = berserk_fidget;
        AiTable[L_STAND].thinkfunc = berserk_stand;
        AiTable[L_SWING].thinkfunc = berserk_swing;
        AiTable[L_SPIKE].thinkfunc = berserk_attack_spike;
        AiTable[L_RUN].thinkfunc = berserk_run;
        AiTable[L_CLUB].thinkfunc = berserk_attack_club;
        AiTable[L_STRIKE].thinkfunc = berserk_strike;
        AiTable[L_DEAD].thinkfunc = berserk_dead;
        Iface.SetMyAiTable(CLASS_BERSERK, AiTable);

        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Attack1"}))) {
            Attack1BlurStart = Anim->AnimDetails.Start + 60;
            Attack1BlurEnd = Anim->AnimDetails.Start + 140;
        } else {
            Attack1BlurStart = 1;
            Attack1BlurEnd = 0;
        }
        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Attack2"}))) {
            Attack2BlurStart = Anim->AnimDetails.Start + 130;
            Attack2BlurEnd = Anim->AnimDetails.Start + 190;
        } else {
            Attack2BlurStart = 1;
            Attack2BlurEnd = 0;
        }
        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Attack3"}))) {
            Attack3BlurStart = Anim->AnimDetails.Start + 100;
            Attack3BlurEnd = Anim->AnimDetails.Start + 240;
        } else {
            Attack3BlurStart = 1;
            Attack3BlurEnd = 0;
        }

        if (!(sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"ber_pain2"})))
            sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"inf_pain1"});
        if (!(sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"ber_deth2"})))
            sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"inf_deth2"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"ber_idle1"});
        if (!(sound_punch = Iface.GetSoundNum((struct IDENTITYNAME){"ber_attack"})))
            sound_punch = Iface.GetSoundNum((struct IDENTITYNAME){"inf_atck2"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"ber_sight"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"ber_srch1"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
    } else
        Iface.PrintF("Clear Init Berserk\r\n");
}

void Berserk_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->AnimFrame >= Attack1BlurStart && Cre->AnimFrame <= Attack1BlurEnd)
        Iface.AddBlurPoints(Cre, (short[]){ 0x118, 0x119, 0x11e, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
    if (Cre->AnimFrame >= Attack2BlurStart && Cre->AnimFrame <= Attack2BlurEnd)
        Iface.AddBlurPoints(Cre, (short[]){ 0x113, 0x107, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
    if (Cre->AnimFrame >= Attack3BlurStart && Cre->AnimFrame <= Attack3BlurEnd)
        Iface.AddBlurPoints(Cre, (short[]){ 0x112, 0x109, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
}

/* The original's .rdata alignment padding holds stray bytes (cre_berserk_jail3's doesn't). */
#ifndef RDATA_PADDING
#define RDATA_PADDING ".byte 0x73, 0x65"
#endif
__asm__(".rdata\n\t" RDATA_PADDING "\n\t.text");

static struct mframe_t berserk_frames_stand[] = {
    { AI_STAND, 0, L_FIDGET },
    { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE },
};
static struct mmove_t berserk_move_stand = { 0, 4, berserk_frames_stand, 0 };

static struct mframe_t berserk_frames_stand_fidget[] = {
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
};
static struct mmove_t berserk_move_stand_fidget = { 5, 24, berserk_frames_stand_fidget, berserk_stand };

static void berserk_fidget(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        return;
    if (Iface.rand() >= 4916)
        return;
    self->monsterinfo.currentmove = &berserk_move_stand_fidget;
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void berserk_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &berserk_move_stand;
}

static struct mframe_t berserk_frames_walk[] = {
    { AI_WALK, 9, L_NONE },
    { AI_WALK, 6, L_NONE },
    { AI_WALK, 5, L_NONE },
    { AI_WALK, 7, L_NONE },
    { AI_WALK, 6, L_NONE },
    { AI_WALK, 8, L_NONE },
    { AI_WALK, 7, L_NONE },
    { AI_WALK, 6, L_NONE },
    { AI_WALK, 5, L_NONE },
    { AI_WALK, 5, L_NONE },
    { AI_WALK, 5, L_NONE },
    { AI_WALK, 5, L_NONE }, /* one frame more than the move uses */
};
static struct mmove_t berserk_move_walk = { 25, 35, berserk_frames_walk, 0 };

static void berserk_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &berserk_move_walk;
}

static struct mframe_t berserk_frames_run1[] = {
    { AI_RUN, 21, L_NONE },
    { AI_RUN, 11, L_NONE },
    { AI_RUN, 21, L_NONE },
    { AI_RUN, 25, L_NONE },
    { AI_RUN, 18, L_NONE },
    { AI_RUN, 19, L_NONE },
};
static struct mmove_t berserk_move_run1 = { 36, 41, berserk_frames_run1, 0 };

static void berserk_run(struct edict_s *self)
{
    self->monsterinfo.currentmove = &berserk_move_run1;
}

static void berserk_swing(struct edict_s *self)
{
    Iface.SoundByCreature(sound_punch, self->Extra.Creature);
}

static struct mframe_t berserk_frames_attack_spike[] = {
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_SWING },
    { AI_CHARGE, 0, L_SPIKE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
};
static struct mmove_t berserk_move_attack_spike = { 76, 83, berserk_frames_attack_spike, berserk_run };

static void berserk_attack_club(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = 48;
    aim.vz = Iface.getminsvx(self);
    Iface.fire_hit(self, aim, 5 + Iface.rand() % 6, 400);
}

static struct mframe_t berserk_frames_attack_club[] = {
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_SWING },
    { AI_CHARGE, 0, L_CLUB },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
    { AI_CHARGE, 0, L_NONE },
};
static struct mmove_t berserk_move_attack_club = { 84, 95, berserk_frames_attack_club, berserk_run };

static void berserk_strike(struct edict_s *self)
{
}

static struct mframe_t berserk_frames_attack_strike[] = {
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_SWING },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_STRIKE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 10, L_NONE },
    { AI_MOVE, 14, L_NONE },
};
static struct mmove_t berserk_move_attack_strike = { 96, 109, berserk_frames_attack_strike, berserk_run };

static void berserk_melee(struct edict_s *self)
{
    if ((Iface.rand() & 1) == 0)
        self->monsterinfo.currentmove = &berserk_move_attack_spike;
    else
        self->monsterinfo.currentmove = &berserk_move_attack_club;
}

static struct mframe_t berserk_frames_pain1[] = {
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
};
static struct mmove_t berserk_move_pain1 = { 199, 202, berserk_frames_pain1, berserk_run };

static struct mframe_t berserk_frames_pain2[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t berserk_move_pain2 = { 203, 222, berserk_frames_pain2, berserk_run };

static void berserk_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain, self->Extra.Creature);
    if ((*Iface.skill)->value == 3)
        return;
    if (damage < 20 || Iface.rand() < 16384)
        self->monsterinfo.currentmove = &berserk_move_pain1;
    else
        self->monsterinfo.currentmove = &berserk_move_pain2;
}

static void berserk_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->nextthink = 0;
    self->svflags |= 2;
}

static struct mframe_t berserk_frames_death1[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
};
static struct mmove_t berserk_move_death1 = { 223, 235, berserk_frames_death1, berserk_dead };

static struct mframe_t berserk_frames_death2[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t berserk_move_death2 = { 236, 243, berserk_frames_death2, berserk_dead };

static void berserk_attack_spike(struct edict_s *self)
{
    static struct VECT aim = { 1020, 288, 0 };

    Iface.fire_hit(self, aim, 15 + Iface.rand() % 6, 400);
}

static void berserk_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health <= self->gib_health) {
        self->deadflag = 2;
        berserk_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, self->Extra.Creature);
    self->deadflag = 2;
    self->takedamage = 1;
    if (damage >= 50)
        self->monsterinfo.currentmove = &berserk_move_death1;
    else
        self->monsterinfo.currentmove = &berserk_move_death2;
}

static void berserk_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void berserk_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search, self->Extra.Creature);
}

void SP_monster_berserk(struct edict_s *self)
{
    self->mass = 250;
    self->pain = berserk_pain;
    self->die = berserk_die;
    self->monsterinfo.stand = berserk_stand;
    self->monsterinfo.walk = berserk_walk;
    self->monsterinfo.run = berserk_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = 0;
    self->monsterinfo.melee = berserk_melee;
    self->monsterinfo.sight = berserk_sight;
    self->monsterinfo.search = berserk_search;
    self->monsterinfo.currentmove = &berserk_move_stand;
    self->monsterinfo.scale = 10;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    Iface.walkmonster_start(self);
}
