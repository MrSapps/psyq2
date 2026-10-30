/* Tank and tank commander (Quake II m_tank.c) */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: this module's AiTable */
#define TANK_REFIRE_ROCKET 1
#define TANK_DOATTACK_ROCKET 2
#define TANK_REATTACK_BLASTER 3
#define TANK_WALK 4
#define TANK_RUN 5
#define TANK_FOOTSTEP 6
#define TANK_STAND 7
#define TANK_BLASTER 8
#define TANK_STRIKE 9
#define TANK_ROCKET 10
#define TANK_WINDUP 11
#define TANK_POSTSTRIKE 12
#define TANK_MACHINEGUN 13
#define TANK_THUD 14
#define TANK_DEAD 15

#define CLASS_TANK 91
#define CLASS_TANK_COMMANDER 92

/* aiflags */
#define AI_STAND_GROUND 0x1
#define AI_BRUTAL 0x200

/* The small functions are emitted at the end in this order (see the README); the spawn
 * function is among them, so it is `inline` to be deferred despite its size. The frame
 * tables come before the interface: the original's .data relocations precede its .header ones. */
static void TankBlaster(struct edict_s *self);
static void TankRocket(struct edict_s *self);
static void TankMachineGun(struct edict_s *self);
static void tank_attack(struct edict_s *self);
static void tank_refire_rocket(struct edict_s *self);
static void tank_doattack_rocket(struct edict_s *self);
static void tank_reattack_blaster(struct edict_s *self);
static void tank_walk(struct edict_s *self);
static void tank_run(struct edict_s *self);
static void tank_footstep(struct edict_s *self);
static void tank_stand(struct edict_s *self);
static void TankStrike(struct edict_s *self);
static void tank_windup(struct edict_s *self);
static void tank_poststrike(struct edict_s *self);
static void tank_thud(struct edict_s *self);
static void tank_dead(struct edict_s *self);
void SP_monster_tank(struct edict_s *self);
static void tank_sight(struct edict_s *self);
static void tank_idle(struct edict_s *self);
static void tank_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void tank_die(struct edict_s *self);
void PreInit_Tank(struct GLOBMODHDR *Hdr);

static struct mframe_t tank_frames_stand[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
};
static struct mmove_t tank_move_stand = { 0, 29, tank_frames_stand, 0 };

static struct mframe_t tank_frames_start_walk[] = {
    {AI_WALK, 0, 0}, {AI_WALK, 6, 0}, {AI_WALK, 6, 0}, {AI_WALK, 11, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_start_walk = { 30, 33, tank_frames_start_walk, tank_walk };

static struct mframe_t tank_frames_walk[] = {
    {AI_WALK, 4, 0}, {AI_WALK, 5, 0}, {AI_WALK, 3, 0}, {AI_WALK, 2, 0}, {AI_WALK, 5, 0},
    {AI_WALK, 5, 0}, {AI_WALK, 4, 0}, {AI_WALK, 4, 0}, {AI_WALK, 3, TANK_FOOTSTEP}, {AI_WALK, 5, 0},
    {AI_WALK, 4, 0}, {AI_WALK, 5, 0}, {AI_WALK, 7, 0}, {AI_WALK, 7, 0}, {AI_WALK, 6, 0},
    {AI_WALK, 6, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_walk = { 34, 49, tank_frames_walk, 0 };

static struct mframe_t tank_frames_stop_walk[] = {
    {AI_WALK, 3, 0}, {AI_WALK, 3, 0}, {AI_WALK, 2, 0}, {AI_WALK, 2, 0}, {AI_WALK, 4, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_stop_walk = { 50, 54, tank_frames_stop_walk, tank_stand };

static struct mframe_t tank_frames_start_run[] = {
    {AI_RUN, 0, 0}, {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 11, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_start_run = { 30, 33, tank_frames_start_run, tank_run };

static struct mframe_t tank_frames_run[] = {
    {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 5, 0},
    {AI_RUN, 5, 0}, {AI_RUN, 1, TANK_FOOTSTEP}, {AI_RUN, 8, 0}, {AI_RUN, 6, 0}, {AI_RUN, 6, 0},
    {AI_RUN, 6, 0}, {AI_RUN, 6, 0}, {AI_RUN, 5, 0}, {AI_RUN, 5, 0}, {AI_RUN, 2, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_run = { 34, 49, tank_frames_run, 0 };

static struct mframe_t tank_frames_stop_run[] = {
    {AI_RUN, 3, 0}, {AI_RUN, 3, 0}, {AI_RUN, 2, 0}, {AI_RUN, 2, 0}, {AI_RUN, 4, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_stop_run = { 50, 54, tank_frames_stop_run, tank_walk };

static struct mframe_t tank_frames_pain1[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t tank_move_pain1 = { 197, 200, tank_frames_pain1, tank_run };

static struct mframe_t tank_frames_pain2[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t tank_move_pain2 = { 201, 205, tank_frames_pain2, tank_run };

static struct mframe_t tank_frames_pain3[] = {
    {AI_MOVE, -7, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 3, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_pain3 = { 206, 221, tank_frames_pain3, tank_run };

static struct mframe_t tank_frames_attack_blast[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, -1, 0},
    {AI_CHARGE, -2, 0}, {AI_CHARGE, -1, 0}, {AI_CHARGE, -1, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, TANK_BLASTER}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_BLASTER},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_BLASTER},
};
static struct mmove_t tank_move_attack_blast = { 55, 70, tank_frames_attack_blast, tank_reattack_blaster };

static struct mframe_t tank_frames_reattack_blast[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_BLASTER}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_BLASTER},
};
static struct mmove_t tank_move_reattack_blast = { 65, 70, tank_frames_reattack_blast, tank_reattack_blaster };

static struct mframe_t tank_frames_attack_post_blast[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 2, 0}, {AI_MOVE, 3, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, -2, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_attack_post_blast = { 71, 76, tank_frames_attack_post_blast, tank_run };

static struct mframe_t tank_frames_attack_strike[] = {
    {AI_MOVE, 3, 0}, {AI_MOVE, 2, 0}, {AI_MOVE, 2, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 6, 0},
    {AI_MOVE, 7, 0}, {AI_MOVE, 9, TANK_FOOTSTEP}, {AI_MOVE, 2, 0}, {AI_MOVE, 1, 0}, {AI_MOVE, 2, 0},
    {AI_MOVE, 2, TANK_FOOTSTEP}, {AI_MOVE, 2, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, -2, 0}, {AI_MOVE, 0, TANK_WINDUP}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, TANK_STRIKE}, {AI_MOVE, 0, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, -1, 0},
    {AI_MOVE, -1, 0}, {AI_MOVE, -1, 0}, {AI_MOVE, -3, 0}, {AI_MOVE, -10, 0}, {AI_MOVE, -10, 0},
    {AI_MOVE, -2, 0}, {AI_MOVE, -3, 0}, {AI_MOVE, -2, TANK_FOOTSTEP},
};
static struct mmove_t tank_move_attack_strike = { 77, 114, tank_frames_attack_strike, tank_poststrike };

static struct mframe_t tank_frames_attack_pre_rocket[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 1, 0}, {AI_CHARGE, 2, 0}, {AI_CHARGE, 7, 0}, {AI_CHARGE, 7, 0},
    {AI_CHARGE, 7, TANK_FOOTSTEP}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, -3, 0},
};
static struct mmove_t tank_move_attack_pre_rocket = { 115, 135, tank_frames_attack_pre_rocket, tank_doattack_rocket };

static struct mframe_t tank_frames_attack_fire_rocket[] = {
    {AI_CHARGE, -3, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_ROCKET}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, TANK_ROCKET}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, -1, TANK_ROCKET},
};
static struct mmove_t tank_move_attack_fire_rocket = { 136, 144, tank_frames_attack_fire_rocket, tank_refire_rocket };

static struct mframe_t tank_frames_attack_post_rocket[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, -1, 0}, {AI_CHARGE, -1, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 2, 0},
    {AI_CHARGE, 3, 0}, {AI_CHARGE, 4, 0}, {AI_CHARGE, 2, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, -9, 0}, {AI_CHARGE, -8, 0}, {AI_CHARGE, -7, 0}, {AI_CHARGE, -1, 0},
    {AI_CHARGE, -1, TANK_FOOTSTEP}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
};
static struct mmove_t tank_move_attack_post_rocket = { 145, 167, tank_frames_attack_post_rocket, tank_run };

static struct mframe_t tank_frames_attack_chain[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN}, {AI_NONE, 0, TANK_MACHINEGUN},
    {AI_NONE, 0, TANK_MACHINEGUN}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
};
static struct mmove_t tank_move_attack_chain = { 168, 196, tank_frames_attack_chain, tank_run };

static struct mframe_t tank_frames_death[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, TANK_THUD}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t tank_move_death = { 222, 253, tank_frames_death, tank_dead };

static struct NEWANIMREMAP AnimRemap[] = {
    { 30, 33, {"Start Walk"}, 0, 0 },
    { 34, 49, {"Walk"}, 0, 0 },
    { 50, 54, {"Stop Walk"}, 0, 0 },
    { 222, 253, {"Death 4"}, 0, 0 },
    { 77, 114, {"Attak 2"}, 0, 0 },
    { 168, 196, {"Attak 4"}, 0, 0 },
    { 197, 200, {"Pain 1"}, 0, 0 },
    { 201, 205, {"Pain2"}, 0, 0 },
    { 206, 221, {"Pain 3"}, 0, 0 },
    { 0, 29, {"Stand"}, 0, 0 },
    { 55, 64, {"Attak 1 Pre"}, 0, 0 },
    { 65, 70, {"Attak 1 Loop"}, 0, 0 },
    { 71, 76, {"Attak 1 End"}, 0, 0 },
    { 115, 135, {"Attak 3 Pre"}, 0, 0 },
    { 136, 144, {"Attak 3 Rckt"}, 0, 0 },
    { 145, 167, {"Attak 3 Post"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { 184, 244, 209, 148, -1 };

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_tank, 0, PreInit_Tank, AnimRemap };

static int sound_thud;
static int sound_pain;
static int sound_idle;
static int sound_die;
static int sound_step;
static int sound_sight;
static int sound_windup;
static int sound_strike;
static int sound_gib;
static struct MODSUBOFF ChainGunMuzzle;
static struct MODSUBOFF BlasterMuzzle;
static struct MODSUBOFF RocketMuzzle[3];
/* unused; two 8-byte pieces, as a 16-byte one would be 16-aligned */
static int Unused1[2];
static int Unused2[2];
struct ai_local_table_t AiTable[32];   /* a common: it follows the bss word-aligned */

void PreInit_Tank(struct GLOBMODHDR *Hdr)
{
    struct MODSUBOFF RocketA;
    struct MODSUBOFF RocketB;
    struct SVECT Step;
    int i;

    if (Hdr) {
        Iface.PrintF("Pre Init Tank\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Tank");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Tank");
        Iface.PrintF("Validated Creature Interface for %s\n", "Tank");
        sound_thud = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_idle1"});
        sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_pain"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_death"});
        sound_step = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_step"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_sight1"});
        sound_windup = Iface.GetSoundNum((struct IDENTITYNAME){"pt1__strt"});
        sound_strike = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_atck1"});
        if (!sound_thud)
            sound_thud = sound_step;
        sound_windup = Iface.GetSoundClone(sound_windup);
        Iface.AlterSampleAttrs(sound_windup, 60, 128, -1);
        sound_gib = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        AiTable[0].thinkfunc = 0;
        AiTable[TANK_REFIRE_ROCKET].thinkfunc = tank_refire_rocket;
        AiTable[TANK_DOATTACK_ROCKET].thinkfunc = tank_doattack_rocket;
        AiTable[TANK_REATTACK_BLASTER].thinkfunc = tank_reattack_blaster;
        AiTable[TANK_WALK].thinkfunc = tank_walk;
        AiTable[TANK_RUN].thinkfunc = tank_run;
        AiTable[TANK_FOOTSTEP].thinkfunc = tank_footstep;
        AiTable[TANK_STAND].thinkfunc = tank_stand;
        AiTable[TANK_BLASTER].thinkfunc = TankBlaster;
        AiTable[TANK_STRIKE].thinkfunc = TankStrike;
        AiTable[TANK_ROCKET].thinkfunc = TankRocket;
        AiTable[TANK_WINDUP].thinkfunc = tank_windup;
        AiTable[TANK_POSTSTRIKE].thinkfunc = tank_poststrike;
        AiTable[TANK_MACHINEGUN].thinkfunc = TankMachineGun;
        AiTable[TANK_THUD].thinkfunc = tank_thud;
        AiTable[TANK_DEAD].thinkfunc = tank_dead;
        Iface.SetMyAiTable(CLASS_TANK, AiTable);
        Hdr->ShadowVerts = ShadowVerts;
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 251, 260, &ChainGunMuzzle))
            Iface.PrintF("Tank ChainGun muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 287, 289, &BlasterMuzzle))
            Iface.PrintF("Tank Blaster muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 307, 306, &RocketA)
            || !Iface.GetModSubOffFrom2Verteces(Hdr, 304, 305, &RocketB))
            Iface.PrintF("Tank Rocket muzzle points are from different submodels\r\n");
        /* the three rocket muzzles, evenly between the two points */
        Step.vx = (RocketB.Offset.vx - RocketA.Offset.vx) * 32 / 3;
        Step.vy = (RocketB.Offset.vy - RocketA.Offset.vy) * 32 / 3;
        Step.vz = (RocketB.Offset.vz - RocketA.Offset.vz) * 32 / 3;
        for (i = 0; i < 3; i++) {
            RocketMuzzle[i].Offset.vx = RocketA.Offset.vx + (Step.vx / 2 + i * Step.vx) / 32;
            RocketMuzzle[i].Offset.vy = RocketA.Offset.vy + (Step.vy / 2 + i * Step.vy) / 32;
            RocketMuzzle[i].Offset.vz = RocketA.Offset.vz + (Step.vz / 2 + i * Step.vz) / 32;
            RocketMuzzle[i].SubModel = RocketA.SubModel;
        }
    } else
        Iface.PrintF("Clear Tank\n");
}

static void TankBlaster(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT end;
    struct VECT dir;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, BlasterMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    end.vx = self->enemy->sorigin.vx;
    end.vy = self->enemy->sorigin.vy;
    end.vz = self->enemy->sorigin.vz;
    end.vy += self->enemy->viewheight;
    dir.vx = end.vx - start.vx;
    dir.vy = end.vy - start.vy;
    dir.vz = end.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_blaster(self, &start, &dir, 16, 800, 0, 8);
}

static void TankRocket(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct VECT vec;
    struct SVECT Rel;
    int flash_number;

    if (self->s.frame == 138)
        flash_number = 0;
    else if (self->s.frame == 141)
        flash_number = 1;
    else
        flash_number = 2;
    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, RocketMuzzle[flash_number], Cre->AnimFrame,
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
    Iface.monster_fire_rocket(self, &start, &dir, 50, 550, 0);
}

static void TankMachineGun(struct edict_s *self)
{
    struct SVECT dir;
    struct VECT vec;
    struct SVECT angles;
    struct VECT start;
    struct VECT forward;
    struct SVECT Rel;
    struct CREATURE *Cre = self->Extra.Creature;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, ChainGunMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    if (self->enemy) {
        vec.vx = self->enemy->sorigin.vx;
        vec.vy = self->enemy->sorigin.vy;
        vec.vz = self->enemy->sorigin.vz;
        vec.vy += self->enemy->viewheight;
        vec.vx = vec.vx - start.vx;
        vec.vy = vec.vy - start.vy;
        vec.vz = vec.vz - start.vz;
        Iface.vectoangles(&vec, &angles);
        dir.vx = angles.vx;
    } else
        dir.vx = 0;
    if (self->s.frame <= 181)
        dir.vz = self->sangles.vz + (self->s.frame - 174) * 91;
    else
        dir.vz = self->sangles.vz - (self->s.frame - 187) * 128;
    dir.vy = 0;
    Iface.AngleVectors(&dir, &forward, 0, 0);
    Iface.monster_fire_bullet(self, &start, &forward, 20, 4, 300, 500, 0);
}

static void tank_attack(struct edict_s *self)
{
    struct VECT vec;
    long range;
    int r;

    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health < 0) {
        self->monsterinfo.currentmove = &tank_move_attack_strike;
        self->monsterinfo.aiflags &= ~AI_BRUTAL;
        return;
    }
    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    range = Iface.VectorLength(&vec);
    r = Iface.rand();
    if (range <= 1500) {
        if (r < 13106)
            self->monsterinfo.currentmove = &tank_move_attack_chain;
        else
            self->monsterinfo.currentmove = &tank_move_attack_blast;
    } else if (range <= 3000) {
        if (r < 16384)
            self->monsterinfo.currentmove = &tank_move_attack_chain;
        else
            self->monsterinfo.currentmove = &tank_move_attack_blast;
    } else {
        if (r < 10813)
            self->monsterinfo.currentmove = &tank_move_attack_chain;
        else if (r < 21626) {
            self->monsterinfo.currentmove = &tank_move_attack_pre_rocket;
            self->pain_debounce_time = Iface.level->time + 50;
        } else
            self->monsterinfo.currentmove = &tank_move_attack_blast;
    }
}

static void tank_refire_rocket(struct edict_s *self)
{
    if ((*Iface.skill)->value >= 2)
        if (((struct CREATURE *)self->enemy->Extra.Creature)->Health > 0)
            if (Iface.visible(self, self->enemy))
                if (Iface.rand() < 13107) {
                    self->monsterinfo.currentmove = &tank_move_attack_fire_rocket;
                    return;
                }
    self->monsterinfo.currentmove = &tank_move_attack_post_rocket;
}

static void tank_doattack_rocket(struct edict_s *self)
{
    self->monsterinfo.currentmove = &tank_move_attack_fire_rocket;
}

static void tank_reattack_blaster(struct edict_s *self)
{
    if ((*Iface.skill)->value >= 2)
        if (Iface.visible(self, self->enemy))
            if (((struct CREATURE *)self->enemy->Extra.Creature)->Health > 0)
                if (Iface.rand() < 19661) {
                    self->monsterinfo.currentmove = &tank_move_reattack_blast;
                    return;
                }
    self->monsterinfo.currentmove = &tank_move_attack_post_blast;
}

static void tank_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &tank_move_walk;
}

static void tank_run(struct edict_s *self)
{
    if (self->enemy && self->enemy->client)
        self->monsterinfo.aiflags |= AI_BRUTAL;
    else
        self->monsterinfo.aiflags &= ~AI_BRUTAL;
    if (self->monsterinfo.aiflags & AI_STAND_GROUND) {
        self->monsterinfo.currentmove = &tank_move_stand;
        return;
    }
    if (self->monsterinfo.currentmove == &tank_move_walk || self->monsterinfo.currentmove == &tank_move_start_run)
        self->monsterinfo.currentmove = &tank_move_run;
    else
        self->monsterinfo.currentmove = &tank_move_start_run;
}

static void tank_footstep(struct edict_s *self)
{
    Iface.SetShockData(4, 14, 4096, &self->sorigin, (struct POWER){2048, 8192});
    Iface.SoundByCreature(sound_step, self->Extra.Creature);
}

static void tank_stand(struct edict_s *self)
{
    self->monsterinfo.currentmove = &tank_move_stand;
}

static void TankStrike(struct edict_s *self)
{
    Iface.SoundByCreature(sound_strike, self->Extra.Creature);
}

static void tank_windup(struct edict_s *self)
{
    Iface.SoundByCreature(sound_windup, self->Extra.Creature);
}

static void tank_poststrike(struct edict_s *self)
{
    self->enemy = 0;
    tank_run(self);
}

static void tank_thud(struct edict_s *self)
{
    Iface.SoundByCreature(sound_thud, self->Extra.Creature);
}

static void tank_dead(struct edict_s *self)
{
    self->movetype = 7;
    self->nextthink = 0;
    self->svflags |= 2;
}

inline void SP_monster_tank(struct edict_s *self)
{
    self->mass = 500;
    self->pain = tank_pain;
    self->die = tank_die;
    self->monsterinfo.stand = tank_stand;
    self->monsterinfo.walk = tank_walk;
    self->monsterinfo.run = tank_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = tank_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = tank_sight;
    self->monsterinfo.idle = tank_idle;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    self->monsterinfo.currentmove = &tank_move_stand;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
    if (self->classname == CLASS_TANK_COMMANDER)
        self->s.skinnum = 2;
}

static void tank_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

static void tank_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void tank_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health < self->max_health / 2)
        self->s.skinnum |= 1;
    if (damage <= 10)
        return;
    if (Iface.level->time < self->pain_debounce_time)
        return;
    if (damage <= 30)
        if (Iface.rand() > 6553)
            return;
    /* on hard or nightmare, no pain while attacking */
    if ((*Iface.skill)->value >= 2) {
        if (self->s.frame >= 115 && self->s.frame <= 144)
            return;
        if (self->s.frame >= 55 && self->s.frame <= 70)
            return;
    }
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain, self->Extra.Creature);
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 30)
        self->monsterinfo.currentmove = &tank_move_pain1;
    else if (damage <= 60)
        self->monsterinfo.currentmove = &tank_move_pain2;
    else
        self->monsterinfo.currentmove = &tank_move_pain3;
}

static void tank_die(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;

    if (Cre->Health <= self->gib_health) {
        self->deadflag = 2;
        tank_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, Cre);
    self->monsterinfo.currentmove = &tank_move_death;
    self->deadflag = 2;
    self->takedamage = 1;
}
