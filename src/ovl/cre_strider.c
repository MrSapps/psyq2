/* Strider (the PlayStation's own monster, built from Quake II's m_tank.c) */
#include "ovl.h"

/* mframe_t.aifunc_global_index: the engine's ai_global_table (InitAiLookupTables) */
#define AI_NONE 0
#define AI_STAND 1
#define AI_WALK 2
#define AI_RUN 3
#define AI_CHARGE 4
#define AI_MOVE 5

/* mframe_t.aifunc_local_index: this module's AiTable */
#define STRIDER_REATTACK_BLASTER 1
#define STRIDER_REATTACK_LASER 2
#define STRIDER_WALK 3
#define STRIDER_RUN 4
#define STRIDER_STAND 5
#define STRIDER_AIM 6
#define STRIDER_HYPERBLASTER 7
#define STRIDER_LASER 8
#define STRIDER_DEAD 9
#define STRIDER_KICK 10
#define STRIDER_STOMP 11
#define STRIDER_FOOTSTEP 12
#define STRIDER_KICK_FOOTSTEP 13
#define STRIDER_DUST 14
#define STRIDER_CORRECT_TURN 15
#define STRIDER_KICK_TURN 16
#define STRIDER_TURN_LEFT 17
#define STRIDER_TURN_RIGHT 18
#define STRIDER_NOTHING 19

#define CLASS_STRIDER 68

int abs(int);

/* aiflags */
#define AI_STAND_GROUND 0x1
#define AI_BRUTAL 0x200

void SP_monster_strider(struct edict_s *self);
void PreInit_Strider(struct GLOBMODHDR *Hdr);
static int strider_can_attack(struct edict_s *self);
static void StriderLaser(struct edict_s *self);
static void StriderHyperBlaster(struct edict_s *self);

/* The small functions, emitted at the end in this order (see the README). The frame
 * tables come before the interface: the original's .data relocations precede its .header ones. */
static void strider_reattack_blaster(struct edict_s *self);
static void strider_reattack_laser(struct edict_s *self);
static void strider_walk(struct edict_s *self);
static void strider_run(struct edict_s *self);
static void strider_stand(struct edict_s *self);
static void strider_aim(struct edict_s *self);
static void strider_dead(struct edict_s *self);
static void strider_kick(struct edict_s *self);
static void strider_stomp(struct edict_s *self);
static void strider_footstep(struct edict_s *self);
static void strider_kick_footstep(struct edict_s *self);
static void strider_dust(struct edict_s *self);
static void strider_correct_turn(struct edict_s *self);
static void strider_kick_turn(struct edict_s *self);
static void strider_do_blaster(struct edict_s *self);
static void strider_do_laser(struct edict_s *self);
void strider_start_left(struct edict_s *self);
void strider_start_right(struct edict_s *self);
static void strider_turn_left(struct edict_s *self);
static void strider_turn_right(struct edict_s *self);
static void strider_nothing(struct edict_s *self);
int strider_facing(struct edict_s *self);
static void strider_sight(struct edict_s *self);
void strider_thud(struct edict_s *self);
void strider_windup(struct edict_s *self);
static void strider_idle(struct edict_s *self);
static void strider_turn(struct edict_s *self, int amount);
static void strider_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void strider_attack(struct edict_s *self);
static void strider_die(struct edict_s *self);

static struct mframe_t strider_frames_stand1[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0},
};
static struct mmove_t strider_move_stand1 = { 18, 43, strider_frames_stand1, 0 };

static struct mframe_t strider_frames_stand2[] = {
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0}, {AI_STAND, 0, 0},
    {AI_STAND, 0, 0},
};
static struct mmove_t strider_move_stand2 = { 44, 69, strider_frames_stand2, 0 };

static struct mframe_t strider_frames_start_walk[] = {
    {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, STRIDER_FOOTSTEP},
};
static struct mmove_t strider_move_start_walk = { 70, 72, strider_frames_start_walk, strider_walk };

static struct mframe_t strider_frames_walk[] = {
    {AI_WALK, 4, 0}, {AI_WALK, 5, 0}, {AI_WALK, 3, 0}, {AI_WALK, 2, 0}, {AI_WALK, 5, 0},
    {AI_WALK, 5, 0}, {AI_WALK, 4, 0}, {AI_WALK, 4, 0}, {AI_WALK, 3, 0}, {AI_WALK, 5, STRIDER_FOOTSTEP},
    {AI_WALK, 4, 0}, {AI_WALK, 5, 0}, {AI_WALK, 7, 0}, {AI_WALK, 7, 0}, {AI_WALK, 6, 0},
    {AI_WALK, 6, 0}, {AI_WALK, 6, 0}, {AI_WALK, 6, 0}, {AI_WALK, 6, STRIDER_FOOTSTEP},
};
static struct mmove_t strider_move_walk = { 73, 91, strider_frames_walk, 0 };

static struct mframe_t strider_frames_end_walk[] = {
    {AI_WALK, 0, 0}, {AI_WALK, 0, 0}, {AI_WALK, 0, STRIDER_FOOTSTEP},
};
static struct mmove_t strider_move_end_walk = { 92, 94, strider_frames_end_walk, strider_stand };

static struct mframe_t strider_frames_start_run[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, STRIDER_FOOTSTEP},
};
static struct mmove_t strider_move_start_run = { 70, 72, strider_frames_start_run, strider_run };

static struct mframe_t strider_frames_run[] = {
    {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK_FOOTSTEP}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK}, {AI_RUN, 10, STRIDER_KICK},
    {AI_RUN, 10, STRIDER_KICK_FOOTSTEP},
};
static struct mmove_t strider_move_run = { 73, 91, strider_frames_run, 0 };

static struct mframe_t strider_frames_end_run[] = {
    {AI_RUN, 0, 0}, {AI_RUN, 0, 0}, {AI_RUN, 0, STRIDER_FOOTSTEP},
};
static struct mmove_t strider_move_end_run = { 92, 94, strider_frames_end_run, strider_walk };

static struct mframe_t strider_frames_pain1[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t strider_move_pain1 = { 95, 102, strider_frames_pain1, strider_run };

static struct mframe_t strider_frames_pain2[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t strider_move_pain2 = { 103, 110, strider_frames_pain2, strider_run };

static struct mframe_t strider_frames_start_blaster[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, STRIDER_AIM},
};
static struct mmove_t strider_move_start_blaster = { 137, 139, strider_frames_start_blaster, strider_do_blaster };

static struct mframe_t strider_frames_end_blaster[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t strider_move_end_blaster = { 145, 147, strider_frames_end_blaster, strider_run };

static struct mframe_t strider_frames_blaster[] = {
    {AI_MOVE, 0, STRIDER_HYPERBLASTER}, {AI_MOVE, 0, STRIDER_HYPERBLASTER},
    {AI_MOVE, 0, STRIDER_HYPERBLASTER}, {AI_MOVE, 0, STRIDER_HYPERBLASTER}, {AI_MOVE, 0, STRIDER_AIM},
};
static struct mmove_t strider_move_blaster = { 140, 144, strider_frames_blaster, strider_reattack_blaster };

static struct mframe_t strider_frames_start_laser[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, STRIDER_AIM},
    {AI_MOVE, 0, 0},
};
static struct mmove_t strider_move_start_laser = { 189, 194, strider_frames_start_laser, strider_do_laser };

static struct mframe_t strider_frames_end_laser[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0},
};
static struct mmove_t strider_move_end_laser = { 199, 205, strider_frames_end_laser, strider_run };

static struct mframe_t strider_frames_laser[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, STRIDER_LASER}, {AI_MOVE, 0, STRIDER_LASER},
    {AI_MOVE, 0, STRIDER_AIM},
};
static struct mmove_t strider_move_laser = { 195, 198, strider_frames_laser, strider_reattack_laser };

static struct mframe_t strider_frames_death[] = {
    {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, 0}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST}, {AI_MOVE, 0, STRIDER_DUST},
    {AI_MOVE, 0, STRIDER_DUST},
};
static struct mmove_t strider_move_death = { 111, 136, strider_frames_death, strider_dead };

static struct mframe_t strider_frames_left[] = {
    {AI_MOVE, 6, STRIDER_TURN_RIGHT}, {AI_MOVE, 6, STRIDER_TURN_RIGHT},
    {AI_MOVE, 6, STRIDER_TURN_RIGHT}, {AI_MOVE, 6, STRIDER_TURN_RIGHT},
    {AI_MOVE, 6, STRIDER_TURN_RIGHT}, {AI_MOVE, 6, STRIDER_TURN_RIGHT},
    {AI_MOVE, 6, STRIDER_TURN_RIGHT}, {AI_MOVE, 6, STRIDER_TURN_RIGHT},
    {AI_MOVE, 6, STRIDER_TURN_RIGHT}, {AI_MOVE, 6, STRIDER_TURN_RIGHT},
    {AI_MOVE, 6, STRIDER_TURN_RIGHT},
};
static struct mmove_t strider_move_left = { 151, 161, strider_frames_left, strider_run };

static struct mframe_t strider_frames_right[] = {
    {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT},
    {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT},
    {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT},
    {AI_MOVE, 6, STRIDER_TURN_LEFT}, {AI_MOVE, 6, STRIDER_TURN_LEFT},
};
static struct mmove_t strider_move_right = { 170, 180, strider_frames_right, strider_run };

static struct mframe_t strider_frames_standendwalk[] = {
    {AI_MOVE, 0, STRIDER_NOTHING}, {AI_MOVE, 0, STRIDER_NOTHING}, {AI_MOVE, 0, STRIDER_NOTHING},
    {AI_MOVE, 0, STRIDER_NOTHING}, {AI_MOVE, 0, STRIDER_NOTHING}, {AI_MOVE, 0, STRIDER_NOTHING},
};
static struct mmove_t strider_move_standendwalk = { 216, 221, strider_frames_standendwalk, strider_run };

static struct mframe_t strider_frames_stomp[] = {
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, STRIDER_STOMP}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0}, {AI_CHARGE, 0, 0},
    {AI_CHARGE, 0, 0},
};
static struct mmove_t strider_move_stomp = { 206, 215, strider_frames_stomp, strider_run };

static unsigned char TurnDir = 0;   /* 1 or 2: strider_correct_turn turns once; nothing sets it */

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 12, {"Inactive"}, 0, 0 },
    { 13, 17, {"Activate"}, 0, 0 },
    { 18, 43, {"Stand 1"}, 0, 0 },
    { 44, 69, {"Stand 2"}, 0, 0 },
    { 70, 72, {"StartWalk"}, 0, 0 },
    { 73, 91, {"Walk"}, 0, 0 },
    { 92, 94, {"EndWalk"}, 0, 0 },
    { 95, 102, {"Pain 1"}, 0, 0 },
    { 103, 110, {"Pain 2"}, 0, 0 },
    { 111, 136, {"Death"}, 0, 0 },
    { 137, 139, {"StartBFire"}, 0, 0 },
    { 140, 144, {"BFire"}, 0, 0 },
    { 145, 147, {"EndBFire"}, 0, 0 },
    { 216, 221, {"Standendwalk"}, 0, 0 },
    { 151, 161, {"Left"}, 0, 0 },
    { 170, 180, {"Right"}, 0, 0 },
    { 189, 194, {"Startlfire"}, 0, 0 },
    { 195, 198, {"Lfire"}, 0, 0 },
    { 199, 205, {"EndLFire"}, 0, 0 },
    { 206, 215, {"Stomp"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

/* the original's .bss starts 8-aligned and tools/ovllink.py aligns it to 4: this word is the gap */
static int BssAlign = 0;

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_strider, 0, PreInit_Strider, AnimRemap };

static int sound_thud;
static int sound_pain;
static int sound_idle;
static int sound_die;
static int sound_step;
static int sound_sight;
static int sound_windup;
static int sound_strike;
static int sound_gib;
static unsigned char DustCount;
static struct MODSUBOFF LaserMuzzle;
static struct MODSUBOFF HyperBlasterMuzzle;
/* unused; two 8-byte pieces, as a 16-byte one would be 16-aligned */
static int Unused1[2];
static int Unused2[2];
struct ai_local_table_t AiTable[32];   /* a common: it follows the bss word-aligned */

/* whether self faces its enemy to within yaw_speed (after M_ChangeYaw); the bare return
 * leaves the result undefined when it faces it exactly */
int strider_facing(struct edict_s *self)
{
    long current;
    long ideal;
    long move;
    struct VECT vec;

    current = Iface.anglemod(self->sangles.vz);
    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    ideal = Iface.vectoyaw(&vec);
    if (current == ideal)
        return;
    move = ideal - current;
    if (ideal > current) {
        if (move >= 2048)
            move = move - 4096;
    } else {
        if (move <= -2048)
            move = move + 4096;
    }
    return abs(move) <= self->yaw_speed;
}

void PreInit_Strider(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Strider - Correcting Turn\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Strider");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Strider");
        Iface.PrintF("Validated Creature Interface for %s\n", "Strider");
        sound_thud = Iface.GetSoundNum((struct IDENTITYNAME){"str_step"});
        sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"str_pain1"});
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"str_idle"});
        sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"str_deth"});
        sound_step = Iface.GetSoundNum((struct IDENTITYNAME){"str_step"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"str_sght"});
        sound_windup = Iface.GetSoundNum((struct IDENTITYNAME){"pt1__strt"});
        sound_strike = Iface.GetSoundNum((struct IDENTITYNAME){"str_atck2c"});
        /* the tank's sounds where the strider's are missing */
        if (!sound_thud)
            sound_thud = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_idle1"});
        if (!sound_pain)
            sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_pain"});
        if (!sound_idle)
            sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_idle1"});
        if (!sound_die)
            sound_die = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_death"});
        if (!sound_step)
            sound_step = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_step"});
        if (!sound_sight)
            sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_sight1"});
        if (!sound_windup)
            sound_windup = Iface.GetSoundNum((struct IDENTITYNAME){"pt1__strt"});
        if (!sound_strike)
            sound_strike = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_atck1"});
        sound_windup = Iface.GetSoundClone(sound_windup);
        Iface.AlterSampleAttrs(sound_windup, 60, 128, -1);
        sound_gib = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        AiTable[0].thinkfunc = 0;
        AiTable[STRIDER_REATTACK_BLASTER].thinkfunc = strider_reattack_blaster;
        AiTable[STRIDER_REATTACK_LASER].thinkfunc = strider_reattack_laser;
        AiTable[STRIDER_WALK].thinkfunc = strider_walk;
        AiTable[STRIDER_RUN].thinkfunc = strider_run;
        AiTable[STRIDER_STAND].thinkfunc = strider_stand;
        AiTable[STRIDER_AIM].thinkfunc = strider_aim;
        AiTable[STRIDER_HYPERBLASTER].thinkfunc = StriderHyperBlaster;
        AiTable[STRIDER_LASER].thinkfunc = StriderLaser;
        AiTable[STRIDER_DEAD].thinkfunc = strider_dead;
        AiTable[STRIDER_KICK].thinkfunc = strider_kick;
        AiTable[STRIDER_STOMP].thinkfunc = strider_stomp;
        AiTable[STRIDER_FOOTSTEP].thinkfunc = strider_footstep;
        AiTable[STRIDER_KICK_FOOTSTEP].thinkfunc = strider_kick_footstep;
        AiTable[STRIDER_DUST].thinkfunc = strider_dust;
        AiTable[STRIDER_CORRECT_TURN].thinkfunc = strider_correct_turn;
        AiTable[STRIDER_KICK_TURN].thinkfunc = strider_kick_turn;
        AiTable[STRIDER_TURN_LEFT].thinkfunc = strider_turn_left;
        AiTable[STRIDER_TURN_RIGHT].thinkfunc = strider_turn_right;
        AiTable[STRIDER_NOTHING].thinkfunc = strider_nothing;
        Iface.SetMyAiTable(CLASS_STRIDER, AiTable);
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 456, 457, &LaserMuzzle))
            Iface.PrintF("Strider Laser muzzle points are from different submodels\r\n");
        if (!Iface.GetModSubOffFrom2Verteces(Hdr, 145, 158, &HyperBlasterMuzzle))
            Iface.PrintF("Strider HyperBlaster muzzle points are from different submodels\r\n");
    } else
        Iface.PrintF("Clear Strider\r\n");
}

static int strider_can_attack(struct edict_s *self)
{
    int ret = 0;

    if (Iface.infront(self, self->enemy)) {
        if (strider_facing(self) && Iface.visible(self, self->enemy))
            ret = 1;
    }
    return ret;
}

static void StriderLaser(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, LaserMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    dir.vx = self->pos1.vx - start.vx;
    dir.vy = self->pos1.vy - start.vy;
    dir.vz = self->pos1.vz - start.vz;
    Iface.VectorNormalize(&dir);
    Iface.monster_fire_laser(self, &start, &dir, 50, 25, 0);
}

static void StriderHyperBlaster(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    struct VECT start;
    struct VECT dir;
    struct SVECT Rel;

    Rel = Iface.GetSubModXYZ(Cre->ModelHdr, HyperBlasterMuzzle, Cre->AnimFrame,
                             Cre->Draw.AnimJump ? Cre->Draw.TargetAnimFrame : -1);
    Iface.RotateS(&Cre->Matrix, &Rel, &Rel);
    start.vx = Cre->Draw.Location.vx + Rel.vx;
    start.vy = Cre->Draw.Location.vy + Rel.vy;
    start.vz = Cre->Draw.Location.vz + Rel.vz;
    dir.vx = self->pos1.vx - start.vx;
    dir.vy = self->pos1.vy - start.vy;
    dir.vz = self->pos1.vz - start.vz;
    Iface.VectorNormalize(&dir);
    dir.vx /= 2;
    dir.vy /= 2;
    dir.vz /= 2;
    Iface.monster_fire_blaster(self, &start, &dir, 25, 100, 0, 64);
}

void SP_monster_strider(struct edict_s *self)
{
    self->mass = 500;
    self->pain = strider_pain;
    self->die = strider_die;
    self->monsterinfo.stand = strider_stand;
    self->monsterinfo.walk = strider_walk;
    self->monsterinfo.run = strider_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = strider_attack;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = strider_sight;
    self->monsterinfo.idle = strider_idle;
    self->monsterinfo.turn = strider_turn;
    self->monsterinfo.turn_threshold = 1024;
    self->movetype = 5;
    self->solid = 2;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    if (Iface.rand() < 16384)
        self->monsterinfo.currentmove = &strider_move_stand1;
    else
        self->monsterinfo.currentmove = &strider_move_stand2;
    self->monsterinfo.scale = 10;
    Iface.walkmonster_start(self);
}

static void strider_reattack_blaster(struct edict_s *self)
{
    if (Iface.range(self, self->enemy) && strider_can_attack(self))
        self->monsterinfo.currentmove = &strider_move_blaster;
    else
        self->monsterinfo.currentmove = &strider_move_end_blaster;
}

static void strider_reattack_laser(struct edict_s *self)
{
    if (strider_can_attack(self) && Iface.range(self, self->enemy) == 2)
        self->monsterinfo.currentmove = &strider_move_laser;
    else
        self->monsterinfo.currentmove = &strider_move_end_laser;
}

static void strider_walk(struct edict_s *self)
{
    self->monsterinfo.currentmove = &strider_move_walk;
}

static void strider_stand(struct edict_s *self)
{
    if (Iface.rand() < 16384)
        self->monsterinfo.currentmove = &strider_move_stand1;
    else
        self->monsterinfo.currentmove = &strider_move_stand2;
}

/* nothing uses these two, but the original has them */
void strider_start_left(struct edict_s *self)
{
    ((struct CREATURE *)self->Extra.Creature)->SequenceData2 = Iface.anglemod(self->sangles.vz - 512) - self->sangles.vz;
    strider_run(self);
}

void strider_start_right(struct edict_s *self)
{
    ((struct CREATURE *)self->Extra.Creature)->SequenceData2 = Iface.anglemod(self->sangles.vz + 512) - self->sangles.vz;
    strider_run(self);
}

static void strider_run(struct edict_s *self)
{
    if (self->enemy && self->enemy->client)
        self->monsterinfo.aiflags |= AI_BRUTAL;
    else
        self->monsterinfo.aiflags &= ~AI_BRUTAL;
    if (self->monsterinfo.aiflags & AI_STAND_GROUND) {
        strider_stand(self);
        return;
    }
    if (self->monsterinfo.currentmove == &strider_move_stand1 || self->monsterinfo.currentmove == &strider_move_stand2
        || self->monsterinfo.currentmove == &strider_move_standendwalk)
        self->monsterinfo.currentmove = &strider_move_start_run;
    else if (self->monsterinfo.currentmove == &strider_move_right || self->monsterinfo.currentmove == &strider_move_left)
        self->monsterinfo.currentmove = &strider_move_standendwalk;
    else
        self->monsterinfo.currentmove = &strider_move_run;
}

static void strider_aim(struct edict_s *self)
{
    self->pos1.vx = self->enemy->sorigin.vx;
    self->pos1.vy = self->enemy->sorigin.vy;
    self->pos1.vz = self->enemy->sorigin.vz;
}

static void strider_dead(struct edict_s *self)
{
    ((struct CREATURE *)self->Extra.Creature)->Health = self->gib_health;
    self->takedamage = 0;
    self->movetype = 7;
    self->nextthink = 0;
    self->svflags |= 2;
}

static void strider_footstep(struct edict_s *self)
{
    Iface.SetShockData(4, 14, 4096, &self->sorigin, (struct POWER){2048, 8192});
    Iface.SoundByCreature(sound_step, self->Extra.Creature);
}

static void strider_correct_turn(struct edict_s *self)
{
    long yaw;

    /* the two tails are the same: gcc's cross-jumping merges them */
    if (TurnDir == 1) {
        yaw = Iface.anglemod(Iface.anglemod(self->sangles.vz) - 512);
        self->sangles.vz = yaw;
        TurnDir = 0;
        ((struct CREATURE *)self->Extra.Creature)->Ang.vy = ((struct CREATURE *)self->Extra.Creature)->IdealYaw = yaw;
    } else if (TurnDir == 2) {
        yaw = Iface.anglemod(Iface.anglemod(self->sangles.vz) + 512);
        self->sangles.vz = yaw;
        TurnDir = 0;
        ((struct CREATURE *)self->Extra.Creature)->Ang.vy = ((struct CREATURE *)self->Extra.Creature)->IdealYaw = yaw;
    }
}

static void strider_kick_footstep(struct edict_s *self)
{
    strider_kick(self);
    strider_footstep(self);
}

static void strider_kick_turn(struct edict_s *self)
{
    strider_kick(self);
    strider_correct_turn(self);
}

/* defined after its callers above, which call it rather than inlining it */
static void strider_kick(struct edict_s *self)
{
    struct VECT aim;

    if (!self->enemy)
        return;
    aim.vx = 1020;
    aim.vy = -48;
    aim.vz = 0;
    Iface.fire_hit(self, aim, Iface.rand() % 50 + 50, 200);
}

static void strider_stomp(struct edict_s *self)
{
    struct VECT aim;

    if (!self->enemy)
        return;
    aim.vx = 2040;
    aim.vy = -48;
    aim.vz = 0;
    Iface.fire_hit(self, aim, Iface.rand() % 50 + 100, 500);
}

static void strider_dust(struct edict_s *self)
{
    struct VECT pos;
    int area;

    if (++DustCount & 1)
        return;
    area = ((struct CREATURE *)self->Extra.Creature)->Box.Area;
    pos.vx = self->sorigin.vx + (Iface.rand() - 16384) / 32;
    pos.vy = self->sorigin.vy + (Iface.rand() - 16384) / 64 - 60;
    pos.vz = self->sorigin.vz + (Iface.rand() - 16384) / 32;
    Iface.QuakeGenExplosion(&pos, area, 4096, 0);
}

static void strider_do_blaster(struct edict_s *self)
{
    self->monsterinfo.currentmove = &strider_move_blaster;
}

static void strider_do_laser(struct edict_s *self)
{
    self->monsterinfo.currentmove = &strider_move_laser;
}

static void strider_turn_left(struct edict_s *self)
{
    self->sangles.vz = Iface.anglemod(self->sangles.vz + 93);
    Iface.linkentity(self, 4);
}

static void strider_turn_right(struct edict_s *self)
{
    self->sangles.vz = Iface.anglemod(self->sangles.vz - 93);
    Iface.linkentity(self, 4);
}

static void strider_nothing(struct edict_s *self)
{
}

static void strider_sight(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
}

/* nothing uses these two, but the original has them */
void strider_thud(struct edict_s *self)
{
    Iface.SoundByCreature(sound_thud, self->Extra.Creature);
}

void strider_windup(struct edict_s *self)
{
    Iface.SoundByCreature(sound_windup, self->Extra.Creature);
}

static void strider_idle(struct edict_s *self)
{
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void strider_turn(struct edict_s *self, int amount)
{
    /* the two turning branches are the same: gcc's cross-jumping merges them */
    if (Iface.visible(self, self->enemy)) {
        if (amount == self->yaw_speed)
            self->monsterinfo.currentmove = &strider_move_right;
        else if (amount == -self->yaw_speed)
            self->monsterinfo.currentmove = &strider_move_left;
        else
            ((struct CREATURE *)self->Extra.Creature)->IdealYaw = self->sangles.vz =
                Iface.anglemod(Iface.anglemod(self->sangles.vz) + amount);
    } else
        ((struct CREATURE *)self->Extra.Creature)->IdealYaw = self->sangles.vz =
            Iface.anglemod(Iface.anglemod(self->sangles.vz) + amount);
}

static void strider_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
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
        if (self->s.frame >= 137 && self->s.frame <= 147)
            return;
        if (self->s.frame >= 189 && self->s.frame <= 201)
            return;
    }
    self->pain_debounce_time = Iface.level->time + 30;
    Iface.SoundByCreature(sound_pain, self->Extra.Creature);
    if ((*Iface.skill)->value == 3)
        return;
    if (damage <= 30)
        self->monsterinfo.currentmove = &strider_move_pain1;
    else
        self->monsterinfo.currentmove = &strider_move_pain2;
}

static void strider_attack(struct edict_s *self)
{
    if (!strider_can_attack(self))
        return;
    if (((struct CREATURE *)self->Extra.Creature)->Health < 0)
        self->monsterinfo.aiflags &= ~AI_BRUTAL;
    if (Iface.range(self, self->enemy) == 0)
        self->monsterinfo.currentmove = &strider_move_stomp;
    else if (Iface.range(self, self->enemy) == 1)
        self->monsterinfo.currentmove = &strider_move_start_blaster;
    else if (Iface.rand() < 16384)
        self->monsterinfo.currentmove = &strider_move_start_blaster;
    else
        self->monsterinfo.currentmove = &strider_move_start_laser;
}

static void strider_die(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;

    if (Cre->Health <= self->gib_health) {
        Iface.SoundByCreature(sound_gib, Cre);
        self->deadflag = 2;
        strider_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    Iface.SoundByCreature(sound_die, Cre);
    self->monsterinfo.currentmove = &strider_move_death;
    self->deadflag = 2;
    self->takedamage = 1;
}
