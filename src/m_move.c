#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
int M_walkmove(struct edict_s *ent, long yaw, long dist);
void M_MoveToGoal(struct edict_s *ent, long dist);
int M_ChangeYaw(struct edict_s *ent);
int M_CheckBottom(struct edict_s *ent);
int SV_movestep(struct edict_s *ent, struct VECT *move, int relink);
int SV_StepDirection(struct edict_s *ent, long yaw, long dist);
void SV_NewChaseDir(struct edict_s *actor, struct edict_s *enemy, long dist);
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

static int SV_CloseEnough(struct edict_s *ent, struct edict_s *goal, long dist)
{
    struct VECTBOX GoalVectBox;
    struct VECTBOX EntVectBox;

    CalcAbsFromCreBox(ent, &EntVectBox);
    CalcAbsFromCreBox(goal, &GoalVectBox);
    ExpandVectBox(&GoalVectBox, &GoalVectBox, dist, (struct SHORTPAIR){-dist, dist});
    return VectBoxOverlap(&GoalVectBox, &EntVectBox);
}

static void SV_FixCheckBottom(struct edict_s *ent)
{
    ent->flags |= 0x100;
}

#define VectorCopy(a, b) ((b).vx = (a).vx, (b).vy = (a).vy, (b).vz = (a).vz)
extern struct trace_t *traceptr;
extern struct trace_t Trace;
extern struct COLLHEADER MoveCollMap;
void gitrace(struct VECT *start, struct VECT *mins, struct VECT *maxs, struct VECT *end, struct edict_s *passent, int contentmask, int start_space);
void getminsmaxs(struct edict_s *ent, struct VECT *mins, struct VECT *maxs);

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

int SV_movestep(struct edict_s *ent, struct VECT *move, int relink)
{
    long dz;
    static struct VECT oldorg;
    static struct VECT neworg;
    static struct VECT end;
    static struct VECT tempvec;
    static struct VECT stepup;
    int i;
    long stepsize;
    static struct VECT mins;
    static struct VECT maxs;

    VectorCopy(ent->sorigin, oldorg);
    neworg.vx = ent->sorigin.vx + move->vx;
    neworg.vy = ent->sorigin.vy + move->vy;
    neworg.vz = ent->sorigin.vz + move->vz;
    getminsmaxs(ent, &mins, &maxs);
    if (ent->flags & 3) {
        for (i = 0; i < 2; i++) {
            neworg.vx = ent->sorigin.vx + move->vx;
            neworg.vy = ent->sorigin.vy + move->vy;
            neworg.vz = ent->sorigin.vz + move->vz;
            if (i == 0 && ent->enemy) {
                if (!ent->goalentity)
                    ent->goalentity = ent->enemy;
                dz = ent->sorigin.vy - ent->goalentity->sorigin.vy;
                if (ent->goalentity->client) {
                    if (dz < -480)
                        neworg.vy += 96;
                    if (!((ent->flags & 2) && ent->waterlevel < 2))
                        if (dz > -360)
                            neworg.vy -= 96;
                } else {
                    if (dz < -96)
                        neworg.vy += 96;
                    else if (dz < 0)
                        neworg.vy += dz;
                    else if (dz > 96)
                        neworg.vy -= 96;
                    else
                        neworg.vy -= dz;
                }
            }
            gitrace(&ent->sorigin, &mins, &maxs, &neworg, ent, 0x2020003, -1);
            if (Trace.fraction == 4096) {
                VectorCopy(Trace.endpos, ent->sorigin);
                if (relink) {
                    linkentity(ent, 0x81);
                    G_TouchTriggers(ent);
                }
                return 1;
            }
            if (!ent->enemy)
                break;
        }
        return 0;
    }
    if (!(ent->monsterinfo.aiflags & 0x400))
        stepsize = 216;
    else
        stepsize = 12;
    end.vx = ent->sorigin.vx + move->vx;
    end.vy = ent->sorigin.vy + move->vy;
    end.vz = ent->sorigin.vz + move->vz;
    end.vy += stepsize;
    VectorCopy(ent->sorigin, stepup);
    stepup.vy -= stepsize;
    gitrace(&ent->sorigin, &mins, &maxs, &stepup, ent, 0x2020003, ((struct CREATURE *)ent->Extra.Creature)->Box.MoveSpace);
    if (!traceptr->startsolid) {
        neworg.vy += traceptr->endpos.vy - ent->sorigin.vy;
        if (move->vx || move->vy || move->vz) {
            VectorCopy(traceptr->endpos, stepup);
            gitrace(&stepup, &mins, &maxs, &neworg, ent, 0x2020003, MoveCollMap.ReturnSpaceNum);
        }
        if (traceptr->fraction != 4096)
            return 0;
        gitrace(&neworg, &mins, &maxs, &end, ent, 0x2020003, MoveCollMap.ReturnSpaceNum);
    }
    traceptr->endpos.vy--;
    if (traceptr->startsolid) {
        neworg.vx = ent->sorigin.vx + move->vx;
        neworg.vy = ent->sorigin.vy + move->vy;
        neworg.vz = ent->sorigin.vz + move->vz;
        if (move->vx || move->vy || move->vz) {
            gitrace(&ent->sorigin, &mins, &maxs, &neworg, ent, 0x2020003, ((struct CREATURE *)ent->Extra.Creature)->Box.MoveSpace);
            if (traceptr->fraction != 4096)
                return 0;
            gitrace(&neworg, &mins, &maxs, &end, ent, 0x2020003, MoveCollMap.ReturnSpaceNum);
        } else {
            gitrace(&ent->sorigin, &mins, &maxs, &end, ent, 0x2020003, ((struct CREATURE *)ent->Extra.Creature)->Box.MoveSpace);
        }
        if (traceptr->startsolid)
            return 0;
    }
    if (traceptr->fraction == 4096) {
        if (ent->flags & 0x100)
            return 1;
        return 0;
    }
    VectorCopy(traceptr->endpos, ent->sorigin);
    if (!M_CheckBottom(ent)) {
        if (ent->flags & 0x100) {
            if (relink) {
                linkentity(ent, 0x81);
                G_TouchTriggers(ent);
            }
            return 1;
        }
        VectorCopy(oldorg, ent->sorigin);
        return 0;
    }
    if (ent->flags & 0x100)
        ent->flags &= ~0x100;
    if (relink) {
        linkentity(ent, 0x81);
        G_TouchTriggers(ent);
    }
    return 1;
}

/* do-while(0) around move = -speed flips the ideal/move register choice to the original's */
int M_ChangeYaw(struct edict_s *ent)
{
    long ideal;
    long current;
    long move;
    long total_move;
    long speed;
    int maxed_out;

    maxed_out = 0;
    current = anglemod(ent->sangles.vz);
    ideal = ent->ideal_yaw;
    if (current == ideal)
        return;
    {
    move = ideal - current;
    speed = ent->yaw_speed;
    if (ideal > current) {
        if (move >= 0x800)
            move -= 0x1000;
    } else if (move <= -0x800) {
        move += 0x1000;
    }
    total_move = move;
    if (move > 0) {
        if (move > speed) {
            move = speed;
            maxed_out = 1;
        }
    } else if (move < -speed) {
        do { move = -speed; } while (0);
        maxed_out = 1;
    }
    if (ent->monsterinfo.turn_threshold && abs(total_move) > ent->monsterinfo.turn_threshold && ent->monsterinfo.turn)
        ent->monsterinfo.turn(ent);
    else
        ((struct CREATURE *)ent->Extra.Creature)->IdealYaw = ent->sangles.vz = (long long)anglemod(current + move);
    }
    return maxed_out;
}

/* do-while(0) wrappers found by decomp-permuter; they change gcc's block layout */
int SV_StepDirection(struct edict_s *ent, long yaw, long dist)
{
  struct VECT move;
  struct VECT oldorigin;
  long delta;
  ent->ideal_yaw = yaw;
  {
    {
      long speed;
      long total_move;
      long move;
      long current;
      long ideal;
      current = anglemod(ent->sangles.vz);
      ideal = ent->ideal_yaw;
      if (current != ideal)
      {
        move = ideal - current;
        speed = ent->yaw_speed;
        if (ideal > current)
        {
          if (move >= 0x800)
          {
            move -= 0x1000;
          }
        }
        else
          if (move <= (-0x800))
        {
          move += 0x1000;
        }
        total_move = move;
        if (move > 0)
        {
          if (move > speed)
          {
            move = speed;
          }
        }
        else
          if (move < (-speed))
        {
 do { move = -speed; } while (0);
        }
        if ((ent->monsterinfo.turn_threshold && (abs(total_move) > ent->monsterinfo.turn_threshold)) && ent->monsterinfo.turn)
        {
          ent->monsterinfo.turn(ent);
        }
        else
        {
          ((struct CREATURE *) (&ent->Extra)->Creature)->IdealYaw = (ent->sangles.vz = (long long) anglemod(current + move));
        }
      }
    }
  }
  move.vx = (rcossin_tbl[yaw & 0xfff][0] * dist) / 4096;
  move.vz = (rcossin_tbl[yaw & 0xfff][1] * dist) / 4096;
  move.vy = 0;
  oldorigin.vx = ent->sorigin.vx;
  oldorigin.vy = ent->sorigin.vy;
  oldorigin.vz = ent->sorigin.vz;
  if (SV_movestep(ent, &move, 0))
  {
    delta = ent->sangles.vz - ent->ideal_yaw;
    if ((delta > 0x200) && (delta < 0xe00))
    {
      ent->sorigin.vx = oldorigin.vx;
      ent->sorigin.vy = oldorigin.vy;
      ent->sorigin.vz = oldorigin.vz;
    }
    linkentity(ent, 0x85);
    G_TouchTriggers(ent);
    return 1;
  }
  linkentity(ent, 0x85);
  G_TouchTriggers(ent);
  return 0;
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
