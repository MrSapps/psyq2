#include "types.h"
struct DR_MOVE *WaterMoves[2];
int MaxWaterMoves;
struct DR_MOVE *LastWaterMove;
struct DR_MOVE *WaterMove;
extern unsigned char DbFrame[];
extern struct WINDOW GameWindows[];
extern struct DB *currdb;
extern int NewVcounts;
extern struct SCRPNT ScreenSize;
extern short rcossin_tbl[][2];
void *memset(void *s, int c, unsigned int n);
void AddPrim_(void *ot, void *p);
void SetTile(struct TILE *p);

/* DR_MOVE as the code writes it: source x/y, destination x/y, size */
struct DRMOVEXY {
    unsigned long tag;
    unsigned long code[2];
    short SrcX;
    short SrcY;
    short DstX;
    short DstY;
    struct SCRPNT Size;
};

void ProcessWaterEffect(struct WINDOW *Window)
{
    int BaseX;
    int BaseY;
    int SizeX;
    int SizeY;
    int AngleX;
    int AngleY;
    int a;
    int Offset;
    int LastOffset;
    int ChunkStart;
    int ScaleX;
    int ScaleY;
    int OffX;
    int OffY;
    struct SCRPNT ThisSize;
    unsigned long *Ot;
    struct TILE *Tile;

    Ot = &currdb->Ot[Window - GameWindows][47];
    if (!Window->Creature)
        return;
    if (((struct CREATURE *)Window->Creature)->Box.InWater) {
        Window->WaterScale += NewVcounts * 24;
        if (Window->WaterScale > 0x1000)
            Window->WaterScale = 0x1000;
    } else {
        Window->WaterScale -= NewVcounts * 24;
        if (Window->WaterScale < 0)
            Window->WaterScale = 0;
    }
    if (!Window->WaterScale) {
        Window->WaterAngleX = 0;
        Window->WaterAngleY = 0;
        memset(&Window->WaterOffset, 0, sizeof(Window->WaterOffset));
        return;
    }
    ScaleX = Window->WaterScale * 4095 * 4 / 4096;
    ScaleY = Window->WaterScale * 4095 * 2 / 4096;
    OffX = ScaleX / 4096;
    OffY = ScaleY / 4096;
    Window->WaterOffset.sx = OffX;
    Window->WaterOffset.sy = OffY;
    BaseX = Window->Pos.sx;
    BaseY = Window->Pos.sy;
    if (DbFrame[0])
        BaseX += ScreenSize.sx;
    SizeX = Window->Size.sx;
    SizeY = Window->Size.sy;
    Window->WaterAngleX += NewVcounts * 5;
    Window->WaterAngleY += NewVcounts * 7;
    AngleX = Window->WaterAngleX;
    AngleX &= 4095;
    AngleY = Window->WaterAngleY;
    AngleY &= 4095;
    LastOffset = 0x7fffffff;
    for (a = 0; a < Window->Size.sx + 1; a++) {
        Offset = rcossin_tbl[AngleY][1] * ScaleY / 0x1000000 + OffY;
        if (Offset != LastOffset || a == Window->Size.sx) {
            if (a) {
                ThisSize.sx = a - ChunkStart;
                ThisSize.sy = SizeY - LastOffset;
                if (ThisSize.sx && ThisSize.sy) {
                    *(struct SCRPNT *)&WaterMove->code[2] = (struct SCRPNT){BaseX + ChunkStart, BaseY + LastOffset};
                    *(struct SCRPNT *)&WaterMove->code[3] = (struct SCRPNT){BaseX + ChunkStart, BaseY};
                    *(struct SCRPNT *)&WaterMove->code[4] = ThisSize;
                    AddPrim_(Ot, WaterMove);
                    WaterMove++;
                    if (WaterMove >= LastWaterMove)
                        goto Done;
                }
            }
            ChunkStart = a;
            LastOffset = Offset;
        }
        AngleY += 20; AngleY &= 0xfff;
    }
    LastOffset = 0x7fffffff;
    for (a = 0; a < Window->Size.sy + 1; a++) {
        Offset = rcossin_tbl[AngleX][1] * ScaleX / 0x1000000 + OffX;
        if (Offset != LastOffset || a == Window->Size.sy) {
            if (a) {
                ThisSize.sy = a - ChunkStart;
                ThisSize.sx = SizeX - LastOffset;
                if (ThisSize.sx && ThisSize.sy) {
                    *(struct SCRPNT *)&WaterMove->code[2] = (struct SCRPNT){BaseX + LastOffset, BaseY + ChunkStart};
                    *(struct SCRPNT *)&WaterMove->code[3] = (struct SCRPNT){BaseX, BaseY + ChunkStart};
                    *(struct SCRPNT *)&WaterMove->code[4] = ThisSize;
                    AddPrim_(Ot, WaterMove);
                    WaterMove++;
                    if (WaterMove >= LastWaterMove)
                        goto Done;
                }
            }
            ChunkStart = a;
            LastOffset = Offset;
        }
        AngleX += 13; AngleX &= 0xfff;
    }
Done:
    Tile = &currdb->WaterTiles[Window - GameWindows];
    SetTile(Tile);
    Tile->code |= 2;
    Tile->r0 = Window->WaterScale * 15 / 4096;
    Tile->g0 = 0;
    Tile->b0 = Window->WaterScale * 55 / 4096;
    Tile->x0 = 0;
    Tile->y0 = 0;
    Tile->w = Window->Size.sx;
    Tile->h = Window->Size.sy;
    AddPrim_(Ot, Tile);
}
