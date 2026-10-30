/* Flyer (m_flyer.c); cre_flyer_magdemo.c builds it with FLYER_MAGDEMO */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum {
    L_NONE, L_CHECK_MELEE, L_LOOP_MELEE, L_MELEE, L_SETSTART, L_STAND, L_NEXTMOVE, L_RUN,
    L_FIRELEFT, L_FIRERIGHT, L_POP_BLADES, L_SLASH_LEFT, L_SLASH_RIGHT
};
enum { ACTION_nothing, ACTION_attack1, ACTION_attack2, ACTION_run };

#define CLASS_FLYER 77
#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* gcc emits the small functions last, in this order */
void flyer_check_melee(struct edict_s *self);
void flyer_loop_melee(struct edict_s *self);
void flyer_melee(struct edict_s *self);
void flyer_setstart(struct edict_s *self);
void flyer_stand(struct edict_s *self);
void flyer_nextmove(struct edict_s *self);
void flyer_run(struct edict_s *self);
void flyer_fireleft(struct edict_s *self);
void flyer_fireright(struct edict_s *self);
void flyer_pop_blades(struct edict_s *self);
void flyer_slash_left(struct edict_s *self);
void flyer_slash_right(struct edict_s *self);
void SP_monster_flyer(struct edict_s *self);
void flyer_sight(struct edict_s *self);
void flyer_idle(struct edict_s *self);
void flyer_walk(struct edict_s *self);
void flyer_stop(struct edict_s *self);
void flyer_start(struct edict_s *self);
void flyer_attack(struct edict_s *self);
void flyer_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
void flyer_die(struct edict_s *self);
void PreInit_Flyer(struct GLOBMODHDR *Hdr);
void Flyer_EveryFrame(struct CREATURE *Cre);
static struct NEWANIMREMAP AnimRemap[];
static struct mmove_t flyer_move_stand, flyer_move_walk, flyer_move_run, flyer_move_start;
static struct mmove_t flyer_move_stop, flyer_move_pain3, flyer_move_pain2, flyer_move_pain1;
static struct mmove_t flyer_move_attack2, flyer_move_start_melee, flyer_move_end_melee;
static struct mmove_t flyer_move_loop_melee;

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_flyer, Flyer_EveryFrame, PreInit_Flyer, AnimRemap };

static int nextmove;
static int sound_sight;
static int sound_idle;
static int sound_pain1;
static int sound_pain2;
static int sound_slash;
static int sound_sproing;
static int sound_die;
static short Attack1BlurStart, Attack1BlurEnd;
static struct MODSUBOFF Blaster1Muzzle;
static struct MODSUBOFF Blaster2Muzzle;
static int Unused[2];
struct ai_local_table_t AiTable[34];

void PreInit_Flyer(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *Anim;

    if (Hdr) {
        Iface.PrintF("Pre Init Flyer, missing roll right and left, pain1, defense bankright, bankleft\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Flyer");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Flyer");
        Iface.PrintF("Validated Creature Interface for %s\n", "Flyer");
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"fly_sght1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"fly_idle1"});
        sound_pain2 = sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"flt_pain2"});
        sound_sproing = Iface.GetSoundNum((struct IDENTITYNAME){"fly_atck3"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"fly_deth1"});
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_CHECK_MELEE].thinkfunc = flyer_check_melee;
        AiTable[L_LOOP_MELEE].thinkfunc = flyer_loop_melee;
        AiTable[L_MELEE].thinkfunc = flyer_melee;
        AiTable[L_SETSTART].thinkfunc = flyer_setstart;
        AiTable[L_STAND].thinkfunc = flyer_stand;
        AiTable[L_NEXTMOVE].thinkfunc = flyer_nextmove;
        AiTable[L_RUN].thinkfunc = flyer_run;
        AiTable[L_FIRELEFT].thinkfunc = flyer_fireleft;
        AiTable[L_FIRERIGHT].thinkfunc = flyer_fireright;
        AiTable[L_POP_BLADES].thinkfunc = flyer_pop_blades;
        AiTable[L_SLASH_LEFT].thinkfunc = flyer_slash_left;
        AiTable[L_SLASH_RIGHT].thinkfunc = flyer_slash_right;
        sound_slash = sound_sproing;
        Iface.SetMyAiTable(CLASS_FLYER, AiTable);
        if ((Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Attack1"}))) {
            Attack1BlurStart = Anim->AnimDetails.Start;
            Attack1BlurEnd = Anim->AnimDetails.Stop;
        } else {
            Attack1BlurStart = 1;
            Attack1BlurEnd = 0;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 121, 128, &Blaster1Muzzle))
            Iface.PrintF("Flyer Blaster 1 muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 112, 115, &Blaster2Muzzle))
            Iface.PrintF("Flyer Blaster2 muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Init Flyer\n");
}

void Flyer_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->AnimFrame >= Attack1BlurStart && Cre->AnimFrame <= Attack1BlurEnd) {
        Iface.AddBlurPoints(Cre, (short[]){ 92, 87, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
        Iface.AddBlurPoints(Cre, (short[]){ 77, 74, -1 }, (struct COLOUR){ 32, 32, 32 }, 6);
    }
}

#ifdef FLYER_MAGDEMO
/* The magdemo original's .rdata alignment padding holds stray bytes. */
__asm__(".rdata\n\t.byte 0x5f, 0x6f\n\t.text");
#endif

static struct NEWANIMREMAP AnimRemap[] = {
    { 58, 63, {"Attack1Start"}, 0, 0 },
    { 64, 75, {"Attack1"}, 0, 0 },
    { 76, 78, {"Attack1End"}, 0, 0 },
    { 79, 95, {"Attack2"}, 0, 0 },
    { 143, 146, {"Pain2"}, 0, 0 },
    { 147, 150, {"Pain3"}, 0, 0 },
    { 0, 5, {"Start"}, 0, 0 },
    { 13, 57, {"Stand"}, 0, 0 },
    { 6, 12, {"Stop"}, 0, 0 },
    { 134, 142, {"Pain1"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static struct mframe_t flyer_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 },
};
static struct mmove_t flyer_move_stand = { 13, 57, flyer_frames_stand, 0 };
static struct mframe_t flyer_frames_walk[] = {
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 5, 0 },
    { AI_WALK, 5, 0 },
};
static struct mmove_t flyer_move_walk = { 13, 57, flyer_frames_walk, 0 };
static struct mframe_t flyer_frames_run[] = {
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 }, { AI_RUN, 10, 0 },
    { AI_RUN, 10, 0 },
};
static struct mmove_t flyer_move_run = { 13, 57, flyer_frames_run, 0 };
static struct mframe_t flyer_frames_start[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, L_NEXTMOVE },
};
static struct mmove_t flyer_move_start = { 0, 5, flyer_frames_start, 0 };
static struct mframe_t flyer_frames_stop[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, L_NEXTMOVE },
};
static struct mmove_t flyer_move_stop = { 6, 12, flyer_frames_stop, 0 };
static struct mframe_t flyer_frames_rollright[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_rollright = { 119, 127, flyer_frames_rollright, 0 };
static struct mframe_t flyer_frames_rollleft[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_rollleft = { 110, 118, flyer_frames_rollleft, 0 };
static struct mframe_t flyer_frames_pain3[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_pain3 = { 147, 150, flyer_frames_pain3, flyer_run };
static struct mframe_t flyer_frames_pain2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_pain2 = { 143, 146, flyer_frames_pain2, flyer_run };
static struct mframe_t flyer_frames_pain1[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_pain1 = { 134, 142, flyer_frames_pain1, flyer_run };
static struct mframe_t flyer_frames_defense[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_defense = { 128, 133, flyer_frames_defense, 0 };
static struct mframe_t flyer_frames_bankright[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_bankright = { 103, 109, flyer_frames_bankright, 0 };
static struct mframe_t flyer_frames_bankleft[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t flyer_move_bankleft = { 96, 102, flyer_frames_bankleft, 0 };
static struct mframe_t flyer_frames_attack2[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, -10, L_FIRELEFT },
    { AI_CHARGE, -10, L_FIRERIGHT }, { AI_CHARGE, -10, L_FIRELEFT }, { AI_CHARGE, -10, L_FIRERIGHT }, { AI_CHARGE, -10, L_FIRELEFT },
    { AI_CHARGE, -10, L_FIRERIGHT }, { AI_CHARGE, -10, L_FIRELEFT }, { AI_CHARGE, -10, L_FIRERIGHT }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 },
};
static struct mmove_t flyer_move_attack2 = { 79, 95, flyer_frames_attack2, flyer_run };
static struct mframe_t flyer_frames_start_melee[] = {
    { AI_CHARGE, 0, L_POP_BLADES }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t flyer_move_start_melee = { 58, 63, flyer_frames_start_melee, flyer_loop_melee };
static struct mframe_t flyer_frames_end_melee[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t flyer_move_end_melee = { 76, 78, flyer_frames_end_melee, flyer_run };
static struct mframe_t flyer_frames_loop_melee[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_SLASH_LEFT }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_SLASH_RIGHT }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t flyer_move_loop_melee = { 64, 75, flyer_frames_loop_melee, flyer_check_melee };

void flyer_fire(struct edict_s *self, int flash_number)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT end;
    struct VECT dir;
    struct SVECT Off;
    int effect;

    Cre = CRE(self);
    if (self->s.frame == 82 || self->s.frame == 85 || self->s.frame == 88)
        effect = 64;
    else
        effect = 0;
    if (flash_number == 59)
        Off = Iface.GetSubModXYZ(Cre->ModelHdr, Blaster1Muzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    else
        Off = Iface.GetSubModXYZ(Cre->ModelHdr, Blaster2Muzzle, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
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
    Iface.monster_fire_blaster(self, &start, &dir, 1, 1000, flash_number, effect);
}

void flyer_check_melee(struct edict_s *self)
{
    if (Iface.range(self, self->enemy) == 0) {
        if (Iface.rand() < 26214)
            self->monsterinfo.currentmove = &flyer_move_loop_melee;
        else
            self->monsterinfo.currentmove = &flyer_move_end_melee;
    } else
        self->monsterinfo.currentmove = &flyer_move_end_melee;
}

void flyer_loop_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_loop_melee;
}

void flyer_melee(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_start_melee;
}

void flyer_setstart(struct edict_s *self)
{
    nextmove = ACTION_run;
    self->monsterinfo.currentmove = &flyer_move_start;
}

void flyer_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_stand;
}

void flyer_nextmove(struct edict_s *self)
{
    if (nextmove == ACTION_attack1)
        self->monsterinfo.currentmove = &flyer_move_start_melee;
    else if (nextmove == ACTION_attack2)
        self->monsterinfo.currentmove = &flyer_move_attack2;
    else if (nextmove == ACTION_run)
        self->monsterinfo.currentmove = &flyer_move_run;
}

void flyer_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &flyer_move_stand;
    else
        self->monsterinfo.currentmove = &flyer_move_run;
}

void flyer_fireleft(struct edict_s *self)
{
    flyer_fire(self, 58);
}

void flyer_fireright(struct edict_s *self)
{
    flyer_fire(self, 59);
}

void flyer_pop_blades(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sproing, CRE(self));
}

void flyer_slash_left(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = 0;
    aim.vz = Iface.getminsvx(self);
    Iface.fire_hit(self, aim, 5, 0);
    Iface.SoundByCreature(sound_slash, CRE(self));
}

void flyer_slash_right(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = 0;
    aim.vz = Iface.getmaxsvx(self);
    Iface.fire_hit(self, aim, 5, 0);
    Iface.SoundByCreature(sound_slash, CRE(self));
}

void SP_monster_flyer(struct edict_s *self)
{
    self->mass = 50;
    self->pain = flyer_pain;
    self->die = flyer_die;
    self->monsterinfo.stand = flyer_stand;
    self->monsterinfo.walk = flyer_walk;
    self->monsterinfo.run = flyer_run;
    self->monsterinfo.attack = flyer_attack;
    self->monsterinfo.melee = flyer_melee;
    self->monsterinfo.sight = flyer_sight;
    self->monsterinfo.idle = flyer_idle;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &flyer_move_stand;
    self->monsterinfo.scale = 10;
    Iface.flymonster_start(self);
}

void flyer_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, CRE(self));
}

void flyer_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, CRE(self));
}

void flyer_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_walk;
}

void flyer_stop(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_stop;
}

void flyer_start(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_start;
}

void flyer_attack(struct edict_s *self)
{
    self->monsterinfo.currentmove = &flyer_move_attack2;
}

void flyer_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    int n;

    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    n = Iface.rand() % 3;
    if (n == 0) {
        Iface.SoundByCreature(sound_pain1, CRE(self));
        self->monsterinfo.currentmove = &flyer_move_pain1;
    } else if (n == 1) {
        Iface.SoundByCreature(sound_pain2, CRE(self));
        self->monsterinfo.currentmove = &flyer_move_pain2;
    } else {
        Iface.SoundByCreature(sound_pain1, CRE(self));
        self->monsterinfo.currentmove = &flyer_move_pain3;
    }
}

void flyer_die(struct edict_s *self)
{
    Iface.SoundByCreature(sound_die, CRE(self));
    self->deadflag = 2;
    self->takedamage = 1;
    self->svflags |= 2;
    self->movetype = 7;
    self->nextthink = 0;
}
