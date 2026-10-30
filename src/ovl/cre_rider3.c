/* Boss32 top (Makron's torso) */
#include "ovl.h"

void SP_Boss32Top(struct edict_s *self);
void PreInit_Boss32Top(struct GLOBMODHDR *Hdr);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_Boss32Top, 0, PreInit_Boss32Top, AnimRemap };

static void Boss32Top_die(struct edict_s *self);
static void Boss32Top_pain(struct edict_s *self);
static void Boss32Top_think(struct edict_s *self);
void Boss32Top_init(struct edict_s *self);

static void Boss32Top_die(struct edict_s *self)
{
}

static void Boss32Top_pain(struct edict_s *self)
{
}

static void Boss32Top_think(struct edict_s *self)
{
    if (++self->s.frame >= 20)
        self->s.frame = 0;
    self->nextthink = Iface.level->time + 1;
}

void Boss32Top_init(struct edict_s *self)
{
    self->pain = Boss32Top_pain;
    self->die = Boss32Top_die;
    self->s.frame = 0;
    self->think = Boss32Top_think;
    self->movetype = 0;
    self->solid = 0;
    self->nextthink = Iface.level->time + 2;
}

void SP_Boss32Top(struct edict_s *self)
{
    self->movetype = 5;
    self->solid = 2;
    ((struct CREATURE *)self->Extra.Creature)->Ang.vy = ((struct CREATURE *)self->Extra.Creature)->IdealYaw =
        self->sangles.vz = Iface.anglemod(Iface.rand());
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
    Boss32Top_init(self);
}

static struct NEWANIMREMAP AnimRemap[] = {
    { 0, 19, {"Death2"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 256, 0, 0, 0, 0, 0, 0 };

void PreInit_Boss32Top(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Boss32 top (Makrons torso)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Boss32");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Boss32");
        Iface.PrintF("Validated Creature Interface for %s\n", "Boss32");
        Hdr->ShadowVerts = ShadowVerts;
    } else
        Iface.PrintF("Clear Boss32 top (Makrons torso)\n");
}
