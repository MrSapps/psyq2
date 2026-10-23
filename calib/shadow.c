#include "types.h"

/* defined in this file (shadow.obj) */
struct POLY_FT4 *ShadowPolys;
struct POLY_FT4 *EndShadowPolys;

extern struct DB *currdb;
extern struct DB db[];
extern short tpscreenx[];
extern short tpscreeny[];
extern short palette[];
unsigned short GetTPage(int tp, int abr, int x, int y);
extern struct COLLHEADER CollMap;
extern struct COLLHEADER MoveCollMap;
int Visable3d(struct COLLHEADER *Map, struct VECT *Start, struct VECT *End, int S0);

void InitShadowPolys(void)
{
    struct POLY_FT4 *Poly;
    int a;
    int b;

    for (a = 0; a < 2; a++) {
        for (b = 0; b < 16; b++) {
            Poly = &db[a].ShadowPolys[b];
            ((unsigned char *)&Poly->tag)[3] = 9;
            Poly->code = 0x2e;
            Poly->r0 = 128;
            Poly->g0 = 128;
            Poly->b0 = 128;
            Poly->u0 = 224;
            Poly->v0 = 224;
            Poly->u1 = 239;
            Poly->v1 = 224;
            Poly->u2 = 224;
            Poly->v2 = 239;
            Poly->u3 = 239;
            Poly->v3 = 239;
            Poly->tpage = GetTPage(0, 2, tpscreenx[15], tpscreeny[15]);
            Poly->clut = palette[1];
        }
    }
}

void CalcCreShadowPos(struct CREATURE *Cre)
{
    struct VECT Start;
    struct VECT End;
    int Distance;
    struct COLLHEADER *Map;
    int Space;
    struct VECT *S, *E;

    E = &Cre->ShadowPos;
    S = &Cre->Box.Location;
    *E = *S;
    if (!(Cre->Box.OnFloor || Cre->Box.OnSolidBox) && !(Cre->Control & 0x10000)) {
        Cre->ShadowPos.vy += Cre->Box.Radius * 3;
        if (Cre->Box.MoveSpace != -1) {
            Map = &MoveCollMap;
            Space = Cre->Box.MoveSpace;
        } else {
            Map = &CollMap;
            Space = Cre->Box.PrimarySpace;
        }
        if (!Visable3d(Map, S, E, Space))
            Cre->ShadowPos.vy = Map->AreaIntersectLast->Intersect.vy;
    }
    if (Cre->Box.MoveSpace != -1)
        Cre->ShadowPos.vy += 0x11e;
    return;
    /* dead code: Start/End only reserve their frame slots (frame 0x40) */
    Visable3d(&CollMap, &Start, &End, 0);
}

void FrameShadowPolys(void)
{
    ShadowPolys = currdb->ShadowPolys;
    EndShadowPolys = &currdb->ShadowPolys[16];
}
