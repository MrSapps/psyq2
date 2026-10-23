#include "types.h"

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
