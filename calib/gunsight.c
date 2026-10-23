#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern struct WINDOW GameWindows[];
extern struct DB *currdb;
extern struct SPRT_16 *Sprite816;
extern struct SPRT_16 *LastSprite816;
extern short palette[];
struct COLOUR GunsightColour;
unsigned char GunSightUVs[2];
void AddPrim_(void *ot, void *p);

#define setlen(p, _len) (*(((unsigned char *)(p)) + 3) = (unsigned char)(_len))
#define setcode(p, _code) (*(((unsigned char *)(p)) + 7) = (unsigned char)(_code))

void ProcessGunSight(struct PLAYER *Player)
{
    struct COLOUR *Colour;
    long *Ot;
    short x, y;
    int win;

    Colour = &GunsightColour;
    if (Engine.GameConfig.bCrossHairs && Player->Window) {
        win = Player->Window - GameWindows;
        x = (Player->Window->Size.sx >> 1) - 8;
        y = (Player->Window->Size.sy >> 1) - 8;
        x += Player->Extras.GunSightOffsetX;
        y += Player->Extras.GunSightOffsetY;
        Ot = (long *)&currdb->Ot[win][50];
        if (Sprite816 < LastSprite816) {
            setlen(Sprite816, 3);
            setcode(Sprite816, 0x7c);
            Sprite816->x0 = x;
            Sprite816->y0 = y;
            Sprite816->u0 = GunSightUVs[0];
            Sprite816->v0 = GunSightUVs[1];
            Sprite816->clut = palette[72];
            Sprite816->r0 = Colour->r;
            Sprite816->g0 = Colour->g;
            Sprite816->b0 = Colour->b;
            AddPrim_(Ot, Sprite816++);
        }
    }
}
