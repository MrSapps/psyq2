#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void InitCDFS(void);
void PlayTrackList(char *list);
void PlayThisXA(int XANumber);
void StopPlayingCD(void);
void RestartXA(void);
void SetCDVolume(int a);
void GetToc(void);
void SeekAndPlay(void);
void CdCheckMusic(void);
extern struct QENGINELEVINTERFACE Engine;
extern int cdtime;
int MusicCheck;
char *CDAudioTrackList;
void GetToc(void);
void StopPlayingCD(void);

extern int UseCdMusic;
extern int cdtime2;
int ntoc;
int MusicTrack;
short trkvolboost;
short totalxaifiles;
struct TRKINFO trackinfo[22];
static struct CdlLOC loc[16];
static struct CdlLOC xailoc[20];
int CdControlB(unsigned char com, unsigned char *param, unsigned char *result);
void *memset(void *, int, int);

short targetcdvol;
short actcdvol;
int ReadError;


extern char *xaifilename[6];
int CdGetToc(struct CdlLOC *loc);
int CdPosToInt(struct CdlLOC *p);
struct CdlLOC *CdIntToPos(int i, struct CdlLOC *p);
struct CdlFILE *CdSearchFile(struct CdlFILE *fp, char *name);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);

void GetToc(void)
{
    unsigned char param[4];
    int i;
    struct CdlFILE file;
    char name2[64];
    struct CdlATV Vol;

    param[0] = 5;
    CdControlB(0xe, param, 0);
    ntoc = CdGetToc(loc);
    for (i = 1; i < ntoc; i++) {
        CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]);
        loc[i].sector += 0x26;
        loc[i].second++;
        if ((loc[i].second & 0xf) >= 10)
            loc[i].second += 6;
        if ((loc[i].second & 0xf0) > 0x50) {
            loc[i].second &= 0xf;
            loc[i].minute++;
            if ((loc[i].minute & 0xf) >= 10)
                loc[i].minute += 6;
        }
    }
    for (i = 0; xaifilename[i]; i++) {
        strcpy(name2, "\\Q2DATA\\");
        strcat(name2, "AUD\\");
        strcat(name2, xaifilename[i]);
        strcat(name2, ";1");
        if (CdSearchFile(&file, name2))
            xailoc[i] = file.pos;
        else
            *(int *)&xailoc[i] = 0;
    }
    totalxaifiles = i;
    CdSetDebug(0);
}

void PlayTrackList(char *list)
{
    unsigned char param[4];
    cdtime = 0;
    MusicCheck = 1;
    if (list)
        CDAudioTrackList = list - 1;
}

void InitCDFS(void)
{
    struct SpuCommonAttr commonAttr;
    unsigned char pbParam[8];
    commonAttr.mask = 0x3c0;
    commonAttr.cd.mix = 1;
    commonAttr.cd.reverb = 0;
    commonAttr.cd.volume.left = 0x7fff;
    commonAttr.cd.volume.right = 0x7fff;
    SpuSetCommonAttr(&commonAttr);
    CdInit();
    CdSetDebug(0);
    GetToc();
}

void SeekAndPlay(void)
{
    unsigned char param[4] = {0};
    unsigned char result[8];
    struct CdlFILTER filt;
    int trktype;
    short error;

    error = 1;
    if (UseCdMusic) {
        trkvolboost = 0;
        trktype = (signed char)trackinfo[MusicTrack].id;
        if (trktype >= 0) {
            if (*(int *)&xailoc[trktype]) {
                if (trktype < totalxaifiles) {
                    param[0] = 0x48;
                    CdControlB(0xe, param, 0);
                    filt.file = 1;
                    filt.chan = trackinfo[MusicTrack].chan;
                    CdControlB(0xd, (unsigned char *)&filt, 0);
                    CdControlB(0x1b, (unsigned char *)&xailoc[trktype], result);
                    error = 0;
                }
            }
        } else if (MusicTrack <= ntoc) {
            param[0] = 5;
            CdControlB(0xe, param, 0);
            CdControlB(3, (unsigned char *)&loc[MusicTrack], result);
            error = 0;
        }
        if (error) {
            CdControlB(8, 0, 0);
            cdtime = 300;
        } else
            cdtime = trackinfo[MusicTrack].len;
        MusicCheck = 1;
        cdtime = cdtime * 6;
        cdtime2 = cdtime;
    }
}

void SetCDVolume(int a)
{
    struct CdlATV cdVol;
    if (Engine.GameConfig.bStereo) {
        cdVol.val0 = a;
        cdVol.val1 = 0;
        cdVol.val2 = a;
        cdVol.val3 = 0;
    } else {
        cdVol.val0 = a / 2;
        cdVol.val1 = a / 2;
        cdVol.val2 = a / 2;
        cdVol.val3 = a / 2;
    }
    CdMix(&cdVol);
}

void CdCheckMusic(void)
{
    int b;
    int a;

    if (!UseCdMusic)
        return;
    if (actcdvol < targetcdvol) {
        actcdvol += 4;
        if (actcdvol > targetcdvol)
            actcdvol = targetcdvol;
    } else if (actcdvol > targetcdvol) {
        actcdvol -= 4;
        if (actcdvol < targetcdvol)
            actcdvol = targetcdvol;
    }
    b = cdtime;
    a = actcdvol;
    if (b < 64)
        a = a * b / 64;
    b = cdtime2 - b;
    if (b < 64)
        a = a * b / 64;
    if (trkvolboost) {
        a = a * trkvolboost / 256;
        if (a > 255)
            a = 255;
    }
    SetCDVolume(a);
    if (MusicCheck) {
        if (ReadError == 1) {
            ReadError = 0;
            SeekAndPlay();
        } else if (cdtime <= 0) {
            if (!*(signed char *)++CDAudioTrackList)
                StopPlayingCD();
            else {
                if (*(signed char *)CDAudioTrackList < 0)
                    CDAudioTrackList += *(signed char *)CDAudioTrackList;
                MusicTrack = *(signed char *)CDAudioTrackList;
                if (UseCdMusic)
                    SeekAndPlay();
            }
        }
    }
}

void StopPlayingCD(void)
{
    CdControl(9, 0, 0);
    MusicCheck = 0;
}

void PlayThisXA(int XANumber)
{
    static char Trax[2];
    return;
    {
        unsigned char param[4];
        param[0] = Trax[XANumber];
        CdControl(0xd, param, 0);
    }
}

void RestartXA(void)
{
}
