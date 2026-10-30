/* Infantry (monster_infantry, m_infantry.c) */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: LocalAiTable, filled in by PreInit_Infantry */
enum {
    L_NONE,
    L_MACHINEGUN,
    L_STAND,
    L_RUN,
    L_DEAD,
    L_DUCK_DOWN,
    L_DUCK_HOLD,
    L_DUCK_UP,
    L_COCK_GUN,
    L_FIRE,
    L_SWING,
    L_SMACK,
};

void PreInit_Infantry(struct GLOBMODHDR *Hdr);
void Infantry_EveryFrame(struct CREATURE *Cre);
static void InfantryMachineGun(struct edict_s *self);
static void infantry_stand(struct edict_s *self);
static void infantry_run(struct edict_s *self);
static void infantry_dead(struct edict_s *self);
static void infantry_duck_down(struct edict_s *self);
static void infantry_duck_hold(struct edict_s *self);
static void infantry_duck_up(struct edict_s *self);
static void infantry_cock_gun(struct edict_s *self);
static void infantry_fire(struct edict_s *self);
static void infantry_swing(struct edict_s *self);
static void infantry_smack(struct edict_s *self);
void SP_monster_infantry(struct edict_s *self);
static void infantry_fidget(struct edict_s *self);
static void infantry_walk(struct edict_s *self);
static void infantry_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void infantry_sight(struct edict_s *self);
static void infantry_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point);
static void infantry_dodge(struct edict_s *self, struct edict_s *attacker);
static void infantry_attack(struct edict_s *self);

static struct mframe_t infantry_frames_stand[] = {
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
};
static struct mmove_t infantry_move_stand = { 50, 71, infantry_frames_stand, 0 };
static struct mframe_t infantry_frames_fidget[] = {
    { AI_STAND, 1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 1, 0 }, { AI_STAND, 3, 0 }, { AI_STAND, 6, 0 },
    { AI_STAND, 3, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, -1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, -2, 0 }, { AI_STAND, 1, 0 }, { AI_STAND, 1, 0 },
    { AI_STAND, 1, 0 }, { AI_STAND, -1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, -1, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, -1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, 1, 0 }, { AI_STAND, 0, 0 },
    { AI_STAND, 0, 0 }, { AI_STAND, -1, 0 }, { AI_STAND, -1, 0 }, { AI_STAND, 0, 0 }, { AI_STAND, -3, 0 },
    { AI_STAND, -2, 0 }, { AI_STAND, -3, 0 }, { AI_STAND, -3, 0 }, { AI_STAND, -2, 0 },
};
static struct mmove_t infantry_move_fidget = { 1, 49, infantry_frames_fidget, infantry_stand };
static struct mframe_t infantry_frames_walk[] = {
    { AI_WALK, 5, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 5, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 5, 0 }, { AI_WALK, 6, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 }, { AI_WALK, 4, 0 },
    { AI_WALK, 4, 0 }, { AI_WALK, 5, 0 },
};
static struct mmove_t infantry_move_walk = { 74, 85, infantry_frames_walk, 0 };
static struct mframe_t infantry_frames_run[] = {
    { AI_RUN, 10, 0 }, { AI_RUN, 20, 0 }, { AI_RUN, 5, 0 }, { AI_RUN, 7, 0 }, { AI_RUN, 30, 0 },
    { AI_RUN, 35, 0 }, { AI_RUN, 2, 0 }, { AI_RUN, 6, 0 },
};
static struct mmove_t infantry_move_run = { 92, 99, infantry_frames_run, 0 };
static struct mframe_t infantry_frames_pain1[] = {
    { AI_MOVE, -3, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, -1, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 6, 0 }, { AI_MOVE, 2, 0 },
};
static struct mmove_t infantry_move_pain1 = { 100, 109, infantry_frames_pain1, infantry_run };
static struct mframe_t infantry_frames_pain2[] = {
    { AI_MOVE, -3, 0 }, { AI_MOVE, -3, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -2, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, 2, 0 },
};
static struct mmove_t infantry_move_pain2 = { 110, 119, infantry_frames_pain2, infantry_run };
static struct mframe_t infantry_frames_death1[] = {
    { AI_MOVE, -4, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, -4, 0 },
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, 3, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, -2, 0 }, { AI_MOVE, 2, 0 }, { AI_MOVE, 2, 0 },
    { AI_MOVE, 9, 0 }, { AI_MOVE, 9, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, -3, 0 }, { AI_MOVE, -3, 0 },
};
static struct mmove_t infantry_move_death1 = { 125, 144, infantry_frames_death1, infantry_dead };
static struct mframe_t infantry_frames_death2[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 5, 0 }, { AI_MOVE, -1, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, 1, 0 }, { AI_MOVE, 1, 0 }, { AI_MOVE, 4, 0 }, { AI_MOVE, 3, 0 }, { AI_MOVE, 0, 0 },
    { AI_MOVE, -2, L_MACHINEGUN }, { AI_MOVE, -2, L_MACHINEGUN }, { AI_MOVE, -3, L_MACHINEGUN },
    { AI_MOVE, -1, L_MACHINEGUN }, { AI_MOVE, -2, L_MACHINEGUN }, { AI_MOVE, 0, L_MACHINEGUN },
    { AI_MOVE, 2, L_MACHINEGUN }, { AI_MOVE, 2, L_MACHINEGUN }, { AI_MOVE, 3, L_MACHINEGUN },
    { AI_MOVE, -10, L_MACHINEGUN }, { AI_MOVE, -7, L_MACHINEGUN }, { AI_MOVE, -8, L_MACHINEGUN },
    { AI_MOVE, -6, 0 }, { AI_MOVE, 4, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t infantry_move_death2 = { 145, 169, infantry_frames_death2, infantry_dead };
static struct mframe_t infantry_frames_death3[] = {
    { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, -6, 0 }, { AI_MOVE, -11, 0 },
    { AI_MOVE, -3, 0 }, { AI_MOVE, -11, 0 }, { AI_MOVE, 0, 0 }, { AI_MOVE, 0, 0 },
};
static struct mmove_t infantry_move_death3 = { 170, 178, infantry_frames_death3, infantry_dead };
static struct mframe_t infantry_frames_duck[] = {
    { AI_MOVE, -2, L_DUCK_DOWN }, { AI_MOVE, -5, L_DUCK_HOLD }, { AI_MOVE, 3, 0 }, { AI_MOVE, 4, L_DUCK_UP },
    { AI_MOVE, 0, 0 },
};
static struct mmove_t infantry_move_duck = { 120, 124, infantry_frames_duck, infantry_run };
static struct mframe_t infantry_frames_attack1[] = {
    { AI_CHARGE, 4, 0 }, { AI_CHARGE, -1, 0 }, { AI_CHARGE, -1, 0 }, { AI_CHARGE, 0, L_COCK_GUN },
    { AI_CHARGE, -1, 0 }, { AI_CHARGE, 1, 0 }, { AI_CHARGE, 1, 0 }, { AI_CHARGE, 2, 0 }, { AI_CHARGE, -2, 0 },
    { AI_CHARGE, -3, 0 }, { AI_CHARGE, 1, L_FIRE }, { AI_CHARGE, 5, 0 }, { AI_CHARGE, -1, 0 },
    { AI_CHARGE, -2, 0 }, { AI_CHARGE, -3, 0 },
};
static struct mmove_t infantry_move_attack1 = { 184, 198, infantry_frames_attack1, infantry_run };
static struct mframe_t infantry_frames_attack2[] = {
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_SWING }, { AI_CHARGE, 0, 0 },
    { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, L_SMACK }, { AI_CHARGE, 0, 0 }, { AI_CHARGE, 0, 0 },
};
static struct mmove_t infantry_move_attack2 = { 199, 206, infantry_frames_attack2, infantry_run };

static int sound_pain1 = -1;
static int sound_pain2 = -1;
static int sound_die1 = -1;
static int sound_die2 = -1;
static int sound_gunshot = -1;
static int sound_weapon_cock = -1;
static int sound_punch_swing = -1;
static int sound_punch_hit = -1;
static int sound_sight = -1;
static int sound_search = -1;
static int sound_idle = -1;

static struct NEWANIMREMAP AnimRemap[] = {
    { 74, 85, {"Walk"}, 0, 0 },
    { 92, 99, {"Run"}, 0, 0 },
    { 184, 198, {"Attack1"}, 0, 0 },
    { 199, 206, {"Attack2"}, 0, 0 },
    { 100, 109, {"Pain1"}, 0, 0 },
    { 110, 119, {"Pain2"}, 0, 0 },
    { 145, 169, {"Death2"}, 0, 0 },
    { 170, 178, {"Death3"}, 0, 0 },
    { 1, 49, {"Fidget"}, 0, 0 },
    { 125, 144, {"Death1"}, 0, 0 },
    { 120, 124, {"Duck"}, 0, 0 },
    { 50, 71, {"Stand1"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { 0x71, 0xac, 0xed, 0xcd, 0x3a, -1 };

/* the death2 frames' machine gun sweep (pitch, 0, yaw) */
static struct VECT aimangles[] = {
    { 0, 0, 0x39 },
    { 0x72, 0, 0xab },
    { 0xe4, 0, 0x11c },
    { 0x11c, 0, 0x18e },
    { 0x155, 0, 0x1c7 },
    { 0x155, 0, 0x200 },
    { 0x11c, 0, 0x239 },
    { 0xe4, 0, 0x1c7 },
    { 0xab, 0, 0x18e },
    { 0x1c7, 0, 0x18e },
    { 0x31c, 0, 0x18e },
    { 0x400, 0, 0x18e },
};

/* the original's .bss starts 4 bytes further on, 8-aligned (not a rule of its linker: most
 * modules' .bss is only word-aligned); this .data word fills the gap */
static int BssAlign = 0;

/* the Unused* are space the original has that no code uses */
static int Unused1[2];

static struct MODSUBOFF MuzzleOff;
static short AttackFirst;
static int Unused2[2];
static struct ai_local_table_t LocalAiTable[32];
static short AttackLast;
static short Unused3;

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_infantry, Infantry_EveryFrame, PreInit_Infantry, AnimRemap };

void PreInit_Infantry(struct GLOBMODHDR *Hdr)
{
    struct ANIMHDR *Anim;

    if (Hdr) {
        Iface.PrintF("Pre Init Infantry\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Infantry");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Infantry");
        Iface.PrintF("Validated Creature Interface for %s\n", "Infantry");
        Hdr->ShadowVerts = ShadowVerts;
        LocalAiTable[L_NONE].thinkfunc = 0;
        LocalAiTable[L_MACHINEGUN].thinkfunc = InfantryMachineGun;
        LocalAiTable[L_STAND].thinkfunc = infantry_stand;
        LocalAiTable[L_RUN].thinkfunc = infantry_run;
        LocalAiTable[L_DEAD].thinkfunc = infantry_dead;
        LocalAiTable[L_DUCK_DOWN].thinkfunc = infantry_duck_down;
        LocalAiTable[L_DUCK_HOLD].thinkfunc = infantry_duck_hold;
        LocalAiTable[L_DUCK_UP].thinkfunc = infantry_duck_up;
        LocalAiTable[L_COCK_GUN].thinkfunc = infantry_cock_gun;
        LocalAiTable[L_FIRE].thinkfunc = infantry_fire;
        LocalAiTable[L_SWING].thinkfunc = infantry_swing;
        LocalAiTable[L_SMACK].thinkfunc = infantry_smack;
        Iface.SetMyAiTable(81, LocalAiTable);
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"inf_pain1"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"inf_pain2"});
        sound_die1 = Iface.GetSoundNum((struct IDENTITYNAME){"inf_deth1"});
        sound_die2 = Iface.GetSoundNum((struct IDENTITYNAME){"inf_deth2"});
        sound_gunshot = Iface.GetSoundNum((struct IDENTITYNAME){"wep_machgf1b"});
        sound_weapon_cock = Iface.GetSoundNum((struct IDENTITYNAME){"wep_sshotr1b"});
        if (!sound_weapon_cock)
            sound_weapon_cock = Iface.GetSoundNum((struct IDENTITYNAME){"wep_shotgr1b"});
        sound_punch_swing = Iface.GetSoundNum((struct IDENTITYNAME){"inf_atck2"});
        sound_punch_hit = Iface.GetSoundNum((struct IDENTITYNAME){"inf_atck3"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"inf_sght1"});
        sound_search = Iface.GetSoundNum((struct IDENTITYNAME){"inf_srch1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"inf_idle1"});
        if (!sound_pain1 || !sound_pain2)
            sound_pain2 = sound_pain1 = sound_pain1 | sound_pain2;
        if (!sound_die1 || !sound_die2)
            sound_die2 = sound_die1 = sound_die1 | sound_die2;
        Anim = Iface.FindAnimHdr(Hdr, (struct IDENTITYNAME){"Attack2"});
        if (Anim) {
            AttackFirst = Anim->AnimDetails.Start;
            AttackLast = Anim->AnimDetails.Stop;
        } else {
            AttackFirst = 1;
            AttackLast = 0;
        }
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 200, 205, &MuzzleOff))
            Iface.PrintF("Infantry ChainGun muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Infantry\n");
}

static const short BlurVerts[] = { 200, 201, -1 };

void Infantry_EveryFrame(struct CREATURE *Cre)
{
    if (Cre->AnimFrame >= AttackFirst && Cre->AnimFrame <= AttackLast)
        Iface.AddBlurPoints(Cre, BlurVerts, *&(struct COLOUR){32, 32, 32}, 6);
    if (Cre->Control < 0)
        Iface.MuzzleFlash(Cre, MuzzleOff, 120);
}

static void InfantryMachineGun(struct edict_s *self)
{
    struct CREATURE *Cre;
    struct VECT start;
    struct VECT target;
    struct VECT forward;
    struct VECT right;
    struct SVECT vec;
    int n;

    Cre = self->Extra.Creature;
    vec = Iface.GetSubModXYZ(Cre->ModelHdr, MuzzleOff, Cre->AnimFrame, Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &vec, &vec);
    start.vx = Cre->Draw.Location.vx + vec.vx;
    start.vy = Cre->Draw.Location.vy + vec.vy;
    start.vz = Cre->Draw.Location.vz + vec.vz;
    if (self->s.frame == 194) {
        if (self->enemy) {
            Iface.VectorMA(&self->enemy->sorigin, -819, &self->enemy->velocity, &target);
            target.vy += self->enemy->viewheight;
            forward.vx = target.vx - start.vx;
            forward.vy = target.vy - start.vy;
            forward.vz = target.vz - start.vz;
            Iface.VectorNormalize(&forward);
        } else
            Iface.AngleVectors(&self->sangles, &forward, &right, 0);
    } else {
        n = self->s.frame - 155;
        vec.vx = self->sangles.vx - aimangles[n].vx;
        vec.vy = self->sangles.vy - aimangles[n].vy;
        vec.vz = self->sangles.vz - aimangles[n].vz;
        Iface.AngleVectors(&vec, &forward, 0, 0);
    }
    Iface.monster_fire_bullet(self, &start, &forward, 3, 4, 300, 500, 0);
}

static void infantry_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &infantry_move_stand;
}

static void infantry_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &infantry_move_stand;
    else
        self->monsterinfo.currentmove = &infantry_move_run;
}

static void infantry_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->svflags |= 2;
}

static void infantry_duck_down(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 0x800)
        return;
    self->monsterinfo.aiflags |= 0x800;
    self->takedamage = 1;
    self->monsterinfo.pausetime = Iface.level->time + 10;
}

static void infantry_duck_hold(struct edict_s *self)
{
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~0x80;
    else
        self->monsterinfo.aiflags |= 0x80;
}

static void infantry_duck_up(struct edict_s *self)
{
    self->monsterinfo.aiflags &= ~0x800;
    self->takedamage = 2;
}

static void infantry_cock_gun(struct edict_s *self)
{
    int n;

    Iface.SoundByCreature(sound_weapon_cock, self->Extra.Creature);
    n = (Iface.rand() & 15) + 3 + 7;
    self->monsterinfo.pausetime = Iface.level->time + n;
}

static void infantry_fire(struct edict_s *self)
{
    InfantryMachineGun(self);
    Iface.SoundByCreature(sound_gunshot, self->Extra.Creature);
    if (Iface.level->time >= self->monsterinfo.pausetime)
        self->monsterinfo.aiflags &= ~0x80;
    else
        self->monsterinfo.aiflags |= 0x80;
}

static void infantry_swing(struct edict_s *self)
{
    Iface.SoundByCreature(sound_punch_swing, self->Extra.Creature);
}

static void infantry_smack(struct edict_s *self)
{
    struct VECT aim;

    aim.vx = 1020;
    aim.vy = 0;
    aim.vz = 0;
    Iface.fire_hit(self, aim, 5 + Iface.rand() % 5, 50);
}

void SP_monster_infantry(struct edict_s *self)
{
    self->mass = 200;
    self->pain = infantry_pain;
    self->die = infantry_die;
    self->monsterinfo.stand = infantry_stand;
    self->monsterinfo.walk = infantry_walk;
    self->monsterinfo.run = infantry_run;
    self->monsterinfo.dodge = infantry_dodge;
    self->monsterinfo.attack = infantry_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = infantry_sight;
    self->monsterinfo.idle = infantry_fidget;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 133);
    self->monsterinfo.currentmove = &infantry_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void infantry_fidget(struct edict_s *self)
{
    self->monsterinfo.currentmove = &infantry_move_fidget;
}

static void infantry_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &infantry_move_walk;
}

static void infantry_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    int n;

    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if ((*Iface.skill)->value == 3)
        return;
    n = Iface.rand() % 2;
    if (n == 0) {
        self->monsterinfo.currentmove = &infantry_move_pain1;
        Iface.SoundByCreature(sound_pain1, self->Extra.Creature);
    } else {
        self->monsterinfo.currentmove = &infantry_move_pain2;
        Iface.SoundByCreature(sound_pain2, self->Extra.Creature);
    }
}

static void infantry_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void infantry_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point)
{
    int n;

    if (((struct CREATURE *)self->Extra.Creature)->Health <= self->gib_health) {
        self->deadflag = 2;
        self->movetype = 7;
        self->svflags |= 2;
        return;
    }
    if (self->deadflag == 2)
        return;
    self->deadflag = 2;
    self->takedamage = 1;
    n = Iface.rand() % 3;
    if (n == 2) {
        self->monsterinfo.currentmove = &infantry_move_death1;
        Iface.SoundByCreature(sound_die2, self->Extra.Creature);
    } else if (n == 1) {
        self->monsterinfo.currentmove = &infantry_move_death2;
        Iface.SoundByCreature(sound_die1, self->Extra.Creature);
    } else {
        self->monsterinfo.currentmove = &infantry_move_death3;
        Iface.SoundByCreature(sound_die2, self->Extra.Creature);
    }
}

static void infantry_dodge(struct edict_s *self, struct edict_s *attacker)
{
    if (Iface.rand() < 8192) {
        if (!self->enemy)
            self->enemy = attacker;
        self->monsterinfo.currentmove = &infantry_move_duck;
    }
}

static void infantry_attack(struct edict_s *self)
{
    if (Iface.range(self, self->enemy) == 0)
        self->monsterinfo.currentmove = &infantry_move_attack2;
    else
        self->monsterinfo.currentmove = &infantry_move_attack1;
}
