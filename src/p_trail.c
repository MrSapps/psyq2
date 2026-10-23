#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void PlayerTrail_Init(void);
struct edict_s *PlayerTrail_PickNext(struct edict_s *self);
struct edict_s *PlayerTrail_LastSpot(void);
struct VECTBOX *PointInCreatureBlocker(struct VECT *point, int space);
void PlayerTrail_Add(struct VECT spot, int space);
struct edict_s *PlayerTrail_PickFirst(struct edict_s *self);
int abs(int); int SnapPointToBoxFace(struct VECT *point, struct VECTBOX *Box, struct VECT *snapped_point, int space);

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

extern struct COLLHEADER MoveCollMap;
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);

extern struct COLLHEADER TrigBoundCollMap;
int InVectBox3D(struct VECTBOX *VectBox, struct VECT *Vect);
void ExpandVectBox(struct VECTBOX *PQBox, struct VECTBOX *PQExp, short Width, struct SHORTPAIR MinMaxY);

/* The small constant MinMaxY (D_800AED6C, %hi/%lo) is a compound literal, not a $gp static. */
/* static in p_trail.obj; non-static here so the reloc keeps its name */
struct VECTBOX PICB_Box;

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

int abs(int);

int SnapPointToBoxFace(struct VECT *point, struct VECTBOX *Box, struct VECT *snapped_point, int space)
{
    unsigned char marked;
    int best;
    int bestdist;

    marked = 0;
    do {
        best = -1;
        bestdist = 99999999;
        if (!(marked & 1) && abs(point->vx - Box->Min.vx) < bestdist) {
            bestdist = abs(point->vx - Box->Min.vx);
            best = 0;
        }
        if (!(marked & 2) && abs(Box->Max.vx - point->vx) < bestdist) {
            bestdist = abs(Box->Max.vx - point->vx);
            best = 1;
        }
        if (!(marked & 0x10) && abs(point->vz - Box->Min.vz) < bestdist) {
            bestdist = abs(point->vz - Box->Min.vz);
            best = 4;
        }
        if (!(marked & 0x20) && abs(Box->Max.vz - point->vz) < bestdist) {
            bestdist = abs(Box->Max.vz - point->vz);
            best = 5;
        }
        snapped_point->vx = point->vx;
        snapped_point->vy = point->vy;
        snapped_point->vz = point->vz;
        switch (best) {
        case 0:
            snapped_point->vx = Box->Min.vx - 1;
            marked |= 1;
            break;
        case 1:
            snapped_point->vx = Box->Max.vx + 1;
            marked |= 2;
            break;
        case 2:
            snapped_point->vy = Box->Min.vy - 1;
            marked |= 4;
            break;
        case 3:
            snapped_point->vy = Box->Max.vy + 1;
            marked |= 8;
            break;
        case 4:
            snapped_point->vz = Box->Min.vz - 1;
            marked |= 0x10;
            break;
        case 5:
            snapped_point->vz = Box->Max.vz + 1;
            marked |= 0x20;
            break;
        default:
            return 0;
        }
        if (Visable3d(&MoveCollMap, point, snapped_point, space) && !PointInCreatureBlocker(snapped_point, MoveCollMap.ReturnSpaceNum))
            return 1;
    } while (marked != 0x3f);
    snapped_point->vx = point->vx;
    snapped_point->vy = point->vy;
    snapped_point->vz = point->vz;
    return 0;
}

struct VECTBOX *PointInCreatureBlocker(struct VECT *point, int space)
{
    struct COLL3DSPACE *trigger;

    for (trigger = TrigBoundCollMap.Coll3dSpace;
         trigger < TrigBoundCollMap.Coll3dSpace + TrigBoundCollMap.TotalNumSpaces; trigger++) {
        if (trigger->u.trigflags & 0x10) {
            ExpandVectBox(&trigger->MinMax, &PICB_Box, 573, *&(struct SHORTPAIR){-571, 573});
            if (InVectBox3D(&PICB_Box, point))
                return &PICB_Box;
        }
    }
    return 0;
}
