#include "types.h"

extern struct level_locals_t level;
extern struct edict_s *EdictTList;
extern int MaxWindows;

void M_walkmove(struct edict_s *ent, long yaw, long dist);
long VectorLength2(struct VECT *v);
long vectoyaw(struct VECT *vec);
void AttackFinished(struct edict_s *self, long time);

void AI_SetSightClient(void)
{
    struct edict_s *ent;
    int start;
    int check;

    if (level.sight_client == 0)
        start = 1;
    else
        start = level.sight_client - EdictTList;

    check = start;
    while (1) {
        check++;
        if (check > MaxWindows)
            check = 1;
        ent = &EdictTList[check];
        if (ent->inuse && ((struct CREATURE *)ent->Extra.Creature)->Health > 0 && !(ent->flags & 0x20)) {
            level.sight_client = ent;
            return;
        }
        if (check == start) {
            level.sight_client = 0;
            return;
        }
    }
}

void ai_move(struct edict_s *self, long dist)
{
    M_walkmove(self, self->sangles.vz, dist);
}

int range(struct edict_s *self, struct edict_s *other)
{
    struct VECT v;
    long len;

    v.vx = self->sorigin.vx - other->sorigin.vx;
    v.vy = self->sorigin.vy - other->sorigin.vy;
    v.vz = self->sorigin.vz - other->sorigin.vz;
    len = VectorLength2(&v);
    if (self->classname == 0x44) {
        if (len <= 0x3f803f)
            return 0;
    } else {
        if (len <= 0xfe00f)
            return 0;
    }
    if (len <= 0x22550ff)
        return 1;
    if (len <= 0x89543ff)
        return 2;
    return 3;
}

void HuntTarget(struct edict_s *self)
{
    struct VECT vec;

    self->goalentity = self->enemy;
    if (self->monsterinfo.aiflags & 1)
        self->monsterinfo.stand(self);
    else
        self->monsterinfo.run(self);
    vec.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    vec.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    vec.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    self->ideal_yaw = vectoyaw(&vec);
    if (!(self->monsterinfo.aiflags & 1))
        AttackFinished(self, 10);
}
