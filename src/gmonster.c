#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void SetMyAiTable(int classname, struct ai_local_table_t *table);
void monster_fire_bullet(struct edict_s *self, struct VECT *start, struct VECT *dir, int damage, int kick, int hspread, int vspread, int flashtype);
int rand(void); void monster_fire_shotgun(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype);
void monster_fire_blaster(struct edict_s *self, struct VECT *Start, struct VECT *dir, int damage, int speed, int flashtype, int effect);
void monster_fire_rocket(struct edict_s *self, struct VECT *start, struct VECT *dir, int damage, int speed, int flashtype);
void monster_fire_railgun(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int flashtype);
void monster_fire_bfg(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int speed, int kick, long damage_radius, int flashtype);
void monster_fire_laser(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int flashtype);
void walkmonster_start(struct edict_s *self);
void swimmonster_start(struct edict_s *self);
void flymonster_start(struct edict_s *self);
void AttackFinished(struct edict_s *self, long time);
void monster_death_use(struct edict_s *self);
void monster_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator);
void monster_triggered_spawn(struct edict_s *self);
void monster_triggered_spawn_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator);
void walkmonster_start_go(struct edict_s *self);
void flymonster_start_go(struct edict_s *self);
void swimmonster_start_go(struct edict_s *self);
void NoGlobalAiFunc(struct edict_s *self, long dist);
void NoLocalAiTable(struct edict_s *self);
void monster_fire_grenade(struct edict_s *Self, struct VECT *Start, struct VECT *AimDir, int Damage, int Speed, int FlashType);
void M_MoveFrame(struct edict_s *self);
int monster_start(struct edict_s *self);
void monster_start_go(struct edict_s *self);
void InitAiLookupTables(void);

/* defined in gmonster.obj */

struct ai_local_tables_t ai_local_tables[256];
struct ai_local_table_t ai_local_table_none[32];
struct ai_global_table_t ai_global_table[16];
extern struct level_locals_t level;

void NoGlobalAiFunc(struct edict_s *self, long dist);
void NoLocalAiTable(struct edict_s *self);
void ai_stand(struct edict_s *self, long dist);
void ai_walk(struct edict_s *self, long dist);
void ai_run(struct edict_s *self, long dist);
void ai_charge(struct edict_s *self, long dist);
void ai_move(struct edict_s *self, long dist);
int monster_start(struct edict_s *self);
void walkmonster_start_go(struct edict_s *self);
void swimmonster_start_go(struct edict_s *self);
void flymonster_start_go(struct edict_s *self);
void monster_triggered_spawn(struct edict_s *self);
void monster_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator);
void FoundTarget(struct edict_s *self);
void Q2CreDropItemOnDeath(struct CREATURE *cre);
void AnyoneFireInstantRail(struct CREATURE *cre, struct VECT *start, struct SVECT *dir, int damage);
void AnyoneFireInstantLaser(struct CREATURE *cre, struct VECT *start, struct SVECT *dir, int damage);

extern struct WINDOW GameWindows[];
void dprintf(struct WINDOW *Window, char *_format, ...);
int AnyoneFireBFG(struct CREATURE *Aggressor, int Damage);
int AnyoneFireRocketL(struct CREATURE *Aggressor, struct VECT *Start, struct SVECT *Dir, int Damage);

void monster_start_go(struct edict_s *self);
int M_walkmove(struct edict_s *ent, long yaw, long dist);

extern struct COLLHEADER CollMap;
extern int SoundMachinegunFire;
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);
void AnyoneFireInstantBullet(struct CREATURE *Cre, struct VECT *Start, struct SVECT *Dir, int Damage, int Space, int BulletType);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);

extern int SoundBlasterFire;
void GetFireLocSpace(struct CREATURE *Cre, struct VECT *FirePosition, short *FireSpace);
int AnyoneFireBlaster(struct CREATURE *Self, struct VECT *FirePosition, struct SVECT *Dir, int Damage, int Flags, int DamageType, int FireSpace);

extern int SoundShotgunFire;

struct edict_s *G_PickTarget(int targetname);
long vectoyaw(struct VECT *vec);
void linkentity(struct edict_s *ent, int flags);
void UpdateEntity(struct edict_s *ent, int flags);
void M_MoveFrame(struct edict_s *self);

int M_CheckAttack(struct edict_s *self);
void SetInitialCreAnimFrame(struct CREATURE *Cre);

void vectoangles(struct VECT *value1, struct SVECT *angles);
void AngleVectors(struct SVECT *angles, struct VECT *forward, struct VECT *right, struct VECT *up);
void VectorMA(struct VECT *veca, long scale, struct VECT *vecb, struct VECT *vecc);
int GetSoundNum(struct IDENTITYNAME Name);
int AnyoneFireGrenadeL(struct CREATURE *Aggressor, struct VECT *Start, struct SVECT *Dir, int Damage, int Life);

void monster_fire_bullet(struct edict_s *self, struct VECT *start, struct VECT *dir, int damage, int kick, int hspread, int vspread, int flashtype)
{
    struct CREATURE *SelfCre;
    static struct SVECT BasicDir;
    struct SVECT Dir;
    short start_space;

    SelfCre = self->Extra.Creature;
    SelfCre->Control |= 0x80000000;
    Dir.vx = dir->vx * 4;
    Dir.vy = dir->vy * 4;
    Dir.vz = dir->vz * 4;
    if (Visable3d(&CollMap, &SelfCre->Box.Location, start, SelfCre->Box.PrimarySpace))
        start_space = CollMap.ReturnSpaceNum;
    else
        start_space = -1;
    AnyoneFireInstantBullet(SelfCre, start, &Dir, damage, start_space, 3);
    SoundByCreature(SoundMachinegunFire, SelfCre);
}

int rand(void);

void monster_fire_shotgun(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
    struct CREATURE *SelfCre;
    int a;
    struct SVECT Dir;
    short start_space;

    SelfCre = self->Extra.Creature;
    SoundByCreature(SoundShotgunFire, SelfCre);
    SelfCre->Control |= 0x80000000;
    Dir.vx = aimdir->vx * 4;
    Dir.vy = aimdir->vy * 4;
    Dir.vz = aimdir->vz * 4;
    if (Visable3d(&CollMap, &SelfCre->Box.Location, start, SelfCre->Box.PrimarySpace))
        start_space = CollMap.ReturnSpaceNum;
    else
        start_space = -1;
    for (a = 0; a < 5; a++)
    {
        Dir.vx += ((rand() - 0x4000) << 10) >> 14;
        Dir.vy += ((rand() - 0x4000) << 10) >> 14;
        AnyoneFireInstantBullet(SelfCre, start, &Dir, damage, start_space, 1);
    }
}

void monster_fire_blaster(struct edict_s *self, struct VECT *Start, struct VECT *dir, int damage, int speed, int flashtype, int effect)
{
    struct CREATURE *SelfCre;
    short FireSpace;
    struct SVECT Dir;
    struct VECT FirePosition;

    FirePosition = *Start;
    SelfCre = self->Extra.Creature;
    Dir.vx = dir->vx / 64;
    Dir.vy = dir->vy / 64;
    Dir.vz = dir->vz / 64;
    SoundByCreature(SoundBlasterFire, SelfCre);
    GetFireLocSpace(SelfCre, &FirePosition, &FireSpace);
    if (effect & 0x40)
        AnyoneFireBlaster(SelfCre, &FirePosition, &Dir, damage, 14, 5, FireSpace);
    else
        AnyoneFireBlaster(SelfCre, &FirePosition, &Dir, damage, 11, 6, FireSpace);
}

void monster_fire_grenade(struct edict_s *Self, struct VECT *Start, struct VECT *AimDir, int Damage, int Speed, int FlashType)
{
    struct CREATURE *SelfCre;
    struct SVECT Dir;
    struct VECT f;
    struct VECT r;
    struct VECT u;

    SelfCre = Self->Extra.Creature;
    vectoangles(AimDir, &Dir);
    AngleVectors(&Dir, &f, &r, &u);
    VectorMA(AimDir, (((rand() - 0x4000) * 10 / 32767 + 200) * 4096) / Speed, &u, AimDir);
    VectorMA(AimDir, (((rand() - 0x4000) * 10 / 32767) * 4096) / Speed, &r, AimDir);
    Dir.vx = AimDir->vx * 3 / 2;
    Dir.vy = AimDir->vy * 3 / 2;
    Dir.vz = AimDir->vz * 3 / 2;
    SoundByCreature(GetSoundNum((struct IDENTITYNAME){"wep_grenlf1a"}), SelfCre);
    AnyoneFireGrenadeL(SelfCre, Start, &Dir, Damage, 600);
}

void monster_fire_rocket(struct edict_s *self, struct VECT *start, struct VECT *dir, int damage, int speed, int flashtype)
{
    struct CREATURE *SelfCre;
    struct SVECT Dir;

    SelfCre = self->Extra.Creature;
    Dir.vx = (short)((dir->vx << 1) + dir->vx) >> 1;
    Dir.vy = (short)((dir->vy << 1) + dir->vy) >> 1;
    Dir.vz = (short)((dir->vz << 1) + dir->vz) >> 1;
    AnyoneFireRocketL(SelfCre, start, &Dir, damage);
}

void monster_fire_railgun(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int flashtype)
{
    struct CREATURE *SelfCre;
    struct SVECT Dir;

    SelfCre = self->Extra.Creature;
    Dir.vx = aimdir->vx;
    Dir.vy = aimdir->vy;
    Dir.vz = aimdir->vz;
    AnyoneFireInstantRail(SelfCre, start, &Dir, damage);
}

void monster_fire_bfg(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int speed, int kick, long damage_radius, int flashtype)
{
    self->pos1 = *start;
    AnyoneFireBFG(self->Extra.Creature, damage);
}

void monster_fire_laser(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int flashtype)
{
    struct SVECT Dir;

    Dir.vx = aimdir->vx;
    Dir.vy = aimdir->vy;
    Dir.vz = aimdir->vz;
    AnyoneFireInstantLaser(self->Extra.Creature, start, &Dir, damage);
}

void AttackFinished(struct edict_s *self, long time)
{
    self->monsterinfo.attack_finished = level.time + time;
}

void M_MoveFrame(struct edict_s *self)
{
    struct mmove_t *move;
    int index;

    move = self->monsterinfo.currentmove;
    self->nextthink = level.time + 1;
    if (self->monsterinfo.nextframe && self->monsterinfo.nextframe >= move->firstframe && self->monsterinfo.nextframe <= move->lastframe) {
        self->s.frame = self->monsterinfo.nextframe;
        self->monsterinfo.nextframe = 0;
    } else {
        if (self->s.frame == move->lastframe) {
            if (move->endfunc) {
                move->endfunc(self);
                move = self->monsterinfo.currentmove;
                if (self->svflags & 2)
                    return;
            }
        }
        if (self->s.frame < move->firstframe || self->s.frame > move->lastframe) {
            self->monsterinfo.aiflags &= ~0x80;
            self->s.frame = move->firstframe;
        } else {
            if (!(self->monsterinfo.aiflags & 0x80)) {
                self->s.frame++;
                if (self->s.frame > move->lastframe)
                    self->s.frame = move->firstframe;
            }
        }
    }
    index = self->s.frame - move->firstframe;
    /* volatile: the original reloads the byte after this test */
    if (((volatile struct mframe_t *)move->frame)[index].aifunc_global_index & 0x80) {
        if (ai_local_tables[self->classname].table[(move->frame[index].aifunc_global_index & 0x7F) + 26].thinkfunc)
            ai_local_tables[self->classname].table[(move->frame[index].aifunc_global_index & 0x7F) + 26].thinkfunc(self);
    } else if (ai_global_table[move->frame[index].aifunc_global_index].aifunc) {
        if (!(self->monsterinfo.aiflags & 0x80))
            ai_global_table[move->frame[index].aifunc_global_index].aifunc(self, (signed char)move->frame[index].dist * self->monsterinfo.scale * 12 / 10);
        else
            ai_global_table[move->frame[index].aifunc_global_index].aifunc(self, 0);
    }
    if (ai_local_tables[self->classname].table[move->frame[index].aifunc_local_index].thinkfunc)
        ai_local_tables[self->classname].table[move->frame[index].aifunc_local_index].thinkfunc(self);
}

void monster_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator)
{
    if (self->enemy)
        return;
    if (((struct CREATURE *)self->Extra.Creature)->Health <= 0)
        return;
    if (activator->flags & 0x20)
        return;
    if (!activator->client && !(activator->monsterinfo.aiflags & 0x100))
        return;
    self->enemy = activator;
    FoundTarget(self);
}

void monster_triggered_spawn(struct edict_s *self)
{
}

void monster_triggered_spawn_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator)
{
    self->think = monster_triggered_spawn;
    self->nextthink = level.time + 1;
    if (activator->client)
        self->enemy = activator;
    self->use = monster_use;
}

void monster_death_use(struct edict_s *self)
{
    struct CREATURE *cre;

    self->flags &= ~(1 | 2);
    self->monsterinfo.aiflags &= 0x100;
    if (self->item)
        self->item = 0;
    cre = self->Extra.Creature;
    if (cre && (cre->SpawnFlags & 0x100))
        Q2CreDropItemOnDeath(cre);
}

int monster_start(struct edict_s *self)
{
    if ((self->spawnflags & 4) && !(self->monsterinfo.aiflags & 0x100)) {
        self->spawnflags &= ~4;
        self->spawnflags |= 1;
    }
    if (!(self->monsterinfo.aiflags & 0x100))
        level.total_monsters++;
    self->nextthink = level.time + 1;
    self->svflags |= 4;
    self->takedamage = 2;
    self->air_finished = level.time + 120;
    self->use = monster_use;
    self->max_health = ((struct CREATURE *)self->Extra.Creature)->Health;
    self->clipmask = 0x2020003;
    self->s.skinnum = 0;
    self->deadflag = 0;
    self->svflags &= ~2;
    if (!self->monsterinfo.checkattack)
        self->monsterinfo.checkattack = M_CheckAttack;
    if (self->monsterinfo.currentmove) {
        self->s.frame = self->monsterinfo.currentmove->firstframe + (rand() % (self->monsterinfo.currentmove->lastframe - self->monsterinfo.currentmove->firstframe + 1));
        SetInitialCreAnimFrame(self->Extra.Creature);
    }
    return 1;
}

void monster_start_go(struct edict_s *self)
{
    struct VECT v;

    if (((struct CREATURE *)self->Extra.Creature)->Health <= 0)
        return;
    if (self->target > 0)
    {
        self->goalentity = self->movetarget = G_PickTarget(self->target);
        if (!self->movetarget)
        {
            self->target = 0;
            self->monsterinfo.pausetime = 1000000000;
            self->monsterinfo.stand(self);
        }
        else if (self->movetarget->classname == 0x72)
        {
            UpdateEntity(self, 1);
            v.vx = self->goalentity->sorigin.vx - self->sorigin.vx;
            v.vy = self->goalentity->sorigin.vy - self->sorigin.vy;
            v.vz = self->goalentity->sorigin.vz - self->sorigin.vz;
            self->ideal_yaw = self->sangles.vz = vectoyaw(&v);
            linkentity(self, 4);
            self->monsterinfo.walk(self);
            self->target = 0;
        }
        else
        {
            self->goalentity = self->movetarget = 0;
            self->monsterinfo.pausetime = 1000000000;
            self->monsterinfo.stand(self);
        }
    }
    else
    {
        self->ideal_yaw = self->sangles.vz;
        linkentity(self, 4);
        self->monsterinfo.pausetime = 1000000000;
        self->monsterinfo.stand(self);
    }
    self->think = M_MoveFrame;
    self->nextthink = level.time + 1;
}

void walkmonster_start_go(struct edict_s *self)
{
    if (!self->yaw_speed)
        self->yaw_speed = 228;
    self->viewheight = ~((struct CREATURE *)self->Extra.Creature)->HeightOffset;
    monster_start_go(self);
    if (self->spawnflags & 2)
    {
        self->solid = 0;
        self->movetype = 0;
        self->svflags |= 1;
        self->nextthink = 0;
        self->use = monster_triggered_spawn_use;
    }
}

void walkmonster_start(struct edict_s *self)
{
    self->think = walkmonster_start_go;
    monster_start(self);
}

void flymonster_start_go(struct edict_s *self)
{
    M_walkmove(self, 0, 0);
    if (!self->yaw_speed)
        self->yaw_speed = 114;
    self->viewheight = -250;
    monster_start_go(self);
    if (self->spawnflags & 2)
    {
        self->solid = 0;
        self->movetype = 0;
        self->svflags |= 1;
        self->nextthink = 0;
        self->use = monster_triggered_spawn_use;
    }
}

void flymonster_start(struct edict_s *self)
{
    self->flags |= 1;
    self->think = flymonster_start_go;
    monster_start(self);
}

void swimmonster_start_go(struct edict_s *self)
{
    if (!self->yaw_speed)
        self->yaw_speed = 114;
    self->viewheight = -100;
    monster_start_go(self);
    if (self->spawnflags & 2)
    {
        self->solid = 0;
        self->movetype = 0;
        self->svflags |= 1;
        self->nextthink = 0;
        self->use = monster_triggered_spawn_use;
    }
}

void swimmonster_start(struct edict_s *self)
{
    self->flags |= 2;
    self->think = swimmonster_start_go;
    monster_start(self);
}

void NoGlobalAiFunc(struct edict_s *self, long dist)
{
    dprintf(GameWindows, "No global ai func from class %i\n", self->classname);
}

void NoLocalAiTable(struct edict_s *self)
{
    dprintf(GameWindows, "No local ai table for class %i\n", self->classname);
}

void SetMyAiTable(int classname, struct ai_local_table_t *table)
{
    ai_local_tables[classname].table = table;
}

void InitAiLookupTables(void)
{
    int i;

    for (i = 0; i < 8; i++)
        ai_global_table[i].aifunc = NoGlobalAiFunc;
    for (i = 0; i < 256; i++)
        ai_local_tables[i].table = ai_local_table_none;
    for (i = 0; i < 32; i++)
        ai_local_table_none[i].thinkfunc = NoLocalAiTable;
    for (i = 0; i < 32; i++)
        ;
    ai_global_table[0].aifunc = 0;
    ai_global_table[1].aifunc = ai_stand;
    ai_global_table[2].aifunc = ai_walk;
    ai_global_table[3].aifunc = ai_run;
    ai_global_table[4].aifunc = ai_charge;
    ai_global_table[5].aifunc = ai_move;
}
