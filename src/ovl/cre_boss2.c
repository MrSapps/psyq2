/* Boss2, the Hornet (Quake II m_boss2.c) */
#include "ovl.h"
#include "bss.h"

#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: AiTable */
#define BOSS2_RUN 2
#define BOSS2_STAND 3
#define BOSS2_DEAD 4
#define BOSS2_ATTACK 5
#define BOSS2_ATTACK_MG 6
#define BOSS2_REATTACK_MG 7
#define BOSS2_MACHINEGUN 9
#define BOSS2_ROCKET 10
#define BOSS2_APPEAR 11

#define CLASS_BOSS2 71

void PreInit_Boss2(struct GLOBMODHDR *Hdr);
static void Boss2Rocket(struct edict_s *self);
static void boss2_firebullet_right(struct edict_s *self);
static void boss2_firebullet_left(struct edict_s *self);
static int Boss2_CheckAttack(struct edict_s *self);

/* the small ones, which gcc emits at the end in this order */
static void boss2_run(struct edict_s *self);
static void boss2_stand(struct edict_s *self);
static void boss2_dead(struct edict_s *self);
static void boss2_attack(struct edict_s *self);
static void boss2_attack_mg(struct edict_s *self);
static void boss2_reattack_mg(struct edict_s *self);
void boss2_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point);
static void Boss2MachineGun(struct edict_s *self);
static void boss2_appear(struct edict_s *self);
void SP_monster_boss2(struct edict_s *self);
static void boss2_search(struct edict_s *self);
static void boss2_walk(struct edict_s *self);
static void boss2_pain(struct edict_s *self, struct edict_s *other, int kick, int damage);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_boss2, 0, PreInit_Boss2, AnimRemap };

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 20, {"Stand"}, 0, 0 },
    { 21, 0, {"Stand2"}, 0, 0 },
    { 50, 69, {"Walk"}, 0, 0 },
    { 70, 78, {"Preattack"}, 0, 0 },
    { 79, 84, {"Attack"}, 0, 0 },
    { 85, 88, {"Postattack"}, 0, 0 },
    { 89, 109, {"Rocketattack"}, 0, 0 },
    { 110, 127, {"Hpain"}, 0, 0 },
    { 128, 131, {"Lpain"}, 0, 0 },
    { 132, 179, {"Death"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };

/* defined into .bss in this order (as .lcomm/.comm, ASPSX would 16-align the
   big ones and the link would put the commons in order of first use) */
static int UnusedBss0 BSS;
static int sound_pain1 BSS;
static int sound_pain2 BSS;
static int sound_pain3 BSS;
static int sound_death BSS;
static int sound_search1 BSS; /* unused */
static int sound_engine BSS;
static int sound_udeath BSS;
static int Unused0 BSS;
static struct MODSUBOFF LeftChainGunMuzzle BSS;
static struct MODSUBOFF RightChainGunMuzzle BSS;
static struct MODSUBOFF RocketMuzzle[4] BSS;
static int Unused1[4] BSS;
static struct ai_local_table_t AiTable[32] BSS;

void PreInit_Boss2(struct GLOBMODHDR *Hdr)
{
    struct MODSUBOFF First;
    struct MODSUBOFF Second;
    struct SVECT Delta;
    int i;

    if (Hdr) {
        Iface.PrintF("Pre Init Boss2\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Boss2");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Boss2");
        Iface.PrintF("Validated Creature Interface for %s\n", "Boss2");
        AiTable[0].thinkfunc = 0;
        AiTable[BOSS2_RUN].thinkfunc = boss2_run;
        AiTable[BOSS2_STAND].thinkfunc = boss2_stand;
        AiTable[BOSS2_DEAD].thinkfunc = boss2_dead;
        AiTable[BOSS2_ATTACK].thinkfunc = boss2_attack;
        AiTable[BOSS2_ATTACK_MG].thinkfunc = boss2_attack_mg;
        AiTable[BOSS2_REATTACK_MG].thinkfunc = boss2_reattack_mg;
        AiTable[BOSS2_MACHINEGUN].thinkfunc = Boss2MachineGun;
        AiTable[BOSS2_ROCKET].thinkfunc = Boss2Rocket;
        AiTable[BOSS2_APPEAR].thinkfunc = boss2_appear;
        Iface.SetMyAiTable(CLASS_BOSS2, AiTable);
        Hdr->ShadowVerts = ShadowVerts;
        sound_pain1 = Iface.GetSoundNum((struct IDENTITYNAME){"bhv_pain1"});
        sound_pain2 = Iface.GetSoundNum((struct IDENTITYNAME){"bhv_pain2"});
        sound_pain3 = Iface.GetSoundNum((struct IDENTITYNAME){"bhv_pain3"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"bhv_deth2"});
        sound_engine = Iface.GetSoundNum((struct IDENTITYNAME){"bhv_engn2"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!sound_pain2)
            sound_pain2 = sound_pain1;
        if (!sound_pain3)
            sound_pain3 = sound_pain1;
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 440, 444, &LeftChainGunMuzzle))
            Iface.PrintF("Boss2 left ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 339, 344, &RightChainGunMuzzle))
            Iface.PrintF("Boss2 right ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 237, 239, &First) || !Iface.GetModSubOffFrom2Verteces(Hdr, 238, 240, &Second))
            Iface.PrintF("Boss2 Rocket muzzle points are from different submodels\r\n");
        Delta.vx = (Second.Offset.vx - First.Offset.vx) * 8;
        Delta.vy = (Second.Offset.vy - First.Offset.vy) * 8;
        Delta.vz = (Second.Offset.vz - First.Offset.vz) * 8;
        for (i = 0; i < 4; i++) {
            RocketMuzzle[i].Offset.vx = First.Offset.vx + (Delta.vx / 2 + i * Delta.vx) / 32;
            RocketMuzzle[i].Offset.vy = First.Offset.vy + (Delta.vy / 2 + i * Delta.vy) / 32;
            RocketMuzzle[i].Offset.vz = First.Offset.vz + (Delta.vz / 2 + i * Delta.vz) / 32;
            RocketMuzzle[i].SubModel = First.SubModel;
        }
    } else
        Iface.PrintF("Clear Boss2\n");
}

/* The original's .rdata alignment padding holds a stray byte. */
__asm__(".rdata\n\t.byte 0x0b\n\t.text");

static struct mframe_t boss2_frames_stand[] = {
    {AI_NONE, 0, 0}, {AI_NONE, 0, BOSS2_APPEAR}, {AI_NONE, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
};
static struct mmove_t boss2_move_stand = { 0, 20, boss2_frames_stand, 0 };

static struct mframe_t boss2_frames_fidget[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
};
/* the original's lastframe is 0 */
static struct mmove_t boss2_move_fidget = { 21, 0, boss2_frames_fidget, 0 };

static struct mframe_t boss2_frames_walk[] = {
    {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0},
    {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0},
    {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0}, {AI_WALK, 8, 0},
    {AI_WALK, 8, 0}, {AI_WALK, 8, 0},
};
static struct mmove_t boss2_move_walk = { 50, 69, boss2_frames_walk, 0 };

static struct mframe_t boss2_frames_run[] = {
    {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0},
    {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0},
    {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0}, {AI_RUN, 8, 0},
    {AI_RUN, 8, 0}, {AI_RUN, 8, 0},
};
static struct mmove_t boss2_move_run = { 50, 69, boss2_frames_run, 0 };

static struct mframe_t boss2_frames_attackpre_mg[] = {
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, BOSS2_ATTACK_MG},
};
static struct mmove_t boss2_move_attackpre_mg = { 70, 78, boss2_frames_attackpre_mg, 0 };

static struct mframe_t boss2_frames_attack_mg[] = {
    {AI_CHARGE, 1, BOSS2_MACHINEGUN}, {AI_CHARGE, 1, BOSS2_MACHINEGUN}, {AI_CHARGE, 1, BOSS2_MACHINEGUN}, {AI_CHARGE, 1, BOSS2_MACHINEGUN}, {AI_CHARGE, 1, BOSS2_MACHINEGUN}, {AI_CHARGE, 1, BOSS2_REATTACK_MG},
};
static struct mmove_t boss2_move_attack_mg = { 79, 84, boss2_frames_attack_mg, 0 };

static struct mframe_t boss2_frames_attackpost_mg[] = {
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
};
static struct mmove_t boss2_move_attackpost_mg = { 85, 88, boss2_frames_attackpost_mg, boss2_run };

static struct mframe_t boss2_frames_attack_rocket[] = {
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
    {AI_MOVE, -20, BOSS2_ROCKET}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
    {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 1, 0},
};
static struct mmove_t boss2_move_attack_rocket = { 89, 109, boss2_frames_attack_rocket, boss2_run };

static struct mframe_t boss2_frames_pain_heavy[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t boss2_move_pain_heavy = { 110, 127, boss2_frames_pain_heavy, boss2_run };

static struct mframe_t boss2_frames_pain_light[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t boss2_move_pain_light = { 128, 131, boss2_frames_pain_light, boss2_run };

static struct mframe_t boss2_frames_death[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t boss2_move_death = { 132, 179, boss2_frames_death, boss2_dead };

static void Boss2Rocket(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct VECT vec;
    struct SVECT Rel;
    int i;

    for (i = 0; i < 4; i++) {
        Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RocketMuzzle[i], Cre->AnimFrame,
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
        Iface.monster_fire_rocket(self, &start, &dir, 50, 500, 0);
    }
}

static void boss2_firebullet_right(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT target;
    struct VECT forward;
    struct VECT start;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RightChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    Iface.VectorMA(&self->enemy->sorigin, -2, &self->enemy->velocity, &target);
    target.vy += self->enemy->viewheight;
    forward.vx = target.vx - start.vx;
    forward.vy = target.vy - start.vy;
    forward.vz = target.vz - start.vz;
    Iface.VectorNormalize(&forward);
    Iface.monster_fire_bullet(self, &start, &forward, 6, 4, 300, 500, 133);
}

static void boss2_firebullet_left(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT forward;
    struct VECT target;
    struct VECT start;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, LeftChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    Iface.VectorMA(&self->enemy->sorigin, -2, &self->enemy->velocity, &target);
    target.vy += self->enemy->viewheight;
    forward.vx = target.vx - start.vx;
    forward.vy = target.vy - start.vy;
    forward.vz = target.vz - start.vz;
    Iface.VectorNormalize(&forward);
    Iface.monster_fire_bullet(self, &start, &forward, 6, 4, 300, 500, 73);
}

static int Boss2_CheckAttack(struct edict_s *self)
{
    struct VECT spot1;
    struct VECT spot2;
    struct VECT temp;
    int chance;
    int enemy_range;

    if (CRE(self->enemy)->Health > 0) {
        spot1.vx = self->sorigin.vx;
        spot1.vy = self->sorigin.vy;
        spot1.vz = self->sorigin.vz;
        spot1.vy += self->viewheight;
        spot2.vx = self->enemy->sorigin.vx;
        spot2.vy = self->enemy->sorigin.vy;
        spot2.vz = self->enemy->sorigin.vz;
        spot2.vy += self->enemy->viewheight;
        Iface.gitrace(&spot1, 0, 0, &spot2, self, 0x2000019, CRE(self)->Box.MoveSpace);
        if (Iface.Trace->ent != self->enemy)
            return 0;
    }
    Iface.infront(self, self->enemy);
    enemy_range = Iface.range(self, self->enemy);
    temp.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    temp.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    temp.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    self->ideal_yaw = Iface.vectoyaw(&temp);
    if (enemy_range == 0) {
        if (self->monsterinfo.melee)
            self->monsterinfo.attack_state = 3;
        else
            self->monsterinfo.attack_state = 4;
        return 1;
    }
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
        chance = 26213;
    else if (enemy_range == 2)
        chance = 26213;
    else
        return 0;
    if (Iface.rand() < chance) {
        self->monsterinfo.attack_state = 4;
        self->monsterinfo.attack_finished = Iface.level->time + Iface.rand() * 20 / 32767;
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

static void boss2_run(struct edict_s *self)
{
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.currentmove = &boss2_move_stand;
    else
        self->monsterinfo.currentmove = &boss2_move_run;
}

static void boss2_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &boss2_move_stand;
}

static void boss2_dead(struct edict_s *self)
{
    CRE(self)->Health = self->gib_health;
    self->nextthink = 0;
    self->takedamage = 0;
    self->movetype = 7;
    self->svflags |= 2;
}

static void boss2_attack(struct edict_s *self)
{
    struct VECT vec;

    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    if (Iface.VectorLength2(&vec) <= 2250000)
        self->monsterinfo.currentmove = &boss2_move_attackpre_mg;
    else if (Iface.rand() < 19961)
        self->monsterinfo.currentmove = &boss2_move_attackpre_mg;
    else
        self->monsterinfo.currentmove = &boss2_move_attack_rocket;
}

static void boss2_attack_mg(struct edict_s *self)
{
    self->monsterinfo.currentmove = &boss2_move_attack_mg;
}

static void boss2_reattack_mg(struct edict_s *self)
{
    if (Iface.infront(self, self->enemy)) {
        if (Iface.rand() < 22937)
            self->monsterinfo.currentmove = &boss2_move_attack_mg;
        else
            self->monsterinfo.currentmove = &boss2_move_attackpost_mg;
    } else
        self->monsterinfo.currentmove = &boss2_move_attackpost_mg;
}

void boss2_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point)
{
    self->monsterinfo.currentmove = &boss2_move_death;
    self->deadflag = 2;
    self->takedamage = 0;
    self->count = 0;
    boss2_dead(self);
}

static void Boss2MachineGun(struct edict_s *self)
{
    boss2_firebullet_left(self);
    boss2_firebullet_right(self);
}

static void boss2_appear(struct edict_s *self)
{
    struct VECT forward;
    struct VECT right;
    struct VECT up;
    struct VECT start;
    struct VECT vec;
    struct edict_s *client;

    if (CRE(self)->Health == 2000) {
        self->sorigin.vy -= 1536;
        Iface.linkentity(self, 0x81);
        CRE(self)->Health--;
        client = Iface.level->sight_client;
        if (client) {
            vec.vx = client->sorigin.vx - self->sorigin.vx;
            vec.vy = client->sorigin.vy - self->sorigin.vy;
            vec.vz = client->sorigin.vz - self->sorigin.vz;
            self->sangles.vz = Iface.vectoyaw(&vec);
            Iface.linkentity(self, 4);
            Iface.linkentity(self, 0x84);
        }
    }
}

void SP_monster_boss2(struct edict_s *self)
{
    self->mass = 1000;
    self->pain = boss2_pain;
    self->die = boss2_die;
    self->monsterinfo.stand = boss2_stand;
    self->monsterinfo.walk = boss2_walk;
    self->monsterinfo.run = boss2_run;
    self->monsterinfo.attack = boss2_attack;
    self->monsterinfo.search = boss2_search;
    self->movetype = 5;
    self->solid = 2;
    self->monsterinfo.checkattack = Boss2_CheckAttack;
    self->flags |= 4;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    self->monsterinfo.currentmove = &boss2_move_stand;
    self->monsterinfo.scale = 10;
    Iface.flymonster_start(self);
}

static void boss2_search(struct edict_s *self)
{
}

static void boss2_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &boss2_move_walk;
}

static void boss2_pain(struct edict_s *self, struct edict_s *other, int kick, int damage)
{
    if (CRE(self)->Health < self->max_health / 2)
        self->s.skinnum = 1;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    if (damage < 10)
        self->monsterinfo.currentmove = &boss2_move_pain_light;
    else if (damage < 30)
        self->monsterinfo.currentmove = &boss2_move_pain_light;
    else
        self->monsterinfo.currentmove = &boss2_move_pain_heavy;
}
