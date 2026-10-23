#include "types.h"
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
