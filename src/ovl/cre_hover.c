/* Hover (m_hover.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum { L_NONE, L_RUN, L_STAND, L_DEAD, L_ATTACK, L_REATTACK, L_FIRE };

#define CLASS_HOVER 80
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* gcc emits the small functions last, in this order */
void hover_run(struct edict_s *self);
void hover_stand(struct edict_s *self);
void hover_dead(struct edict_s *self);
void hover_attack(struct edict_s *self);
void hover_reattack(struct edict_s *self);
void hover_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);
void SP_monster_hover(struct edict_s *self);
void hover_sight(struct edict_s *self);
void hover_search(struct edict_s *self);
void hover_walk(struct edict_s *self);
void hover_start_attack(struct edict_s *self);
void hover_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
void hover_float(struct edict_s *self);
void PreInit_Hover(struct GLOBMODHDR *Hdr);
void hover_fire_blaster(struct edict_s *self);
static struct NEWANIMREMAP AnimRemap[];
static short ShadowVerts[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_hover, 0, PreInit_Hover, AnimRemap };

static int sound_pain1;
static int sound_pain2;
static int sound_death1;
static int sound_death2;
static int sound_sight;
static int sound_search1;
static int sound_search2;
static int sound_udeath;
static struct MODSUBOFF LeftMuzzle;
static struct MODSUBOFF RightMuzzle;
static int Unused1[2];
struct ai_local_table_t AiTable[34];

void PreInit_Hover(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Hover\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Hover");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Hover");
        Iface.PrintF("Validated Creature Interface for %s\n", "Hover");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_RUN].thinkfunc = hover_run;
        AiTable[L_STAND].thinkfunc = hover_stand;
        AiTable[L_DEAD].thinkfunc = hover_dead;
        AiTable[L_ATTACK].thinkfunc = hover_attack;
        AiTable[L_REATTACK].thinkfunc = hover_reattack;
        AiTable[L_FIRE].thinkfunc = hover_fire_blaster;
        Iface.SetMyAiTable(CLASS_HOVER, AiTable);
        Hdr->ShadowVerts = ShadowVerts;
        sound_pain1 = sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"hov_pain2"});
        sound_death1 = sound_death2 = Iface.GetSoundNum((struct IDENTITYNAME){"hov_deth2"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"hov_sght1"});
        sound_search1 = sound_search2 = Iface.GetSoundNum((struct IDENTITYNAME){"hov_srch2"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 275, 277, &LeftMuzzle))
            Iface.PrintF("Hover left Blaster muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 267, 269, &RightMuzzle))
            Iface.PrintF("Hover right Blaster muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Hover\r\n");
}

/* The original's .rdata alignment padding holds a stray byte. */
__asm__(".rdata\n\t.byte 0x00, 0x82\n\t.text");

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 29, {"Stand"}, 0, 0 },
    { 30, 64, {"Forward"}, 0, 0 },
    { 65, 72, {"Stop1"}, 0, 0 },
    { 74, 81, {"Stop2"}, 0, 0 },
    { 82, 111, {"Takeoff"}, 0, 0 },
    { 112, 112, {"Landing"}, 0, 0 },
    { 113, 140, {"Pain1"}, 0, 0 },
    { 141, 152, {"Pain2"}, 0, 0 },
    { 153, 161, {"Pain3"}, 0, 0 },
    { 162, 172, {"Death"}, 0, 0 },
    { 173, 196, {"Back"}, 0, 0 },
    { 197, 199, {"Startattack"}, 0, 0 },
    { 200, 202, {"Attack"}, 0, 0 },
    { 203, 204, {"Endattack"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1 };

static struct mframe_t hover_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t hover_move_stand = { 0, 29, hover_frames_stand, 0 };
static struct mframe_t hover_frames_stop1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_stop1 = { 65, 72, hover_frames_stop1, 0 };
static struct mframe_t hover_frames_stop2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_stop2 = { 74, 81, hover_frames_stop2, 0 };
static struct mframe_t hover_frames_takeoff[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, -1, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 },
    { AI_MOVE, -1, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, -6, 0 },
    { AI_MOVE, -9, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 3, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_takeoff = { 82, 111, hover_frames_takeoff, 0 };
static struct mframe_t hover_frames_pain3[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_pain3 = { 153, 161, hover_frames_pain3, hover_run };
static struct mframe_t hover_frames_pain2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_pain2 = { 141, 152, hover_frames_pain2, hover_run };
static struct mframe_t hover_frames_pain1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, -8, 0 },
    { AI_MOVE, -4, 0 }, { AI_MOVE, -6, 0 }, { AI_MOVE, -4, 0 }, { AI_MOVE, -3, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 3, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 },
    { AI_MOVE, 3, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 7, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, 3, 0 }, { AI_MOVE, 4, 0 },
};
static struct mmove_t hover_move_pain1 = { 113, 140, hover_frames_pain1, hover_run };
static struct mframe_t hover_frames_land[] = {
    { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_land = { 112, 112, hover_frames_land, 0 };
static struct mframe_t hover_frames_forward[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_forward = { 30, 64, hover_frames_forward, 0 };
static struct mframe_t hover_frames_walk[] = {
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
};
static struct mmove_t hover_move_walk = { 30, 64, hover_frames_walk, 0 };
static struct mframe_t hover_frames_run[] = {
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
};
static struct mmove_t hover_move_run = { 30, 64, hover_frames_run, 0 };
static struct mframe_t hover_frames_death1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -10, 0 }, { AI_MOVE, 3, 0 },
    { AI_MOVE, 5, 0 }, { AI_MOVE, 4, 0 }, { AI_MOVE, 7, 0 },
};
static struct mmove_t hover_move_death1 = { 162, 172, hover_frames_death1, hover_dead };
static struct mframe_t hover_frames_backward[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t hover_move_backward = { 173, 196, hover_frames_backward, 0 };
static struct mframe_t hover_frames_start_attack[] = {
    { AI_CHARGE, 1, 0 }, { AI_CHARGE, 1, 0 }, { AI_CHARGE, 1, 0 },
};
static struct mmove_t hover_move_start_attack = { 197, 199, hover_frames_start_attack, hover_attack };
static struct mframe_t hover_frames_attack1[] = {
    { AI_CHARGE, -10, L_FIRE }, { AI_CHARGE, -10, L_FIRE }, { AI_CHARGE, 0, L_REATTACK },
};
static struct mmove_t hover_move_attack1 = { 200, 202, hover_frames_attack1, 0 };
static struct mframe_t hover_frames_end_attack[] = {
    { AI_CHARGE, 1, 0 }, { AI_CHARGE, 1, 0 },
};
static struct mmove_t hover_move_end_attack = { 203, 204, hover_frames_end_attack, hover_run };

static int Unused0 = 0; /* in .data */

void hover_fire_blaster(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT end;
    struct VECT dir;
    struct SVECT Off;
    int effect;

    Cre = CRE(self);
    effect = self->s.frame == 200 ? 64 : 0;
    if (self->s.frame & 1)
        Off = Iface.GetSubModXYZ(Cre->ModelHdr, LeftMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    else
        Off = Iface.GetSubModXYZ(Cre->ModelHdr, RightMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Off, &Off);
    start.vx = Cre->Draw.Location.vx + Off.vx;
    start.vy = Cre->Draw.Location.vy + Off.vy;
    start.vz = Cre->Draw.Location.vz + Off.vz;
    end.vx = self->enemy->sorigin.vx;
    end.vy = self->enemy->sorigin.vy;
    end.vz = self->enemy->sorigin.vz;
    end.vy += self->enemy->viewheight;
    dir.vx = end.vx - start.vx;
    dir.vy = end.vy - start.vy;
    dir.vz = end.vz - start.vz;
    Iface.monster_fire_blaster(self, &start, &dir, 1, 1000, 0, effect);
}

void hover_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &hover_move_stand;
    else
        self->monsterinfo.currentmove = &hover_move_run;
}

void hover_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &hover_move_stand;
}

void hover_dead(struct edict_s *self)
{
    CRE(self)->Health = self->gib_health;
    self->nextthink = 0;
    self->takedamage = 0;
    self->svflags |= 2;
    self->movetype = 7;
}

void hover_attack(struct edict_s *self)
{
    self->monsterinfo.currentmove = &hover_move_attack1;
}

void hover_reattack(struct edict_s *self)
{
    if (CRE(self->enemy)->Health > 0)
        if (Iface.visible(self, self->enemy))
            if (Iface.rand() < 19661) {
                self->monsterinfo.currentmove = &hover_move_attack1;
                return;
            }
    self->monsterinfo.currentmove = &hover_move_end_attack;
}

void hover_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    if (CRE(self)->Health <= self->gib_health) {
        Iface.SoundByCreature(sound_udeath, CRE(self));
        self->deadflag = 2;
        hover_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_death2, CRE(self));
    self->deadflag = 2;
    self->takedamage = 1;
    self->monsterinfo.currentmove = &hover_move_death1;
}

void SP_monster_hover(struct edict_s *self)
{
    self->mass = 150;
    self->pain = hover_pain;
    self->die = hover_die;
    self->monsterinfo.stand = hover_stand;
    self->monsterinfo.walk = hover_walk;
    self->monsterinfo.run = hover_run;
    self->monsterinfo.attack = hover_start_attack;
    self->monsterinfo.sight = hover_sight;
    self->monsterinfo.search = hover_search;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &hover_move_stand;
    self->monsterinfo.scale = 10;
    Iface.flymonster_start(self);
}

void hover_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, CRE(self));
}

void hover_search(struct edict_s *self)
{
    Iface.SoundByCreature(sound_search1, CRE(self));
}

void hover_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &hover_move_walk;
}

void hover_start_attack(struct edict_s *self)
{
    self->monsterinfo.currentmove = &hover_move_start_attack;
}

void hover_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 25) {
        if (Iface.rand() < 16384) {
            Iface.SoundByCreature(sound_pain1, CRE(self));
            self->monsterinfo.currentmove = &hover_move_pain3;
        } else {
            Iface.SoundByCreature(sound_pain1, CRE(self));
            self->monsterinfo.currentmove = &hover_move_pain2;
        }
    } else {
        Iface.SoundByCreature(sound_pain1, CRE(self));
        self->monsterinfo.currentmove = &hover_move_pain1;
    }
}

/* nothing uses it */
void hover_float(struct edict_s *self)
{
    if (!self->groundentity && Iface.level->time < self->timestamp)
        self->nextthink = Iface.level->time + 1;
}
