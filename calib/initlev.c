#include "types.h"
extern short Language;
extern unsigned char moviequitflag;
extern int FlareType;
extern struct ZONEDATA *ThisZoneData;
extern short level_ambience;
extern struct IDENTITYNAME MainStartName;
void ClearSaveMemory();
void SetupGameSpecific();
void InitWaterStuffZone();
void MallocParticles();
unsigned long MaxMallocSize();
void AquireGt4s(int numgt4s, int NumSubPolys);
int InitSamplesForLevel(void);
void LoadLevelData(struct IDENTITYNAME StartName);
void FadeOutLoading();
void LoadInGameVram();
void FinishedLoading();
void GameInitStuff();

void LoadGameData(void)
{
    int FileFound;
    level_ambience = 16;
    FileFound = InitSamplesForLevel();
    LoadLevelData(MainStartName);
    FadeOutLoading();
    if (FileFound)
        LoadInGameVram();
    FinishedLoading();
    GameInitStuff();
}

void InitialiseAfterReset(void)
{
    Language = 32;
    moviequitflag = 0;
    FlareType = 1;
    ClearSaveMemory();
    SetupGameSpecific();
}

void AquireResources(void)
{
    InitWaterStuffZone();
    MallocParticles(ThisZoneData->Resource.NumParticles);
    {
        int NumGt4s = ThisZoneData->Resource.NumGt4s;
        int NumSubPolys = ThisZoneData->Resource.NumSubPolys;
        {
            unsigned long NumPackets;
            unsigned long PacketTotalSize;
            unsigned long SubSize;
            unsigned long MaxSize;
            MaxSize = MaxMallocSize();
            SubSize = NumSubPolys * 1664 + NumSubPolys * 212 + (NumSubPolys * 2 + 1) * 2 + NumSubPolys * 2;
            PacketTotalSize = 3330;
            NumPackets = (MaxSize - SubSize) / PacketTotalSize;
            if (NumPackets > 256)
                NumPackets = 256;
            NumPackets *= 32;
            if (NumGt4s < 0)
                NumPackets = -NumGt4s;
            AquireGt4s(NumPackets, NumSubPolys);
        }
    }
}

void ClearLots(void)
{
}
