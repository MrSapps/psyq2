#include "types.h"

extern short MaxEdictT;
extern struct pathcorner_s *PathTList;
extern struct edict_s *EdictTList;
extern short MaxPathT;

void *memset(void *s, int c, unsigned int n);
struct edict_s *FindFreePathT(void);
struct edict_s *FindFreeEdictT(void);
long ratan2(long y, long x);

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

long vectoyaw(struct VECT *vec)
{
    long yaw;

    if (vec->vz == 0 && vec->vx == 0)
        yaw = 0;
    else
        yaw = ratan2(vec->vx, vec->vz);
    return yaw;
}
