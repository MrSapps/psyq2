#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void G_ProjectSourceNM(struct VECT *point, struct SVECT *distance, struct VECT *forward, struct VECT *right, struct VECT *result);
struct edict_s *G_PickTarget(int targetname);
void G_InitEdict(struct edict_s *e);
struct edict_s *G_Spawn_Path(void);
struct edict_s *G_Spawn_NoCre(void);
long vectoyaw(struct VECT *vec);
void vectoangles(struct VECT *value1, struct SVECT *angles);
struct edict_s *FirstEdict(void);
struct CREATURE *CreFindRadius(struct CREATURE *from, struct VECT org, long rad);
struct edict_s *NextEdict(struct edict_s *Edict);
void G_TouchTriggers(struct edict_s *ent);

extern short MaxEdictT;
extern struct pathcorner_s *PathTList;
extern struct edict_s *EdictTList;
extern short MaxPathT;

void *memset(void *s, int c, unsigned int n);
struct edict_s *FindFreePathT(void);
struct edict_s *FindFreeEdictT(void);
long ratan2(long y, long x);

extern struct CREATURE Creatures[];
extern struct CREATURE *CreFreeList[];
long VectorLength2(struct VECT *v);

void path_corner_touch(struct edict_s *self, struct edict_s *other, struct cplane_s *plane, struct csurface_s *surf);

static struct edict_s *G_Find(struct edict_s *from, int match)
{
    do { from = NextEdict(from); if (!from) return 0; if (!from->inuse) continue; if (from->targetname == match) return from; } while (from); return 0;
}

void G_ProjectSourceNM(struct VECT *point, struct SVECT *distance, struct VECT *forward, struct VECT *right, struct VECT *result)
{
    result->vx = point->vx + forward->vx * distance->vx / 4096 + right->vx * distance->vz / 4096;
    result->vy = point->vy + forward->vy * distance->vx / 4096 + right->vy * distance->vz / 4096 + distance->vy;
    result->vz = point->vz + forward->vz * distance->vx / 4096 + right->vz * distance->vz / 4096;
}

struct edict_s *FirstEdict(void)
{
    return EdictTList;
}

struct edict_s *NextEdict(struct edict_s *Edict)
{
    if (Edict >= EdictTList && Edict < EdictTList + MaxEdictT) {
        if (Edict == EdictTList + MaxEdictT - 1)
            return (struct edict_s *)PathTList;
        return Edict + 1;
    }
    if ((struct pathcorner_s *)Edict >= PathTList && (struct pathcorner_s *)Edict < PathTList + MaxPathT) {
        if ((struct pathcorner_s *)Edict == PathTList + MaxPathT - 1)
            return 0;
        return (struct edict_s *)((struct pathcorner_s *)Edict + 1);
    }
    if (Edict == 0)
        return EdictTList;
    return 0;
}

/* Creatures + 48 is CreFreeList to splat */
struct CREATURE *CreFindRadius(struct CREATURE *from, struct VECT org, long rad)
{
    struct VECT eorg;
    int rad2;

    rad2 = rad * rad;
    if (!from)
        from = Creatures;
    else
        from++;
    for (; from < (struct CREATURE *)CreFreeList; from++) {
        if (from->Life <= 0)
            continue;
        if (!from->ModelHdr)
            continue;
        eorg.vx = org.vx - from->Box.Location.vx;
        eorg.vy = org.vy - from->Box.Location.vy;
        eorg.vz = org.vz - from->Box.Location.vz;
        if (VectorLength2(&eorg) > rad2)
            continue;
        return from;
    }
}

struct edict_s *G_PickTarget(int targetname)
{
    struct edict_s *ent;
    int num_choices;
    struct edict_s *choice[8];

    ent = 0;
    num_choices = 0;
    if (targetname <= 0)
        return 0;
    while (1) {
        ent = G_Find(ent, targetname);
        if (!ent)
            break;
        choice[num_choices++] = ent;
        if (num_choices == 8)
            break;
    }
    if (!num_choices)
        return 0;
    return choice[rand() % num_choices];
}

long vectoyaw(struct VECT *vec)
{
    long yaw;

    if (vec->vz == 0 && vec->vx == 0)
        yaw = 0;
    else
        yaw = ratan2(vec->vx, vec->vz);
    return yaw;
}

void vectoangles(struct VECT *value1, struct SVECT *angles)
{
    long yaw;
    long pitch;

    if (value1->vx == 0 && value1->vz == 0) {
        yaw = 0;
        if (value1->vy > 0)
            pitch = 0x400;
        else
            pitch = 0xc00;
    } else {
        if (value1->vx)
            yaw = ratan2(value1->vx, value1->vz);
        else if (value1->vz > 0)
            yaw = 0x400;
        else
            yaw = 0xc00;
        if (yaw < 0)
            yaw += 0x1000;
        pitch = ratan2(value1->vy, SquareRoot0(value1->vx * value1->vx + value1->vz * value1->vz));
        if (pitch < 0)
            pitch += 0x1000;
    }
    angles->vx = pitch;
    angles->vz = yaw;
    angles->vy = 0;
}

void G_InitEdict(struct edict_s *e)
{
    memset(e, 0, sizeof(struct edict_s));
    e->classname = 0xc8;
    e->inuse = 1;
}

struct edict_s *G_Spawn_Path(void)
{
    struct edict_s *e;

    e = FindFreePathT();
    memset(e, 0, sizeof(struct pathcorner_s));
    e->classname = 0x72;
    e->inuse = 1;
    return e;
}

struct edict_s *G_Spawn_NoCre(void)
{
    struct edict_s *e;

    e = FindFreeEdictT();
    memset(e, 0, sizeof(struct edict_s));
    e->classname = 0xc8;
    e->inuse = 1;
    return e;
}

void G_TouchTriggers(struct edict_s *ent)
{
    int i;
    int num;
    struct edict_s *touch[MaxEdictT];
    struct edict_s *hit;
    struct VECTBOX VectBox;

    if ((ent->client || (ent->svflags & 4)) && ((struct CREATURE *)ent->Extra.Creature)->Health <= 0)
        return;
    neilserver(ent, 0x4001);
    CalcAbsFromCreBox(ent, &VectBox);
    num = BoxEdicts(&VectBox, touch, MaxEdictT, 2);
    for (i = 0; i < num; i++) {
        hit = touch[i];
        if (!hit->inuse)
            continue;
        if (hit->classname == 0x72)
            path_corner_touch(hit, ent, 0, 0);
        else if (hit->touch)
            hit->touch(hit, ent, 0, 0);
    }
}
