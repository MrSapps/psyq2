#include "types.h"
long anglemod(long a);
extern short rcossin_tbl[][2];
int SV_movestep(struct edict_s *ent, struct VECT *move, int relink);
int SV_StepDirection(struct edict_s *ent, long yaw, long dist);
void SV_NewChaseDir(struct edict_s *actor, struct edict_s *enemy, long dist);
int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB);
void ExpandVectBox();
void CalcAbsFromCreBox(struct edict_s *ent, struct VECTBOX *VectBox);
int rand(void);

extern struct COLLHEADER CollMap;
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);

int M_CheckBottom(struct edict_s *ent)
{
    struct CREATURE *Cre;
    int Space;
    int a;
    struct VECT Stop[4];

    Cre = ent->Extra.Creature;
    Stop[0].vx = ent->sorigin.vx - 143;
    Stop[0].vy = ent->sorigin.vy + 502;
    Stop[0].vz = ent->sorigin.vz - 143;
    Stop[1].vx = ent->sorigin.vx - 143;
    Stop[1].vy = ent->sorigin.vy + 502;
    Stop[1].vz = ent->sorigin.vz + 143;
    Stop[2].vx = ent->sorigin.vx + 143;
    Stop[2].vy = ent->sorigin.vy + 502;
    Stop[2].vz = ent->sorigin.vz + 143;
    Stop[3].vx = ent->sorigin.vx + 143;
    Stop[3].vy = ent->sorigin.vy + 502;
    Stop[3].vz = ent->sorigin.vz - 143;
    Space = Find3dSpaceGivenSecondary(&ent->sorigin, &Cre->Box.Location, Cre->Box.MoveSpace);
    for (a = 0; a < 4; a++) {
        if (Visable3d(&CollMap, &ent->sorigin, &Stop[a], Space) && VisableSolidBoxLine(&ent->sorigin, &Stop[a], 0))
            return 0;
    }
    return 1;
}

static int SV_CloseEnough(struct edict_s *ent, struct edict_s *goal, long dist)
{
    struct VECTBOX GoalVectBox;
    struct VECTBOX EntVectBox;

    CalcAbsFromCreBox(ent, &EntVectBox);
    CalcAbsFromCreBox(goal, &GoalVectBox);
    ExpandVectBox(&GoalVectBox, &GoalVectBox, dist, (struct SHORTPAIR){-dist, dist});
    return VectBoxOverlap(&GoalVectBox, &EntVectBox);
}

void M_MoveToGoal(struct edict_s *ent, long dist)
{
    struct edict_s *goal;

    goal = ent->goalentity;
    if (ent->enemy && SV_CloseEnough(ent, ent->enemy, dist))
        return;
    if ((rand() & 3) == 1 || !SV_StepDirection(ent, ent->ideal_yaw, dist)) {
        if (ent->inuse)
            SV_NewChaseDir(ent, goal, dist);
    }
}

int M_walkmove(struct edict_s *ent, long yaw, long dist)
{
    struct VECT move;

    move.vx = rcossin_tbl[yaw & 0xfff][0] * dist / 4096;
    move.vz = rcossin_tbl[yaw & 0xfff][1] * dist / 4096;
    move.vy = 0;
    return SV_movestep(ent, &move, 1);
}

static void SV_FixCheckBottom(struct edict_s *ent)
{
    ent->flags |= 0x100;
}

void SV_NewChaseDir(struct edict_s *actor, struct edict_s *enemy, long dist)
{
    long deltax;
    long deltaz;
    long d[3];
    long tdir;
    long olddir;
    long turnaround;

    if (!enemy)
        return;
    olddir = anglemod((actor->ideal_yaw / 512) * 512);
    turnaround = anglemod(olddir - 0x800);
    deltax = enemy->sorigin.vx - actor->sorigin.vx;
    deltaz = enemy->sorigin.vz - actor->sorigin.vz;
    if (deltax > 120)
        d[1] = 0x400;
    else if (deltax < -120)
        d[1] = 0xc00;
    else
        d[1] = -1;
    if (deltaz < -120)
        d[2] = 0x800;
    else if (deltaz > 120)
        d[2] = 0;
    else
        d[2] = -1;
    if (d[1] != -1 && d[2] != -1) {
        if (d[1] == 0x400)
            tdir = d[2] == 0 ? 0x200 : 0x600;
        else
            tdir = d[2] == 0 ? 0xe00 : 0xa00;
        if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
            return;
    }
    if ((rand() & 1) || abs(deltaz) > abs(deltax)) {
        tdir = d[1];
        d[1] = d[2];
        d[2] = tdir;
    }
    if (d[1] != -1 && d[1] != turnaround && SV_StepDirection(actor, d[1], dist))
        return;
    if (d[2] != -1 && d[2] != turnaround && SV_StepDirection(actor, d[2], dist))
        return;
    if (olddir != -1 && SV_StepDirection(actor, olddir, dist))
        return;
    if (rand() & 1) {
        for (tdir = 0; tdir <= 0xe00; tdir += 0x200)
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
    } else {
        for (tdir = 0xe00; tdir >= 0; tdir -= 0x200)
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
    }
    if (turnaround != -1 && SV_StepDirection(actor, turnaround, dist))
        return;
    actor->ideal_yaw = olddir;
    if (!M_CheckBottom(actor))
        SV_FixCheckBottom(actor);
}
