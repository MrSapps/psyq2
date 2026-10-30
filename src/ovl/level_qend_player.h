/* Shared by level_qendmis1, level_qdummy and level_qmagextr: the picture fades, the sprite
 * helpers, the movie player (PSY-Q's strplay sample) and the small engine wrappers.
 * The including module defines the .data/.bss these use and the prototype block that sets
 * the order of the small (deferred) functions. */

static void PictureUpdate(int len, short clut, int next)
{
    int fade[3];
    int t;

    SprtPtr = Sprites[*ENGINE->DbFrame];
    SprtEnd = SprtPtr + 5;
    TPagePtr = TPages[*ENGINE->DbFrame];
    TPageEnd = TPagePtr + 5;
    fade[0] = fade[1] = fade[2] = 0;
    t = ++PictureFrame;
    PictureTime = t;
    if (t < 8)
        fade[0] = t * 16;
    else if (t < len + 8)
        fade[0] = 128;
    else if (t < len + 16)
        fade[0] = 128 - (t - (len + 8)) * 16;
    if (fade[0]) {
        DrawPicture(0, 0, 2, 1, (struct COLOUR){fade[0], fade[0], fade[0]}, 1);
    }
    if (PictureTime >= len + 18)
        *ENGINE->LevelComplete = next;
}

static void DrawSprite(struct DVECTOR xy, struct DVECTOR wh, short clut, short tpage, struct COLOUR col, int abr)
{
    unsigned long *ot = &(*ENGINE->currdb)->Ot[0][48];
    short tp = tpage + (abr >> 1);

    if (SprtPtr < SprtEnd) {
        *(struct COLOUR *)&SprtPtr->r0 = col;
        *(struct DVECTOR *)&SprtPtr->x0 = xy;
        SprtPtr->u0 = 0;
        SprtPtr->v0 = (abr & 1) << 7;
        SprtPtr->clut = ENGINE->palette[clut];
        *(struct DVECTOR *)&SprtPtr->w = wh;
        SprtPtr->code = 0x66;
        SprtPtr->tag = 0x04000000;
        ENGINE->AddPrim_(ot, SprtPtr++);
    }
    if (TPagePtr < TPageEnd) {
        SetDrawTPage(TPagePtr, 1, 0, GetTPage(0, 1, ENGINE->tpscreenx[tp], ENGINE->tpscreeny[tp]));
        ENGINE->AddPrim_(ot, TPagePtr++);
    }
}

static void DrawPicture(short y, short clut, short tpage, int is24, struct COLOUR col, int semitrans)
{
    unsigned long *ot;
    int v;
    int mode;
    int tpstep;
    int i;
    int x;
    int h;
    int w;

    v = y;
    x = 0;
    h = 240;
    if (is24) {
        tpstep = 2;
        mode = 1;
    } else {
        tpstep = 1;
        mode = 0;
    }
    w = 256 - y;
    ot = &(*ENGINE->currdb)->Ot[0][48];
    for (i = 0; i < 4; i++) {
        if (h) {
            if (h < w)
                w = h;
            if (SprtPtr < SprtEnd) {
                *(struct COLOUR *)&SprtPtr->r0 = col;
                SprtPtr->x0 = (i << 8) & 0x1ff;
                SprtPtr->y0 = x;
                SprtPtr->u0 = 0;
                SprtPtr->v0 = v;
                SprtPtr->clut = ENGINE->palette[clut];
                SprtPtr->w = 256;
                SprtPtr->h = w;
                SprtPtr->code = 0x64;
                if (semitrans)
                    SprtPtr->code |= 2;
                SprtPtr->tag = 0x04000000;
                ENGINE->AddPrim_(ot, SprtPtr++);
            }
            if (i == 1) {
                h -= w;
                w = 256;
                v = 0;
                x = x + 256 - y;
            }
            if (TPagePtr < TPageEnd) {
                SetDrawTPage(TPagePtr, 1, 0, GetTPage(mode, 1, ENGINE->tpscreenx[tpage], ENGINE->tpscreeny[tpage]));
                tpage += tpstep;
                ENGINE->AddPrim_(ot, TPagePtr++);
            }
        }
    }
}

int printf(char *fmt, ...)
{
}

static void strCallback(void)
{
    struct RECT snap_rect;
    int rest;

    if (*ENGINE->StCdIntrFlagAddr) {
        ENGINE->StCdInterrupt();
        *ENGINE->StCdIntrFlagAddr = 0;
    }
    ENGINE->LoadImage(&dec.slice, dec.imgbuf);
    rest = dec.rect[dec.rectid].w % 24;
    if (IsRGB24Start && rest) {
        IsRGB24Start = 0;
        dec.slice.x += rest * 3 / 2;
    } else
        dec.slice.x += 24;
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w)
        DecDCTout((unsigned long *)dec.imgbuf, bound24(dec.slice.w) * bound16(dec.slice.h) >> 1);
    else {
        dec.isdone = 1;
        dec.rectid = dec.rectid ? 0 : 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        IsRGB24Start = 1;
        dec.slice.y = dec.rect[dec.rectid].y + YOffset;
    }
}

static unsigned long *strNext(void)
{
    unsigned long *addr;
    struct StHEADER *sector;
    int cnt;
    struct RECT rect;

    cnt = 0x800000;
    while (ENGINE->StGetNext(&addr, &sector)) {
        if (--cnt == 0)
            return 0;
    }
    if (addr[0] != ((unsigned long *)sector)[5] || addr[1] != ((unsigned long *)sector)[6]) {
        ENGINE->StFreeRing(addr);
        return 0;
    }
    if (((unsigned long *)sector)[2] >= EndFrame)
        isEndOfFlag = 1;
    if (MovieWidth != ((unsigned short *)sector)[8] || MovieHeight != ((unsigned short *)sector)[9]) {
        rect.w = 480;
        rect.x = 0;
        rect.y = 0;
        rect.h = 240;
        ENGINE->ClearImage(&rect, 0, 0, 0);
        MovieWidth = ((unsigned short *)sector)[8];
        MovieHeight = ((unsigned short *)sector)[9];
    }
    dec.rect[0].w = dec.rect[1].w = MovieWidth * 3 / 2;
    dec.slice.h = dec.rect[0].h = dec.rect[1].h = MovieHeight;
    YOffset = (240 - MovieHeight) / 2;
    return addr;
}

static void strSetDispEnv(struct DISPENV *disp, struct DRAWENV *draw)
{
    int id;

    id = dec.rectid ? 0 : 1;
    ENGINE->SetDefDispEnv(disp, dec.rect[id].x, dec.rect[id].y, dec.rect[id].w, dec.rect[id].h);
    disp->screen.y = ScreenY + 8;
    disp->screen.x = ScreenX;
    ENGINE->SetDefDrawEnv(draw, dec.rect[id].x, dec.rect[id].y, dec.rect[id].w, dec.rect[id].h);
    disp->isrgb24 = 1;
    disp->disp.w = disp->disp.w * 2 / 3;
    ENGINE->PutDispEnv(disp);
    ENGINE->PutDrawEnv(draw);
    ENGINE->SetDispMask(1);
}

void strKickCD(void)
{
    while (!ENGINE->CdControl(0x15, &Loc, 0))
        ;
    while (!ENGINE->CdRead2(0x1c0))
        ;
}

void strInit(unsigned long *ring, void (*callback)(), int endframe)
{
    DecDCTReset(0);
    isEndOfFlag = 0;
    DecDCToutCallback(callback);
    ENGINE->StSetRing(RingBuf, 64);
    ENGINE->StSetStream(1, 1, ring, 0, endframe);
    strKickCD();
}

void strSetDefDecEnv(int x0, int y0, int x1, int y1)
{
    setRECT(&dec.rect[0], x0, y0, 480, 240);
    setRECT(&dec.rect[1], x1, y1, 480, 240);
    ENGINE->ClearImage(&dec.rect[0], 0, 0, 0);
    ENGINE->ClearImage(&dec.rect[1], 0, 0, 0);
    ENGINE->DrawSync(0);
    setRECT(&dec.slice, x0, y0, 24, 240);
    dec.vlcid = 0;
    dec.rectid = 0;
    dec.isdone = 0;
}

int strNextVlc(void)
{
    int cnt;
    unsigned long *next;

    cnt = 0x800000;
    while ((next = strNext()) == 0) {
        if (--cnt == 0)
            return -1;
    }
    if (MaxVlcSize < DecDCTBufSize(next))
        MaxVlcSize = DecDCTBufSize(next);
    dec.vlcid = dec.vlcid ^ 1;
    DecDCTvlc2(next, dec.vlcbuf[dec.vlcid], VlcTable);
    ENGINE->StFreeRing(next);
    return 0;
}

void strSync(void)
{
    volatile int cnt;

    cnt = 0x800000;
    while (!dec.isdone) {
        if (--cnt == 0) {
            dec.isdone = 1;
            dec.rectid = dec.rectid ? 0 : 1;
            dec.slice.x = dec.rect[dec.rectid].x;
            dec.slice.y = dec.rect[dec.rectid].y + YOffset;
        }
    }
    dec.isdone = 0;
}

static void MovieFreeze(void);

static short PlayMovie(char *name, int endframe, int vlcsize, int clear, int volume)
{
    long unused[12];    /* never used, but in the original's frame */
    char path[40];
    struct DISPENV disp;
    struct DRAWENV draw;
    int frame;
    int i;
    short ret;

    frame = 0;
    ret = 0;
    YOffset = 0;
#ifdef QEND_QDUMMY
    ENGINE->PrintF("Got to playstream\n");
#endif
#ifndef QEND_OLD_PLAYER     /* level_qmagextr's build doesn't take the screen position */
    ScreenY = ENGINE->GameConfig.sScreenPosY;
    ScreenX = ENGINE->GameConfig.sScreenPosX;
#endif
    ENGINE->strcpy(path, "\\Q2DATA\\");
    ENGINE->strcat(path, "MOVIES\\");
    ENGINE->strcat(path, name);
    ENGINE->strcat(path, ";1");
    ENGINE->PrintF("Play movie %s %d\n", path, endframe);
    EndFrame = endframe;
    if (vlcsize)
        VlcSize = vlcsize;
    else
        VlcSize = 0x20400;
    MaxVlcSize = 0;
#ifdef QEND_QDUMMY     /* level_qdummy's build */
    if (clear) {
        ENGINE->ClearImage(&ENGINE->Db[0].DispEnv.disp, 0, 0, 0);
        ENGINE->ClearImage(&ENGINE->Db[1].DispEnv.disp, 0, 0, 0);
        ENGINE->DrawSync(0);
    }
#else
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    if (clear) {
        ENGINE->ClearImage(&ENGINE->Db[0].DispEnv.disp, 0, 0, 0);
        ENGINE->ClearImage(&ENGINE->Db[1].DispEnv.disp, 0, 0, 0);
    }
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
#endif
    Done = 0;
    VlcTable = ENGINE->My_malloc_name(0x11000, "ring buffer");
    RingBuf = ENGINE->My_malloc_name(0x20000, "ring buffer");
    dec.vlcbuf[0] = ENGINE->My_malloc_name(VlcSize, "vlc buffer 0");
    dec.vlcbuf[1] = ENGINE->My_malloc_name(VlcSize, "vlc buffer 1");
    dec.imgbuf = ENGINE->My_malloc_name(0x5a00, "image buffer");
    DecDCTvlcBuild(VlcTable);
    IsRGB24Start = 1;
    if (!ENGINE->CdSearchFile(&File, path)) {
        ENGINE->PrintF("Movie not found\n");
        Done = 1;
    }
    strSetDefDecEnv(0, YOffset, 512, YOffset);
    Loc = File.pos;
    FileSize = File.size;
    strSetDispEnv(&disp, &draw);
    strInit((unsigned long *)0x0fffffff, strCallback, 0);
    if (!Done)
        strNextVlc();
    dec.slice.y = dec.rect[dec.rectid].y + YOffset;
    while (!isEndOfFlag && !Done) {
        frame++;
        if (frame <= 16)
            ENGINE->SetCDVolume(frame * volume / 16);
        DecDCTin(dec.vlcbuf[dec.vlcid], 3);
        DecDCTout((unsigned long *)dec.imgbuf, bound24(dec.slice.w) * bound16(dec.slice.h) >> 1);
        if (strNextVlc() < 0) {
            Done = 1;
            ret = -1;
        }
        strSync();
        ENGINE->VSync(0);
        ENGINE->NewPadRead();
        if (ENGINE->GameWindows->Input.Original & 8) {
            Done = 1;
            ret = 2;
        }
        if (ENGINE->GameWindows->Input.Original & 0x4000) {
            Done = 1;
            ret = 1;
        }
        strSetDispEnv(&disp, &draw);
    }
    for (i = 4; i > 0; i--) {
        ENGINE->SetCDVolume(i * volume / 4);
        ENGINE->VSync(0);
    }
    ENGINE->StUnSetRing();
    ENGINE->CdControlB(9, 0, 0);
    DecDCToutCallback(0);
#ifndef QEND_QDUMMY
    DecDCToutCallback(0);
#endif
    ENGINE->CdDataCallback(0);
    ENGINE->CdReadyCallback(0);
    ENGINE->MyFree(&VlcTable);
    ENGINE->MyFree(&RingBuf);
    ENGINE->MyFree(&dec.vlcbuf[0]);
    ENGINE->MyFree(&dec.vlcbuf[1]);
    ENGINE->MyFree(&dec.imgbuf);
#ifdef QEND_QDUMMY
    MovieFreeze();
    ENGINE->PrintF("Max size of each VLC buffer = %d bytes.\n\n", MaxVlcSize * 4 + 1024);
    ENGINE->DrawSync(0);
    ENGINE->PutDispEnv(&(*ENGINE->currdb)->DispEnv);
#else
    ENGINE->DrawSync(0);
    MovieFreeze();
    ENGINE->PrintF("Max size of each VLC buffer = %d bytes.\n\n", MaxVlcSize * 4 + 1024);
    ENGINE->DrawSync(0);
    ENGINE->VSync(0);
    ENGINE->PutDispEnv(&(*ENGINE->currdb)->DispEnv);
    ENGINE->VSync(0);
    ENGINE->DrawSync(0);
#endif
#ifndef QEND_OLD_PLAYER
    ENGINE->ClearOTag(ENGINE->Db[0].InitOt, 0xd9);
    ENGINE->ClearOTag(ENGINE->Db[1].InitOt, 0xd9);
#endif
    return ret;
}

/* copies the last frame (24-bit) into a 15-bit picture in the display area and fades it out */
static void MovieFreeze(void)
{
    struct RECT src;
    struct RECT dst;
    struct TILE tile;
    struct DR_MODE mode;
    struct DISPENV disp;
    struct DRAWENV draw;
    unsigned char *buf;
    int yoff;
    int x, y;
    int i;
    unsigned char *s;
    unsigned short *d;

    yoff = YOffset;
    if (dec.rectid) {
        int id = dec.rectid ? 0 : 1;

        dec.rect[id].y += yoff;
        ENGINE->MoveImage(&dec.rect[id], dec.rect[1].x, dec.rect[1].y + yoff);
        dec.rect[id].y -= yoff;
        ENGINE->DrawSync(0);
    }
    dec.rectid = 0;
    strSetDispEnv(&disp, &draw);
    setRECT(&src, dec.rect[1].x, dec.rect[1].y + YOffset, dec.rect[1].w, dec.rect[1].h);
    setRECT(&dst, dec.rect[0].x, dec.rect[0].y, dec.rect[0].w * 2 / 3, dec.rect[0].h);
    buf = ENGINE->My_malloc_name(src.w * src.h * 2, "source buf");
    ENGINE->StoreImage(&src, buf);
    ENGINE->DrawSync(0);
    s = buf;
    d = (unsigned short *)buf;
    for (y = 0; y < src.h; y++) {
        for (x = 0; x < src.w; x += 3) {
            d[0] = (s[0] >> 3) + ((s[2] & 0xf8) << 7) + ((s[1] & 0xf8) << 2);
            d[1] = (s[3] >> 3) + ((s[5] & 0xf8) << 7) + ((s[4] & 0xf8) << 2);
            s += 6;
            d += 2;
        }
    }
    i = dst.h;
    dst.h = 240;
    ENGINE->ClearImage(&dst, 0, 0, 0);
    dst.h = i;
    dst.y += yoff;
    ENGINE->LoadImage(&dst, buf);
    dst.h = 240;
    dst.y -= yoff;
    ENGINE->DrawSync(0);
    ENGINE->SetDefDispEnv(&disp, dst.x, dst.y, dst.w, dst.h);
    ENGINE->SetDefDrawEnv(&draw, dst.x, dst.y, dst.w, dst.h);
    disp.isrgb24 = 1;
    disp.isinter = 0;
    disp.screen.x = ScreenX;
    disp.screen.y = ScreenY + 8;
    ENGINE->VSync(0);
    ENGINE->PutDispEnv(&disp);
    ENGINE->PutDrawEnv(&draw);
    tile.x0 = 0;
    tile.y0 = 0;
    tile.w = dst.w;
    tile.r0 = 8;
    tile.g0 = 8;
    tile.b0 = 8;
    tile.h = dst.h;
    ENGINE->SetTile(&tile);
    tile.code = (tile.code & 0xfe) | 2;
    ENGINE->SetDrawMode(&mode, 1, 0, GetTPage(0, 2, 512, 256), 0);
    for (i = 32; i > 0; i--) {
        ENGINE->DrawSync(0);
        ENGINE->ClearOTag(ENGINE->Db->BigOt, 11);
        ENGINE->AddPrim_(ENGINE->Db->BigOt, &tile);
        ENGINE->AddPrim_(ENGINE->Db->BigOt, &mode);
        ENGINE->VSync(0);
        ENGINE->DrawOTag(ENGINE->Db->BigOt);
    }
    ENGINE->MyFree(&buf);
    ENGINE->ClearImage(&ENGINE->Db[0].DispEnv.disp, 0, 0, 0);
    ENGINE->ClearImage(&ENGINE->Db[1].DispEnv.disp, 0, 0, 0);
}

int ResetCallback(void)
{
    return ENGINE->ResetCallback();
}

void DummyUpdate(void)
{
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 10;
}

void IntroUpdate(void)
{
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 6;
}

void ExtroUpdate(void)
{
#ifdef QEND_OLD_PLAYER
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 16;
#else
    ENGINE->GameWindows->CameraRoutine = StaticCamera;
    if (*ENGINE->ChrisCount >= 4)
        *ENGINE->LevelComplete = 17;
#endif
}

void MagazineIntrUpdate(void)
{
    PictureUpdate(150, 1, 6);
}

void MagazineExtrUpdate(void)
{
    PictureUpdate(750, 0, 9);
}

/* sets the semi-transparency bit of every non-black colour */
void SetClutSemiTrans(short *clut, int n)
{
    while (n) {
        *clut &= 0x7fff;
        if (*clut)
            *clut |= 0x8000;
        n--;
        clut++;
    }
}

void LoadClut(unsigned short *clut, short slot, int is256)
{
    struct RECT rect;

    rect.x = (ENGINE->palette[slot] & 0x3f) << 4;
    rect.y = ENGINE->palette[slot] >> 6;
    rect.h = 1;
    rect.w = is256 ? 256 : 16;
    ENGINE->LoadImage(&rect, clut);
    ENGINE->DrawSync(0);
}

#ifndef QEND_QDUMMY     /* level_qdummy's build has no StaticCamera */
void StaticCamera(void)
{
    struct WINDOW *w;
    short i;

    for (i = 0; i < *ENGINE->MaxWindows; i++) {
        w = &ENGINE->GameWindows[i];
        w->CamPos.vx = 0;
        w->CamPos.vy = 0;
        w->CamPos.vz = 0;
        w->CamAng.vx = 0;
        w->CamAng.vy = 0;
        w->CamAng.vz = 0;
        w->ControlBits |= 7;
        ENGINE->QGenWindowSortList(w);
        ENGINE->UpdateWindowStuff(w);
    }
}
#endif

int DMACallback(int dma, void (*func)())
{
    return ENGINE->DMACallback(dma, func);
}

