#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern int cdtime;
int MusicCheck;
char *CDAudioTrackList;
void GetToc(void);

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

void PlayTrackList(char *list)
{
    unsigned char param[4];
    cdtime = 0;
    MusicCheck = 1;
    if (list)
        CDAudioTrackList = list - 1;
}

void StopPlayingCD(void)
{
    CdControl(9, 0, 0);
    MusicCheck = 0;
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
