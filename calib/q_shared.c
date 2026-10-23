#include "types.h"

extern short MaxEdictT;
extern struct edict_s *EdictTList;
extern struct COLLHEADER CollMap;
void UpdateEntity(struct edict_s *ent, int flags);
int Find3dSpaceGivenSecondary(struct VECT *Location, struct VECT *MoveLoc, int MoveSpace);
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);

/* defined before UpdateEntity so that it is called, not inlined */
int visible(struct edict_s *self, struct edict_s *other)
{
    struct CREATURE *CSelf;
    struct CREATURE *COther;
    struct VECT PointA;
    struct VECT PointB;

    CSelf = (struct CREATURE *)self->Extra.Creature;
    COther = (struct CREATURE *)other->Extra.Creature;
    if (self >= EdictTList && self <= &EdictTList[MaxEdictT - 1] && CSelf)
        UpdateEntity(self, 5);
    if (other >= EdictTList && other <= &EdictTList[MaxEdictT - 1] && COther)
        UpdateEntity(other, 5);
    PointA = self->sorigin;
    if (self >= EdictTList && self <= &EdictTList[MaxEdictT - 1])
        PointA.vy += self->viewheight;
    PointB = other->sorigin;
    if (other >= EdictTList && other <= &EdictTList[MaxEdictT - 1])
        PointB.vy += other->viewheight;
    return Visable3d(&CollMap, &PointA, &PointB, Find3dSpaceGivenSecondary(&PointA, &CSelf->Box.Location, CSelf->Box.MoveSpace));
}

int getminsvx(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vx;
}

int getminsvy(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vy;
}

int getminsvz(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return -96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vz;
}

int getmaxsvx(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vx;
}

int getmaxsvy(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vy;
}

int getmaxsvz(struct edict_s *ent)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature)
        return 96;
    return ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vz;
}

struct edict_s *FirstEdict(void);
struct edict_s *NextEdict(struct edict_s *Edict);
int VectBoxOverlap(struct VECTBOX *VBoxA, struct VECTBOX *VBoxB);
void CalcAbsFromCreBox(struct edict_s *ent, struct VECTBOX *VectBox);

int BoxEdicts(struct VECTBOX *VectBox, struct edict_s **touch, int maxedicts, int type)
{
    int count = 0;
    struct edict_s *temp;
    struct VECTBOX VBox;

    for (temp = FirstEdict(); temp; temp = NextEdict(temp)) {
        if (!temp->inuse)
            continue;
        CalcAbsFromCreBox(temp, &VBox);
        if (!VectBoxOverlap(VectBox, &VBox))
            continue;
        if ((type == 2 && temp->solid == 1) || (type == 1 && (temp->solid == 2 || temp->solid == 3))) {
            *touch++ = temp;
            count++;
            if (count == maxedicts)
                break;
        }
    }
    return count;
}

long anglemod(long a)
{
    return a & 0xfff;
}

void VectorMA(struct VECT *veca, long scale, struct VECT *vecb, struct VECT *vecc)
{
    vecc->vx = veca->vx + scale * vecb->vx / 4096;
    vecc->vy = veca->vy + scale * vecb->vy / 4096;
    vecc->vz = veca->vz + scale * vecb->vz / 4096;
}

long SquareRoot0(long a);

long VectorLength(struct VECT *v)
{
    long length;
    if (abs(v->vx) >= 0x6883 || abs(v->vy) >= 0x6883 || abs(v->vz) >= 0x6883)
        return 0xb504;
    length = v->vx * v->vx;
    length += v->vy * v->vy;
    length += v->vz * v->vz;
    return SquareRoot0(length);
}

long VectorLength2(struct VECT *v)
{
    long length;
    if (abs(v->vx) >= 0x6883 || abs(v->vy) >= 0x6883 || abs(v->vz) >= 0x6883)
        return 0xb504;
    length = v->vx * v->vx + v->vy * v->vy + v->vz * v->vz;
    return length;
}
void UpdateEntity(struct edict_s *ent, int flags)
{
    if (flags & 1) {
        ent->sorigin.vx = ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vx;
        ent->sorigin.vy = ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vy;
        ent->sorigin.vz = ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vz;
    }
    if (flags & 4) {
        ent->sangles.vz = ((struct CREATURE *)ent->Extra.Creature)->Ang.vy;
    }
}

void getminsmaxs(struct edict_s *ent, struct VECT *mins, struct VECT *maxs)
{
    if (ent->classname != 0x72 && ent->Extra.Creature) {
        mins->vx = ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vx;
        mins->vy = ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vy;
        mins->vz = ((struct CREATURE *)ent->Extra.Creature)->Box.Min.vz;
        maxs->vx = ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vx;
        maxs->vy = ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vy;
        maxs->vz = ((struct CREATURE *)ent->Extra.Creature)->Box.Max.vz;
        return;
    }
    mins->vx = -96;
    mins->vy = -96;
    mins->vz = -96;
    maxs->vx = 96;
    maxs->vy = 96;
    maxs->vz = 96;
}

void linkentity(struct edict_s *ent, int flags)
{
    if (flags & 1) {
        ((struct CREATURE *)ent->Extra.Creature)->IdealLocation.vx = ent->sorigin.vx;
        ((struct CREATURE *)ent->Extra.Creature)->IdealLocation.vy = ent->sorigin.vy;
        ((struct CREATURE *)ent->Extra.Creature)->IdealLocation.vz = ent->sorigin.vz;
        if (!(flags & 0x80)) {
            ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vx = ent->sorigin.vx;
            ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vy = ent->sorigin.vy;
            ((struct CREATURE *)ent->Extra.Creature)->Box.Location.vz = ent->sorigin.vz;
        }
    }
    if (flags & 4) {
        ((struct CREATURE *)ent->Extra.Creature)->IdealYaw = ent->sangles.vz;
        if (!(flags & 0x80))
            ((struct CREATURE *)ent->Extra.Creature)->Ang.vy = ent->sangles.vz;
    }
}

void CalcAbsFromCreBox(struct edict_s *ent, struct VECTBOX *VectBox)
{
    if (ent->classname == 0x72 || !ent->Extra.Creature) {
        VectBox->Min.vx = ent->sorigin.vx - 96;
        VectBox->Min.vy = ent->sorigin.vy - 96;
        VectBox->Min.vz = ent->sorigin.vz - 96;
        VectBox->Max.vx = ent->sorigin.vx + 96;
        VectBox->Max.vy = ent->sorigin.vy + 96;
        VectBox->Max.vz = ent->sorigin.vz + 96;
        return;
    }
    *VectBox = ((struct CREATURE *)ent->Extra.Creature)->Box.VectBox;
}

void neilserver(struct edict_s *ent, int flags)
{
    if (flags & 0x4000)
        UpdateEntity(ent, flags);
    if (flags & 0x8000)
        linkentity(ent, flags & 0xff);
}

long SquareRoot12(long a);

long VectorNormalize(struct VECT *v)
{
    long length;
    long ilength;

    if (abs(v->vx) >= 0x6883 || abs(v->vy) >= 0x6883 || abs(v->vz) >= 0x6883)
        length = 0x7ffea810;
    else {
        length = v->vx * v->vx;
        length += v->vy * v->vy;
        length += v->vz * v->vz;
    }
    if (length > 0x7ffff)
        length = SquareRoot0(length) << 12;
    else
        length = SquareRoot12(length << 12);
    if (length) {
        ilength = 0x40000000 / length;
        v->vx = v->vx * ilength / 64;
        v->vy = v->vy * ilength / 64;
        v->vz = v->vz * ilength / 64;
    }
    return length;
}

extern short rcossin_tbl[][2];

void AngleVectors(struct SVECT *angles, struct VECT *forward, struct VECT *right, struct VECT *up)
{
    long angle;
    long sr;
    long sp;
    long sy;
    long cr;
    long cp;
    long cy;

    angle = angles->vz & 0xfff;
    sy = rcossin_tbl[angle][1];
    cy = rcossin_tbl[angle][0];
    angle = angles->vx & 0xfff;
    sp = rcossin_tbl[angle][0];
    cp = rcossin_tbl[angle][1];
    angle = angles->vy & 0xfff;
    sr = rcossin_tbl[angle][0];
    cr = rcossin_tbl[angle][1];
    if (forward) {
        forward->vx = cp * cy / 4096;
        forward->vz = cp * sy / 4096;
        forward->vy = sp;
    }
    if (right) {
        right->vx = sr * -sp / 4096 * cy / 4096 + cr * sy / 4096;
        right->vz = sr * -sp / 4096 * sy / 4096 + cr * -cy / 4096;
        right->vy = -(sr * -cp / 4096);
    }
    if (up) {
        up->vx = cr * sp / 4096 * cy / 4096 + -sr * -sy / 4096;
        up->vz = cr * sp / 4096 * sy / 4096 + -sr * cy / 4096;
        up->vy = -(cr * cp / 4096);
    }
}

/* defined in this file (q_shared.obj) */
struct trace_t *traceptr;
extern struct VECT vec3_origin;
extern struct COLLHEADER MoveCollMap;
extern struct CREATURE **ThisBumpList;
extern struct CREATURE **ThisBumpListEnd;
int GetPrimarySpaceFromSecondary(int Indx, struct VECT *Location);
int VisableSolidBoxMove(struct VECT *P, struct VECT *Q, struct VECT *Stop, short Flags);
int VisableSolidBoxLine(struct VECT *P, struct VECT *Q, struct VECT *Stop);
struct CREATURE *LineIntersectCreCylinder(struct VECT *P, struct VECT *Q, struct CREATURE *CreIgnore, struct VECT *I, struct CREATURE **BumpStart, struct CREATURE **BumpStop);

static int VectorCompare(struct VECT *v1, struct VECT *v2)
{
    if (v1->vx != v2->vx || v1->vy != v2->vy || v1->vz != v2->vz)
        return 0;
    return 1;
}

/* the fraction of the move done, along its largest axis (the original repeats this code three times) */
#define TRACE_FRACTION()                                                        \
    traceptr->endpos.vx = StopPos.vx;                                           \
    traceptr->endpos.vy = StopPos.vy;                                           \
    traceptr->endpos.vz = StopPos.vz;                                           \
    traceptr->startsolid = 0;                                                   \
    traceptr->allsolid = 0;                                                     \
    totalvec.vx = end->vx - start->vx;                                          \
    totalvec.vy = end->vy - start->vy;                                          \
    totalvec.vz = end->vz - start->vz;                                          \
    StopPos.vx = traceptr->endpos.vx - start->vx;                               \
    StopPos.vy = traceptr->endpos.vy - start->vy;                               \
    StopPos.vz = traceptr->endpos.vz - start->vz;                               \
    largestindex = 0;                                                           \
    if (abs(totalvec.vy) > abs(totalvec.vx))                                    \
        largestindex = 1;                                                       \
    if (abs(totalvec.vz) > abs(totalvec.vy) && abs(totalvec.vz) > abs(totalvec.vx)) \
        largestindex = 2;                                                       \
    switch (largestindex) {                                                     \
    case 0:                                                                     \
        traceptr->fraction = (StopPos.vx << 12) / totalvec.vx;                  \
        break;                                                                  \
    case 1:                                                                     \
        traceptr->fraction = (StopPos.vy << 12) / totalvec.vy;                  \
        break;                                                                  \
    case 2:                                                                     \
        traceptr->fraction = (StopPos.vz << 12) / totalvec.vz;                  \
        break;                                                                  \
    }

void gitrace(struct VECT *start, struct VECT *mins, struct VECT *maxs, struct VECT *end, struct edict_s *passent, int contentmask, int start_space)
{
    int largestindex;
    static struct VECT totalvec;
    static struct VECT StopPos;
    struct CREATURE *hitcre;
    struct CREATURE *passcre;
    struct COLLBOX3D *Box;
    int gotthere;

    passcre = 0;
    hitcre = 0;
    gotthere = 0;
    Box = 0;
    traceptr->ent = 0;
    traceptr->cre = 0;
    if (passent)
        passcre = (struct CREATURE *)passent->Extra.Creature;
    if (mins && maxs && !(VectorCompare(mins, &vec3_origin) && VectorCompare(maxs, &vec3_origin)) && passcre)
        Box = &passcre->Box;
    if (Box) {
        gotthere = Visable3d(&MoveCollMap, start, end, start_space);
        StopPos = MoveCollMap.AreaIntersectLast->Intersect;
        if (!VisableSolidBoxMove(start, &StopPos, &StopPos, 0x810))
            gotthere = 0;
    } else if (passcre) {
        gotthere = Visable3d(&CollMap, start, end, GetPrimarySpaceFromSecondary(start_space, &passcre->Box.Location));
        StopPos = CollMap.AreaIntersectLast->Intersect;
        if (!VisableSolidBoxLine(start, &StopPos, &StopPos))
            gotthere = 0;
    }
    if (gotthere) {
        if (contentmask & 0x2000000)
            hitcre = LineIntersectCreCylinder(start, &StopPos, passcre, &StopPos, ThisBumpList, ThisBumpListEnd);
        if (hitcre) {
            traceptr->ent = hitcre->EdictT;
            traceptr->cre = hitcre;
            TRACE_FRACTION();
        } else {
            traceptr->endpos.vx = StopPos.vx;
            traceptr->endpos.vy = StopPos.vy;
            traceptr->endpos.vz = StopPos.vz;
            traceptr->startsolid = 0;
            traceptr->allsolid = 0;
            traceptr->fraction = 0x1000;
        }
    } else if (VectorCompare(&StopPos, start)) {
        traceptr->endpos.vx = StopPos.vx;
        traceptr->endpos.vy = StopPos.vy;
        traceptr->endpos.vz = StopPos.vz;
        traceptr->startsolid = 1;
        traceptr->fraction = 0;
    } else {
        if (contentmask & 0x2000000)
            hitcre = LineIntersectCreCylinder(start, &StopPos, passcre, &StopPos, ThisBumpList, ThisBumpListEnd);
        if (hitcre) {
            traceptr->ent = hitcre->EdictT;
            traceptr->cre = hitcre;
            TRACE_FRACTION();
        } else {
            TRACE_FRACTION();
        }
    }
}
