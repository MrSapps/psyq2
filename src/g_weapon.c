#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
int fire_hit(struct edict_s *self, struct VECT aim, int damage, int kick);

extern struct VECT vec3_origin;
long VectorLength(struct VECT *v);
long VectorNormalize(struct VECT *v);
void VectorMA(struct VECT *veca, int scale, struct VECT *vecb, struct VECT *vecc);
void AngleVectors(struct SVECT *angles, struct VECT *forward, struct VECT *right, struct VECT *up);
void CalcAbsFromCreBox(struct edict_s *ent, struct VECTBOX *VectBox);
int getminsvx(struct edict_s *ent);
int getmaxsvx(struct edict_s *ent);
int getminsvy(struct edict_s *ent);
int getmaxsvy(struct edict_s *ent);
int getminsvz(struct edict_s *ent);
int getmaxsvz(struct edict_s *ent);
void DoDamage(void *Victim, void *Attacker, int Damage, int Type, struct VECT *Point);
void *memset(void *, int, int);

int fire_hit(struct edict_s *self, struct VECT aim, int damage, int kick)
{
    struct VECT forward;
    struct VECT right;
    struct VECT up;
    struct VECT v;
    struct VECT point;
    long range;
    struct VECT dir;
    struct VECT size;
    struct VECTBOX VectBox;
    struct CREATURE *Target;

    dir.vx = self->enemy->sorigin.vx - self->sorigin.vx;
    dir.vy = self->enemy->sorigin.vy - self->sorigin.vy;
    dir.vz = self->enemy->sorigin.vz - self->sorigin.vz;
    range = VectorLength(&dir);
    if (range > aim.vx)
        return 0;
    if (aim.vz > getminsvx(self) && aim.vz < getmaxsvx(self))
        range -= getmaxsvx(self->enemy);
    else if (aim.vz < 0)
        aim.vz = getminsvx(self->enemy);
    else
        aim.vz = getmaxsvx(self->enemy);
    VectorMA(&self->sorigin, range << 12, &dir, &point);
    AngleVectors(&self->sangles, &forward, &right, &up);
    VectorMA(&self->sorigin, range, &forward, &point);
    VectorMA(&point, aim.vz, &right, &point);
    VectorMA(&point, -aim.vy, &up, &point);
    dir.vx = point.vx - self->enemy->sorigin.vx;
    dir.vy = point.vy - self->enemy->sorigin.vy;
    dir.vz = point.vz - self->enemy->sorigin.vz;
    DoDamage(self->Extra.Creature, self->enemy->Extra.Creature, damage, 7, &point);
    CalcAbsFromCreBox(self->enemy, &VectBox);
    size.vx = getmaxsvx(self->enemy) - getminsvx(self->enemy);
    size.vy = getmaxsvy(self->enemy) - getminsvy(self->enemy);
    size.vz = getmaxsvz(self->enemy) - getminsvz(self->enemy);
    VectorMA(&VectBox.Min, 0x800, &size, &v);
    v.vx = v.vx - point.vx;
    v.vy = v.vy - point.vy;
    v.vz = v.vz - point.vz;
    VectorNormalize(&v);
    VectorMA(&vec3_origin, kick, &v, &self->enemy->velocity);
    Target = self->enemy->Extra.Creature;
    Target->Control |= 0x4000;
    memset(&Target->Box.FloorNorm, 0, sizeof(struct SVECT));
    Target->Box.OnFloor = 0;
    Target->Box.OnRope = 0;
    self->enemy->velocity.vy <<= 4;
    if (self->enemy->velocity.vy < -0xc00)
        self->enemy->velocity.vy = -0xc00;
    Target->ForceVel.vx += self->enemy->velocity.vx << 4;
    Target->ForceVel.vy += self->enemy->velocity.vy;
    Target->ForceVel.vz += self->enemy->velocity.vz << 4;
    if (self->enemy->velocity.vy < 0)
        self->enemy->groundentity = 0;
    return 1;
}
