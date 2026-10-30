/* Dead commander's body (monster_commander_body, g_misc.c) */
#include "ovl.h"

void PreInit_CommanderBody(struct GLOBMODHDR *Hdr);
void SP_monster_commander_body(struct edict_s *self);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_monster_commander_body, 0, PreInit_CommanderBody, AnimRemap };

void commander_body_think(struct edict_s *self);
static void commander_body_use(struct edict_s *self);
static void commander_body_drop(struct edict_s *self);
static void commander_body_pain(struct edict_s *self);

static int sound_idle;
static int sound_pain;
static int sound_death;
static int sound_sight;
static int sound_udeath;
static int Unused[2]; /* space the original has that no code uses */

void PreInit_CommanderBody(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init CommanderBody\r\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "CommanderBody");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "CommanderBody");
        Iface.PrintF("Validated Creature Interface for %s\n", "CommanderBody");
        sound_idle = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_idle1"});
        sound_pain = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_pain"});
        sound_death = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_death"});
        sound_sight = Iface.GetSoundNum((struct IDENTITYNAME){"tnk_sight1"});
        sound_udeath = Iface.GetSoundNum((struct IDENTITYNAME){"msc_udeath"});
    } else
        Iface.PrintF("Clear Init CommanderBody\r\n");
}

/* The original's .rdata alignment padding holds a stray byte. */
__asm__(".rdata\n\t.byte 0x20\n\t.text");

void SP_monster_commander_body(struct edict_s *self)
{
    self->use = commander_body_use;
    self->movetype = 0;
    self->solid = 2;
    self->takedamage = 1;
    self->flags = 16;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    self->pain = commander_body_pain;
    self->think = commander_body_drop;
    self->takedamage = 0;
    self->nextthink = Iface.level->time + 5;
}

void commander_body_think(struct edict_s *self)
{
    if (++self->s.frame < 23)
        self->nextthink = Iface.level->time + 1;
    else
        self->nextthink = 0;
    Iface.SoundByCreature(sound_idle, self->Extra.Creature);
}

static void commander_body_use(struct edict_s *self)
{
    Iface.SoundByCreature(sound_sight, self->Extra.Creature);
    self->think = commander_body_think;
    self->nextthink = Iface.level->time + 1;
}

static void commander_body_drop(struct edict_s *self)
{
    self->movetype = 7;
    self->sorigin.vy -= 24;
}

static void commander_body_pain(struct edict_s *self)
{
}

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 23, {"Activate"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};
