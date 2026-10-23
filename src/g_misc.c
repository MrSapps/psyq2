#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void path_corner_touch(struct edict_s *self, struct edict_s *other, struct cplane_s *plane, struct csurface_s *surf);
extern struct level_locals_t level;
struct edict_s *G_PickTarget(int targetname);
int getminsvy(struct edict_s *ent);
void linkentity(struct edict_s *ent, int flags);
void UpdateEntity(struct edict_s *ent, int flags);
long vectoyaw(struct VECT *vec);

void path_corner_touch(struct edict_s *self, struct edict_s *other, struct cplane_s *plane, struct csurface_s *surf)
{
    struct VECT v;
    struct edict_s *next;

    if (other->movetarget != self)
        return;
    if (other->enemy)
        return;
    if (self->target)
        next = G_PickTarget(self->target);
    else
        next = 0;
    if (next && (next->spawnflags & 1)) {
        v.vx = next->sorigin.vx;
        v.vy = next->sorigin.vy;
        v.vz = next->sorigin.vz;
        v.vy -= getminsvy(next);
        v.vy += getminsvy(other);
        other->sorigin.vx = v.vx;
        other->sorigin.vy = v.vy;
        other->sorigin.vz = v.vz;
        next = G_PickTarget(next->target);
        linkentity(other, 1);
    }
    other->goalentity = other->movetarget = next;
    if (self->wait) {
        other->monsterinfo.pausetime = level.time + self->wait;
        other->monsterinfo.stand(other);
        return;
    }
    if (!other->movetarget) {
        other->monsterinfo.pausetime = level.time + 1000000000;
        other->monsterinfo.stand(other);
    } else {
        UpdateEntity(other, 1);
        v.vx = other->goalentity->sorigin.vx - other->sorigin.vx;
        v.vy = other->goalentity->sorigin.vy - other->sorigin.vy;
        v.vz = other->goalentity->sorigin.vz - other->sorigin.vz;
        other->ideal_yaw = vectoyaw(&v);
    }
}
