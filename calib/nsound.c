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


