#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void DownloadPals(void);
void InitLevPals(void);

/* defined in pal.obj, so $gp-relative */
short NumMapPals;
short NumModelPals;
int TotalNumGamePals;
short *Global16colourpals;
static struct SCRPNT PalCoord;
unsigned short *GamePals;

int LoadImage(struct RECT *rect, unsigned long *p);
int DrawSync(int mode);
void *MY_malloc_namehb(int s, char *name);
extern struct PALLIST pallist0[];
extern short palette[];

void DownloadPals(void)
{
    struct PALLIST *Pals;
    struct RECT desrect;
    int tcnt;
    short *pal;
    short tp[16];
    int i;

    desrect.x = 0;
    desrect.y = 255;
    desrect.w = 256;
    desrect.h = 1;
    Pals = pallist0;
    LoadImage(&desrect, (unsigned long *)(Pals++)->pal);
    palette[0] = ((desrect.x & 0x3ff) >> 4) + ((desrect.y & 0x1ff) << 6);
    DrawSync(0);
    desrect.x = 0;
    desrect.y = 248;
    desrect.w = 16;
    desrect.h = 1;
    while (1) {
        if (!(pal = Pals->pal))
            break;
        tcnt = Pals->transcnt;
        for (i = 0; i < 16; i++) {
            if (tcnt & 1)
                tp[i] = *pal++ | 0x8000;
            else
                tp[i] = *pal++;
            tcnt >>= 1;
        }
        LoadImage(&desrect, (unsigned long *)tp);
        palette[Pals->palnum] = ((desrect.x & 0x3ff) >> 4) + ((desrect.y & 0x1ff) << 6);
        Pals++;
        desrect.y++;
        if (desrect.y >= 255) {
            desrect.y = 248;
            desrect.x += 16;
        }
        DrawSync(0);
    }
}

void InitLevPals(void)
{
    int a;
    int PalX;
    int PalY;
    struct RECT Rect;

    TotalNumGamePals = NumMapPals + NumModelPals;
    for (a = 0; a < TotalNumGamePals * 16; a++)
        if (Global16colourpals[a])
            Global16colourpals[a] |= 0x8000;
    Rect.y = PalCoord.sy = 256;
    Rect.x = PalCoord.sx = 0;
    Rect.w = 64;
    Rect.h = (TotalNumGamePals + 3) / 4;
    LoadImage(&Rect, (unsigned long *)Global16colourpals);
    GamePals = MY_malloc_namehb(TotalNumGamePals * 2, "Game Pal's");
    for (a = 0; a < TotalNumGamePals; a++) {
        PalX = PalCoord.sx + (a & 3) * 16;
        PalY = PalCoord.sy + (a >> 2);
        GamePals[a] = ((PalX & 0x3ff) >> 4) + ((PalY & 0x1ff) << 6);
    }
    DrawSync(0);
}
