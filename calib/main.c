#include "types.h"

/* defined in dbuffer.obj: externs, so %hi/%lo; the arrays must be incomplete to get the original addressing */
extern unsigned char DbFrame[];
extern unsigned char TbFrame[];
extern struct DB *olddb;
extern struct DB *currdb;
extern struct DB db[2];

void DoubleTrebleBuffer(void)
{
    int Temp;

    Temp = TbFrame[0];
    TbFrame[0] = TbFrame[1];
    TbFrame[1] = TbFrame[2];
    TbFrame[2] = Temp;
    DbFrame[1] = DbFrame[0];
    DbFrame[0] = 1 - DbFrame[0];
    currdb = &db[DbFrame[0]];
    olddb = &db[DbFrame[1]];
}
int GpuTimerEnable;
extern int DrawStopTime;
extern int InVBlank;
int cdtime;
extern void (*VBlankSpecific)();
extern int UsePollHost;
int VBFrameCount;
extern int PalNtscRate;
extern int Paused;
extern int LevelIsMultiPlayer;
extern int VBLevelTicks;
long GetRCnt();
void ReadPaddHardware(void);
void DualShockStuff(void);

/* libsn.h */
#define pollhost() __asm__ volatile("break 1024")

void VBlank(void)
{
    InVBlank = 1;
    ReadPaddHardware();
    cdtime--;
    DualShockStuff();
    if (VBlankSpecific)
        VBlankSpecific();
    if (UsePollHost)
        pollhost();
    VBFrameCount += PalNtscRate;
    if (!Paused) {
        if (LevelIsMultiPlayer)
            VBLevelTicks += 5;
        else
            VBLevelTicks += PalNtscRate;
    }
    InVBlank = 0;
}

void DrawSyncInt(void)
{
    if (GpuTimerEnable)
        DrawStopTime = GetRCnt(0xf2000001);
}

extern struct IDENTITYNAME NextLevelName;
extern int Mission;
extern short LevelComplete;
extern struct IDENTITYNAME MainStartName;
extern struct CREATURE Creatures[];
void SetInterMission(void);
void KillModel(struct CREATURE *Cre);
void PrintLevelInterMission(void);
int MainInterMissionLoop(void);
void ClearLevelIntermission(void);

#define IDNAME(s) ((struct IDENTITYNAME){s})

void GameInterMission(void)
{
    SetInterMission();
    KillModel(&Creatures[0]);
    KillModel(&Creatures[1]);
    PrintLevelInterMission();
    while (!MainInterMissionLoop())
        ;
    if (Mission == 5) {
        NextLevelName = IDNAME("Extro FMV");
        LevelComplete = 13;
    } else {
        NextLevelName = IDNAME("EndMission 0");
        MainStartName = IDNAME("Default");
        LevelComplete = 2;
        NextLevelName.Name[11] += Mission;
    }
    ClearLevelIntermission();
}

extern int NewVcounts;
extern short FrameCompOn;
extern unsigned int LevelTicks;
extern int FrameCount;
extern int RealVCounts;
extern int LastNewVcounts;
extern int ProcessorStopTime;
extern short TimebarOn;
extern int ProcessorStartTime;
extern void (*EndSpecific)();
extern int DrawStartTime;
void CdCheckMusic(void);
void NewPadRead(void);
void SwapAllButtons(void);
char PadEndInterMission(void);
void DBInterMission(void);
void QuakeInterMissionLoop(void);
void DrawTimeBar(void);
void ClearOTag(unsigned long *ot, int n);
void DrawOTag(unsigned long *p);
int DrawSync(int mode);
int VSync(int mode);
struct DISPENV *PutDispEnv(struct DISPENV *env);

int MainInterMissionLoop(void)
{
    char EndInterMission;

    CdCheckMusic();
    NewPadRead();
    DoubleTrebleBuffer();
    ClearOTag(currdb->InitOt, 217);
    LevelTicks = VBLevelTicks;
    FrameCount = VBFrameCount;
    RealVCounts = VBFrameCount;
    LastNewVcounts = NewVcounts;
    if (!FrameCompOn)
        NewVcounts = PalNtscRate * 2;
    else
        NewVcounts = VBFrameCount;
    if (NewVcounts > 30)
        NewVcounts = 30;
    VBFrameCount = 0;
    FrameCount = 0;
    SwapAllButtons();
    EndInterMission = PadEndInterMission();
    DBInterMission();
    QuakeInterMissionLoop();
    ProcessorStopTime = GetRCnt(0xf2000001);
    DrawSync(0);
    VSync(2);
    if (TimebarOn)
        DrawTimeBar();
    ProcessorStartTime = GetRCnt(0xf2000001);
    PutDispEnv(&currdb->DispEnv);
    DrawOTag(currdb->InitOt);
    DrawStartTime = GetRCnt(0xf2000001);
    GpuTimerEnable = 1;
    if (EndSpecific)
        EndSpecific();
    return EndInterMission;
}

/* the label the splat config puts on 0x800ae870 is __SN_GP_BASE (start of sdata); the variable there is
   the usual PSY-Q _ramsize = 2MB */
unsigned long __SN_GP_BASE = 0x00200000;
unsigned long _stacksize = 0x00000500;
extern unsigned long __bss; /* linker section table word: start of bss */
extern unsigned int GameTimeOutTime;
extern unsigned int GameNoInputTime;
extern unsigned int GameTime;
extern long *primlistarray[];
extern long *DummyMemory;
extern short *screen256pal;
extern int DoLogos2;
extern int TitleLoad;
extern int DoLogos;
extern int DoIntro;
extern int DoExtro;
extern int PalNtsc;
extern struct ControllerPacket padbuffer1;
extern struct ControllerPacket padbuffer2;
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void QuakeFillInLevelEngineSideInterface(void);
void FrontEndResetVideoDefaults(void);
void Config_InstallVideo(void);
void InitialiseAfterReset(void);
void StoreGP_(void);
void initMYmalloc2(void);
void ResetCallback(void);
void SetDispMask(int mask);
void MemCardInit(long val);
void MemCardStart(void);
void PadInitMtap(void *p1, void *p2);
void PadStartCom(void);
void ResetGraph(int mode);
void SetGraphDebug(int level);
void SfxInit(unsigned char bMode);
void MYPCinit(void);
void InitialiseDrawNewGame(void);
void InitSoundNewGame(void);
void VSyncCallback(void (*f)());
void DrawSyncCallback(void (*f)());
void dprintf(struct WINDOW *Window, char *_format);
void ProcessGame(void);

int main(void)
{
    int a;
    int *heap;

    GameTimeOutTime = 0;
    GameNoInputTime = 0;
    GameTimeOutTime = 0;
    GameTime = 0;
    a = -(_stacksize + 0x80000000) + __SN_GP_BASE;
    a -= __bss;
    EnterCriticalSection();
    heap = (int *)__bss;
    while (a > 0) {
        heap[0] = 0;
        heap[1] = 0;
        heap += 2;
        a -= 8;
    }
    ExitCriticalSection();
    DummyMemory = (long *)primlistarray;
    screen256pal = (short *)primlistarray;
    DoLogos2 = 1;
    TitleLoad = 1;
    FrameCompOn = 1;
    DoLogos = 0;
    DoIntro = 0;
    DoExtro = 0;
    PalNtsc = 5;
    QuakeFillInLevelEngineSideInterface();
    FrontEndResetVideoDefaults();
    Config_InstallVideo();
    FrameCount = 0;
    PalNtscRate = PalNtsc;
    InitialiseAfterReset();
    StoreGP_();
    initMYmalloc2();
    ResetCallback();
    SetDispMask(0);
    MemCardInit(0);
    MemCardStart();
    PadInitMtap(&padbuffer1, &padbuffer2);
    PadStartCom();
    ResetGraph(0);
    SetGraphDebug(0);
    SfxInit(2);
    MYPCinit();
    GpuTimerEnable = 0;
    InitialiseDrawNewGame();
    InitSoundNewGame();
    VSyncCallback(VBlank);
    DrawSyncCallback(DrawSyncInt);
    dprintf(0, "Begin Game");
    ProcessGame();
    return 0;
}
