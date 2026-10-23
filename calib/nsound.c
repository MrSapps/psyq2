#include "types.h"

unsigned long Big2Little(unsigned long v)
{
    return (v >> 24) | ((v >> 8) & 0xff00) | ((v & 0xff00) << 8) | (v << 24);
}

extern struct SoundList SampleInfoTable[];
extern struct SNDCTRL SoundControlBuffers[];
extern int LevelIsMultiPlayer;
extern char levelpath[];
extern char seq_table[];
extern unsigned char chanfreelist[];
int DefaultMix;
unsigned char *SndHdr;
unsigned char *SndBody;
int commonvab;
int NextFreeSoundNumber;
int SampleCount;
unsigned char *chanfreepntr;
struct SNDVRAM *SndVramFile;
static struct EffectStruct VAGList[97];

int SoundAtLocation_Vol(int SFXNumber, struct VECT *loc, int vol);
int SoundByCreature(int SFXNumber, struct CREATURE *Cre);
int SoundStopSoundID(short SoundID, short Channel);
int SoundTestIfRunning(short SoundID, short Channel);
void unfreechanoff(int chan);
int ReadSoundVideoFile(char *levelpath);
int DownloadVAB(unsigned char *Unused, int *VAB_BadDr);
void RemoveSndBody(void);

/* SNDCTRL.type and .chain are signed char in the original (lb, li -1/-7): types.h has plain char,
   hence the (signed char *) casts below. */

void SoundSetPitch(int SampleNum, int Pitch)
{
    if (Pitch) {
        int SampleNumber = SampleNum;
        SampleInfoTable[SampleNumber].Pitch += Pitch;
    }
}

void SoundSetRelativeVolume(int SampleNum, int Volume)
{
    if (Volume != 128) {
        int SampleNumber = SampleNum;
        SampleInfoTable[SampleNumber].Volume += Volume;
        if (SampleInfoTable[SampleNumber].Volume >= 64)
            SampleInfoTable[SampleNumber].Volume = 63;
    }
}

void SoundSetRelativePitch(int SampleNum, int Pitch)
{
    if (Pitch) {
        int SampleNumber = SampleNum;
        SampleInfoTable[SampleNumber].Pitch += Pitch;
    }
}

int SoundAtLocation(int SFXNumber, struct VECT *loc)
{
    if (SFXNumber != -1)
        if (SFXNumber)
            return SoundAtLocation_Vol(SFXNumber, loc, 16);
}

int SoundHere(int SFXNumber, struct VECT *loc, struct SOUNDINFO *SoundInfo)
{
    if (SFXNumber != -1)
        if (SFXNumber) {
            short Chan = SoundAtLocation_Vol(SFXNumber, loc, 16);
            if (Chan >= 0) {
                SoundInfo->Channel = Chan;
                SoundInfo->SoundID = SoundControlBuffers[Chan].SoundID;
            }
        }
}

int SoundByPlayer(int SFXNumber, struct CREATURE *Cre)
{
    int Chan;
    struct SNDCTRL *sbuf;
    Chan = SoundByCreature(SFXNumber, Cre);
    if (Chan >= 0) {
        sbuf = &SoundControlBuffers[Chan];
        *(signed char *)&sbuf->type = -7;
        sbuf->mixflag = 1;
        sbuf->currvol = 100;
    }
    return Chan;
}

int SoundStopOnCreDeath(short SoundID, short Channel)
{
    struct SNDCTRL *Sbuf = &SoundControlBuffers[Channel];
    if (Sbuf->SoundID == SoundID)
        Sbuf->type = 8;
}

int SoundStopAllLoopingSamples(void)
{
    struct SNDCTRL *SoundControlBuffer;
    int Channel;
    int StopCount = 0;
    SoundControlBuffer = SoundControlBuffers;
    for (Channel = 0; Channel < 24; Channel++, SoundControlBuffer++) {
        if (*(signed char *)&SoundControlBuffer->type) {
            if (SampleInfoTable[SoundControlBuffer->samplenum].Flags & 4) {
                SoundControlBuffer->type = 8;
                StopCount++;
            }
        }
    }
    return StopCount;
}

int InitSamplesForLevel(void)
{
    if (LevelIsMultiPlayer)
        DefaultMix = 1;
    else
        DefaultMix = 2;
    if (ReadSoundVideoFile(levelpath)) {
        commonvab = DownloadVAB(SndHdr, (int *)SndBody);
        RemoveSndBody();
        return 1;
    }
    return 0;
}

int SoundStop(struct SOUNDINFO *SoundInfo)
{
    return SoundStopSoundID(SoundInfo->SoundID, SoundInfo->Channel);
}

int SoundTestRunning(struct SOUNDINFO *SoundInfo)
{
    return SoundTestIfRunning(SoundInfo->SoundID, SoundInfo->Channel);
}

int SoundStopSoundID(short SoundID, short Channel)
{
    if (SoundID == 0)
        return 0;
    if (SoundControlBuffers[Channel].SoundID != SoundID)
        return 0;
    unfreechanoff(Channel);
    return 1;
}

int SoundTestIfRunning(short SoundID, short Channel)
{
    if ((unsigned short)Channel < 24)
        return SoundControlBuffers[Channel].SoundID == SoundID;
    return 0;
}

void SoundSetVolume(int SampleNum, int Volume)
{
    if (Volume != 128) {
        int SampleNumber = SampleNum;
        SampleInfoTable[SampleNumber].Volume = Volume;
    }
}

void AlterSampleAttrs(int SampleNumber, int Pitch, int Volume, int Time)
{
    if (Pitch)
        SampleInfoTable[SampleNumber].Pitch = Pitch;
    if (Volume != 128)
        SampleInfoTable[SampleNumber].Volume = Volume;
    if (Time != -1)
        SampleInfoTable[SampleNumber].Time = Time;
}

int GetSoundClone(int OriginalSoundNum)
{
    if (NextFreeSoundNumber < 128) {
        SampleInfoTable[NextFreeSoundNumber] = SampleInfoTable[OriginalSoundNum];
        NextFreeSoundNumber++;
        return NextFreeSoundNumber - 1;
    }
    return -1;
}

void SetRandomPitch(int SoundNum, int MaxPlus, int MaxMinus)
{
    SampleInfoTable[SoundNum].MaxPitch = SampleInfoTable[SoundNum].Pitch + MaxPlus;
    SampleInfoTable[SoundNum].MinPitch = SampleInfoTable[SoundNum].Pitch + MaxMinus;
    SampleInfoTable[SoundNum].Flags |= 1;
}

void InitSoundNewGame(void)
{
    SsSetTableSize(seq_table, 1, 1);
    SsSetTickMode(1);
    SsStart();
    SsUtReverbOff();
    SsUtSetReverbType(0);
    SsUtSetReverbDepth(0, 0);
    SsSetSerialAttr(0, 1, 0);
    SsSetSerialAttr(1, 1, 0);
    SsSetSerialAttr(0, 0, 1);
    SsSetSerialAttr(1, 0, 1);
}

void unfreechanoff(int chan)
{
    *--chanfreepntr = chan;
    SpuSetKey(0, 1 << chan);
    SoundControlBuffers[chan].type = 0;
    SoundControlBuffers[chan].s_dtype = 0xff;
    SoundControlBuffers[chan].SoundID = 0;
}

static int VagCount;
static char *SoundNames;
static int CommonVagCount;
char *CreatureVagPntr;
void *memset(void *s, int c, unsigned int n);
char *strcpy(char *d, const char *s);
long SfxTransEffect(void *pVagPtr, void *pImgPtr);
void InitSampleInfoTable(int First, int Count);
int FlagIfLoopingSample(char *VagPntr, int SampleIndex);

int DownloadVAB(unsigned char *Unused, int *VAB_BadDr)
{
    int a;
    char *pntr;

    if (!VAB_BadDr)
        return 0;
    VagCount = VAB_BadDr[0] / 4 - 1;
    if (VagCount > 96)
        VagCount = 96;
    SampleCount = VagCount + 1;
    CommonVagCount = VagCount;
    memset(SoundNames, 0, 0x60c);
    strcpy(SoundNames, "Null sound");
    InitSampleInfoTable(0, 128);
    for (a = 0; a < VagCount; a++) {
        pntr = (char *)VAB_BadDr + VAB_BadDr[a];
        FlagIfLoopingSample(pntr, a);
        SfxTransEffect(pntr, 0);
        strcpy(SoundNames + (a + 1) * 12, pntr + 0x20);
    }
    pntr = (char *)VAB_BadDr + VAB_BadDr[a];
    CreatureVagPntr = pntr;
    return 0;
}

void InitSampleInfoTable(int First, int Count)
{
    int a;
    for (a = First; a < First + Count; a++) {
        if (a && a < SampleCount)
            SampleInfoTable[a].SampleNumber = a - 1;
        else
            SampleInfoTable[a].SampleNumber = 0;
        SampleInfoTable[a].Pitch = 35;
        SampleInfoTable[a].Volume = 63;
        SampleInfoTable[a].Time = 200;
        SampleInfoTable[a].MinPitch = 35;
        SampleInfoTable[a].MaxPitch = 35;
        SampleInfoTable[a].MaxVol = 127;
        SampleInfoTable[a].MinVol = 63;
        SampleInfoTable[a].Flags = 0;
    }
    NextFreeSoundNumber = SampleCount;
}

int SfxFreeEffect(long VagNumber)
{
    if (!VAGList[VagNumber].dwAddr)
        return -1;
    if (VAGList[VagNumber].dwImgAddr) {
        SpuFree(VAGList[VagNumber].dwImgAddr);
        VAGList[VagNumber].dwImgSize = 0;
        VAGList[VagNumber].dwMulti = 0;
    }
    SpuFree(VAGList[VagNumber].dwAddr);
    VAGList[VagNumber].dwSize = 0;
    VAGList[VagNumber].dwMulti = 0;
    return 1;
}

/* static in the original (only called from functions not in this file) */
int SFXCurrDist;
extern struct VECTOR SFXRotCoord;
void CalcSFXCurrDist(struct VECT *loc);
void SendPitch(int chan, int pitch);

void ReCalcPitch(int chan, struct SNDCTRL *sbuf)
{
    struct SpuVoiceAttr voiceAttr;
    int dwMulti;
    int d;

    dwMulti = VAGList[sbuf->samplenum].dwMulti;
    switch (sbuf->pitchflag) {
    case 0:
        break;
    case 1:
    default:
        if (sbuf->currpitch != sbuf->lastpitch) {
            sbuf->lastpitch = sbuf->currpitch;
            SendPitch(chan, sbuf->currpitch * dwMulti / 256);
        }
        break;
    case 2:
        if (!SFXCurrDist) {
            if (*(signed char *)&sbuf->type == 1)
                CalcSFXCurrDist(&((struct CREATURE *)sbuf->pntr)->Draw.Location);
            else
                CalcSFXCurrDist(sbuf->pntr);
        }
        d = sbuf->lastdist - SFXCurrDist;
        sbuf->lastdist = SFXCurrDist;
        sbuf->currpitch = sbuf->basepitch + d / 256;
        if (sbuf->currpitch != sbuf->lastpitch) {
            sbuf->lastpitch = sbuf->currpitch;
            SendPitch(chan, sbuf->currpitch * dwMulti / 256);
        }
        break;
    }
}

void SendPitch(int chan, int pitch)
{
    static struct SpuVoiceAttr voiceAttr;
    voiceAttr.voice = 1 << chan;
    voiceAttr.mask = 0x10;
    voiceAttr.pitch = pitch;
    SpuSetVoiceAttr(&voiceAttr);
}

void RemoveSndBody(void)
{
    if (SndVramFile)
        SndVramFile = (struct SNDVRAM *)MYrealloc(SndVramFile, SndVramFile->vabbody, 0);
}

int FlagIfLoopingSample(char *VagPntr, int SampleIndex)
{
    if ((VagPntr[0x41] & 2) && (VagPntr[0x51] & 2)) {
        SampleInfoTable[SampleIndex].Flags |= 4;
        return 1;
    }
    return 0;
}



int SFXMasterVolume;
extern int *AllIffPics;
int Read_File_Alloc(char *path, char *name2, void **Pntr);
void MyFreeRep(void **Addr, char *Name);

/* SNDCTRL with the signed char type/chain fields of the original */
struct SNDCTRL_S {
    void *pntr;
    struct VECT pos;
    int lastdist;
    signed char type;
    signed char chain;
    char pad[0x38 - 0x16];
};

int MYSFXInit(void)
{
    int a;
    SFXMasterVolume = 256;
    chanfreepntr = chanfreelist;
    for (a = 0; a < 24; a++) {
        chanfreelist[a] = a;
        ((struct SNDCTRL_S *)SoundControlBuffers)[a].type = 0;
        ((struct SNDCTRL_S *)SoundControlBuffers)[a].chain = -1;
    }
    return 1;
}

void freesound(void)
{
    int v;
    for (v = VagCount - 1; v >= 0; v--)
        SfxFreeEffect(v);
    MyFreeRep((void **)&SndVramFile, "SndVram");
}

int ReadSoundVideoFile(char *levelpath)
{
    int a;
    a = Read_File_Alloc(levelpath, "sndvram.dat", (void **)&SndVramFile);
    if (a) {
        AllIffPics = (int *)((char *)SndVramFile + SndVramFile->vramhdr);
        SndBody = (unsigned char *)SndVramFile + SndVramFile->vabbody;
    } else {
        AllIffPics = 0;
        SndHdr = 0;
        SndBody = 0;
    }
    return a;
}

/* The original's bound is &chanfreelist[24], which the linker resolves to the next symbol, NameBuffer
   (same address); written against NameBuffer so cmpfunc's relocation check passes. */
char NameBuffer[16];

int GetFreeChannel(void)
{
    if (chanfreepntr < (unsigned char *)NameBuffer) {
        chanfreepntr++;
        return chanfreepntr[-1];
    }
    return -1;
}

void SpuSetKey(long on_off, unsigned long voice_bit);

void killallsounds(void)
{
    SpuSetKey(0, 0xffffff);
    MYSFXInit();
}

static char pbSpuMallocRec[1032];
void SpuInit(void);
long SpuInitMalloc(long num, char *top);
void SpuSetCommonAttr(struct SpuCommonAttr *attr);
long SpuSetTransferMode(long mode);

void SfxInit(unsigned char bMode)
{
    struct SpuCommonAttr commonAttr;
    int wSampleId;

    MYSFXInit();
    SpuInit();
    SpuInitMalloc(128, pbSpuMallocRec);
    commonAttr.mask = 3;
    commonAttr.mvol.left = 0x3fc0;
    commonAttr.mvol.right = 0x3fc0;
    SpuSetCommonAttr(&commonAttr);
    SpuSetTransferMode(0);
    for (wSampleId = 0; wSampleId < 97; wSampleId++) {
        VAGList[wSampleId].dwAddr = 0;
        VAGList[wSampleId].dwSize = 0;
        VAGList[wSampleId].dwImgAddr = 0;
        VAGList[wSampleId].dwImgSize = 0;
    }
}

void *MYrealloc(void *p, int s, int HiFlag);
void MyFree(void **Addr);
void *MY_malloc_namehb(int s, char *name);
void *memcpy(void *dst, const void *src, unsigned int n);

void RemoveVram(void)
{
    char *Temp;
    int Length;

    if (SndVramFile) {
        Length = SndVramFile->vramhdr;
        SndVramFile = MYrealloc(SndVramFile, Length, 1);
        Temp = MY_malloc_namehb(Length, "SndVram -- High");
        if ((char *)SndVramFile < Temp) {
            memcpy(Temp, SndVramFile, Length);
            MyFree((void **)&SndVramFile);
            SndVramFile = (struct SNDVRAM *)Temp;
        } else
            MyFree((void **)&Temp);
    }
}


extern int MaxWindows;
extern struct WINDOW GameWindows[];
void Rotate(struct MATRIX *m, struct SVECT *v1, struct VECT *v2);
long SquareRoot0(long a);
struct VECTOR SFXRotCoord;

void CalcSFXCurrDist(struct VECT *loc)
{
    struct SVECTOR s;
    int x;
    int z;
    int a;

    SFXCurrDist = 0;
    for (a = 0; a < MaxWindows; a++) {
        s.vx = loc->vx - GameWindows[a].CamPos.vx;
        s.vy = loc->vy - GameWindows[a].CamPos.vy;
        s.vz = loc->vz - GameWindows[a].CamPos.vz;
        Rotate(&GameWindows[a].CamMatNorm, (struct SVECT *)&s, (struct VECT *)&SFXRotCoord);
        x = SFXRotCoord.vx;
        z = SFXRotCoord.vz;
        SFXCurrDist += SquareRoot0(x * x + z * z);
    }
    SFXCurrDist /= MaxWindows;
    if (SFXCurrDist <= 0)
        SFXCurrDist = 1;
}
