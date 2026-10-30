/* Iron Maiden (m_chick.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum {
    L_NONE, L_STAND, L_RUN, L_RESLASH, L_REROCKET, L_ATTACK1, L_MOAN, L_FIDGET, L_DEAD, L_DUCK_DOWN,
    L_DUCK_HOLD, L_DUCK_UP, L_PREATTACK1, L_ROCKET, L_RELOAD, L_SLASH_START, L_SLASH
};

#define CLASS_CHICK 74
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* gcc emits the small functions last, in this order */
void chick_stand(struct edict_s *self);
void chick_run(struct edict_s *self);
void chick_reslash(struct edict_s *self);
void chick_rerocket(struct edict_s *self);
void chick_attack1(struct edict_s *self);
void ChickMoan(struct edict_s *self);
void chick_fidget(struct edict_s *self);
void chick_dead(struct edict_s *self);
void chick_duck_down(struct edict_s *self);
void chick_duck_hold(struct edict_s *self);
void chick_duck_up(struct edict_s *self);
void Chick_PreAttack1(struct edict_s *self);
void ChickReload(struct edict_s *self);
void chick_slash(struct edict_s *self);
void ChickSlash(struct edict_s *self);
void SP_monster_chick(struct edict_s *self);
void Chick_EveryFrame(struct CREATURE *Cre);
void chick_walk(struct edict_s *self);
void chick_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
void chick_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);
void chick_dodge(struct edict_s *self, struct edict_s *attacker, long eta);
void chick_melee(struct edict_s *self);
void chick_attack(struct edict_s *self);
void chick_sight(struct edict_s *self);
void PreInit_Chick(struct GLOBMODHDR *Hdr);
void ChickRocket(struct edict_s *self);

static struct mframe_t chick_frames_fidget[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, L_MOAN }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t chick_move_fidget = { 151, 180, chick_frames_fidget, chick_stand };
static struct mframe_t chick_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, L_FIDGET },
};
static struct mmove_t chick_move_stand = { 121, 150, chick_frames_stand, 0 };
static struct mframe_t chick_frames_start_run[] = {
    { AI_RUN, 1, 0 }, { AI_RUN, 0, 0 }, { AI_RUN, 0, 0 }, { AI_RUN, -1, 0 },
    { AI_RUN, -1, 0 }, { AI_RUN, 0, 0 }, { AI_RUN, 1, 0 }, { AI_RUN, 3, 0 },
    { AI_RUN, 6, 0 }, { AI_RUN, 3, 0 },
};
static struct mmove_t chick_move_start_run = { 181, 190, chick_frames_start_run, chick_run };
static struct mframe_t chick_frames_run[] = {
    { AI_RUN, 6, 0 }, { AI_RUN, 8, 0 }, { AI_RUN, 13, 0 }, { AI_RUN, 5, 0 },
    { AI_RUN, 7, 0 }, { AI_RUN, 4, 0 }, { AI_RUN, 11, 0 }, { AI_RUN, 5, 0 },
    { AI_RUN, 9, 0 }, { AI_RUN, 7, 0 },
};
static struct mmove_t chick_move_run = { 191, 200, chick_frames_run, 0 };
static struct mframe_t chick_frames_walk[] = {
    { AI_WALK, 6, 0 }, { AI_WALK, 8, 0 }, { AI_WALK, 13, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 7, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 11, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 9, 0 }, { AI_WALK, 7, 0 },
};
static struct mmove_t chick_move_walk = { 191, 200, chick_frames_walk, 0 };
static struct mframe_t chick_frames_pain1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t chick_move_pain1 = { 90, 94, chick_frames_pain1, chick_run };
static struct mframe_t chick_frames_pain2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t chick_move_pain2 = { 95, 99, chick_frames_pain2, chick_run };
static struct mframe_t chick_frames_pain3[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -6, 0 }, { AI_MOVE, 3, 0 },
    { AI_MOVE, 11, 0 }, { AI_MOVE, 3, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 4, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -3, 0 },
    { AI_MOVE, -4, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, 7, 0 }, { AI_MOVE, -2, 0 },
    { AI_MOVE, 3, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, -8, 0 },
    { AI_MOVE, 2, 0 },
};
static struct mmove_t chick_move_pain3 = { 100, 120, chick_frames_pain3, chick_run };
static struct mframe_t chick_frames_death2[] = {
    { AI_MOVE, -6, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -5, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 10, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 3, 0 }, { AI_MOVE, 1, 0 },
    { AI_MOVE, 2, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 3, 0 }, { AI_MOVE, 3, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, -3, 0 }, { AI_MOVE, -5, 0 }, { AI_MOVE, 4, 0 },
    { AI_MOVE, 15, 0 }, { AI_MOVE, 14, 0 },
};
static struct mmove_t chick_move_death2 = { 60, 81, chick_frames_death2, chick_dead };
static struct mframe_t chick_frames_death1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -7, 0 }, { AI_MOVE, 4, 0 },
    { AI_MOVE, 11, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t chick_move_death1 = { 48, 59, chick_frames_death1, chick_dead };
static struct mframe_t chick_frames_duck[] = {
    { AI_MOVE, 0, L_DUCK_DOWN }, { AI_MOVE, 1, 0 }, { AI_MOVE, 4, L_DUCK_HOLD }, { AI_MOVE, -4, 0 },
    { AI_MOVE, -5, L_DUCK_UP }, { AI_MOVE, 3, 0 }, { AI_MOVE, 1, 0 },
};
static struct mmove_t chick_move_duck = { 83, 89, chick_frames_duck, chick_run };
static struct mframe_t chick_frames_start_attack1[] = {
    { AI_CHARGE, 0, L_PREATTACK1 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 4, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, -3, 0 }, { AI_CHARGE, 3, 0 }, { AI_CHARGE, 5, 0 },
    { AI_CHARGE, 7, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, L_ATTACK1 },
};
static struct mmove_t chick_move_start_attack1 = { 0, 12, chick_frames_start_attack1, 0 };
static struct mframe_t chick_frames_attack1[] = {
    { AI_CHARGE, 19, L_ROCKET }, { AI_CHARGE, -6, 0 }, { AI_CHARGE, -5, 0 }, { AI_CHARGE, -2, 0 },
    { AI_CHARGE, -7, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 1, 0 }, { AI_CHARGE, 10, L_RELOAD },
    { AI_CHARGE, 4, 0 }, { AI_CHARGE, 5, 0 }, { AI_CHARGE, 6, 0 }, { AI_CHARGE, 6, 0 },
    { AI_CHARGE, 4, 0 }, { AI_CHARGE, 3, L_REROCKET },
};
static struct mmove_t chick_move_attack1 = { 13, 26, chick_frames_attack1, 0 };
static struct mframe_t chick_frames_end_attack1[] = {
    { AI_CHARGE, -3, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, -6, 0 }, { AI_CHARGE, -4, 0 },
    { AI_CHARGE, -2, 0 },
};
static struct mmove_t chick_move_end_attack1 = { 27, 31, chick_frames_end_attack1, chick_run };
static struct mframe_t chick_frames_slash[] = {
    { AI_CHARGE, 1, 0 }, { AI_CHARGE, 7, L_SLASH }, { AI_CHARGE, -7, 0 }, { AI_CHARGE, 1, 0 },
    { AI_CHARGE, -1, 0 }, { AI_CHARGE, 1, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 1, 0 },
    { AI_CHARGE, -2, L_RESLASH },
};
static struct mmove_t chick_move_slash = { 35, 43, chick_frames_slash, 0 };
static struct mframe_t chick_frames_end_slash[] = {
    { AI_CHARGE, -6, 0 }, { AI_CHARGE, -1, 0 }, { AI_CHARGE, -6, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t chick_move_end_slash = { 44, 47, chick_frames_end_slash, chick_run };
static struct mframe_t chick_frames_start_slash[] = {
    { AI_CHARGE, 1, 0 }, { AI_CHARGE, 8, 0 }, { AI_CHARGE, 3, 0 },
};
static struct mmove_t chick_move_start_slash = { 32, 34, chick_frames_start_slash, chick_slash };

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 12, {"Attack1"}, 0, 0 },
    { 13, 26, {"Attack2"}, 0, 0 },
    { 27, 31, {"End Attack1"}, 0, 0 },
    { 32, 34, {"Start Slash"}, 0, 0 },
    { 35, 43, {"Slash"}, 0, 0 },
    { 44, 47, {"End Slash"}, 0, 0 },
    { 83, 89, {"Duck"}, 0, 0 },
    { 121, 150, {"Stand"}, 0, 0 },
    { 151, 180, {"Fidget"}, 0, 0 },
    { 181, 190, {"Start Walk"}, 0, 0 },
    { 191, 200, {"Walk"}, 0, 0 },
    { 90, 94, {"Pain1"}, 0, 0 },
    { 95, 99, {"Pain2"}, 0, 0 },
    { 100, 120, {"Pain3"}, 0, 0 },
    { 48, 59, {"Death1"}, 0, 0 },
    { 60, 82, {"Death2"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static int Unused0 = 0; /* in .data */

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_chick, Chick_EveryFrame, PreInit_Chick, AnimRemap };

static int sound_missile_prelaunch;
static int sound_missile_launch;
static int sound_melee_swing;
static int sound_melee_hit;
static int sound_missile_reload;
static int sound_death1;
static int sound_death2;
static int sound_fall_down;
static int sound_idle1;
static int sound_idle2;
static int sound_pain1;
static int sound_pain2;
static int sound_pain3;
static int sound_sight;
static int sound_search;
static int sound_udeath;
static struct MODSUBOFF RocketMuzzle;
static int Unused1[2];
static struct ai_local_table_t AiTable[32];
static short SlashBlurStart;
static int Unused2, Unused3;
static short SlashBlurEnd;
static short Unused4;

void PreInit_Chick(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *Anim;

    if (Hdr) {
        Iface.PrintF("Pre Init Ironmaiden\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "IronMaiden (chick)");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "IronMaiden (chick)");
        Iface.PrintF("Validated Creature Interface for %s\n", "IronMaiden (chick)");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_STAND].thinkfunc = chick_stand;
        AiTable[L_RUN].thinkfunc = chick_run;
        AiTable[L_RESLASH].thinkfunc = chick_reslash;
        AiTable[L_REROCKET].thinkfunc = chick_rerocket;
        AiTable[L_ATTACK1].thinkfunc = chick_attack1;
        AiTable[L_MOAN].thinkfunc = ChickMoan;
        AiTable[L_FIDGET].thinkfunc = chick_fidget;
        AiTable[L_DEAD].thinkfunc = chick_dead;
        AiTable[L_DUCK_DOWN].thinkfunc = chick_duck_down;
        AiTable[L_DUCK_HOLD].thinkfunc = chick_duck_hold;
        AiTable[L_DUCK_UP].thinkfunc = chick_duck_up;
        AiTable[L_PREATTACK1].thinkfunc = Chick_PreAttack1;
        AiTable[L_ROCKET].thinkfunc = ChickRocket;
        AiTable[L_RELOAD].thinkfunc = ChickReload;
        AiTable[L_SLASH_START].thinkfunc = chick_slash;
        AiTable[L_SLASH].thinkfunc = ChickSlash;
        Iface.SetMyAiTable(CLASS_CHICK, AiTable);
        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Slash"}))) {
            SlashBlurStart = Anim->AnimDetails.Start;
            SlashBlurEnd = Anim->AnimDetails.Stop;
        } else {
            SlashBlurStart = 1;
            SlashBlurEnd = 0;
        }
        sound_missile_prelaunch = Iface.GetSoundNum((struct IDENTITYNAME){"chk_atck1"});
        sound_missile_launch = Iface.GetSoundNum((struct IDENTITYNAME){"wep_rocklf1a"});
        sound_melee_swing = Iface.GetSoundNum((struct IDENTITYNAME){"ara_melee1"});
        sound_melee_hit = Iface.GetSoundNum((struct IDENTITYNAME){"chk_atck4"});
        sound_death1 = sound_death2 = Iface.GetSoundNum((struct IDENTITYNAME){"chk_deth2"});
        sound_idle1 = sound_idle2 = Iface.GetSoundNum((struct IDENTITYNAME){"chk_idle2"});
        sound_pain1 = sound_pain2 = sound_pain3 = Iface.GetSoundNum((struct IDENTITYNAME){"chk_pain3"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"chk_atck4"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"chk_sght1"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 233, 240, &RocketMuzzle))
            Iface.PrintF("Ironmaiden Rocket muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Ironmaiden\r\n");
}

void ChickRocket(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT dir;
    struct VECT vec;
    struct SVECT Off;

    Cre = CRE(self);
    Off = Iface.GetSubModXYZ(Cre->ModelHdr, RocketMuzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Off, &Off);
    start.vx = Cre->Draw.Location.vx + Off.vx;
    start.vy = Cre->Draw.Location.vy + Off.vy;
    start.vz = Cre->Draw.Location.vz + Off.vz;
    vec.vx = self->enemy->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz;
    vec.vy += self->enemy->viewheight;
    dir.vx = vec.vx - start.vx;
    dir.vy = vec.vy - start.vy;
    dir.vz = vec.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_rocket(self, &start, &dir, 50, 500, 0);
}

void chick_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_stand;
}

void chick_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1) {
        self->monsterinfo.currentmove = &chick_move_stand;
        return;
    }
    if (self->monsterinfo.currentmove == &chick_move_walk || self->monsterinfo.currentmove == &chick_move_start_run)
        self->monsterinfo.currentmove = &chick_move_run;
    else
        self->monsterinfo.currentmove = &chick_move_start_run;
}

void chick_reslash(struct edict_s *self)
{
    if (CRE(self->enemy)->Health > 0) {
        if (Iface.range(self, self->enemy) == 0) {
            if (Iface.rand() < 29491) {
                self->monsterinfo.currentmove = &chick_move_slash;
                return;
            } else {
                self->monsterinfo.currentmove = &chick_move_end_slash;
                return;
            }
        }
    }
    self->monsterinfo.currentmove = &chick_move_end_slash;
}

void chick_rerocket(struct edict_s *self)
{
    if (CRE(self->enemy)->Health > 0) {
        if (Iface.range(self, self->enemy) > 0)
            if (Iface.visible(self, self->enemy))
                if (Iface.rand() < 19961) {
                    self->monsterinfo.currentmove = &chick_move_attack1;
                    return;
                }
    }
    self->monsterinfo.currentmove = &chick_move_end_attack1;
}

void chick_attack1(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_attack1;
}

void ChickMoan(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle2, CRE(self));
}

void chick_fidget(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        return;
    if (Iface.rand() < 9831)
        self->monsterinfo.currentmove = &chick_move_fidget;
}

void chick_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->svflags |= 2;
    self->nextthink = 0;
}

void chick_duck_down(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 0x800)
        return;
    self->monsterinfo.aiflags |= 0x800;
    self->takedamage = 1;
    self->monsterinfo.pausetime = Iface.level->time + 10;
}

void chick_duck_hold(struct edict_s *self)
{
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~0x80;
    else
        self->monsterinfo.aiflags |= 0x80;
}

void chick_duck_up(struct edict_s *self)
{
    self->monsterinfo.aiflags &= ~0x800;
    self->takedamage = 2;
}

void Chick_PreAttack1(struct edict_s *self)
{
    Iface.SoundByCreature(sound_missile_prelaunch, CRE(self));
}

void ChickReload(struct edict_s *self)
{
}

void chick_slash(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_slash;
}

void ChickSlash(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = -120;
    aim.vz = -Iface.getminsvx(self);
    Iface.fire_hit(self, aim, 10 + Iface.rand() % 6, 100);
    Iface.SoundByCreature(sound_melee_swing, CRE(self));
}

void SP_monster_chick(struct edict_s *self)
{
    self->mass = 200;
    self->pain = chick_pain;
    self->die = chick_die;
    self->monsterinfo.stand = chick_stand;
    self->monsterinfo.walk = chick_walk;
    self->monsterinfo.run = chick_run;
    self->monsterinfo.dodge = chick_dodge;
    self->monsterinfo.attack = chick_attack;
    self->monsterinfo.melee = chick_melee;
    self->monsterinfo.sight = chick_sight;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &chick_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

void Chick_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->AnimFrame >= SlashBlurStart && Cre->AnimFrame <= SlashBlurEnd)
        Iface.AddBlurPoints(Cre, (short[]){ 337, 351, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
}

void chick_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_walk;
}

void chick_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain1, CRE(self));
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 10)
        self->monsterinfo.currentmove = &chick_move_pain1;
    else if (damage <= 25)
        self->monsterinfo.currentmove = &chick_move_pain2;
    else
        self->monsterinfo.currentmove = &chick_move_pain3;
}

void chick_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    int n;

    if (CRE(self)->Health <= self->gib_health) {
        Iface.SoundByCreature(sound_udeath, CRE(self));
        self->deadflag = 2;
        chick_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    self->deadflag = 2;
    self->takedamage = 1;
    n = Iface.rand() % 2;
    if (n == 0)
        self->monsterinfo.currentmove = &chick_move_death1;
    else
        self->monsterinfo.currentmove = &chick_move_death2;
    Iface.SoundByCreature(sound_death1, CRE(self));
}

void chick_dodge(struct edict_s *self, struct edict_s *attacker, long eta)
{
    if (Iface.rand() >= 8192)
        return;
    if (!self->enemy)
        self->enemy = attacker;
    self->monsterinfo.currentmove = &chick_move_duck;
}

void chick_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_start_slash;
}

void chick_attack(struct edict_s *self)
{
    self->monsterinfo.currentmove = &chick_move_start_attack1;
}

void chick_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, CRE(self));
}
