/* Rider Stand (Makron standing on Boss1, the Jorg) */
#include "ovl.h"

#define CRE(e) ((struct CREATURE *)(e)->Extra.Creature)

/* gcc emits the small functions at the end in this order. fade and nopain are
   global: as statics nothing emitted before them refers to them, so they would
   come out later than the original has them. */
void PreInit_RiderStand(struct GLOBMODHDR *Hdr);
void RiderStand_fade(struct edict_s *self);
void SP_RiderStand(struct edict_s *self);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_RiderStand, 0, PreInit_RiderStand, AnimRemap };

static void RiderStand_think(struct edict_s *self);
void RiderStand_init(struct edict_s *self);
static void RiderStand_use(struct edict_s *self);
void RiderStand_nopain(struct edict_s *self);
static void RiderStand_pain(struct edict_s *self);
static void RiderStand_die(struct edict_s *self);

static struct NEWANIMREMAP AnimRemap[] = {
    { 414, 473, {"Stand"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0 };

static int sound_unused0;
static int sound_tele;
static int sound_unused1[2];
static int sound_laf;

void PreInit_RiderStand(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Rider Stand (Makron on Boss1)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "RiderStand");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "RiderStand");
        Iface.PrintF("Validated Creature Interface for %s\n", "RiderStand");
        Hdr->ShadowVerts = ShadowVerts;
        Iface.AlterSampleAttrs(sound_tele = Iface.GetSoundNum((struct IDENTITYNAME){"msc_tele1"}), 0, 127, -1);
        Iface.AlterSampleAttrs(sound_laf = Iface.GetSoundNum((struct IDENTITYNAME){"mak_laf2"}), 0, 127, -1);
    } else
        Iface.PrintF("Clear Rider Stand (Makron on Boss1)\n");
}

/* The original's .rdata alignment padding holds a stray byte. */
__asm__(".rdata\n\t.byte 0x00, 0x00, 0x82\n\t.text");

/* not referenced by the code */
static struct mframe_t RiderStand_frames_stand[] = {
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
    {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
};
static struct mmove_t RiderStand_move_stand = { 414, 473, RiderStand_frames_stand, 0 };

static void RiderStand_think(struct edict_s *self)
{
    if (++self->s.frame >= 474)
        self->s.frame = 414;
    self->nextthink = Iface.level->time + 1;
}

void RiderStand_init(struct edict_s *self)
{
    self->s.frame = 414;
    self->think = RiderStand_think;
    self->movetype = 0;
    self->solid = 0;
    self->nextthink = Iface.level->time + 2;
}

static void RiderStand_die(struct edict_s *self)
{
    CRE(self)->Health = self->gib_health;
    self->nextthink = 0;
    self->takedamage = 0;
    self->deadflag = 2;
    self->movetype = 7;
    self->svflags |= 2;
}

static void RiderStand_use(struct edict_s *self)
{
    self->think = RiderStand_fade;
    self->nextthink = Iface.level->time + 1;
    Iface.SoundByCreature(sound_laf, Iface.Creatures);
}

void RiderStand_nopain(struct edict_s *self)
{
}

static void RiderStand_pain(struct edict_s *self)
{
    self->think = RiderStand_fade;
    self->nextthink = Iface.level->time + 1;
    Iface.SoundByCreature(sound_laf, Iface.Creatures);
    self->pain = RiderStand_nopain;
}

void RiderStand_fade(struct edict_s *self)
{
    CRE(self)->ModelOpaque -= 512;
    if (CRE(self)->ModelOpaque == 2048)
        Iface.SoundByCreature(sound_tele, Iface.Creatures);
    if (CRE(self)->ModelOpaque == 0)
        RiderStand_die(self);
    else
        RiderStand_think(self);
}

void SP_RiderStand(struct edict_s *self)
{
    self->die = RiderStand_die;
    self->pain = RiderStand_pain;
    self->use = RiderStand_use;
    self->movetype = 0;
    self->solid = 2;
    self->takedamage = 1;
    Iface.linkentity(self, 5);
    Iface.linkentity(self, 0x85);
    RiderStand_init(self);
}
