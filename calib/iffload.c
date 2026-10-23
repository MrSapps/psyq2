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

char *strcpy();
char *strcat();
char *strupr(char *s);
long GetFLen(char *name);
long LoadFile(char *name, char *buffer);
void *MYrealloc(void *p, int s, int HiFlag);
void *MY_malloc_name(int s, char *name);
void *MY_malloc_namehb(int s, char *name);
int MYPCopen(char *name);
int MYPCclose(int h);
int MYPClseek(int h, int offset, int mode);

/* inline helpers: SYM shows them as nested blocks of Read_File_Alloc2 (no out-of-line copies exist) */
static int CdFileLen(char *name)
{
    char name2[100];

    strcpy(name2, name);
    strcat(name2, ";1");
    strupr(name2);
    return GetFLen(name2);
}

static int PcSeekEnd(int h)
{
    return PClseek(h, 0, 2);
}

static int PcSeekStart(int h)
{
    return PClseek(h, 0, 0);
}

static int PcClose(int h)
{
    return PCclose(h);
}

static int PcFileLen(char *name)
{
    int h;
    char name2[100];
    int len;

    strcpy(name2, name);
    h = PCopen(name2, 0, 0);
    if (h >= 0) {
        len = PcSeekEnd(h);
        PcSeekStart(h);
        PcClose(h);
        return len;
    }
    return 0;
}

static int FileLen(char *Name)
{
    if (UseCdData)
        return CdFileLen(Name);
    return PcFileLen(Name);
}

static void CdLoad(char *buffer, char *name)
{
    char name2[100];

    strcpy(name2, name);
    strcat(name2, ";1");
    strupr(name2);
    LoadFile(name2, buffer);
}

static void PcReadAll(int h, char *dest, int len)
{
    int pos;
    int lr;

    lr = -1;
    pos = PClseek(h, 0, 1);
    while (lr != len) {
        PClseek(h, pos, 0);
        lr = PCread(h, dest, len);
    }
}

static void PcLoad(char *buffer, char *name)
{
    int h;
    int len;

    h = MYPCopen(name);
    len = MYPClseek(h, 0, 2);
    MYPClseek(h, 0, 0);
    PcReadAll(h, buffer, len);
    MYPCclose(h);
}

static void LoadToBuffer(char *Name, char *Buffer)
{
    if (UseCdData)
        CdLoad(Buffer, Name);
    else
        PcLoad(Buffer, Name);
}

int Read_File_Alloc2(char *path, char *name2, void **Pntr, int hiflag)
{
    int c;
    char name[100];
    short *store2;

    strcpy(name, path);
    strcat(name, name2);
    c = FileLen(name);
    if (c) {
        if (hiflag)
            store2 = MY_malloc_namehb((c + 2047) & ~2047, name2);
        else
            store2 = MY_malloc_name((c + 2047) & ~2047, name2);
        if (store2) {
            LoadToBuffer(name, (char *)store2);
            store2 = MYrealloc(store2, c, hiflag);
            *Pntr = store2;
        } else
            c = 0;
    }
    return c;
}

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
