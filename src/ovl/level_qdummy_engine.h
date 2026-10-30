/* The level interface of the engine level_qdummy was built against: an older layout than
 * types.h's QENGINELEVINTERFACE (size 0x488), with the same names at the offsets this module
 * uses. Fields this module does not use are guesses from the later interface. */
struct QGAMECONFIG_OLD {  /* size 0xa8 */
    char Unknown0[0x8c];
    short sMultiMode;  /* 0x8c */
    char Unknown8e[0x1a];
};
struct QENGINELEVINTERFACE_OLD {  /* size 0x488 */
    int SizeOf;  /* 0x0 */
    int (*PrintF)();  /* 0x4 */
    struct IDENTITYNAME *ThisLevelName;  /* 0x8 */
    POLY_FT4 *PolyStack0;  /* 0xc */
    POLY_FT4 *PolyStack1;  /* 0x10 */
    void *MissionData;  /* 0x14 */
    unsigned int *usp;  /* 0x18 */
    struct CREATURE *(*InitialiseQuakePlayer)();  /* 0x1c */
    void (*QuakeSinglePlayerInit)();  /* 0x20 */
    int (*ActivateCreatureBatch)();  /* 0x24 */
    void (*LoadSaveCreatureBatches)();  /* 0x28 */
    struct CREATURE *(*InitialiseQuakePickup)();  /* 0x2c */
    void (*InitMultiQuakePlayer)();  /* 0x30 */
    int (*PrimaryQuakePickup)();  /* 0x34 */
    void (*AddCross)();  /* 0x38 */
    int (*AddLight)();  /* 0x3c */
    struct CREATURE *(*AddLaser)();  /* 0x40 */
    int (*AddThickLine)();  /* 0x44 */
    struct PARTICLE *(*AddParticlesSL)();  /* 0x48 */
    void (*SceneObjFragments)();  /* 0x4c */
    void (*QuakeGenExplosion)();  /* 0x50 */
    void (*SelfRecordOn)();  /* 0x54 */
    void (*SelfPlayOn)();  /* 0x58 */
    int (*QuakeEndSelfPlay)();  /* 0x5c */
    void (**LevelSpecific)();  /* 0x60 */
    void (**LevelExtraDraw)();  /* 0x64 */
    int *NoInputTime;  /* 0x68 */
    short *(*SetNoInputTimeOutRoutine)();  /* 0x6c */
    int (*IsPlayerPadShockable)();  /* 0x70 */
    int *InSelfPlayMode;  /* 0x74 */
    struct BRUSHMOVEBUF *BrushMoveBufs;  /* 0x78 */
    struct SOLIDBOX *(*GenSolidBox)();  /* 0x7c */
    int (*GetSceneNum)();  /* 0x80 */
    int (*Find3dSpaceIndxLink)();  /* 0x84 */
    struct COLLHEADER *CollMap;  /* 0x88 */
    struct COLLHEADER *MoveCollMap;  /* 0x8c */
    char *GameDeathWatchA;  /* 0x90 */
    char *GameDeathWatchB;  /* 0x94 */
    char *GameDeathWatchC;  /* 0x98 */
    int (*Event_SetDisableNamed)();  /* 0x9c */
    int (*Event_GetDisableNamed)();  /* 0xa0 */
    int (*Event_DoNamed)();  /* 0xa4 */
    int (*Event_GetNamedOffset)();  /* 0xa8 */
    struct PSXMAINEVENTHDR **eventlistbase;  /* 0xac */
    int (*SquareRootNR)();  /* 0xb0 */
    struct DB_OLD *Db;  /* 0xb4 */
    unsigned char *DbFrame;  /* 0xb8 */
    void (*RemoveMeRoutine)();  /* 0xbc */
    void (*SetGeomStuffWin)();  /* 0xc0 */
    int (*AreaValid)();  /* 0xc4 */
    void (*SetupScreenGivenArea)();  /* 0xc8 */
    int *NewVcounts;  /* 0xcc */
    int *MaxWindows;  /* 0xd0 */
    struct PARTCOLOURS **ParticleColoursTable;  /* 0xd4 */
    struct CREATURE *Creatures;  /* 0xd8 */
    struct SCENE2 *Scene2Data;  /* 0xdc */
    int *ChrisCount;  /* 0xe0 */
    unsigned int *LevelTicks;  /* 0xe4 */
    struct IDENTITYNAME *ZoneName;  /* 0xe8 */
    struct STARTPOSITION **MainPlayerStart;  /* 0xec */
    struct CSTBL *rcossin_tbl;  /* 0xf0 */
    int *GlobalKey;  /* 0xf4 */
    struct MISSIONEVENT *LevelEvents;  /* 0xf8 */
    struct WINDOW_OLD *GameWindows;  /* 0xfc */
    void ***OnZoneLoadTable;  /* 0x100 */
    int (*rand)();  /* 0x104 */
    int (*sprintf)();  /* 0x108 */
    MATRIX *(*RotMatrixYXZ)();  /* 0x10c */
    char *(*PrintIdentityName)();  /* 0x110 */
    void (*centreprintf)();  /* 0x114 */
    void (*dprintf)();  /* 0x118 */
    char *(*GetStringFromStringtable)();  /* 0x11c */
    void (*PlayThisXA)();  /* 0x120 */
    int (*SoundByCreature)();  /* 0x124 */
    int (*GetSoundNum)();  /* 0x128 */
    int (*GetSoundClone)();  /* 0x12c */
    void (*SoundSetVolume)();  /* 0x130 */
    void (*SoundSetPitch)();  /* 0x134 */
    void (*SoundSetRelativeVolume)();  /* 0x138 */
    void (*SoundSetRelativePitch)();  /* 0x13c */
    int (*SoundTestIfRunning)();  /* 0x140 */
    int (*SoundStopSoundID)();  /* 0x144 */
    int (*SoundAtLocation)();  /* 0x148 */
    int (*SoundHere)();  /* 0x14c */
    int (*MapPadInputToFire)();  /* 0x150 */
    void *(*MY_malloc_namehb)();  /* 0x154 */
    long (*VectorNormalSS)();  /* 0x158 */
    void (*SetupScreen2by2)();  /* 0x15c */
    void (*SetupScreen1by2)();  /* 0x160 */
    void (*SetupScreen2by1)();  /* 0x164 */
    void (*SetupScreen1by1)();  /* 0x168 */
    void (*AlterFocalSubDiv)();  /* 0x16c */
    void (*GetAbsoluteModelVert)();  /* 0x170 */
    int (*AddBlurPoints)();  /* 0x174 */
    struct LEVELDATA *(*GetLevelDataFromIdentityName)();  /* 0x178 */
    int Unknown17c;  /* 0x17c, not in the later interface */
    int (*LoadImage)();  /* 0x180 */
    int (*DrawSync)();  /* 0x184 */
    unsigned short (*GetTPage)();  /* 0x188 */
    void (*AddPrim_)();  /* 0x18c */
    void (*ChrisInitEveryFrame)();  /* 0x190 */
    void (*ProcessMoveBufs)();  /* 0x194 */
    void (*ProcessSounds)();  /* 0x198 */
    void (*DrawAllScenes)();  /* 0x19c */
    void (*DrawAllSortedObjs)();  /* 0x1a0 */
    void (*ProcessAllLensFlare)();  /* 0x1a4 */
    void (*DrawAllModels)();  /* 0x1a8 */
    void (*ProcessLights)();  /* 0x1ac */
    void (*ProcessBlurs)();  /* 0x1b0 */
    void (*FreeSubDivs)();  /* 0x1b4 */
    void (*ReallyFreePolyPackets)();  /* 0x1b8 */
    void (*QGenWindowSortList)();  /* 0x1bc */
    void (*UpdateWindowStuff)();  /* 0x1c0 */
    void (*MoveModels)();  /* 0x1c4 */
    unsigned long (*SetSp)();  /* 0x1c8 */
    void (*ReloadAutoSave)();  /* 0x1cc */
    short *tpscreenx;  /* 0x1d0 */
    short *tpscreeny;  /* 0x1d4 */
    short *palette;  /* 0x1d8 */
    struct DB_OLD **currdb;  /* 0x1dc */
    void (*InGameInitScreen)();  /* 0x1e0 */
    short (*FrontEndRestoreCursor)();  /* 0x1e4 */
    void (*FrontEndClearCursorMemory)();  /* 0x1e8 */
    void (*FrontEndResetPlayerDefaults)();  /* 0x1ec */
    void (*FrontEndResetSoundDefaults)();  /* 0x1f0 */
    void (*FrontEndResetVideoDefaults)();  /* 0x1f4 */
    void (*FrontEndResetMultiplayerDefaults)();  /* 0x1f8 */
    void (*FrontEndCreateMenu)();  /* 0x1fc */
    void (*FrontEndAddText)();  /* 0x200 */
    void (*FrontEndModifyText)();  /* 0x204 */
    void (*DoFrontEndStuff)();  /* 0x208 */
    void (*MoveBottomPoly)();  /* 0x20c */
    void (*CHECK_FOR_SLIDER)();  /* 0x210 */
    void (*CHECK_FOR_MCFILEWINDOW)();  /* 0x214 */
    long (*MemCardSync)();  /* 0x218 */
    long (*MemCardGetDirentry)();  /* 0x21c */
    long (*MemCardReadFile)();  /* 0x220 */
    long (*MemCardWriteFile)();  /* 0x224 */
    long (*MemCardCreateFile)();  /* 0x228 */
    void (*FrontEndInitText)();  /* 0x22c */
    void (*MemoryCardInit)();  /* 0x230 */
    void (*MemoryCardStart)();  /* 0x234 */
    void (*MemoryCardStop)();  /* 0x238 */
    long (*MemoryCardGetStatus)();  /* 0x23c */
    void (*MemoryCardSetStatus)();  /* 0x240 */
    char *(*MemoryCardGetFilename)();  /* 0x244 */
    long (*MemoryCardGetNumFiles)();  /* 0x248 */
    void (*MemoryCardGetCardname)();  /* 0x24c */
    long (*MemoryCardGetNumCards)();  /* 0x250 */
    void (*MemoryCardSetActiveCard)();  /* 0x254 */
    void (*MemoryCardSetActiveFile)();  /* 0x258 */
    long (*MemoryCardGetNumFreeBlocks)();  /* 0x25c */
    void (*MemoryCardUpdate)();  /* 0x260 */
    void (*InGame_PrintMemCardMessage)();  /* 0x264 */
    void (*FrontEndLevelCompleteTimeout)();  /* 0x268 */
    void (*GameSetup)();  /* 0x26c */
    void (*ReReadToc)();  /* 0x270 */
    int (*MyGetCDStatus)();  /* 0x274 */
    void *Backfunc;  /* 0x278 */
    void *Specialfunc;  /* 0x27c */
    void *Lightingfunc;  /* 0x280 */
    void *Controllerfunc;  /* 0x284 */
    void *Draw256func;  /* 0x288 */
    struct STRING_TABLE *MyStringTable;  /* 0x28c */
    short *NumPadds;  /* 0x290 */
    short *MultiTapExists;  /* 0x294 */
    short gFrontEndPad[4];  /* 0x298 */
    short gFrontEndOPad[4];  /* 0x2a0 */
    short gFrontEndPadShot;  /* 0x2a8 */
    char gFrontEndLoadType;  /* 0x2aa */
    unsigned char gFrontEndLoadSubType : 7;  /* bit 5464 */
    unsigned char gFrontEndLoadIsSpecial : 1;  /* bit 5471 */
    short gFrontEndTimeout;  /* 0x2ac */
    short gFrontEndLevelComplete;  /* 0x2ae */
    short gFrontEndPrevScreen;  /* 0x2b0 */
    short currentscreen;  /* 0x2b2 */
    short cursorpos;  /* 0x2b4 */
    short cursorlower;  /* 0x2b6 */
    short cursorupper;  /* 0x2b8 */
    short maxcursor;  /* 0x2ba */
    struct QGAMECONFIG_OLD GameConfig;
    short MissionCDStatus;  /* 0x364 */
    void (*PatchEngineVariables)();  /* 0x368 */
    struct IDENTITYNAME *DirectAccessLevelName;  /* 0x36c */
    void (**MainLoop)();  /* 0x370 */
    void (*InitialiseQuakePickups)();  /* 0x374 */
    struct CREATURE *(*SpawnGeneral)();  /* 0x378 */
    void (*KillModel)();  /* 0x37c */
    struct IDENTITYNAME *MainStartName;  /* 0x380 */
    short *LevelComplete;  /* 0x384 */
    void (*GenVectBox)();  /* 0x388 */
    void (*OrderedSort)();  /* 0x38c */
    struct AREA *AreaData;  /* 0x390 */
    struct SHIELDGLINT *ReactorShieldGlints;  /* 0x394 */
    void (*GenShieldMatrix)();  /* 0x398 */
    void (*Rotate)();  /* 0x39c */
    void (*SetWinFade)();  /* 0x3a0 */
    struct ANIMHDR *(*FindAnimHdr)();  /* 0x3a4 */
    void (*DoDamage)();  /* 0x3a8 */
    void (*GetToc)();  /* 0x3ac */
    void (*HelpComputer_NewMessage)();  /* 0x3b0 */
    short *DoSnapShot;  /* 0x3b4 */
    short (*SnapShotPadViolation)();  /* 0x3b8 */
    char *(*ControlGetPortName)();  /* 0x3bc */
    void (*Config_InstallVideo)();  /* 0x3c0 */
    int Unknown3c4;  /* 0x3c4, not in the later interface */
    void *(*My_malloc_name)();  /* 0x3c8 */
    void (*MyFree)();  /* 0x3cc */
    void (*StCdInterrupt)();  /* 0x3d0 */
    int *StCdIntrFlagAddr;  /* 0x3d4 */
    int (*ClearImage)();  /* 0x3d8 */
    int (*MoveImage)();  /* 0x3dc */
    int (*StoreImage)();  /* 0x3e0 */
    unsigned long (*StGetNext)();  /* 0x3e4 */
    unsigned long (*StFreeRing)();  /* 0x3e8 */
    void (*StUnSetRing)();  /* 0x3ec */
    void (*StSetRing)();  /* 0x3f0 */
    void (*StSetStream)();  /* 0x3f4 */
    void (*SetDispMask)();  /* 0x3f8 */
    DISPENV *(*SetDefDispEnv)();  /* 0x3fc */
    DRAWENV *(*SetDefDrawEnv)();  /* 0x400 */
    DISPENV *(*PutDispEnv)();  /* 0x404 */
    DRAWENV *(*PutDrawEnv)();  /* 0x408 */
    void (*SetDrawMode)();  /* 0x40c */
    void (*SetTile)();  /* 0x410 */
    unsigned long *(*ClearOTag)();  /* 0x414 */
    void (*DrawOTag)();  /* 0x418 */
    int (*VSync)();  /* 0x41c */
    char *(*strcpy)();  /* 0x420 */
    char *(*strcat)();  /* 0x424 */
    int (*CdControl)();  /* 0x428 */
    int (*CdControlB)();  /* 0x42c */
    void *(*CdDataCallback)();  /* 0x430 */
    void (*(*CdReadyCallback)())();  /* 0x434 */
    int (*CdRead2)();  /* 0x438 */
    CdlFILE *(*CdSearchFile)();  /* 0x43c */
    void (*SetCDVolume)();  /* 0x440 */
    void (*NewPadRead)();  /* 0x444 */
    int (*ResetCallback)();  /* 0x448 */
    int (*DMACallback)();  /* 0x44c */
    char (*AddLevelInterMission)();  /* 0x450 */
    void (*SetDrawEnv)();  /* 0x454 */
    int (*ModNameCmp)();  /* 0x458 */
    short *TPages;  /* 0x45c */
    int *NewMission;  /* 0x460 */
    int *NextMission;  /* 0x464 */
    int *Mission;  /* 0x468 */
    int *DebugWatch;  /* 0x46c */
    void (*LoadSaveGameConfigData)();  /* 0x470 */
    void (*ProcessMessageText)();  /* 0x474 */
    void (*PrintDebugInfoScene)();  /* 0x478 */
    int *DebugInfoPntr;  /* 0x47c */
    int CantPause;  /* 0x480 */
    int FrontEndStartScreen;  /* 0x484 */
};
