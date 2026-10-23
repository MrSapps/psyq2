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
