/* Insane marine (m_insane.c) */
#include "ovl.h"

/* ai_global_table indices (gmonster.c) */
enum { AI_NONE, AI_STAND, AI_WALK, AI_RUN, AI_CHARGE, AI_MOVE };
/* indices into this module's local ai table */
enum { L_NONE, L_STAND, L_DEAD, L_UNUSED, L_WALK, L_RUN, L_CHECKDOWN, L_CHECKUP, L_ONGROUND,
       L_FIST, L_SHAKE, L_MOAN, L_SCREAM, L_LIE };

#define CLASS_INSANE 94

/* gcc emits the small functions last, in this order */
void PreInit_Insane(struct GLOBMODHDR *Hdr);
static void insane_pain(struct edict_s *self, struct edict_s *other, long kick, int damage);
static void insane_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage);
void SP_misc_insane(struct edict_s *self);
static void insane_stand(struct edict_s *self);
static void insane_dead(struct edict_s *self);
static void insane_walk(struct edict_s *self);
static void insane_run(struct edict_s *self);
static void insane_checkdown(struct edict_s *self);
static void insane_checkup(struct edict_s *self);
static void insane_onground(struct edict_s *self);
static void insane_fist(struct edict_s *self);
static void insane_shake(struct edict_s *self);
static void insane_moan(struct edict_s *self);
static void insane_scream(struct edict_s *self);
void insane_lie(struct edict_s *self);
static void insane_getup(struct edict_s *self);
static void insane_tocrawl(struct edict_s *self);

static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_misc_insane, 0, PreInit_Insane, AnimRemap };

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 39, {"Up Down"}, 0, 0 },
    { 40, 58, {"Down Up"}, 0, 0 },
    { 59, 64, {"Stand N"}, 0, 0 },
    { 64, 92, {"Stand I"}, 0, 0 },
    { 94, 98, {"Jump Down"}, 0, 0 },
    { 99, 108, {"Down 1"}, 0, 0 },
    { 109, 147, {"Down 2"}, 0, 0 },
    { 148, 158, {"Down 3"}, 0, 0 },
    { 160, 172, {"Walk 1"}, 0, 0 },
    { 173, 198, {"Walk 2"}, 0, 0 },
    { 199, 209, {"St Pain"}, 0, 0 },
    { 210, 226, {"St Death"}, 0, 0 },
    { 282, 282, {"On Back"}, 0, 0 },
    { 227, 235, {"Crawl"}, 0, 0 },
    { 236, 244, {"Cr Pain"}, 0, 0 },
    { 245, 251, {"Cr Death"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};


static int sound_insane2;
static int sound_insane9;
static int sound_udeath;
static struct ai_local_table_t AiTable[32];
static int Unused[2]; /* space the original has that no code uses */

void PreInit_Insane(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Insane\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Insane");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Insane");
        Iface.PrintF("Validated Creature Interface for %s\n", "Insane");
        AiTable[L_NONE].thinkfunc = 0;
        AiTable[L_STAND].thinkfunc = insane_stand;
        AiTable[L_DEAD].thinkfunc = insane_dead;
        AiTable[L_WALK].thinkfunc = insane_walk;
        AiTable[L_RUN].thinkfunc = insane_run;
        AiTable[L_CHECKDOWN].thinkfunc = insane_checkdown;
        AiTable[L_CHECKUP].thinkfunc = insane_checkup;
        AiTable[L_ONGROUND].thinkfunc = insane_onground;
        AiTable[L_FIST].thinkfunc = insane_fist;
        AiTable[L_SHAKE].thinkfunc = insane_shake;
        AiTable[L_MOAN].thinkfunc = insane_moan;
        AiTable[L_SCREAM].thinkfunc = insane_scream;
        AiTable[L_LIE].thinkfunc = insane_lie;
        Iface.SetMyAiTable(CLASS_INSANE, AiTable);
        sound_insane2 = Iface.GetSoundNum((struct IDENTITYNAME){"insane2"});
        sound_insane9 = Iface.GetSoundNum((struct IDENTITYNAME){"insane9"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
        if (!sound_insane2)
            sound_insane2 = sound_insane9;
        Iface.AlterSampleAttrs(sound_insane2, 0, 127, -1);
        Iface.AlterSampleAttrs(sound_insane9, 0, 127, -1);
    } else
        Iface.PrintF("Clear Insane\r\n");
}

static struct mframe_t insane_frames_stand_normal[] = {
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_CHECKDOWN },
};
static struct mmove_t insane_move_stand_normal = { 59, 64, insane_frames_stand_normal, insane_stand };

static struct mframe_t insane_frames_lie[] = {
    { AI_STAND, 0, L_NONE },
};
static struct mmove_t insane_move_lie = { 282, 282, insane_frames_lie, insane_dead };

static struct mframe_t insane_frames_stand_insane[] = {
    { AI_STAND, 0, L_SHAKE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE }, { AI_STAND, 0, L_NONE },
    { AI_STAND, 0, L_CHECKDOWN },
};
static struct mmove_t insane_move_stand_insane = { 64, 92, insane_frames_stand_insane, insane_stand };

static struct mframe_t insane_frames_uptodown[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_MOAN },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 3, L_NONE }, { AI_MOVE, 4, L_NONE }, { AI_MOVE, 6, L_NONE }, { AI_MOVE, 8, L_NONE },
    { AI_MOVE, 4, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_FIST },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_FIST }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_uptodown = { 0, 39, insane_frames_uptodown, insane_tocrawl };

static struct mframe_t insane_frames_downtoup[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_downtoup = { 40, 58, insane_frames_downtoup, insane_stand };

static struct mframe_t insane_frames_jumpdown[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 9, L_NONE }, { AI_MOVE, 13, L_NONE },
    { AI_MOVE, 5, L_NONE },
};
static struct mmove_t insane_move_jumpdown = { 94, 98, insane_frames_jumpdown, insane_tocrawl };

static struct mframe_t insane_frames_down1[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_down1 = { 99, 108, insane_frames_down1, insane_onground };

static struct mframe_t insane_frames_down[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_FIST }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_MOAN }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_SCREAM },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_CHECKUP },
};
static struct mmove_t insane_move_down = { 109, 147, insane_frames_down, insane_onground };

static struct mframe_t insane_frames_getup[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_SCREAM }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_getup = { 148, 158, insane_frames_getup, insane_getup };

static struct mframe_t insane_frames_walk_normal[] = {
    { AI_WALK, 2, L_SCREAM }, { AI_WALK, 2, L_NONE }, { AI_WALK, 2, L_NONE }, { AI_WALK, 2, L_NONE },
    { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 1, L_NONE }, { AI_WALK, 2, L_NONE },
    { AI_WALK, 2, L_NONE }, { AI_WALK, 2, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE },
    { AI_WALK, 3, L_NONE },
};
static struct mmove_t insane_move_walk_normal = { 160, 172, insane_frames_walk_normal, insane_walk };
static struct mmove_t insane_move_run_normal = { 160, 172, insane_frames_walk_normal, insane_run };

static struct mframe_t insane_frames_walk_insane[] = {
    { AI_WALK, 2, L_SCREAM }, { AI_WALK, 2, L_NONE }, { AI_WALK, 2, L_NONE }, { AI_WALK, 2, L_NONE },
    { AI_WALK, 2, L_NONE }, { AI_WALK, 1, L_NONE }, { AI_WALK, 1, L_NONE }, { AI_WALK, 1, L_NONE },
    { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 1, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 0, L_NONE }, { AI_WALK, 2, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 2, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE }, { AI_WALK, 4, L_NONE },
    { AI_WALK, 4, L_NONE }, { AI_WALK, 2, L_NONE },
};
static struct mmove_t insane_move_walk_insane = { 173, 198, insane_frames_walk_insane, insane_walk };
static struct mmove_t insane_move_run_insane = { 173, 198, insane_frames_walk_insane, insane_run };

static struct mframe_t insane_frames_stand_pain[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_stand_pain = { 199, 209, insane_frames_stand_pain, insane_run };

static struct mframe_t insane_frames_stand_death[] = {
    { AI_MOVE, -10, L_NONE }, { AI_MOVE, -10, L_NONE }, { AI_MOVE, -9, L_NONE }, { AI_MOVE, -9, L_NONE },
    { AI_MOVE, -8, L_NONE }, { AI_MOVE, -7, L_NONE }, { AI_MOVE, -5, L_NONE }, { AI_MOVE, -3, L_NONE },
    { AI_MOVE, -1, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_stand_death = { 210, 226, insane_frames_stand_death, insane_dead };

static struct mframe_t insane_frames_crawl[] = {
    { AI_WALK, 3, L_SCREAM }, { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE },
    { AI_WALK, 0, L_NONE }, { AI_WALK, 2, L_NONE }, { AI_WALK, 3, L_NONE }, { AI_WALK, 3, L_NONE },
    { AI_WALK, 3, L_NONE },
};
static struct mmove_t insane_move_crawl = { 227, 235, insane_frames_crawl, 0 };
static struct mmove_t insane_move_runcrawl = { 227, 235, insane_frames_crawl, 0 };

static struct mframe_t insane_frames_crawl_pain[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_crawl_pain = { 236, 244, insane_frames_crawl_pain, insane_run };

static struct mframe_t insane_frames_crawl_death[] = {
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
    { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE }, { AI_MOVE, 0, L_NONE },
};
static struct mmove_t insane_move_crawl_death = { 245, 251, insane_frames_crawl_death, insane_dead };

static int UnusedData = 0; /* .data the original has that no code uses */

/* The original's .rdata alignment padding holds a stray byte. */
__asm__(".rdata\n\t.byte 0x5f\n\t.text");

/* Inlined everywhere, but global, so the original still has them out of line at the end. */
int insane_crawler(struct edict_s *self)
{
    if (((struct CREATURE *)self->Extra.Creature)->SpawnFlags & 8)
        return 1;
    return 0;
}

int insane_ondown(struct edict_s *self)
{
    if (self->monsterinfo.currentmove == &insane_move_down || self->monsterinfo.currentmove == &insane_move_getup
        || self->monsterinfo.currentmove == &insane_move_crawl || self->monsterinfo.currentmove == &insane_move_runcrawl
        || self->monsterinfo.currentmove == &insane_move_crawl_pain)
        return 1;
    return 0;
}

void insane_sound(struct edict_s *self)
{
    struct CREATURE *Cre = self->Extra.Creature;
    int r = Iface.rand();

    if (Cre->SpawnFlags & 0x80) {
        if ((r & 0x3fff) <= 8000)
            return;
        if (r & 8)
            Iface.SoundByCreature(sound_insane2, self->Extra.Creature);
        else
            Iface.SoundByCreature(sound_insane9, self->Extra.Creature);
    } else {
        if ((r & 0x3fff) <= 10000)
            return;
        Iface.SoundByCreature(sound_insane9, self->Extra.Creature);
    }
}

static void insane_pain(struct edict_s *self, struct edict_s *other, long kick, int damage)
{
    int l, r;

    if (Iface.level->time < self->pain_debounce_time)
        return;
    self->pain_debounce_time = Iface.level->time + 30;
    /* l and r chose the pain sound in m_insane.c; nothing uses them here */
    r = 1 + (Iface.rand() & 1);
    if (((struct CREATURE *)self->Extra.Creature)->Health < 25)
        l = 25;
    else if (((struct CREATURE *)self->Extra.Creature)->Health < 50)
        l = 50;
    else if (((struct CREATURE *)self->Extra.Creature)->Health < 75)
        l = 75;
    else
        l = 100;
    if ((*Iface.skill)->value == 3)
        return;
    if (self->spawnflags & 0x40) {
        insane_lie(self);
        return;
    }
    if (insane_ondown(self))
        self->monsterinfo.currentmove = &insane_move_crawl_pain;
    else
        self->monsterinfo.currentmove = &insane_move_stand_pain;
}

static void insane_dead(struct edict_s *self)
{
    if (self->spawnflags & 8)
        self->flags |= 1;
    else
        self->movetype = 7;
    self->svflags |= 2;
    self->nextthink = 0;
}

static void insane_die(struct edict_s *self, struct edict_s *inflictor, struct edict_s *attacker, int damage)
{
    if (((struct CREATURE *)self->Extra.Creature)->Health <= self->gib_health) {
        self->deadflag = 2;
        insane_dead(self);
        return;
    }
    if (self->deadflag == 2)
        return;
    self->deadflag = 2;
    self->takedamage = 1;
    if (self->spawnflags & 8)
        insane_dead(self);
    else if (insane_ondown(self))
        self->monsterinfo.currentmove = &insane_move_crawl_death;
    else
        self->monsterinfo.currentmove = &insane_move_stand_death;
}

void SP_misc_insane(struct edict_s *self)
{
    self->movetype = 5;
    self->solid = 2;
    ((struct CREATURE *)self->Extra.Creature)->Health = 100;
    self->gib_health = -50;
    self->mass = 300;
    self->pain = insane_pain;
    self->die = insane_die;
    self->monsterinfo.stand = insane_stand;
    self->monsterinfo.walk = insane_walk;
    self->monsterinfo.run = insane_run;
    self->monsterinfo.dodge = 0;
    self->monsterinfo.attack = 0;
    self->monsterinfo.melee = 0;
    self->monsterinfo.sight = 0;
    self->monsterinfo.aiflags |= 0x100;
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    if (self->spawnflags & 16)
        self->monsterinfo.aiflags |= 1;
    self->monsterinfo.currentmove = &insane_move_stand_normal;
    self->monsterinfo.scale = 10;
    if (self->spawnflags & 8) {
        self->flags |= 0x800;
        Iface.flymonster_start(self);
    } else {
        Iface.walkmonster_start(self);
        self->s.skinnum = Iface.rand() % 3;
    }
    if (self->spawnflags & 0x40) {
        self->monsterinfo.stand = insane_lie;
        self->monsterinfo.walk = insane_lie;
        self->monsterinfo.run = insane_lie;
        self->monsterinfo.currentmove = &insane_move_lie;
        self->s.frame = 282;
    }
}

static void insane_walk(struct edict_s *self)
{
    if (insane_crawler(self))
        self->monsterinfo.currentmove = &insane_move_crawl;
    else if (self->target) {
        if (insane_ondown(self))
            self->monsterinfo.currentmove = &insane_move_getup;
        else
            insane_stand(self);
    } else if (Iface.rand() <= 16384)
        self->monsterinfo.currentmove = &insane_move_walk_normal;
    else
        self->monsterinfo.currentmove = &insane_move_walk_insane;
}

static void insane_run(struct edict_s *self)
{
    if (insane_crawler(self))
        self->monsterinfo.currentmove = &insane_move_runcrawl;
    else if (self->target) {
        if (insane_ondown(self))
            self->monsterinfo.currentmove = &insane_move_getup;
        else
            insane_stand(self);
    } else if (Iface.rand() <= 16384)
        self->monsterinfo.currentmove = &insane_move_run_normal;
    else
        self->monsterinfo.currentmove = &insane_move_run_insane;
}

static void insane_stand(struct edict_s *self)
{
    if (self->spawnflags & 8) {
        if (Iface.rand() < 16384)
            self->monsterinfo.currentmove = &insane_move_uptodown;
        else
            self->monsterinfo.currentmove = &insane_move_jumpdown;
    } else if (Iface.rand() < 16384)
        self->monsterinfo.currentmove = &insane_move_stand_normal;
    else
        self->monsterinfo.currentmove = &insane_move_stand_insane;
}

static void insane_godown(struct edict_s *self)
{
    if (self->spawnflags & 8) {
        if (Iface.rand() < 16384)
            self->monsterinfo.currentmove = &insane_move_uptodown;
        else
            self->monsterinfo.currentmove = &insane_move_jumpdown;
    } else
        self->monsterinfo.currentmove = &insane_move_down1;
}

static void insane_checkdown(struct edict_s *self)
{
    int r = Iface.rand();

    if (self->monsterinfo.currentmove == &insane_move_stand_normal ? r < 11796 : r < 9830)
        insane_godown(self);
}

static void insane_checkup(struct edict_s *self)
{
    int r = Iface.rand();

    if (!(self->spawnflags & 8) && r < 16384)
        self->monsterinfo.currentmove = &insane_move_getup;
}

static void insane_onground(struct edict_s *self)
{
    self->monsterinfo.currentmove = &insane_move_down;
}

static void insane_fist(struct edict_s *self)
{
    insane_sound(self);
}

static void insane_shake(struct edict_s *self)
{
    insane_sound(self);
}

static void insane_moan(struct edict_s *self)
{
    insane_sound(self);
}

static void insane_scream(struct edict_s *self)
{
    insane_sound(self);
}

void insane_lie(struct edict_s *self)
{
    self->monsterinfo.currentmove = &insane_move_lie;
}

static void insane_getup(struct edict_s *self)
{
    self->monsterinfo.currentmove = &insane_move_downtoup;
}

static void insane_tocrawl(struct edict_s *self)
{
    self->monsterinfo.currentmove = &insane_move_crawl;
}
