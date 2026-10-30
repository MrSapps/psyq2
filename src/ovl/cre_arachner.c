/* Arachner: the PlayStation's railgun spider, built on m_gladiator.c */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum { L_NONE, L_WALK, L_HIT1, L_FIRE_RIGHT, L_FIRE_LEFT, L_RUN, L_UNUSED, L_DEAD, L_HIT2 };

#define CLASS_ARACHNER 64
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)


/* gcc emits the small functions last, in this order */
void arachner_walk(struct edict_s *self);
void arachner_hit1(struct edict_s *self);
void arachner_hit2(struct edict_s *self);
void arachner_run(struct edict_s *self);
void arachner_dead(struct edict_s *self);
void SP_monster_arachner(struct edict_s *self);
void arachner_attack1(struct edict_s *self);
void arachner_melee_loop(struct edict_s *self);
void arachner_end_melee(struct edict_s *self);
void arachner_stand(struct edict_s *self);
void arachner_idle(struct edict_s *self);
void arachner_sight(struct edict_s *self);
void arachner_search(struct edict_s *self);
void arachner_swing(struct edict_s *self);
void arachner_melee(struct edict_s *self);
void arachner_attack(struct edict_s *self);
void arachner_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
void arachner_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);
void PreInit_Arachner(struct GLOBMODHDR *Hdr);
void Arachner_EveryFrame(struct CREATURE *Cre);
void arachner_fire_right(struct edict_s *self);
void arachner_fire_left(struct edict_s *self);
static struct NEWANIMREMAP AnimRemap[];
static struct mmove_t arachner_move_stand, arachner_move_walk, arachner_move_run, arachner_move_melee;
static struct mmove_t arachner_move_start_melee, arachner_move_end_melee, arachner_move_attack1;
static struct mmove_t arachner_move_start_attack, arachner_move_attack3, arachner_move_pain1;
static struct mmove_t arachner_move_pain2, arachner_move_death;

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_arachner, Arachner_EveryFrame, PreInit_Arachner, AnimRemap };

static int sound_pain1;
static int sound_pain2;
static int sound_die;
static int sound_gun;
static int sound_melee;
static int Unused0;
static int Unused1;
static int sound_idle;
static int sound_search;
static int sound_sight;
static int sound_udeath;
static struct MODSUBOFF LeftMuzzle;
static struct MODSUBOFF RightMuzzle;
static int Unused2;
static short MeleeBlurStart;
static struct ai_local_table_t AiTable[32];
static short MeleeBlurEnd;
static short Unused3;

void PreInit_Arachner(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *Anim;

    if (Hdr) {
        Iface.PrintF("Pre Init Arachner\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Arachner");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Arachner");
        Iface.PrintF("Validated Creature Interface for %s\n", "Arachner");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_WALK].thinkfunc = arachner_walk;
        AiTable[L_HIT1].thinkfunc = arachner_hit1;
        AiTable[L_HIT2].thinkfunc = arachner_hit2;
        AiTable[L_FIRE_RIGHT].thinkfunc = arachner_fire_right;
        AiTable[L_FIRE_LEFT].thinkfunc = arachner_fire_left;
        AiTable[L_RUN].thinkfunc = arachner_run;
        AiTable[L_DEAD].thinkfunc = arachner_dead;
        Iface.SetMyAiTable(CLASS_ARACHNER, AiTable);
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"ara_pain2"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"ara_deth1"});
        sound_gun = Iface.GetSoundNum((struct IDENTITYNAME){"wep_railgf1a"});
        sound_melee = Iface.GetSoundNum((struct IDENTITYNAME){"ara_melee1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"ara_idle1"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"ara_srch1"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"ara_sght1"});
        sound_pain2 = Iface.GetSoundClone(sound_pain1);
        Iface.AlterSampleAttrs(sound_pain2, 50, 128, -1);
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Melee"}))) {
            MeleeBlurStart = Anim->AnimDetails.Start;
            MeleeBlurEnd = Anim->AnimDetails.Stop;
        } else {
            MeleeBlurStart = 1;
            MeleeBlurEnd = 0;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 239, 253, &LeftMuzzle))
            Iface.PrintF("Arachner left Railgun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 290, 307, &RightMuzzle))
            Iface.PrintF("Arachner right Railgun muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Arachner\r\n");
}

void Arachner_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->AnimFrame >= MeleeBlurStart && Cre->AnimFrame <= MeleeBlurEnd) {
        Iface.AddBlurPoints(Cre, (short[]){ 290, 299, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
        Iface.AddBlurPoints(Cre, (short[]){ 239, 244, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
    }
}

/* The original's .rdata alignment padding holds stray bytes. */
__asm__(".rdata\n\t.byte 0x06, 0x78\n\t.text");

static struct NEWANIMREMAP AnimRemap[] = {
    { 25, 34, {"Attack 1"}, 0, 0 },
    { 94, 109, {"Attack 3"}, 0, 0 },
    { 110, 129, {"Death 2"}, 0, 0 },
    { 53, 64, {"Melee"}, 0, 0 },
    { 35, 39, {"Pain 1"}, 0, 0 },
    { 40, 45, {"Pain 2"}, 0, 0 },
    { 78, 93, {"Rear"}, 0, 0 },
    { 130, 132, {"Start Attack"}, 0, 0 },
    { 133, 135, {"Start Melee"}, 0, 0 },
    { 0, 12, {"Stand"}, 0, 0 },
    { 65, 77, {"Sway"}, 0, 0 },
    { 16, 24, {"Walk"}, 0, 0 },
    { 136, 138, {"End Melee"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static struct mframe_t arachner_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 },
};
static struct mmove_t arachner_move_stand = { 0, 12, arachner_frames_stand, arachner_stand };
static struct mframe_t arachner_frames_sway[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 },
};
static struct mmove_t arachner_move_sway = { 65, 77, arachner_frames_sway, arachner_stand };
static struct mframe_t arachner_frames_rear[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t arachner_move_rear = { 78, 93, arachner_frames_rear, arachner_stand };
static struct mframe_t arachner_frames_walk[] = {
    { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 },
    { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 }, { AI_WALK, 10, 0 },
    { AI_WALK, 10, 0 },
};
static struct mmove_t arachner_move_walk = { 16, 24, arachner_frames_walk, 0 };
static struct mframe_t arachner_frames_run[] = {
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 },
};
static struct mmove_t arachner_move_run = { 16, 24, arachner_frames_run, 0 };
static struct mframe_t arachner_frames_melee[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_HIT1 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_HIT2 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t arachner_move_melee = { 53, 64, arachner_frames_melee, arachner_end_melee };
static struct mframe_t arachner_frames_start_melee[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t arachner_move_start_melee = { 133, 135, arachner_frames_start_melee, arachner_melee_loop };
static struct mframe_t arachner_frames_end_melee[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t arachner_move_end_melee = { 136, 138, arachner_frames_end_melee, arachner_run };
static struct mframe_t arachner_frames_attack1[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE_RIGHT }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE_LEFT }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, /* one more frame than the move uses */
};
static struct mmove_t arachner_move_attack1 = { 25, 33, arachner_frames_attack1, arachner_run };
static struct mframe_t arachner_frames_start_attack[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t arachner_move_start_attack = { 130, 132, arachner_frames_start_attack, arachner_attack1 };
static struct mframe_t arachner_frames_attack3[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE_RIGHT }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_FIRE_LEFT },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t arachner_move_attack3 = { 94, 109, arachner_frames_attack3, arachner_run };
static struct mframe_t arachner_frames_pain1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t arachner_move_pain1 = { 35, 39, arachner_frames_pain1, arachner_run };
static struct mframe_t arachner_frames_pain2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t arachner_move_pain2 = { 40, 45, arachner_frames_pain2, arachner_run };
static struct mframe_t arachner_frames_death[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t arachner_move_death = { 110, 129, arachner_frames_death, arachner_dead };

void arachner_fire_right(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT dir;
    struct SVECT Off;

    Cre = CRE(self);
    Off = Iface.GetSubModXYZ(Cre->ModelHdr, RightMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Off, &Off);
    start.vx = Cre->Draw.Location.vx + Off.vx;
    start.vy = Cre->Draw.Location.vy + Off.vy;
    start.vz = Cre->Draw.Location.vz + Off.vz;
    dir.vx = self->pos1.vx - start.vx;
    dir.vy = self->pos1.vy - start.vy;
    dir.vz = self->pos1.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.SoundByCreature(sound_gun, CRE(self));
    Iface.monster_fire_railgun(self, &start, &dir, 50, 100, 0);
    /* aim the next shot */
    self->pos1.vx = self->enemy->sorigin.vx;
    self->pos1.vy = self->enemy->sorigin.vy;
    self->pos1.vz = self->enemy->sorigin.vz;
    self->pos1.vy += self->enemy->viewheight;
}

void arachner_fire_left(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT dir;
    struct SVECT Off;

    Cre = CRE(self);
    Off = Iface.GetSubModXYZ(Cre->ModelHdr, LeftMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Off, &Off);
    start.vx = Cre->Draw.Location.vx + Off.vx;
    start.vy = Cre->Draw.Location.vy + Off.vy;
    start.vz = Cre->Draw.Location.vz + Off.vz;
    dir.vx = self->pos1.vx - start.vx;
    dir.vy = self->pos1.vy - start.vy;
    dir.vz = self->pos1.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.SoundByCreature(sound_gun, CRE(self));
    Iface.monster_fire_railgun(self, &start, &dir, 50, 100, 0);
}

void arachner_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_walk;
}

void arachner_hit1(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = -48;
    aim.vz = 0;
    Iface.fire_hit(self, aim, 20 + Iface.rand() % 5, 100);
}

void arachner_hit2(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = -48;
    aim.vz = 0;
    Iface.fire_hit(self, aim, 20 + Iface.rand() % 5, 100);
}

void arachner_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &arachner_move_stand;
    else
        self->monsterinfo.currentmove = &arachner_move_run;
}

void arachner_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->svflags |= 2;
    self->nextthink = 0;
}

void SP_monster_arachner(struct edict_s *self)
{
    self->mass = 400;
    self->pain = arachner_pain;
    self->die = arachner_die;
    self->monsterinfo.stand = arachner_stand;
    self->monsterinfo.walk = arachner_walk;
    self->monsterinfo.run = arachner_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = arachner_attack;
    self->monsterinfo.melee = arachner_melee;
    self->monsterinfo.sight = arachner_sight;
    self->monsterinfo.idle = arachner_idle;
    self->monsterinfo.search = arachner_search;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    self->monsterinfo.currentmove = &arachner_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

void arachner_attack1(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_attack1;
}

void arachner_melee_loop(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_melee;
}

void arachner_end_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_end_melee;
}

void arachner_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_stand;
}

void arachner_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, CRE(self));
}

void arachner_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, CRE(self));
}

void arachner_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search, CRE(self));
}

void arachner_swing(struct edict_s *self)
{
    Iface.SoundByCreature(sound_melee, CRE(self));
}

void arachner_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &arachner_move_start_melee;
}

void arachner_attack(struct edict_s *self)
{
    struct VECT v;

    v.vx = self->sorigin.vx - self->enemy->sorigin.vx;
    v.vy = self->sorigin.vy - self->enemy->sorigin.vy;
    v.vz = self->sorigin.vz - self->enemy->sorigin.vz;
    if (Iface.VectorLength(&v) <= 1052)
        return;
    self->pos1.vx = self->enemy->sorigin.vx;
    self->pos1.vy = self->enemy->sorigin.vy;
    self->pos1.vz = self->enemy->sorigin.vz;
    self->pos1.vy += self->enemy->viewheight;
    if (abs(v.vy) > abs(v.vx) + abs(v.vz))
        self->monsterinfo.currentmove = &arachner_move_attack3;
    else
        self->monsterinfo.currentmove = &arachner_move_start_attack;
}

void arachner_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if (Iface.rand() & 4)
        Iface.SoundByCreature(sound_pain1, CRE(self));
    else
        Iface.SoundByCreature(sound_pain2, CRE(self));
    if ((*Iface.skill)->value == 3)
        return;
    if (Iface.rand() > 16384)
        self->monsterinfo.currentmove = &arachner_move_pain2;
    else
        self->monsterinfo.currentmove = &arachner_move_pain1;
}

void arachner_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    if (CRE(self)->Health <= self->gib_health) {
        self->deadflag = 2;
        arachner_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, CRE(self));
    self->deadflag = 2;
    self->takedamage = 1;
    self->monsterinfo.currentmove = &arachner_move_death;
}
