#include "types.h"

#define TRAIL_LENGTH 8
#define NEXT(n) (((n) + 1) & (TRAIL_LENGTH - 1))
#define PREV(n) (((n) - 1) & (TRAIL_LENGTH - 1))

/* defined in p_trail.obj; trail_head/trail_active are $gp-relative */
struct trail_t trailstore[TRAIL_LENGTH];
struct edict_s *trail[TRAIL_LENGTH];
int trail_head;
int trail_active;

void *memset(void *s, int c, unsigned int n);
int visible(struct edict_s *self, struct edict_s *other);

extern struct level_locals_t level;
int SnapPointToBoxFace(struct VECT *point, struct VECTBOX *Box, struct VECT *snapped_point, int space);
struct VECTBOX *PointInCreatureBlocker(struct VECT *point, int space);
long vectoyaw(struct VECT *vec);

void PlayerTrail_Add(struct VECT spot, int space)
{
    struct VECT temp;
    struct VECT corrected;
    struct VECTBOX *cbbox;

    if (!trail_active)
        return;
    cbbox = PointInCreatureBlocker(&spot, space);
    if (cbbox)
        SnapPointToBoxFace(&spot, cbbox, &corrected, space);
    else {
        corrected.vx = spot.vx;
        corrected.vy = spot.vy;
        corrected.vz = spot.vz;
    }
    trail[trail_head]->sorigin.vx = corrected.vx;
    trail[trail_head]->sorigin.vy = corrected.vy;
    trail[trail_head]->sorigin.vz = corrected.vz;
    trail[trail_head]->timestamp = level.time;
    temp.vx = corrected.vx - trail[PREV(trail_head)]->sorigin.vx;
    temp.vy = corrected.vy - trail[PREV(trail_head)]->sorigin.vy;
    temp.vz = corrected.vz - trail[PREV(trail_head)]->sorigin.vz;
    trail[trail_head]->sangles.vx = vectoyaw(&temp);
    trail_head = NEXT(trail_head);
}

struct edict_s *PlayerTrail_PickFirst(struct edict_s *self)
{
    int marker;
    int n;

    if (!trail_active)
        return 0;

    for (marker = trail_head, n = TRAIL_LENGTH; n; n--) {
        if (trail[marker]->timestamp <= self->monsterinfo.trail_time)
            marker = NEXT(marker);
        else
            break;
    }

    if (visible(self, trail[marker]))
        return trail[marker];

    if (visible(self, trail[PREV(marker)]))
        return trail[PREV(marker)];

    return trail[marker];
}

void PlayerTrail_Init(void)
{
    int n;

    for (n = 0; n < TRAIL_LENGTH; n++) {
        memset(&trailstore[n], 0, sizeof(struct trail_t));
        trail[n] = (struct edict_s *)&trailstore[n];
    }
    trail_head = 0;
    trail_active = 1;
}

struct edict_s *PlayerTrail_PickNext(struct edict_s *self)
{
    int marker;
    int n;

    if (!trail_active)
        return 0;

    for (marker = trail_head, n = TRAIL_LENGTH; n; n--) {
        if (trail[marker]->timestamp <= self->monsterinfo.trail_time)
            marker = NEXT(marker);
        else
            break;
    }

    return trail[marker];
}

struct edict_s *PlayerTrail_LastSpot(void)
{
    return trail[PREV(trail_head)];
}

extern struct COLLHEADER TrigBoundCollMap;
int InVectBox3D(struct VECTBOX *VectBox, struct VECT *Vect);
void ExpandVectBox(struct VECTBOX *PQBox, struct VECTBOX *PQExp, short Width, struct SHORTPAIR MinMaxY);

/* Same code except one thing C can't reach here: the original addresses the small
   initialised static MinMaxY with lui/addiu %hi/%lo, maspsx with $gp:
     lui v0,%hi(MinMaxY); addiu s4,v0,%lo(MinMaxY); lhu s3,%lo(MinMaxY)(v0)
   vs addiu s4,gp,%gp_rel(MinMaxY); lhu s3,%gp_rel(MinMaxY)(gp) */
#ifdef CALIB_TOOLCHAIN_GAPS
/* static in p_trail.obj; non-static here so the reloc keeps its name */
struct VECTBOX PICB_Box;

struct VECTBOX *PointInCreatureBlocker(struct VECT *point, int space)
{
    static const struct SHORTPAIR MinMaxY = {-571, 573};
    struct COLL3DSPACE *trigger;

    for (trigger = TrigBoundCollMap.Coll3dSpace;
         trigger < TrigBoundCollMap.Coll3dSpace + TrigBoundCollMap.TotalNumSpaces; trigger++) {
        if (trigger->u.trigflags & 0x10) {
            ExpandVectBox(&trigger->MinMax, &PICB_Box, 573, MinMaxY);
            if (InVectBox3D(&PICB_Box, point))
                return &PICB_Box;
        }
    }
    return 0;
}
#endif
