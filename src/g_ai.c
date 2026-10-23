#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void AI_SetSightClient(void);
void ai_move(struct edict_s *self, long dist);
void ai_walk(struct edict_s *self, long dist);
void ai_charge(struct edict_s *self, long dist);
int range(struct edict_s *self, struct edict_s *other);
void HuntTarget(struct edict_s *self);
int rand(void); int infront(struct edict_s *self, struct edict_s *other);
void ai_stand(struct edict_s *self, long dist);
void FoundTarget(struct edict_s *self);
int FindTarget(struct edict_s *self);
int M_CheckAttack(struct edict_s *self);
int ai_checkattack(struct edict_s *self, long dist);
void ai_run(struct edict_s *self, long dist);

extern struct level_locals_t level;
extern struct edict_s *EdictTList;
extern int MaxWindows;

int M_walkmove(struct edict_s *ent, long yaw, long dist);
long VectorLength2(struct VECT *v);
long vectoyaw(struct VECT *vec);
void AttackFinished(struct edict_s *self, long time);

void AngleVectors(struct SVECT *angles, struct VECT *forward, struct VECT *right, struct VECT *up);
long VectorNormalize(struct VECT *v);
int visible(struct edict_s *self, struct edict_s *other);
int M_ChangeYaw(struct edict_s *ent);
void M_MoveToGoal(struct edict_s *ent, long dist);
int FindTarget(struct edict_s *self);

struct edict_s *G_PickTarget(int targetname);

int ai_checkattack(struct edict_s *self, long dist);

int enemy_range;
extern struct trace_t *traceptr;
extern struct cvar_s *skill;
void gitrace(struct VECT *start, struct VECT *mins, struct VECT *maxs, struct VECT *end, struct edict_s *passent, int contentmask, int start_space);

int enemy_vis;
int enemy_infront;
long enemy_yaw;
long anglemod(long a);

static int FacingIdeal(struct edict_s *self)
{
    long delta;

    delta = anglemod(self->sangles.vz - self->ideal_yaw);
    if (delta > 0x200 && delta < 0xE00)
        return 0;
    return 1;
}

static void ai_run_melee(struct edict_s *self)
{
    self->ideal_yaw = enemy_yaw;
    M_ChangeYaw(self);
    if (FacingIdeal(self)) {
        self->monsterinfo.melee(self);
        self->monsterinfo.attack_state = 1;
    }
}

static void ai_run_missile(struct edict_s *self)
{
    self->ideal_yaw = enemy_yaw;
    M_ChangeYaw(self);
    if (FacingIdeal(self)) {
        self->monsterinfo.attack(self);
        self->monsterinfo.attack_state = 1;
    }
}

extern long *DummyMemory;
long VectorLength(struct VECT *v);
void getminsmaxs(struct edict_s *ent, struct VECT *mins, struct VECT *maxs);
void G_ProjectSourceNM(struct VECT *point, struct SVECT *distance, struct VECT *forward, struct VECT *right, struct VECT *result);
void G_InitEdict(struct edict_s *e);
struct edict_s *PlayerTrail_PickFirst(struct edict_s *self);
struct edict_s *PlayerTrail_PickNext(struct edict_s *self);

static void ai_run_slide(struct edict_s *self, long distance)
{
    long ofs;

    self->ideal_yaw = enemy_yaw;
    M_ChangeYaw(self);
    if (self->monsterinfo.lefty)
        ofs = 0x400;
    else
        ofs = -0x400;
    if (M_walkmove(self, self->ideal_yaw + ofs, distance))
        return;
    self->monsterinfo.lefty = 1 - self->monsterinfo.lefty;
    M_walkmove(self, self->ideal_yaw - ofs, distance);
}

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

void ai_stand(struct edict_s *self, long dist)
{
    struct VECT v;
    int retval;

    if (dist)
        M_walkmove(self, self->sangles.vz, dist);
    if (self->monsterinfo.aiflags & 1) {
        if (self->enemy) {
            v.vx = self->enemy->sorigin.vx - self->sorigin.vx;
            v.vy = self->enemy->sorigin.vy - self->sorigin.vy;
            v.vz = self->enemy->sorigin.vz - self->sorigin.vz;
            self->ideal_yaw = vectoyaw(&v);
            if (self->sangles.vz != self->ideal_yaw && (self->monsterinfo.aiflags & 2)) {
                self->monsterinfo.aiflags &= ~3;
                self->monsterinfo.run(self);
            }
            if (!(self->monsterinfo.aiflags & 0x10000))
                M_ChangeYaw(self);
            retval = ai_checkattack(self, 0);
            if (self->enemy && self->enemy->inuse && visible(self, self->enemy)) {
                self->monsterinfo.aiflags &= ~8;
                if (self->enemy->client) {
                    self->monsterinfo.last_sighting.vx = self->enemy->client->corrected_sorigin.vx;
                    self->monsterinfo.last_sighting.vy = self->enemy->client->corrected_sorigin.vy;
                    self->monsterinfo.last_sighting.vz = self->enemy->client->corrected_sorigin.vz;
                } else {
                    self->monsterinfo.last_sighting.vx = self->enemy->sorigin.vx;
                    self->monsterinfo.last_sighting.vy = self->enemy->sorigin.vy;
                    self->monsterinfo.last_sighting.vz = self->enemy->sorigin.vz;
                }
                if (self->monsterinfo.blindfire) {
                    self->pos1.vx = self->enemy->sorigin.vx;
                    self->pos1.vy = self->enemy->sorigin.vy;
                    self->pos1.vz = self->enemy->sorigin.vz;
                }
                self->monsterinfo.trail_time = level.time;
                self->wait = 0;
            } else if (!retval) {
                FindTarget(self);
                return;
            }
            return;
        } else {
            FindTarget(self);
            return;
        }
    }
    if (FindTarget(self))
        return;
    if (level.time > self->monsterinfo.pausetime) {
        self->monsterinfo.walk(self);
        return;
    }
    if (!(self->spawnflags & 1) && self->monsterinfo.idle && level.time > self->monsterinfo.idle_time) {
        if (self->monsterinfo.idle_time) {
            self->monsterinfo.idle(self);
            self->monsterinfo.idle_time = level.time + 150 + (rand() & 0x7FFF) * 150 / 32767;
        } else {
            self->monsterinfo.idle_time = level.time + (rand() & 0x7FFF) * 150 / 32767;
        }
    }
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

void FoundTarget(struct edict_s *self)
{
    if (self->enemy->client) {
        level.sight_entity = self;
        level.sight_entity_framenum = level.framenum;
    }
    self->show_hostile = level.time + 10;
    if (self->enemy->client) {
        self->monsterinfo.last_sighting.vx = self->enemy->client->corrected_sorigin.vx;
        self->monsterinfo.last_sighting.vy = self->enemy->client->corrected_sorigin.vy;
        self->monsterinfo.last_sighting.vz = self->enemy->client->corrected_sorigin.vz;
    } else {
        self->monsterinfo.last_sighting.vx = self->enemy->sorigin.vx;
        self->monsterinfo.last_sighting.vy = self->enemy->sorigin.vy;
        self->monsterinfo.last_sighting.vz = self->enemy->sorigin.vz;
    }
    self->monsterinfo.trail_time = level.time;
    if (self->monsterinfo.blindfire) {
        self->pos1.vx = self->enemy->sorigin.vx;
        self->pos1.vy = self->enemy->sorigin.vy;
        self->pos1.vz = self->enemy->sorigin.vz;
        self->wait = 0;
    }
    if (self->combattarget <= 0) {
        HuntTarget(self);
        return;
    }
    self->goalentity = self->movetarget = G_PickTarget(self->combattarget);
    if (!self->movetarget) {
        self->goalentity = self->movetarget = self->enemy;
        HuntTarget(self);
        return;
    }
    self->combattarget = 0;
    self->monsterinfo.aiflags |= 0x1000;
    self->movetarget->targetname = 0;
    self->monsterinfo.pausetime = 0;
    self->monsterinfo.run(self);
}

int FindTarget(struct edict_s *self)
{
    struct edict_s *client;
    int heardit;
    int r;

    if (self->monsterinfo.aiflags & 0x100) {
        if (self->goalentity && self->goalentity->inuse && self->goalentity->classname) {
            if (self->goalentity->classname == 0xCC)
                return 0;
        }
        return 0;
    }
    if (self->monsterinfo.aiflags & 0x1000)
        return 0;
    heardit = 0;
    if (level.sight_entity_framenum >= level.framenum - 1 && !(self->spawnflags & 1)) {
        client = level.sight_entity;
        if (client->enemy == self->enemy)
            return 0;
    } else if (level.sound_entity_framenum >= level.framenum - 1) {
        client = level.sound_entity;
        heardit = 1;
    } else if (!self->enemy && level.sound2_entity_framenum >= level.framenum - 1 && !(self->spawnflags & 1)) {
        client = level.sound2_entity;
        heardit = 1;
    } else {
        client = level.sight_client;
        if (!client)
            return 0;
    }
    if (!client->inuse)
        return 0;
    if (client == self->enemy)
        return 1;
    if (client->client) {
        if (client->flags & 0x20)
            return 0;
    } else if (client->svflags & 4) {
        if (!client->enemy)
            return 0;
        if (client->enemy->flags & 0x20)
            return 0;
    } else if (heardit) {
        if (client->owner->flags & 0x20)
            return 0;
    } else
        return 0;
    if (!heardit) {
        r = range(self, client);
        if (r == 3)
            return 0;
        if (!visible(self, client))
            return 0;
        if (r == 1) {
            if (client->show_hostile < level.time && !infront(self, client))
                return 0;
        } else if (r == 2) {
            if (!infront(self, client))
                return 0;
        }
        self->enemy = client;
        if (self->enemy->classname != 0xC9) {
            self->monsterinfo.aiflags &= ~4;
            if (!self->enemy->client) {
                self->enemy = self->enemy->enemy;
                if (!self->enemy->client) {
                    self->enemy = 0;
                    return 0;
                }
            }
        }
    } else {
        struct VECT temp;

        if (self->spawnflags & 1) {
            if (!visible(self, client))
                return 0;
        }
        temp.vx = client->sorigin.vx - self->sorigin.vx;
        temp.vy = client->sorigin.vy - self->sorigin.vy;
        temp.vz = client->sorigin.vz - self->sorigin.vz;
        if (VectorLength2(&temp) > 0x8954400)
            return 0;
        self->ideal_yaw = vectoyaw(&temp);
        M_ChangeYaw(self);
        self->monsterinfo.aiflags |= 4;
        self->enemy = client;
    }
    FoundTarget(self);
    if (!(self->monsterinfo.aiflags & 4) && self->monsterinfo.sight)
        self->monsterinfo.sight(self, self->enemy);
    return 1;
}

int M_CheckAttack(struct edict_s *self)
{
    struct VECT spot1;
    struct VECT spot2;
    long chance;

    if (((struct CREATURE *)self->enemy->Extra.Creature)->Health > 0) {
        spot1.vx = self->sorigin.vx;
        spot1.vy = self->sorigin.vy;
        spot1.vz = self->sorigin.vz;
        spot1.vy += self->viewheight;
        spot2.vx = self->enemy->sorigin.vx;
        spot2.vy = self->enemy->sorigin.vy;
        spot2.vz = self->enemy->sorigin.vz;
        spot2.vy += self->enemy->viewheight;
        gitrace(&spot1, 0, 0, &spot2, self, 0x200001B, ((struct CREATURE *)self->Extra.Creature)->Box.MoveSpace);
        if (traceptr->ent != self->enemy) {
            if (traceptr->ent->svflags & 4)
                return 0;
            if (visible(self, self->enemy))
                return 0;
            if (!self->monsterinfo.blindfire)
                return 0;
            if (self->wait > 200)
                return 0;
            if (level.time < self->monsterinfo.attack_finished)
                return 0;
            if (level.time < self->monsterinfo.trail_time + self->wait)
                return 0;
            gitrace(&spot1, 0, 0, &self->pos1, self, 0x2000000, ((struct CREATURE *)self->Extra.Creature)->Box.MoveSpace);
            if (traceptr->startsolid || (traceptr->fraction < 0x1000 && traceptr->ent != self->enemy))
                return 0;
            self->monsterinfo.attack_state = 5;
            return 1;
        }
    }
    if (enemy_range == 0) {
        if (skill->value == 0 && (rand() & 3))
            return 0;
        if (self->monsterinfo.melee)
            self->monsterinfo.attack_state = 3;
        else
            self->monsterinfo.attack_state = 4;
        return 1;
    }
    if (!self->monsterinfo.attack)
        return 0;
    if (level.time < self->monsterinfo.attack_finished)
        return 0;
    if (enemy_range == 3)
        return 0;
    if (self->monsterinfo.aiflags & 1)
        chance = 0x666;
    else if (enemy_range == 0)
        chance = 0x333;
    else if (enemy_range == 1)
        chance = 0x19A;
    else if (enemy_range == 2)
        chance = 0x52;
    else
        return 0;
    if (skill->value == 0)
        chance /= 2;
    else if (skill->value >= 2)
        chance *= 2;
    if ((rand() & 0xFFF) < chance) {
        self->monsterinfo.attack_state = 4;
        self->monsterinfo.attack_finished = level.time + (rand() & 0x7FFF) * 20 / 32767;
        return 1;
    }
    if (self->flags & 1) {
        if ((rand() & 0x7FFF) < 0x2666)
            self->monsterinfo.attack_state = 2;
        else
            self->monsterinfo.attack_state = 1;
    }
    return 0;
}

int ai_checkattack(struct edict_s *self, long dist)
{
    struct VECT temp;
    int hesDeadJim;

    if (self->goalentity) {
        if (self->monsterinfo.aiflags & 0x1000)
            return 0;
        if (self->monsterinfo.aiflags & 4) {
            if (level.time - self->enemy->teleport_time > 50) {
                if (self->goalentity == self->enemy) {
                    if (self->movetarget)
                        self->goalentity = self->movetarget;
                    else
                        self->goalentity = 0;
                }
                self->monsterinfo.aiflags &= ~4;
                if (self->monsterinfo.aiflags & 2)
                    self->monsterinfo.aiflags &= ~3;
            } else {
                self->show_hostile = level.time + 10;
                return 0;
            }
        }
    }
    enemy_vis = 0;
    hesDeadJim = 0;
    if (!self->enemy || !self->enemy->inuse) {
        hesDeadJim = 1;
    } else if (self->monsterinfo.aiflags & 0x2000) {
        if (((struct CREATURE *)self->enemy->Extra.Creature)->Health > 0) {
            hesDeadJim = 1;
            self->monsterinfo.aiflags &= ~0x2000;
        }
    } else {
        if (self->monsterinfo.aiflags & 0x200) {
            if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= -80)
                hesDeadJim = 1;
        } else {
            if (((struct CREATURE *)self->enemy->Extra.Creature)->Health <= 0)
                hesDeadJim = 1;
        }
    }
    if (hesDeadJim) {
        self->enemy = 0;
        if (self->oldenemy && ((struct CREATURE *)self->oldenemy->Extra.Creature)->Health > 0) {
            self->enemy = self->oldenemy;
            self->oldenemy = 0;
            HuntTarget(self);
        } else {
            if (self->movetarget) {
                self->goalentity = self->movetarget;
                self->monsterinfo.walk(self);
            } else {
                self->monsterinfo.pausetime = level.time + 1000000000;
                self->monsterinfo.stand(self);
            }
            return 1;
        }
    }
    self->show_hostile = level.time + 10;
    enemy_vis = visible(self, self->enemy);
    if (enemy_vis) {
        self->monsterinfo.search_time = level.time + 50;
        if (self->enemy->client) {
            self->monsterinfo.last_sighting.vx = self->enemy->client->corrected_sorigin.vx;
            self->monsterinfo.last_sighting.vy = self->enemy->client->corrected_sorigin.vy;
            self->monsterinfo.last_sighting.vz = self->enemy->client->corrected_sorigin.vz;
        } else {
            self->monsterinfo.last_sighting.vx = self->enemy->sorigin.vx;
            self->monsterinfo.last_sighting.vy = self->enemy->sorigin.vy;
            self->monsterinfo.last_sighting.vz = self->enemy->sorigin.vz;
        }
        self->monsterinfo.aiflags &= ~8;
        self->monsterinfo.trail_time = level.time;
        if (self->monsterinfo.blindfire) {
            self->pos1.vx = self->enemy->sorigin.vx;
            self->pos1.vy = self->enemy->sorigin.vy;
            self->pos1.vz = self->enemy->sorigin.vz;
            self->wait = 0;
        }
    }
    enemy_infront = infront(self, self->enemy);
    enemy_range = range(self, self->enemy);
    temp.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    temp.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    temp.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    enemy_yaw = vectoyaw(&temp);
    if (self->monsterinfo.attack_state == 4) {
        ai_run_missile(self);
        return 1;
    }
    if (self->monsterinfo.attack_state == 3) {
        ai_run_melee(self);
        return 1;
    }
    if (!enemy_vis)
        return 0;
    return self->monsterinfo.checkattack(self);
}

void ai_run(struct edict_s *self, long dist)
{
    struct VECT v;
    struct SVECT sv;
    struct edict_s *tempgoal;
    struct edict_s *save;
    int new;
    struct edict_s *marker;
    long d1;
    long d2;
    struct VECT v_forward;
    struct VECT v_right;
    long left;
    long center;
    long right;
    struct VECT left_target;
    struct VECT right_target;
    struct VECT mins;
    struct VECT maxs;

    tempgoal = (struct edict_s *)DummyMemory;
    if (self->monsterinfo.aiflags & 0x1000) {
        M_MoveToGoal(self, dist);
        return;
    }
    if (self->monsterinfo.aiflags & 4) {
        v.vx = self->sorigin.vx - self->enemy->sorigin.vx;
        v.vy = self->sorigin.vy - self->enemy->sorigin.vy;
        v.vz = self->sorigin.vz - self->enemy->sorigin.vz;
        if (VectorLength2(&v) < 0x90000) {
            self->monsterinfo.aiflags |= 3;
            self->monsterinfo.stand(self);
            return;
        }
        M_MoveToGoal(self, dist);
        if (!FindTarget(self))
            return;
    }
    if (ai_checkattack(self, dist))
        return;
    if (self->monsterinfo.attack_state == 2) {
        ai_run_slide(self, dist);
        return;
    }
    if (enemy_vis) {
        M_MoveToGoal(self, dist);
        self->monsterinfo.aiflags &= ~8;
        if (self->enemy->client) {
            self->monsterinfo.last_sighting.vx = self->enemy->client->corrected_sorigin.vx;
            self->monsterinfo.last_sighting.vy = self->enemy->client->corrected_sorigin.vy;
            self->monsterinfo.last_sighting.vz = self->enemy->client->corrected_sorigin.vz;
        } else {
            self->monsterinfo.last_sighting.vx = self->enemy->sorigin.vx;
            self->monsterinfo.last_sighting.vy = self->enemy->sorigin.vy;
            self->monsterinfo.last_sighting.vz = self->enemy->sorigin.vz;
        }
        self->monsterinfo.trail_time = level.time;
        if (self->monsterinfo.blindfire) {
            self->pos1.vx = self->enemy->sorigin.vx;
            self->pos1.vy = self->enemy->sorigin.vy;
            self->pos1.vz = self->enemy->sorigin.vz;
            self->wait = 0;
        }
        return;
    }
    if (self->monsterinfo.search_time && level.time > self->monsterinfo.search_time + 200) {
        M_MoveToGoal(self, dist);
        self->monsterinfo.search_time = 0;
        return;
    }
    G_InitEdict(tempgoal);
    save = self->goalentity;
    self->goalentity = tempgoal;
    new = 0;
    if (!(self->monsterinfo.aiflags & 8)) {
        self->monsterinfo.aiflags |= 0x18;
        self->monsterinfo.aiflags &= ~0x60;
        new = 1;
    }
    if (self->monsterinfo.aiflags & 0x20) {
        self->monsterinfo.aiflags &= ~0x20;
        self->monsterinfo.search_time = level.time + 50;
        if (self->monsterinfo.aiflags & 0x40) {
            self->monsterinfo.aiflags &= ~0x40;
            marker = 0;
            self->monsterinfo.last_sighting.vx = self->monsterinfo.saved_goal.vx;
            self->monsterinfo.last_sighting.vy = self->monsterinfo.saved_goal.vy;
            self->monsterinfo.last_sighting.vz = self->monsterinfo.saved_goal.vz;
            new = 1;
        } else if (self->monsterinfo.aiflags & 0x10) {
            self->monsterinfo.aiflags &= ~0x10;
            marker = PlayerTrail_PickFirst(self);
        } else {
            marker = PlayerTrail_PickNext(self);
        }
        if (marker) {
            self->monsterinfo.last_sighting.vx = marker->sorigin.vx;
            self->monsterinfo.last_sighting.vy = marker->sorigin.vy;
            self->monsterinfo.last_sighting.vz = marker->sorigin.vz;
            self->monsterinfo.trail_time = marker->timestamp;
            ((struct CREATURE *)self->Extra.Creature)->Ang.vy = self->sangles.vz = self->ideal_yaw = marker->sangles.vx;
            new = 1;
        }
    }
    v.vx = self->sorigin.vx - self->monsterinfo.last_sighting.vx;
    v.vy = self->sorigin.vy - self->monsterinfo.last_sighting.vy;
    v.vz = self->sorigin.vz - self->monsterinfo.last_sighting.vz;
    d1 = VectorLength2(&v);
    if (d1 <= dist * dist) {
        self->monsterinfo.aiflags |= 0x20;
        dist = VectorLength(&v);
    }
    self->goalentity->sorigin.vx = self->monsterinfo.last_sighting.vx;
    self->goalentity->sorigin.vy = self->monsterinfo.last_sighting.vy;
    self->goalentity->sorigin.vz = self->monsterinfo.last_sighting.vz;
    if (new) {
        getminsmaxs(self, &mins, &maxs);
        gitrace(&self->sorigin, &mins, &maxs, &self->monsterinfo.last_sighting, self, 0x2010003, ((struct CREATURE *)self->Extra.Creature)->Box.MoveSpace);
        if (traceptr->fraction < 0x1000) {
            v.vx = self->goalentity->sorigin.vx - self->sorigin.vx;
            v.vy = self->goalentity->sorigin.vy - self->sorigin.vy;
            v.vz = self->goalentity->sorigin.vz - self->sorigin.vz;
            d1 = VectorLength(&v);
            center = traceptr->fraction;
            d2 = d1 * ((center + 0x1000) / 2) / 4096;
            self->sangles.vz = self->ideal_yaw = vectoyaw(&v);
            AngleVectors(&self->sangles, &v_forward, &v_right, 0);
            sv.vx = d2;
            sv.vy = 0;
            sv.vz = -192;
            G_ProjectSourceNM(&self->sorigin, &sv, &v_forward, &v_right, &left_target);
            gitrace(&self->sorigin, &mins, &maxs, &left_target, self, 0x2010003, ((struct CREATURE *)self->Extra.Creature)->Box.MoveSpace);
            left = traceptr->fraction;
            sv.vx = d2;
            sv.vy = 0;
            sv.vz = 192;
            G_ProjectSourceNM(&self->sorigin, &sv, &v_forward, &v_right, &right_target);
            gitrace(&self->sorigin, &mins, &maxs, &right_target, self, 0x2010003, ((struct CREATURE *)self->Extra.Creature)->Box.MoveSpace);
            right = traceptr->fraction;
            center = d1 * center / d2;
            if (left >= center && left > right) {
                if (left < 0x1000) {
                    sv.vx = d2 * left / 8192;
                    sv.vy = 0;
                    sv.vz = -192;
                    G_ProjectSourceNM(&self->sorigin, &sv, &v_forward, &v_right, &left_target);
                }
                self->monsterinfo.saved_goal.vx = self->monsterinfo.last_sighting.vx;
                self->monsterinfo.saved_goal.vy = self->monsterinfo.last_sighting.vy;
                self->monsterinfo.saved_goal.vz = self->monsterinfo.last_sighting.vz;
                self->monsterinfo.aiflags |= 0x40;
                self->goalentity->sorigin.vx = left_target.vx;
                self->goalentity->sorigin.vy = left_target.vy;
                self->goalentity->sorigin.vz = left_target.vz;
                self->monsterinfo.last_sighting.vx = left_target.vx;
                self->monsterinfo.last_sighting.vy = left_target.vy;
                self->monsterinfo.last_sighting.vz = left_target.vz;
                v.vx = self->goalentity->sorigin.vx - self->sorigin.vx;
                v.vy = self->goalentity->sorigin.vy - self->sorigin.vy;
                v.vz = self->goalentity->sorigin.vz - self->sorigin.vz;
                self->sangles.vz = self->ideal_yaw = vectoyaw(&v);
            } else if (right >= center && right > left) {
                if (right < 0x1000) {
                    sv.vx = d2 * right / 8192;
                    sv.vy = 0;
                    sv.vz = 192;
                    G_ProjectSourceNM(&self->sorigin, &sv, &v_forward, &v_right, &right_target);
                }
                self->monsterinfo.saved_goal.vx = self->monsterinfo.last_sighting.vx;
                self->monsterinfo.saved_goal.vy = self->monsterinfo.last_sighting.vy;
                self->monsterinfo.saved_goal.vz = self->monsterinfo.last_sighting.vz;
                self->monsterinfo.aiflags |= 0x40;
                self->goalentity->sorigin.vx = right_target.vx;
                self->goalentity->sorigin.vy = right_target.vy;
                self->goalentity->sorigin.vz = right_target.vz;
                self->monsterinfo.last_sighting.vx = right_target.vx;
                self->monsterinfo.last_sighting.vy = right_target.vy;
                self->monsterinfo.last_sighting.vz = right_target.vz;
                v.vx = self->goalentity->sorigin.vx - self->sorigin.vx;
                v.vy = self->goalentity->sorigin.vy - self->sorigin.vy;
                v.vz = self->goalentity->sorigin.vz - self->sorigin.vz;
                self->sangles.vz = self->ideal_yaw = vectoyaw(&v);
            }
        }
    }
    M_MoveToGoal(self, dist);
    if (self)
        self->goalentity = save;
}
