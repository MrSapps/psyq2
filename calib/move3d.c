#include "types.h"

extern struct COLLHEADER CollMap;
extern struct COLLHEADER MoveCollMap;
struct COLLHEADER *AsPointMap;
short sDamage;
short *pSpace;

int CreCollideAsPointFunc(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre);

int CreCollideAsFatPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage)
{
    AsPointMap = &MoveCollMap;
    pSpace = &Cre->Box.MoveSpace;
    sDamage = Damage;
    return CreCollideAsPointFunc(Cre, CreIgnore, Bounce, ppHitCre);
}

int CreCollideAsPoint(struct CREATURE *Cre, struct CREATURE *CreIgnore, int Bounce, struct CREATURE **ppHitCre, int Damage)
{
    AsPointMap = &CollMap;
    pSpace = &Cre->Box.PrimarySpace;
    sDamage = Damage;
    return CreCollideAsPointFunc(Cre, CreIgnore, Bounce, ppHitCre);
}
