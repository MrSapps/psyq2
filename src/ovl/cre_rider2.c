/* Boss32 bottom (Makron's legs) */
#include "ovl.h"

void SP_Boss32Bottom(struct edict_s *self);
void PreInit_Boss32Bottom(struct GLOBMODHDR *Hdr);
static struct NEWANIMREMAP AnimRemap[];

struct QCREINTERFACE Iface OVL_HEADER = { SP_Boss32Bottom, 0, PreInit_Boss32Bottom, AnimRemap };

static void Boss32Bottom_die(struct edict_s *self);
static void Boss32Bottom_pain(struct edict_s *self);
static void Boss32Bottom_think(struct edict_s *self);
void Boss32Bottom_init(struct edict_s *self);

static void Boss32Bottom_die(struct edict_s *self)
{
}

static void Boss32Bottom_pain(struct edict_s *self)
{
}

static void Boss32Bottom_think(struct edict_s *self)
{
    if (++self->s.frame < 345)
        self->nextthink = Iface.level->time + 1;
    else
        self->nextthink = 0;
}

void Boss32Bottom_init(struct edict_s *self)
{
    self->pain = Boss32Bottom_pain;
    self->die = Boss32Bottom_die;
    self->movetype = 0;
    self->solid = 0;
    if (((struct CREATURE *)self->Extra.Creature)->Health == 5000)
        self->s.frame = 344;
    else
        self->s.frame = 251;
    ((struct CREATURE *)self->Extra.Creature)->Health = 5000;
    self->think = Boss32Bottom_think;
    self->nextthink = Iface.level->time + 2;
}

void SP_Boss32Bottom(struct edict_s *self)
{
    self->movetype = 5;
    self->solid = 2;
    ((struct CREATURE *)self->Extra.Creature)->Ang.vy = ((struct CREATURE *)self->Extra.Creature)->IdealYaw =
        self->sangles.vz = Iface.anglemod(Iface.rand());
    Boss32Bottom_init(self);
    Iface.linkentity(self, 1);
    Iface.linkentity(self, 0x81);
}

static struct NEWANIMREMAP AnimRemap[] = {
    { 251, 334, {"Deatha"}, 0, 0 },
    { 335, 345, {"Deathb"}, 0, 0 },
    { -1, -1, {"\377"}, 0, 0 },
};

static short ShadowVerts[] = { -1, 0x652e, 0, 0, 0, 0, 0, 0 };

void PreInit_Boss32Bottom(struct GLOBMODHDR *Hdr)
{
    if (Hdr) {
        Iface.PrintF("Pre Init Boss32 bottom (Makrons legs)\n");
        if (Iface.StructSize != sizeof(struct QCREINTERFACE))
            for (;;)
                Iface.PrintF("Invalid Creature Interface size for %s\n", "Boss32");
        if (Iface.Version != 1)
            for (;;)
                Iface.PrintF("Invalid Creature Interface version for %s\n", "Boss32");
        Iface.PrintF("Validated Creature Interface for %s\n", "Boss32");
        Hdr->ShadowVerts = ShadowVerts;
    } else
        Iface.PrintF("Clear Boss32 bottom (Makrons legs)\n");
}
