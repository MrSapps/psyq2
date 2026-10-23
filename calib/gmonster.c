#include "types.h"

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
void monster_start(struct edict_s *self);
void walkmonster_start_go(struct edict_s *self);
void swimmonster_start_go(struct edict_s *self);
void flymonster_start_go(struct edict_s *self);
void monster_triggered_spawn(struct edict_s *self);
void monster_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator);
void FoundTarget(struct edict_s *self);
void Q2CreDropItemOnDeath(struct CREATURE *cre);
void AnyoneFireInstantRail(struct CREATURE *cre, struct VECT *start, struct SVECT *dir, int damage);
void AnyoneFireInstantLaser(struct CREATURE *cre, struct VECT *start, struct SVECT *dir, int damage);

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

void SetMyAiTable(int classname, struct ai_local_table_t *table)
{
    ai_local_tables[classname].table = table;
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

void monster_fire_laser(struct edict_s *self, struct VECT *start, struct VECT *aimdir, int damage, int kick, int flashtype)
{
    struct SVECT Dir;

    Dir.vx = aimdir->vx;
    Dir.vy = aimdir->vy;
    Dir.vz = aimdir->vz;
    AnyoneFireInstantLaser(self->Extra.Creature, start, &Dir, damage);
}

void walkmonster_start(struct edict_s *self)
{
    self->think = walkmonster_start_go;
    monster_start(self);
}

void swimmonster_start(struct edict_s *self)
{
    self->flags |= 2;
    self->think = swimmonster_start_go;
    monster_start(self);
}

void flymonster_start(struct edict_s *self)
{
    self->flags |= 1;
    self->think = flymonster_start_go;
    monster_start(self);
}

void AttackFinished(struct edict_s *self, long time)
{
    self->monsterinfo.attack_finished = level.time + time;
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

void monster_triggered_spawn_use(struct edict_s *self, struct edict_s *other, struct edict_s *activator)
{
    self->think = monster_triggered_spawn;
    self->nextthink = level.time + 1;
    if (activator->client)
        self->enemy = activator;
    self->use = monster_use;
}
