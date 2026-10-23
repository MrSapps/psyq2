#include "types.h"
extern int UseCdData;
extern int UseCdMusic;
extern int DebugWatch;
extern short tpscreenx[];
extern short tpscreeny[];
extern short tppalx[];
extern short tppaly[];
extern short *screen256pal;
extern struct RECT desrect;
char *LoadIffToRam_ConCat_NewMode(char *loadname, struct HEADERTYPE *Header, short *PSXColours);
int Read_File_Alloc2(char *path, char *name2, void **Pntr, int hiflag);
int LoadTpageInternal(char *name, int tpx, int tpy, int palx, int paly, int palflag, int skipheight);
void InitCDFS(void);
void GetToc(void);

int MYPCopen(char *name)
{
    char name2[100];
    strcpy(name2, name);
    return PCopen(name2, 0, 0);
}

int MYPCinit(void)
{
    if (UseCdData || UseCdMusic)
        InitCDFS();
}

int MYPCclose(int h)
{
    return PCclose(h);
}

int MYPClseek(int h, int offset, int mode)
{
    return PClseek(h, offset, mode);
}

int LoadPicture256(char *Name, int TPage)
{
    DebugWatch = 225;
    return LoadTpageInternal(Name, tpscreenx[TPage], tpscreeny[TPage], 0, 383 - TPage, 2, 0);
}

int loadtp16b_ConCat(char *name, int tpnum, int skipheight)
{
    return LoadTpageInternal(name, tpscreenx[tpnum], tpscreeny[tpnum], tppalx[tpnum], tppaly[tpnum], 2, skipheight);
}

int Read_File_Alloc(char *path, char *name2, void **Pntr)
{
    return Read_File_Alloc2(path, name2, Pntr, 0);
}

int MyGetCDStatus(void)
{
    unsigned char result[8];
    CdControlB(1, 0, result);
    if (result[0] & 0x10)
        return 0;
    if (result[0] & 2)
        return 2;
    return 1;
}

void ReReadToc(void)
{
    unsigned char result[8];
    unsigned char com;
    struct CdlLOC pos;
    CdControlB(0xe, &com, result);
    while (MyGetCDStatus() != 2)
        ;
    while (!CdControlB(0x13, 0, result))
        ;
    com = 0x80;
    CdControlB(0xe, &com, result);
    VSync(3);
    GetToc();
}

int LoadTpageInternal(char *name, int tpx, int tpy, int palx, int paly, int palflag, int skipheight)
{
    struct HEADERTYPE Header;
    char *tempbuf;
    tempbuf = LoadIffToRam_ConCat_NewMode(name, &Header, screen256pal);
    if (tempbuf) {
        desrect.x = tpx;
        desrect.y = tpy + skipheight;
        desrect.w = Header.Width >> 1;
        desrect.h = Header.Height;
        LoadImage(&desrect, tempbuf);
        DrawSync(0);
        MyFree(&tempbuf);
        return 1;
    }
    return 0;
}
