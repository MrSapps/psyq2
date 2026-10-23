#include "types.h"

/* libspu functions return void (an int return value in v0 changes DoMix's register allocation) */
void SpuSetVoiceAttr(struct SpuVoiceAttr *arg);
void SpuGetVoiceAttr(struct SpuVoiceAttr *arg);
void SpuSetKey(long on_off, unsigned long voice_bit);

unsigned long Big2Little(unsigned long v);
long SpuMalloc(long size);
void SpuSetTransferStartAddr(unsigned long addr);
unsigned long SpuWrite(unsigned char *addr, unsigned long size);
long SpuIsTransferCompleted(long flag);
static struct EffectStruct VAGList[97];

/* before Big2Little, which the original calls rather than inlines */
long SfxTransEffect(void *pVagPtr, void *pImgPtr)
{
    long dwSize;
    long dwAddr;
    long dwSrate;
    long dwMulti;
    int wSampleId;
    struct VagHdr *pHeader;

    pHeader = pVagPtr;
    dwSize = Big2Little(pHeader->size);
    dwAddr = SpuMalloc(dwSize);
    if (dwAddr == -1)
        return -2;
    wSampleId = 0;
    while (VAGList[wSampleId].dwSize) {
        if (++wSampleId == 96)
            return -3;
    }
    dwSrate = Big2Little(pHeader->srate);
    VAGList[wSampleId].dwAddr = dwAddr;
    VAGList[wSampleId].dwSize = dwSize;
    VAGList[wSampleId].dwMulti = (dwSrate << 12) / 44100;
    SpuSetTransferStartAddr(dwAddr);
    SpuWrite((unsigned char *)(pHeader + 1), dwSize);
    SpuIsTransferCompleted(1);
    if (pImgPtr) {
        pHeader = pImgPtr;
        dwSize = Big2Little(pHeader->size);
        dwAddr = SpuMalloc(dwSize);
        if (dwAddr == -1)
            return -2;
        if (Big2Little(pHeader->srate) != dwSrate)
            return -4;
        VAGList[wSampleId].dwImgAddr = dwAddr;
        VAGList[wSampleId].dwImgSize = dwSize;
        SpuSetTransferStartAddr(dwAddr);
        SpuWrite((unsigned char *)(pHeader + 1), dwSize);
        SpuIsTransferCompleted(1);
    }
    return wSampleId;
}


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

extern int Paused;
extern unsigned int LevelTicks;
extern struct CREATURE Creatures[];
int GetFreeChannel(void);
int rand(void);

/* SoundAtLocation_Vol and SoundByCreature share SoundID (same SYM section offset): an inline helper */
static unsigned short NewSoundID(void)
{
    {
        static unsigned short SoundID = 0;
        SoundID = (SoundID + 1) & 0x7fff;
        if (!SoundID)
            SoundID++;
        return SoundID;
    }
}

/* the VAGList test guards the body (return -2 last): with an early `return -2` the delay slot after
   `bnez` on SoundID gets filled, which the original does not do */
int SoundAtLocation_Vol(int SFXNumber, struct VECT *loc, int vol)
{
    int chan;
    struct SNDCTRL *sbuf;
    struct SoundList *ThisSound;

    if (SFXNumber == -1)
        return -1;
    if (!SFXNumber)
        return -1;
    ThisSound = &SampleInfoTable[SFXNumber];
    if (VAGList[ThisSound->SampleNumber].dwAddr) {
        chan = GetFreeChannel();
        if (chan >= 0) {
            sbuf = &SoundControlBuffers[chan];
            sbuf->s_dtype = SFXNumber;
            sbuf->pos.vx = loc->vx;
            sbuf->pos.vy = loc->vy;
            sbuf->pos.vz = loc->vz;
            *(signed char *)&sbuf->type = -3;
            *(signed char *)&sbuf->chain = -1;
            sbuf->lastpitch = sbuf->currpitch = sbuf->basepitch = ThisSound->Pitch << 3;
            sbuf->lastvol = sbuf->currvol = sbuf->basevol = ThisSound->Volume * vol >> 4;
            sbuf->mixflag = DefaultMix;
            sbuf->pitchflag = 0;
            sbuf->samplenum = ThisSound->SampleNumber;
            sbuf->statusdelay = 5;
            sbuf->SoundID = NewSoundID();
        }
        return chan;
    }
    return -2;
}

int SoundByCreature(int SFXNumber, struct CREATURE *Cre)
{
    int Chan;
    int Range;
    struct SNDCTRL *sbuf;
    struct SoundList *ThisSound;

    if (SFXNumber == -1)
        return -1;
    if (!SFXNumber)
        return -1;
    ThisSound = &SampleInfoTable[SFXNumber];
    if (!VAGList[ThisSound->SampleNumber].dwAddr)
        return -2;
    Chan = GetFreeChannel();
    if (Chan >= 0) {
        if (ThisSound->Flags & 1) {
            Range = ThisSound->MaxPitch - ThisSound->MinPitch;
            ThisSound->Pitch = ThisSound->MinPitch + rand() % Range;
        }
        if (ThisSound->Flags & 2) {
            Range = ThisSound->MaxVol - ThisSound->MinVol;
            ThisSound->Volume = ThisSound->MinVol + rand() % Range;
        }
        sbuf = &SoundControlBuffers[Chan];
        sbuf->s_dtype = SFXNumber;
        sbuf->pos.vx = Cre->Box.Location.vx;
        sbuf->pos.vy = Cre->Box.Location.vy;
        sbuf->pos.vz = Cre->Box.Location.vz;
        *(signed char *)&sbuf->type = -1;
        sbuf->mixflag = DefaultMix;
        sbuf->lastvol = sbuf->currvol = sbuf->basevol = ThisSound->Volume;
        if (LevelIsMultiPlayer) {
            *(signed char *)&sbuf->type = -7;
            sbuf->mixflag = 1;
            if (Paused) {
                sbuf->currvol = sbuf->basevol = 0xfe;
                sbuf->mixflag = 2;
            } else {
                sbuf->currvol = sbuf->basevol = 0x54;
            }
        } else if (Cre <= &Creatures[1]) {
            *(signed char *)&sbuf->type = -7;
            sbuf->mixflag = 1;
            sbuf->currvol = sbuf->basevol = 0x7f;
        }
        *(signed char *)&sbuf->chain = -1;
        sbuf->lastpitch = sbuf->currpitch = sbuf->basepitch = ThisSound->Pitch << 3;
        sbuf->pitchflag = 0;
        sbuf->samplenum = ThisSound->SampleNumber;
        sbuf->statusdelay = 5;
        sbuf->CreIdent = Cre->Ident;
        sbuf->pntr = Cre;
        sbuf->SoundID = NewSoundID();
        if (!Cre->SoundFrame)
            sbuf->SoundEndFrame = LevelTicks + 9000;
        else
            sbuf->SoundEndFrame = LevelTicks + Cre->SoundFrame;
        Cre->SoundID = sbuf->SoundID;
        Cre->SoundChannel = Chan;
    }
    return Chan;
}

int SoundStopOnCreDeath(short SoundID, short Channel)
{
    struct SNDCTRL *Sbuf = &SoundControlBuffers[Channel];
    if (Sbuf->SoundID == SoundID)
        Sbuf->type = 8;
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

int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
char *PrintIdentityName(struct IDENTITYNAME Name);

int GetSoundNum(struct IDENTITYNAME Name)
{
    struct IDENTITYNAME *SoundName;
    int SFXNumber;

    SoundName = (struct IDENTITYNAME *)SoundNames;
    SFXNumber = 0;
    while (1) {
        switch (ModNameCmp(&Name, SoundName)) {
        case 0:
            SoundName++;
            if (++SFXNumber == SampleCount) {
                PrintIdentityName(Name);
                return 0;
            }
            break;
        case 1:
            PrintIdentityName(Name);
            return SFXNumber;
        case 2:
            PrintIdentityName(Name);
            return 0;
        }
    }
}

extern unsigned char pbStereoLookup[];
extern unsigned char pbRearLookup[];
extern unsigned char pbQPanLookup[];

/* case 2/3 and the two-pass cases share their clamp tails by cross-jumping; the then-branch's
   SpuSetVoiceAttr call is merged with the else-branch's */
void DoMix(int chan, struct SNDCTRL *sbuf)
{
    struct SpuVoiceAttr voiceAttr;
    int pan;
    unsigned int wLeftVol;
    unsigned int wRightVol;
    int fbpan;
    int lrvol;
    int rearvol;

    wRightVol = wLeftVol = 0;
    if (*(signed char *)&sbuf->mixflag > 0) {
        voiceAttr.mask = 15;
        voiceAttr.volmode.left = 0;
        voiceAttr.volmode.right = 0;
        switch (*(signed char *)&sbuf->mixflag) {
        case 1:
            wLeftVol = sbuf->currvol * SFXMasterVolume / 2;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            wRightVol = wLeftVol;
            break;
        case 2:
            wLeftVol = sbuf->currvol * pbStereoLookup[sbuf->lrpan] * SFXMasterVolume / 512;
            wRightVol = sbuf->currvol * pbStereoLookup[30 - sbuf->lrpan] * SFXMasterVolume / 512;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            break;
        case 3:
            wLeftVol = sbuf->currvol * pbStereoLookup[sbuf->lrpan] / 4;
            wRightVol = sbuf->currvol * pbStereoLookup[30 - sbuf->lrpan] / 4;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            break;
        case 4:
            wLeftVol = sbuf->currvol * SFXMasterVolume / 2;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            wRightVol = -wLeftVol;
            break;
        case 5:
            lrvol = sbuf->currvol;
            fbpan = sbuf->fbpan;
            if (fbpan < 15) {
                rearvol = lrvol;
                lrvol = lrvol * fbpan;
                lrvol = lrvol / 15;
            } else if (fbpan > 15) {
                rearvol = lrvol * (31 - fbpan) / 15;
            } else {
                rearvol = lrvol;
            }
            wLeftVol = rearvol * pbRearLookup[sbuf->lrpan] * SFXMasterVolume / 512;
            wRightVol = rearvol * pbRearLookup[30 - sbuf->lrpan] * SFXMasterVolume / 512;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            voiceAttr.voice = 1 << sbuf->channel2;
            voiceAttr.volume.left = wLeftVol;
            voiceAttr.volume.right = wRightVol;
            SpuSetVoiceAttr(&voiceAttr);
            wLeftVol = lrvol * pbStereoLookup[sbuf->lrpan] * SFXMasterVolume / 512;
            wRightVol = pbStereoLookup[30 - sbuf->lrpan] * lrvol * SFXMasterVolume / 512;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            break;
        case 6:
            pan = sbuf->lrpan;
            lrvol = sbuf->currvol;
            pan <<= 2;
            wLeftVol = *(pbQPanLookup + (pan + 2)) * lrvol * SFXMasterVolume / 512;
            wRightVol = *(pbQPanLookup + (pan + 3)) * lrvol * SFXMasterVolume / 512;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            voiceAttr.voice = 1 << sbuf->channel2;
            voiceAttr.volume.left = wLeftVol;
            voiceAttr.volume.right = wRightVol;
            SpuSetVoiceAttr(&voiceAttr);
            wLeftVol = *(pbQPanLookup + pan) * lrvol * SFXMasterVolume / 512;
            wRightVol = *(pbQPanLookup + (pan + 1)) * lrvol * SFXMasterVolume / 512;
            if (wLeftVol > 0x55F0)
                wLeftVol = 0x55F0;
            if (wRightVol > 0x55F0)
                wRightVol = 0x55F0;
            break;
        }
        voiceAttr.voice = 1 << chan;
        voiceAttr.volume.left = wLeftVol;
        voiceAttr.volume.right = wRightVol;
        SpuSetVoiceAttr(&voiceAttr);
    } else {
        voiceAttr.voice = 1 << chan;
        SpuGetVoiceAttr(&voiceAttr);
        voiceAttr.volume.left -= 256;
        if (voiceAttr.volume.left < 0)
            voiceAttr.volume.left = 0;
        voiceAttr.volume.right -= 256;
        if (voiceAttr.volume.right < 0)
            voiceAttr.volume.right = 0;
        SpuSetVoiceAttr(&voiceAttr);
    }
}


extern struct QENGINELEVINTERFACE Engine;
extern struct VECTOR SFXRotCoord;
static int Muted;
int ttt;
long SpuGetKeyStatus(unsigned long voice_bit);
char *strncpy(char *d, const char *s, unsigned int n);
void SpriText();

static void SetVolPan(struct SNDCTRL *sbuf)
{
    {
        int vol;
        int pan;
        vol = 0x3000 - SFXCurrDist;
        vol = vol * sbuf->basevol;
        vol = vol / 4096;
        if (vol > 255)
            vol = 255;
        if (vol < 0)
            vol = 0;
        sbuf->currvol = vol;
        pan = SFXRotCoord.vx * 15 / SFXCurrDist + 15;
        sbuf->lrpan = pan;
        pan = SFXRotCoord.vz * 15 / SFXCurrDist + 15;
        sbuf->fbpan = pan;
        if (Paused) {
            sbuf->currvol = 255;
            sbuf->lrpan = 15;
            sbuf->fbpan = 15;
        }
    }
}

void ProcessSounds(void)
{
    struct SpuVoiceAttr voiceAttr;
    struct SNDCTRL *SoundControlBuffer;
    int Channel;
    int wStatus;
    struct CREATURE *Cre;
    int sendkeymask;
    int newsound;

    SoundControlBuffer = SoundControlBuffers;
    sendkeymask = 0;
    if (Engine.GameConfig.bStereo)
        DefaultMix = 2;
    else
        DefaultMix = 1;
    if (SFXMasterVolume != Engine.GameConfig.sSoundFX) {
        SFXMasterVolume = Engine.GameConfig.sSoundFX;
        SoundStopAllLoopingSamples();
    }
    for (Channel = 0; Channel < 24; SoundControlBuffer++, Channel++) {
        if (!*(signed char *)&SoundControlBuffer->type)
            continue;
        SFXCurrDist = 0;
        newsound = 0;
        if (*(signed char *)&SoundControlBuffer->type < 0) {
            newsound = 1;
            *(signed char *)&SoundControlBuffer->type = -*(signed char *)&SoundControlBuffer->type;
        }
        switch (*(signed char *)&SoundControlBuffer->type) {
        case 1:
            if (!SoundControlBuffer->pntr)
                break;
            Cre = SoundControlBuffer->pntr;
            CalcSFXCurrDist(&Cre->Draw.Location);
            SetVolPan(SoundControlBuffer);
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
            if (Cre->Life <= 0 || LevelTicks >= SoundControlBuffer->SoundEndFrame || Cre->Ident != SoundControlBuffer->CreIdent)
                unfreechanoff(Channel);
            break;
        case 7:
            if (!SoundControlBuffer->pntr)
                break;
            Cre = SoundControlBuffer->pntr;
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
            if (Cre->Life <= 0 || LevelTicks >= SoundControlBuffer->SoundEndFrame || Cre->Ident != SoundControlBuffer->CreIdent)
                unfreechanoff(Channel);
            break;
        case 2:
            if (!SoundControlBuffer->pntr)
                break;
            CalcSFXCurrDist(SoundControlBuffer->pntr);
            SetVolPan(SoundControlBuffer);
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
            break;
        case 5:
            if (!SoundControlBuffer->pntr)
                break;
            CalcSFXCurrDist(SoundControlBuffer->pntr);
            SetVolPan(SoundControlBuffer);
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
            if (LevelTicks > SoundControlBuffer->SoundEndFrame)
                unfreechanoff(Channel);
            break;
        case 3:
            CalcSFXCurrDist(&SoundControlBuffer->pos);
            SetVolPan(SoundControlBuffer);
        case 4:
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
            break;
        case 6:
            if (!SoundControlBuffer->pntr)
                break;
            {
                int vol = SoundControlBuffer->basevol * 3;
                if (vol > 255)
                    vol = 255;
                if (vol < 0)
                    vol = 0;
                SoundControlBuffer->currvol = vol;
                SoundControlBuffer->lrpan = (SFXRotCoord.vx + 1) * 15;
                SoundControlBuffer->fbpan = (SFXRotCoord.vz + 1) * 15;
            }
            DoMix(Channel, SoundControlBuffer);
            ReCalcPitch(Channel, SoundControlBuffer);
        case 8:
            if (!SoundControlBuffer->pntr)
                break;
            /* two inlined unfreechanoff copies (SYM has two chan blocks here): the second has no label
               before it, so cases 1/7/5 cross-jump to a new label outside jump_chain and stay apart */
            if (LevelTicks >= SoundControlBuffer->SoundEndFrame)
                unfreechanoff(Channel);
            else if (SampleInfoTable[SoundControlBuffer->samplenum].Flags & 4)
                unfreechanoff(Channel);
            break;
        }
        if (Muted)
            continue;
        if (newsound) {
            strncpy(NameBuffer, SoundNames + (SoundControlBuffer->samplenum + 1) * 12, 12);
            NameBuffer[12] = 0;
            ttt = 0;
            DoMix(Channel, SoundControlBuffer);
            sendkeymask |= 1 << Channel;
            voiceAttr.voice = 1 << Channel;
            voiceAttr.mask = 0x80;
            voiceAttr.addr = VAGList[SoundControlBuffer->samplenum].dwAddr;
            SpuSetVoiceAttr(&voiceAttr);
            SendPitch(Channel, SoundControlBuffer->currpitch * VAGList[SoundControlBuffer->samplenum].dwMulti / 256);
            if (SoundControlBuffer->mixflag == 4 || SoundControlBuffer->mixflag == 5) {
                DoMix(SoundControlBuffer->channel2, SoundControlBuffer);
                SendPitch(SoundControlBuffer->channel2, SoundControlBuffer->currpitch * VAGList[SoundControlBuffer->samplenum].dwMulti / 256);
                if (*(signed char *)&SoundControlBuffer->mixflag == 5) {
                    sendkeymask |= 1 << SoundControlBuffer->channel2;
                    voiceAttr.voice = 1 << SoundControlBuffer->channel2;
                    voiceAttr.mask = 0x80;
                    voiceAttr.addr = VAGList[SoundControlBuffer->samplenum + 1].dwAddr;
                    SpuSetVoiceAttr(&voiceAttr);
                }
            }
        } else {
            wStatus = SpuGetKeyStatus(1 << Channel);
            if (wStatus == 0 || wStatus == 3)
                unfreechanoff(Channel);
        }
    }
    if (sendkeymask)
        SpuSetKey(1, sendkeymask);
    if (Muted)
        SpriText(GameWindows, "@033C6^7F0000M u t e");
}

/* after ProcessSounds, which calls it rather than inlining it */
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
