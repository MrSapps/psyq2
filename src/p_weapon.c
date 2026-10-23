#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void PlayerNoise(struct edict_s *who, struct VECT *where, int type);

extern int LevelIsMultiPlayer;
extern struct cvar_s *deathmatch;
extern struct level_locals_t level;
struct edict_s *G_Spawn_NoCre(void);
void linkentity(struct edict_s *ent, int flags);

void PlayerNoise(struct edict_s *who, struct VECT *where, int type)
{
    struct edict_s *noise;

    if (!who || LevelIsMultiPlayer)
        return;
    if (type == 1) {
        if (((struct CREATURE *)who->Extra.Creature)->Player->Extras.Silencer_Shots) {
            ((struct CREATURE *)who->Extra.Creature)->Player->Extras.Silencer_Shots--;
            return;
        }
    }
    if (deathmatch->value)
        return;
    if (who->flags & 0x20)
        return;
    if (!who->mynoise) {
        noise = G_Spawn_NoCre();
        noise->classname = 0xc9;
        noise->owner = who;
        noise->svflags = 1;
        who->mynoise = noise;
        noise = G_Spawn_NoCre();
        noise->classname = 0xc9;
        noise->owner = who;
        noise->svflags = 1;
        who->mynoise2 = noise;
    }
    if (type == 0 || type == 1) {
        noise = who->mynoise;
        level.sound_entity = noise;
        level.sound_entity_framenum = level.framenum;
    } else {
        noise = who->mynoise2;
        level.sound2_entity = noise;
        level.sound2_entity_framenum = level.framenum;
    }
    noise->sorigin.vx = where->vx;
    noise->sorigin.vy = where->vy;
    noise->sorigin.vz = where->vz;
    noise->teleport_time = level.time;
    linkentity(noise, 0x81);
}
