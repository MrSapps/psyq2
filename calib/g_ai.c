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

void AngleVectors(struct SVECT *angles, struct VECT *forward, struct VECT *right, struct VECT *up);
long VectorNormalize(struct VECT *v);
int visible(struct edict_s *self, struct edict_s *other);
int M_ChangeYaw(struct edict_s *ent);
void M_MoveToGoal(struct edict_s *ent, long dist);
int FindTarget(struct edict_s *self);
int rand(void);

int infront(struct edict_s *self, struct edict_s *other)
{
    struct VECT vec;
    long dot;
    struct VECT forward;

    AngleVectors(&self->sangles, &forward, 0, 0);
    vec.vx = other->sorigin.vx - self->sorigin.vx;
    vec.vy = other->sorigin.vy - self->sorigin.vy;
    vec.vz = other->sorigin.vz - self->sorigin.vz;
    VectorNormalize(&vec);
    dot = vec.vx * forward.vx / 4096 + vec.vy * forward.vy / 4096 + vec.vz * forward.vz / 4096;
    if (dot > 1229)
        return 1;
    return 0;
}

void ai_charge(struct edict_s *self, long dist)
{
    struct VECT v;

    if (self->monsterinfo.blindfire && visible(self, self->enemy))
    {
        self->pos1.vx = self->enemy->sorigin.vx;
        self->pos1.vy = self->enemy->sorigin.vy;
        self->pos1.vz = self->enemy->sorigin.vz;
    }
    if (!(self->monsterinfo.aiflags & 0x10000)) {
        v.vx = self->enemy->sorigin.vx - self->sorigin.vx;
        v.vy = self->enemy->sorigin.vy - self->sorigin.vy;
        v.vz = self->enemy->sorigin.vz - self->sorigin.vz;
        self->ideal_yaw = vectoyaw(&v);
    }
    M_ChangeYaw(self);
    if (dist)
        M_walkmove(self, self->sangles.vz, dist);
}

void ai_walk(struct edict_s *self, long dist)
{
    M_MoveToGoal(self, dist);
    if (FindTarget(self))
        return;
    if (self->monsterinfo.search && level.time > self->monsterinfo.idle_time) {
        if (self->monsterinfo.idle_time) {
            self->monsterinfo.search(self);
            self->monsterinfo.idle_time = level.time + 150 + (rand() & 0x7fff) * 150 / 32767;
        } else
            self->monsterinfo.idle_time = level.time + (rand() & 0x7fff) * 150 / 32767;
    }
}
