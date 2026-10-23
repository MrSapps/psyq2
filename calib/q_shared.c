#include "types.h"

int getminsvx(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vx;
}

int getminsvy(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vy;
}

int getminsvz(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vz;
}

int getmaxsvx(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vx;
}

int getmaxsvy(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vy;
}

int getmaxsvz(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vz;
}
