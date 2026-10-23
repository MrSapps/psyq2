#include "types.h"
int visible(struct edict_s *self, struct edict_s *other);
void FoundTarget(struct edict_s *self);

void M_ReactToDamage(struct edict_s *targ, struct edict_s *attacker)
{
    if (!attacker->client && !(attacker->svflags & 4))
        return;
    if (attacker == targ || attacker == targ->enemy)
        return;
    if (targ->monsterinfo.aiflags & 0x100) {
        if (attacker->client || (attacker->monsterinfo.aiflags & 0x100))
            return;
    }
    if (attacker->client) {
        if (targ->enemy && targ->enemy->client) {
            if (visible(targ, targ->enemy)) {
                targ->oldenemy = attacker;
                return;
            }
            targ->oldenemy = targ->enemy;
        }
        targ->enemy = attacker;
        if (!(targ->monsterinfo.aiflags & 0x800))
            FoundTarget(targ);
        return;
    }
    if ((targ->flags & 3) == (attacker->flags & 3) && targ->classname != attacker->classname
        && attacker->classname != 0x5b && attacker->classname != 0x5a
        && attacker->classname != 0x53 && attacker->classname != 0x52) {
        if (targ->enemy && targ->enemy->client)
            targ->oldenemy = targ->enemy;
        targ->enemy = attacker;
        if (!(targ->monsterinfo.aiflags & 0x800))
            FoundTarget(targ);
    } else {
        if (targ->enemy && targ->enemy->client)
            targ->oldenemy = targ->enemy;
        targ->enemy = attacker->enemy;
        if (!(targ->monsterinfo.aiflags & 0x800))
            FoundTarget(targ);
    }
}

extern struct cvar_s *skill;
extern struct cvar_s *deathmatch;
extern struct level_locals_t level;
long VectorNormalize(struct VECT *v);
void monster_death_use(struct edict_s *self);

static void Killed(struct edict_s *targ, struct edict_s *inflictor, struct edict_s *attacker, int damage, struct VECT *point)
{
    if (((struct CREATURE *)targ->Extra.Creature)->Health < -9999)
        ((struct CREATURE *)targ->Extra.Creature)->Health = -9999;
    targ->enemy = attacker;
    if ((targ->svflags & 4) && targ->deadflag != 2) {
        if (!(targ->monsterinfo.aiflags & 0x100)) {
            level.killed_monsters++;
            if (attacker->classname == 0x54)
                targ->owner = attacker;
        }
    }
    if (targ->movetype == 2 || targ->movetype == 3 || targ->movetype == 0) {
        targ->die(targ, inflictor, attacker, damage, point);
        return;
    }
    if ((targ->svflags & 4) && targ->deadflag != 2) {
        targ->touch = 0;
        monster_death_use(targ);
    }
    targ->die(targ, inflictor, attacker, damage, point);
}

void T_Damage(struct edict_s *targ, struct edict_s *inflictor, struct edict_s *attacker, struct VECT *dir, struct VECT *point, struct VECT *normal, int damage, int knockback, int dflags, int mod)
{
    struct gclient_s *client;
    int take;

    if (!targ->takedamage)
        return;
    if (!skill->value && !deathmatch->value && targ->client) {
        damage = damage / 2;
        if (!damage)
            damage = 1;
    }
    client = targ->client;
    VectorNormalize(dir);
    if (!(dflags & 1) && (targ->svflags & 4) && attacker->client && !targ->enemy
        && ((struct CREATURE *)targ->Extra.Creature)->Health > 0)
        damage *= 2;
    if (targ->flags & 0x800)
        knockback = 0;
    take = damage;
    if ((targ->flags & 0x10) && !(dflags & 0x20))
        take = 0;
    if (take) {
        ((struct CREATURE *)targ->Extra.Creature)->Health -= take;
        if (((struct CREATURE *)targ->Extra.Creature)->Health <= 0) {
            if ((targ->svflags & 4) || client)
                targ->flags |= 0x800;
            Killed(targ, inflictor, attacker, take, point);
            return;
        }
    }
    if (targ->svflags & 4) {
        M_ReactToDamage(targ, attacker);
        if (!(targ->monsterinfo.aiflags & 0x800) && take) {
            targ->pain(targ, attacker, knockback, take);
            if (skill->value == 3)
                targ->pain_debounce_time = level.time + 50;
        }
    } else if (take) {
        if (targ->pain)
            targ->pain(targ, attacker, knockback, take);
    }
}
