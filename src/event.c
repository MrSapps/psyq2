#include "types.h"
#include "bss.h"

/* defined below; gcc defers these to the end of the file in this order */
int Event_GetNamedOffset(struct IDENTITYNAME name3);
void ProcessMoveBufs(void);
struct PSXSUBEVENT *PSXMISSIONCOMPLETE(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMBUTTON_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXLOADMAP(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXLOADMAP_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXROTHATCHEVENT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMROTEVENT2(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMROTEVENT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXLIFT1EVENT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXCAGELIFT1EVENT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXBUTTON(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXLASERWALL_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXLASERWALL(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);
struct PSXSUBEVENT *PSXDISH(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXB3ROCKS(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXTELEPORT_T(struct PSXSUBEVENT *subevent);
void PSXINACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXUNDERACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXINLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXUNDERLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXINWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXUNDERWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXINCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void PSXINSECRET_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXCREBATCH_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXMISEVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSTRING_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXEXPLO(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXEXPLO_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXHELPCOMPUTER_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXFLICKERINGLIGHT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXLASERBEAM(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);
struct PSXSUBEVENT *PSXLASERBEAM_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXSETWIBBLE(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSETWIBBLE_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSHOOTTHEN(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);
struct PSXSUBEVENT *PSXPLATFORMEVENT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXTLIGHT(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXPISTON(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXDONTJUMP(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXDISABLEME(struct PSXSUBEVENT *subevent);
void PSXOBJDRAWOFF(struct PSXSUBEVENT *subevent);
void PSXOBJDRAWON(struct PSXSUBEVENT *subevent);
void PSXONKEYDO(struct PSXSUBEVENT *subevent);
void InitUserFuncs(char *addr);
void Event_KillTimers(void);
void DrawLaserBeams(void);
void DoSolidBoxEventDamage(struct SOLIDBOX *solidbox, short damage, struct VECT *Location);
int RestoreEvents(void);
struct PSXSUBEVENT *PSXDOOR1UP(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);
void PrimarySimRot_PDH2(struct BRUSHMOVEBUF *MoveBuf);
void PrimarySimRot_PDH(struct BRUSHMOVEBUF *MoveBuf);
void PSXINLOWCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
void dofunc_error(void);
void transfunc_error(void);
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
int Event_GetNamedOffset(struct IDENTITYNAME name3);
/* data */
struct PSXSUBEVENT *PSXB3ROCKS_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXBUTTON_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXCAGELIFT1EVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXCREBATCH(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);
struct PSXSUBEVENT *PSXDISH_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXFLICKERINGLIGHT(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXGLASS(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);
struct PSXSUBEVENT *PSXGLASS_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXHELPCOMPUTER(struct PSXSUBEVENT *subevent);
void PSXINSECRET(struct PSXSUBEVENT *subevent, struct CREATURE *cre);
struct PSXSUBEVENT *PSXLIFT1EVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXMISEVENT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);
struct PSXSUBEVENT *PSXOBJDRAWOFF_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXPISTON_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXPLATFORMEVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXROTBUTTON(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXROTBUTTON_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXROTHATCHEVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSHOOTTHEN_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMBUTTON(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMPLESOUND(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMROTEVENT2_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSIMROTEVENT_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXSTRING(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXTELEPORT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);
struct PSXSUBEVENT *PSXTIMER(struct PSXSUBEVENT *subevent);
/* data of Event.Obj, generated by tools/gendata.py */
struct USERFUNCLIST EventUserFuncList[44] = {
    { { "STRING" }, (void *)PSXSTRING, (void *)PSXSTRING_T },
    { { "LIFT1" }, (void *)PSXLIFT1EVENT, (void *)PSXLIFT1EVENT_T },
    { { "CAGELIFT1" }, (void *)PSXCAGELIFT1EVENT, (void *)PSXCAGELIFT1EVENT_T },
    { { "ROTHATCH" }, (void *)PSXROTHATCHEVENT, (void *)PSXROTHATCHEVENT_T },
    { { "SIMROT" }, (void *)PSXSIMROTEVENT, (void *)PSXSIMROTEVENT_T },
    { { "SIMROT2" }, (void *)PSXSIMROTEVENT2, (void *)PSXSIMROTEVENT2_T },
    { { "INWATER" }, (void *)PSXINWATER, 0 },
    { { "INCROUCH" }, (void *)PSXINCROUCH, 0 },
    { { "INLOWCROUCH" }, (void *)PSXINLOWCROUCH, 0 },
    { { "UNDERWATER" }, (void *)PSXUNDERWATER, 0 },
    { { "INACID" }, (void *)PSXINACID, 0 },
    { { "UNDERACID" }, (void *)PSXUNDERACID, 0 },
    { { "INLAVA" }, (void *)PSXINLAVA, 0 },
    { { "UNDERLAVA" }, (void *)PSXUNDERLAVA, 0 },
    { { "SIMBUTTON" }, (void *)PSXSIMBUTTON, (void *)PSXSIMBUTTON_T },
    { { "ROTBUTTON" }, (void *)PSXROTBUTTON, (void *)PSXROTBUTTON_T },
    { { "BUTTON" }, (void *)PSXBUTTON, (void *)PSXBUTTON_T },
    { { "LASERWALL" }, (void *)PSXLASERWALL, (void *)PSXLASERWALL_T },
    { { "LASERBEAM" }, (void *)PSXLASERBEAM, (void *)PSXLASERBEAM_T },
    { { "GLASS" }, (void *)PSXGLASS, (void *)PSXGLASS_T },
    { { "DISH" }, (void *)PSXDISH, (void *)PSXDISH_T },
    { { "B3ROCKS" }, (void *)PSXB3ROCKS, (void *)PSXB3ROCKS_T },
    { { "PISTON" }, (void *)PSXPISTON, (void *)PSXPISTON_T },
    { { "LOADMAP" }, (void *)PSXLOADMAP, (void *)PSXLOADMAP_T },
    { { "INSECRET" }, (void *)PSXINSECRET, (void *)PSXINSECRET_T },
    { { "CREBATCH" }, (void *)PSXCREBATCH, (void *)PSXCREBATCH_T },
    { { "MISEVENT" }, (void *)PSXMISEVENT, (void *)PSXMISEVENT_T },
    { { "EXPLO" }, (void *)PSXEXPLO, (void *)PSXEXPLO_T },
    { { { 72, 69, 76, 80, 67, 79, 77, 80, 85, 84, 69, 82 } }, (void *)PSXHELPCOMPUTER, (void *)PSXHELPCOMPUTER_T },
    { { "FLKLIGHT" }, (void *)PSXFLICKERINGLIGHT, (void *)PSXFLICKERINGLIGHT_T },
    { { "SHOOTTHEN" }, (void *)PSXSHOOTTHEN, (void *)PSXSHOOTTHEN_T },
    { { "MISCOMPLETE" }, (void *)PSXMISSIONCOMPLETE, 0 },
    { { "PLATFORM" }, (void *)PSXPLATFORMEVENT, (void *)PSXPLATFORMEVENT_T },
    { { "TIMER" }, (void *)PSXTIMER, 0 },
    { { "TIMEDLIGHT" }, (void *)PSXTLIGHT, 0 },
    { { "ONKEYDO" }, (void *)PSXONKEYDO, 0 },
    { { "TELEPORT" }, (void *)PSXTELEPORT, (void *)PSXTELEPORT_T },
    { { "SETWIBBLE" }, (void *)PSXSETWIBBLE, (void *)PSXSETWIBBLE_T },
    { { "DONTJUMP" }, (void *)PSXDONTJUMP, 0 },
    { { "DISABLEME" }, (void *)PSXDISABLEME, 0 },
    { { "SIMPLESOUND" }, (void *)PSXSIMPLESOUND, 0 },
    { { "OBJDRAWOFF" }, (void *)PSXOBJDRAWOFF, (void *)PSXOBJDRAWOFF_T },
    { { "OBJDRAWON" }, (void *)PSXOBJDRAWON, 0 },
    { { "" }, 0, 0 },
};
struct PSXMAINEVENTHDR *eventlistbase = 0;
struct PSXMAINEVENTHDR *neweventlistbase = 0;
static int Sound_KeyUse SBSS;
static int Sound_KeyTry SBSS;
static int Sound_Rockdoor SBSS;
static int Sound_PlatformStart SBSS;
static int Sound_PlatformMiddle SBSS;
static int Sound_PlatformEnd SBSS;
static int Sound_Button SBSS;
static int Sound_Menu1 SBSS;
static int Sound_Menu2 SBSS;
static int Sound_Menu3 SBSS;
static int Sound_Secret SBSS;
static int Sound_Explosion SBSS;
static int Sound_GlassSmash SBSS;
static struct BRUSHMOVEBUF *GirderFallMB SBSS;
static struct BRUSHMOVEBUF *GirderRotateMB SBSS;
struct PSXSUBEVHDR *firsteventlist SBSS;
short NumLaserBeams SBSS;
short ggg SBSS;
short NumLaserWalls SBSS;
void **usrfunclist SBSS;
short Event_StartLevFlag SBSS;
short *eventhdr_start SBSS;
short numuserfunctions SBSS;
short Event_GlobalEVOFFSET SBSS;
short NumTelePorts SBSS;
short *eventhdr_end SBSS;
short Event_MuteLevFlag SBSS;
int E3FinishedLevel SBSS;
char nomoresubevents SBSS;
char *EventGlobals SBSS;
short numbrushes SBSS;
struct BRUSHMOVEBUF *MoveBuf SBSS;
short eventstack[16] BSS;
short TelePortData[8] BSS;
short LaserWallEvents[8][2] BSS;
struct EVENTTIMER EventTimers[8] BSS;
short LaserBeamEvents[32][2] BSS;


extern struct EVENTTIMER EventTimers[];
extern int NewVcounts;
int rand(void);
int Event_DoTimerSub(struct EVENTTIMER *timer, struct CREATURE *Cre);

static int Event_FindTimer(void *timerevent)
{
    int a;

    for (a = 0; a < 8; a++)
        if (timerevent == EventTimers[a].timersubevent)
            return -1;
    for (a = 0; a < 8; a++)
        if (!EventTimers[a].timersubevent)
            return a;
    return -2;
}

extern long *DummyMemory;
extern struct WINDOW GameWindows[];
int sprintf(char *, const char *, ...);

extern int MaxWindows;
int rand(void);
int AreaValid1(int Win, int Area);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);
extern short eventstack[];
int Do_SubEvent(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);

static void Event_DoOffset(int offset, struct CREATURE *Cre)
{
    if (eventlistbase) {
        struct PSXSUBEVENT *subevent;
        struct PSXSUBEVHDR *eventhdr;
        int a;

        eventhdr_end = eventhdr_start = eventstack;
        *eventhdr_end++ = offset;
        while (eventhdr_start < eventhdr_end) {
            Event_GlobalEVOFFSET = *eventhdr_start;
            eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + *eventhdr_start++);
            if (!eventhdr->disabled) {
                eventhdr->disabled = eventhdr->oneshot;
                eventhdr->used = 1;
                subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
                nomoresubevents = 0;
                for (a = 0; a < eventhdr->numevents; a++) {
                    if (Do_SubEvent(subevent, Cre))
                        break;
                    subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
                }
            }
        }
    }
}
extern int GlobalSecretsFound;
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
char AddSecretInterMission(char *CurrentMapTitle);
int SoundAtLocation(int SFXNumber, struct VECT *loc);
extern struct SCENE2 *Scene2Data;
void PrimarySimRot_PDH2(struct BRUSHMOVEBUF *MoveBuf);

void PrimarySimRot_PDH(struct BRUSHMOVEBUF *MoveBuf);
void Event_KillTimers(void);
struct PSXSUBEVENT *PSXDOOR1UP_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXDOOR1SIDE_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXDOOR2SIDE_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXEXPLODE_T(struct PSXSUBEVENT *subevent);


/* Sound_Explosion: splat names Event.Obj's copy Sound_Explosion_800B2A68 (quakecre.obj has another) */
static int Sound_Explosion_800B2A68;
void SceneObjFragments(int ObjIndx, int NumBits, struct VECT *Loc);
void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type);
void HideObjWithSolid(int Obj);

struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
int abs(int);
void PrimaryDoor2_PDH(struct BRUSHMOVEBUF *MoveBuf);


extern struct SHORTPAIR PlayerHitRadius;
struct STARTPOSITION *FindStartPosition(struct IDENTITYNAME Name);
int MaybeLoadZoneName(struct IDENTITYNAME name);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
void ReCalcCollBox(struct CREATURE *Cre);


extern struct IDENTITYNAME ThisLevelName;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
char *strcpy(char *, const char *);

extern struct IDENTITYNAME ThisLevelName;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
char *strcpy(char *, const char *);


int AttatchSolidBoxForObj(int Obj);
extern unsigned int LevelTicks;


extern int ChrisCount;
void DoDamage();

/* not in types.h: event type 9, a counter */
struct PSXEVCOUNTER {
    unsigned char type : 6;
    unsigned char oneshot : 1;
    unsigned char disabled : 1;
    unsigned char evsize;
    short pad;
    int lastcount;
    short count;
    short reset;
};

/* inlined into Do_SubEvent (PSXDOOR1UP's out-of-line copy is in event_9); renamed here so that
   PSXDOOR1UP_T can take the address of the real, external PSXDOOR1UP */
static struct PSXSUBEVENT *PSXDOOR1UP_inl(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR1UP *ev_door1up;

    if (subevent->evsize != sizeof(struct PSXEVDOOR1UP))
        return subevent;
    ev_door1up = (struct PSXEVDOOR1UP *)subevent;
    if (damage) {
        ev_door1up->shotdamage -= damage;
        if (ev_door1up->shotdamage > 0)
            return 0;
    }
    if (ev_door1up->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door1up->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door1up->StartDelay * 300;
            if (ev_door1up->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door1up->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1UP));
}

static struct PSXSUBEVENT *PSXDOOR1SIDE(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR1SIDE *ev_door1side;

    if (subevent->evsize != sizeof(struct PSXEVDOOR1SIDE))
        return subevent;
    ev_door1side = (struct PSXEVDOOR1SIDE *)subevent;
    if (ev_door1side->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door1side->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door1side->StartDelay * 300;
            if (ev_door1side->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door1side->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1SIDE));
}

struct PSXSUBEVENT *PSXDOOR1UP(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);

extern struct COLLHEADER TrigBoundCollMap;
int In3dSpaceIndx(struct COLLHEADER *Map, struct VECT *Loc, int Indx);

extern struct COLLHEADER TrigBoundCollMap;
extern short TelePortData[8];
extern struct PARTCOLOURS *ParticleColoursTable[19];
struct PARTICLE *AddParticlesSL(struct VECT *Pos, struct SVECT *Vel, struct PARTCOLOURS *PColour0, struct PARTCOLOURS *PColour1, int NumToDraw, int Timer, int Size, int Area);
extern int GlobalKey;
int MoveSolidBoxs(struct SOLIDBOX *SolidBox, struct SVECT Movement);
extern struct WINDOW GameWindows[];
void MissionEvent(struct IDENTITYNAME Name);
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
int ActivateCreatureBatch(struct IDENTITYNAME BatchName);
void centreprintf(struct WINDOW *Window, char *_format, ...);
extern int NewVcounts;
extern struct SCENE2 *Scene2Data;
int SoundAtLocation(int SFXNumber, struct VECT *loc);

extern struct BRUSHMOVEBUF BrushMoveBufs[];
void *memset(void *s, int c, unsigned int n);
struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
struct PSXSUBEVENT *PSXSHOOTTHEN(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);

void HelpComputer_NewMessage(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective, int FrameDelay);

void PSXBUTTON_M(struct BRUSHMOVEBUF *MoveBuf);

struct PSXSUBEVENT *PSXGLASS(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);

int GetSoundNum(struct IDENTITYNAME name);
int SoundAtLocation(int SFXNumber, struct VECT *loc);

extern unsigned int LevelTicks;

void PSXDISH_M(struct BRUSHMOVEBUF *MoveBuf);

void SceneObjFragments(int ObjIndx, int NumBits, struct VECT *Loc);
void FreeSolidBoxs(struct SOLIDBOX *SolidBox);

int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);

void PSXB3ROCKS_ROCK_M(struct BRUSHMOVEBUF *MoveBuf);
void PSXB3ROCKS_GIRDF_M(struct BRUSHMOVEBUF *MoveBuf);
void PSXB3ROCKS_GIRDR_M(struct BRUSHMOVEBUF *MoveBuf);

int rand(void);
unsigned short QuickRand(void);

extern short LaserWallEvents[][2];
extern struct COLLHEADER CollMap;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
struct CREATURE *AddLaser(struct VECT *Start, struct VECT *Stop, short Space, short LaserType, struct SHORTPAIR Parts);

int SoundHere(int SFXNumber, struct VECT *loc, struct SOUNDINFO *SoundInfo);
int SoundStop(struct SOUNDINFO *SoundInfo);
/* BRUSHMOVEBUF + 0x50 with Triggered as a signed char bit (SYM says char); the int only gives
   the word alignment the original access uses */
struct BMBTAIL {
    unsigned short Active : 14;
    unsigned short Axis : 2;
    unsigned char OpenFlag;
    signed char Triggered : 1;
    int Align;
};

extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct SCENE2 *Scene2Data;
void *memset(void *s, int c, unsigned int n);
struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
int abs(int);
void PrimaryDoor2_PDH(struct BRUSHMOVEBUF *MoveBuf);

extern short rcossin_tbl[][2];
int MoveSolidBoxs(struct SOLIDBOX *SolidBox, struct SVECT Movement);

extern int GlobalKey;
void QuakeSuggestKey(short Key);
void SetShockData(int InputWin, int Ident, int InputScale, struct VECT *Location, struct POWER Power);
/* unprototyped here: Time goes over as lhu and Speed * NewVcounts untruncated */
void SeekL();

int SquareRootNR(int n);
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
int GetSoundNum(struct IDENTITYNAME Name);
extern struct IDENTITYNAME MainStartName;
extern short LevelComplete;

extern struct IDENTITYNAME ThisLevelName;
extern struct IDENTITYNAME NextLevelName;
extern struct IDENTITYNAME MainStartName;
extern short LevelComplete;
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern unsigned int LevelTicks;
extern short LaserWallEvents[][2];

/* not in the SYM types; layout from the asm */
struct PSXEVDISH {
    unsigned char type;
    unsigned char evsize;
    unsigned char func4;
    unsigned char unused;
    signed char axis;
    signed char speed;
    short obj;
};
extern int GlobalSecretsCount;
void DoDamage();

extern struct COLLHEADER TrigBoundCollMap;
extern short TelePortData[];
extern short LaserBeamEvents[][2];
extern struct COLLHEADER CollMap;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
extern struct SCENE2 *Scene2Data;
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct SCENE2 *Scene2Data;
void HideObjWithSolid();

void PSXPISTON_M(struct BRUSHMOVEBUF *MoveBuf);
int AddTimedLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, int Life);

/* not in the SYM types; layout from the asm */
struct PSXEVTLIGHT {
    unsigned char type;
    unsigned char evsize;
    short unused;
    struct VECT Loc;
    short unused2;
    unsigned short Life;
    struct POWER Power;
    struct COLOUR Colour;
};
extern int GlobalKey;
void FreeSolidBoxs(struct SOLIDBOX *SolidBox);
int Event_DoFromSub(int eventhdroffset, struct CREATURE *Cre, struct PSXSUBEVENT *subevent2);
extern struct EVENTTIMER EventTimers[];

extern struct USERFUNCLIST EventUserFuncList[];
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
void InitEventSoundsEachLevel(void);
void dofunc_error(void);
extern short LaserBeamEvents[][2];
struct CREATURE *AddLaser(struct VECT *Start, struct VECT *Stop, short Space, short LaserType, struct SHORTPAIR Parts);
extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern unsigned int usp;
extern int NewVcounts;
int Event_DoFromSub(int eventhdroffset, struct CREATURE *Cre, struct PSXSUBEVENT *subevent2);

void ProcessMoveBufs(void)
{
    struct BRUSHMOVEBUF *MoveBuf;
    for (MoveBuf = BrushMoveBufs; MoveBuf < &BrushMoveBufs[48]; MoveBuf++)
        if (MoveBuf->Primary)
            MoveBuf->Primary(MoveBuf);
}

void QuakeSuggestKey(short Key)
{
    char *String;
    char *tc;

    String = (char *)DummyMemory;
    switch (Key) {
    case 0x1:
        tc = "Blue Key";
        break;
    case 0x2:
        tc = "Red Key";
        break;
    case 0x4:
        tc = "Security Pass";
        break;
    case 0x10:
        tc = "Commander's Head";
        break;
    case 0x20:
        tc = "Red Pyramid Key";
        break;
    case 0x40:
        tc = "Purple Pyramid Key";
        break;
    case 0x80:
        tc = "Data CD";
        break;
    case 0x100:
        tc = "Data Spinner";
        break;
    case 0x200:
        tc = "Green Key";
        break;
    case 0x400:
        tc = "Yellow Key";
        break;
    case 0x800:
        tc = "White Key";
        break;
    default:
        tc = "<<Key Error!!>>";
        break;
    }
    sprintf(String, "You need the %s", tc);
    centreprintf(GameWindows, String);
}

void PrimaryDoor2_PDH(struct BRUSHMOVEBUF *MoveBuf)
{
    struct BRUSHMOVEBUF *MoveBuf2;
    short in;
    unsigned short *shortpnt;
    struct VECT Location;
    short OldXYZ;
    int NewXYZ;
    short dist;
    struct SVECT xyz;
    short closed;
    struct SCENE2 *Portal;

    in = -MoveBuf->Triggered;
    memset(&xyz, 0, sizeof(xyz));
    if (MoveBuf->Speed < 0)
        MoveBuf->Speed = -MoveBuf->Speed;
    MoveBuf->Triggered = 0;
    switch (MoveBuf->OpenFlag) {
    case 0:
        if (in) {
            if (!MoveBuf->Key || (MoveBuf->Key & GlobalKey)) {
                MoveBuf->OpenFlag = 5;
                if (MoveBuf->Key) {
                    MoveBuf2 = (struct BRUSHMOVEBUF *)MoveBuf->Obj;
                    Location.vx = (Scene2Data[(short)MoveBuf2].VectBox.Min.vx + Scene2Data[(short)MoveBuf2].VectBox.Max.vx) / 2;
                    Location.vy = (Scene2Data[(short)MoveBuf2].VectBox.Min.vy + Scene2Data[(short)MoveBuf2].VectBox.Max.vy) / 2;
                    Location.vz = (Scene2Data[(short)MoveBuf2].VectBox.Min.vz + Scene2Data[(short)MoveBuf2].VectBox.Max.vz) / 2;
                    SoundAtLocation(Sound_KeyUse, &Location);
                    MoveBuf->SoundJustDone = 1;
                }
            } else if (!MoveBuf->SoundJustDone) {
                MoveBuf2 = (struct BRUSHMOVEBUF *)MoveBuf->Obj;
                Location.vx = (Scene2Data[(short)MoveBuf2].VectBox.Min.vx + Scene2Data[(short)MoveBuf2].VectBox.Max.vx) / 2;
                Location.vy = (Scene2Data[(short)MoveBuf2].VectBox.Min.vy + Scene2Data[(short)MoveBuf2].VectBox.Max.vy) / 2;
                Location.vz = (Scene2Data[(short)MoveBuf2].VectBox.Min.vz + Scene2Data[(short)MoveBuf2].VectBox.Max.vz) / 2;
                SoundAtLocation(Sound_KeyTry, &Location);
                MoveBuf->SoundJustDone = 1;
                QuakeSuggestKey(MoveBuf->Key);
            }
        } else
            MoveBuf->SoundJustDone = 0;
        return;
    case 2:
        MoveBuf->OpenFlag = 6;
        return;
    case 3:
        in = 0;
        break;
    case 4:
        in = MoveBuf->Params[1] == 3;
        break;
    case 1:
        in = 1;
        break;
    case 5:
        if ((short)(MoveBuf->StartDelay - NewVcounts) > 0) {
            MoveBuf->StartDelay -= NewVcounts;
            return;
        }
        MoveBuf->OpenFlag = 1;
        NewXYZ = MoveBuf->Obj;
        Location.vx = (Scene2Data[NewXYZ].VectBox.Min.vx + Scene2Data[NewXYZ].VectBox.Max.vx) / 2;
        Location.vy = (Scene2Data[NewXYZ].VectBox.Min.vy + Scene2Data[NewXYZ].VectBox.Max.vy) / 2;
        Location.vz = (Scene2Data[NewXYZ].VectBox.Min.vz + Scene2Data[NewXYZ].VectBox.Max.vz) / 2;
        SoundAtLocation(Sound_PlatformStart, &Location);
        return;
    case 6:
        if (MoveBuf->Time == 0xffff)
            return;
        if ((short)(MoveBuf->Time - NewVcounts) > 0) {
            MoveBuf->Time -= NewVcounts;
            return;
        }
        if (in) {
            MoveBuf->Time = 1;
            return;
        }
        MoveBuf->OpenFlag = 3;
        NewXYZ = MoveBuf->Obj;
        Location.vx = (Scene2Data[NewXYZ].VectBox.Min.vx + Scene2Data[NewXYZ].VectBox.Max.vx) / 2 + MoveBuf->Offset.vx;
        Location.vy = (Scene2Data[NewXYZ].VectBox.Min.vy + Scene2Data[NewXYZ].VectBox.Max.vy) / 2 + MoveBuf->Offset.vy;
        Location.vz = (Scene2Data[NewXYZ].VectBox.Min.vz + Scene2Data[NewXYZ].VectBox.Max.vz) / 2 + MoveBuf->Offset.vz;
        SoundAtLocation(Sound_PlatformStart, &Location);
        return;
    }
    shortpnt = (unsigned short *)&MoveBuf->Offset;
    shortpnt += MoveBuf->Axis;
    NewXYZ = OldXYZ = *shortpnt;
    closed = 0;
    if (in) {
        if (MoveBuf->Range > 0) {
            NewXYZ += MoveBuf->Speed * NewVcounts;
            if (NewXYZ > MoveBuf->Range) {
                NewXYZ = MoveBuf->Range;
                MoveBuf->OpenFlag = 2;
            }
        } else {
            NewXYZ -= MoveBuf->Speed * NewVcounts;
            if (NewXYZ < MoveBuf->Range) {
                NewXYZ = MoveBuf->Range;
                MoveBuf->OpenFlag = 2;
            }
        }
        if (MoveBuf->OpenFlag == 4) {
            if (--MoveBuf->Params[0] == 0)
                MoveBuf->OpenFlag = MoveBuf->Params[1];
        }
    } else {
        if (MoveBuf->Range > 0) {
            NewXYZ -= MoveBuf->Speed * NewVcounts;
            if (NewXYZ < 0) {
                NewXYZ = 0;
                closed = 1;
                MoveBuf->OpenFlag = 0;
            }
        } else {
            NewXYZ += MoveBuf->Speed * NewVcounts;
            if (NewXYZ > 0) {
                NewXYZ = 0;
                closed = 1;
                MoveBuf->OpenFlag = 0;
            }
        }
    }
    {
        int NoProblem;

        NoProblem = 1;
        dist = NewXYZ - OldXYZ;
        (&xyz.vx)[MoveBuf->Axis] = dist;
        if (MoveBuf->SolidBox)
            NoProblem = MoveSolidBoxs(MoveBuf->SolidBox, xyz);
        if (NoProblem) {
            if (MoveBuf->PortalObj != -1) {
                Portal = &Scene2Data[MoveBuf->PortalObj];
                if (MoveBuf->OtherDoor > 0 && closed) {
                    MoveBuf2 = &BrushMoveBufs[MoveBuf->OtherDoor - 1];
                    if (!MoveBuf2->OpenFlag)
                        Portal->NoDraw = closed;
                } else
                    Portal->NoDraw = closed;
            }
            while (MoveBuf) {
                (&MoveBuf->Offset.vx)[MoveBuf->Axis] += dist;
                MoveBuf = MoveBuf->Link;
            }
        } else {
            if (MoveBuf->OpenFlag == 3) {
                if (!(MoveBuf->Params[2] & 2)) {
                    MoveBuf->OpenFlag = 4;
                    MoveBuf->Params[1] = 3;
                }
            } else if (MoveBuf->OpenFlag == 1) {
                if (!(MoveBuf->Params[2] & 1)) {
                    MoveBuf->OpenFlag = 4;
                    MoveBuf->Params[1] = 1;
                }
            }
            MoveBuf->Params[0] = 16;
        }
    }
}

struct PSXSUBEVENT *PSXDOOR1UP(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR1UP *ev_door1up;

    if (subevent->evsize != sizeof(struct PSXEVDOOR1UP))
        return subevent;
    ev_door1up = (struct PSXEVDOOR1UP *)subevent;
    if (damage) {
        ev_door1up->shotdamage -= damage;
        if (ev_door1up->shotdamage > 0)
            return 0;
    }
    if (ev_door1up->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door1up->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door1up->StartDelay * 300;
            if (ev_door1up->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door1up->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1UP));
}

struct PSXSUBEVENT *PSXDOOR1UP_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVDOOR1UP *ev_door1up;
    short c;
    short Obj;
    short *shortpnt;
    short *shortpnt2;
    struct BRUSHMOVEBUF *LastMoveBuf;
    struct SOLIDBOX *LastSolidBox;

    LastSolidBox = 0;
    if (subevent->evsize != sizeof(struct PSXEVDOOR1UP))
        return subevent;
    ev_door1up = (struct PSXEVDOOR1UP *)subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    shortpnt = &ev_door1up->portal;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    ev_door1up->portal = *shortpnt2++;
    LastMoveBuf = 0;
    shortpnt = ev_door1up->crossobj;
    if (ev_door1up->portal != -1)
        Scene2Data[ev_door1up->portal].NoDraw = 1;
    for (c = 0; c < 4; c++)
        shortpnt[c] = -1;
    for (c = 0; c < 4; c++) {
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            if (LastMoveBuf) {
                LastMoveBuf->Link = MoveBuf;
                MoveBuf->Primary = 0;
                MoveBuf->PortalObj = -1;
            } else {
                MoveBuf->Primary = (void (*)())PrimaryDoor2_PDH;
                MoveBuf->PortalObj = ev_door1up->portal;
            }
            if (ev_door1up->shotdamage > 0)
                MoveBuf->DamageFunction = (int (*)())PSXDOOR1UP;
            MoveBuf->EvOffset = (char *)ev_door1up - (char *)eventlistbase;
            MoveBuf->EvHdrOffset = Event_GlobalEVOFFSET;
            MoveBuf->Key = ev_door1up->key;
            MoveBuf->Range = -ev_door1up->range;
            MoveBuf->Speed = abs(ev_door1up->speed);
            MoveBuf->Triggered = 0;
            MoveBuf->Params[2] = 1;
            MoveBuf->Axis = 1;
            LastMoveBuf = MoveBuf;
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            if (ev_door1up->shotdamage > 0)
                MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 4, 1);
            else
                MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 0, 1);
            if (LastSolidBox && MoveBuf->SolidBox)
                LastSolidBox->LinkedData = MoveBuf->SolidBox;
            LastSolidBox = MoveBuf->SolidBox;
            *shortpnt = numbrushes;
            MoveBuf++;
            numbrushes++;
            shortpnt++;
        }
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1UP));
}

struct PSXSUBEVENT *PSXDOOR1SIDE_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVDOOR1SIDE *ev_door1side;
    short c;
    short Obj;
    short *shortpnt;
    short *shortpnt2;
    struct BRUSHMOVEBUF *LastMoveBuf;

    LastMoveBuf = 0;
    if (subevent->evsize != sizeof(struct PSXEVDOOR1SIDE))
        return subevent;
    ev_door1side = (struct PSXEVDOOR1SIDE *)subevent;
    shortpnt = &ev_door1side->portal;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    shortpnt = ev_door1side->crossobj;
    MoveBuf = &BrushMoveBufs[numbrushes];
    ev_door1side->portal = *shortpnt2;
    shortpnt2 += 2;
    if (ev_door1side->portal != -1)
        Scene2Data[ev_door1side->portal].NoDraw = 1;
    for (c = 0; c < 4; c++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            if (LastMoveBuf) {
                LastMoveBuf->Link = MoveBuf;
                MoveBuf->Primary = 0;
                MoveBuf->PortalObj = -1;
            } else {
                MoveBuf->Primary = (void (*)())PrimaryDoor2_PDH;
                MoveBuf->PortalObj = ev_door1side->portal;
            }
            MoveBuf->Triggered = 0;
            MoveBuf->Axis = ev_door1side->axis;
            MoveBuf->Key = ev_door1side->key;
            LastMoveBuf = MoveBuf;
            if (ev_door1side->axis)
                MoveBuf->Range = -ev_door1side->range;
            else
                MoveBuf->Range = ev_door1side->range;
            MoveBuf->Speed = abs(ev_door1side->speed);
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 0, 1);
            *shortpnt = numbrushes;
            MoveBuf++;
            numbrushes++;
        }
        shortpnt++;
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR1SIDE));
}

struct PSXSUBEVENT *PSXDOOR2SIDE(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVDOOR2SIDE *ev_door2side;

    if (subevent->evsize != sizeof(struct PSXEVDOOR2SIDE))
        return subevent;
    ev_door2side = (struct PSXEVDOOR2SIDE *)subevent;
    if (ev_door2side->crossobjl[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door2side->crossobjl[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door2side->StartDelay * 300;
            if (ev_door2side->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door2side->EndDelay * 300;
        }
    }
    if (ev_door2side->crossobjr[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev_door2side->crossobjr[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 3)
            MoveBuf->OpenFlag = 1;
        else if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev_door2side->StartDelay * 300;
            if (ev_door2side->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev_door2side->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR2SIDE));
}

struct PSXSUBEVENT *PSXDOOR2SIDE_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVDOOR2SIDE *ev_door2side;
    short c;
    short d;
    short Obj;
    short *shortpnt;
    short *shortpnt2;
    struct BRUSHMOVEBUF *LastMoveBuf;
    struct SOLIDBOX **SolidBoxPntrPntr;
    int NewGroup;
    short OtherDoor;

    if (subevent->evsize != sizeof(struct PSXEVDOOR2SIDE))
        return subevent;
    ev_door2side = (struct PSXEVDOOR2SIDE *)subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    OtherDoor = numbrushes;
    for (d = 0; d < 2; d++) {
        if (d == 0) {
            shortpnt = &ev_door2side->portal;
            shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
            shortpnt = ev_door2side->crossobjl;
            ev_door2side->portal = *shortpnt2;
            shortpnt2 += 2;
        } else {
            shortpnt = ev_door2side->crossobjr;
            shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
        }
        if (ev_door2side->portal != -1)
            Scene2Data[ev_door2side->portal].NoDraw = 1;
        LastMoveBuf = 0;
        NewGroup = 1;
        SolidBoxPntrPntr = &MoveBuf->SolidBox;
        for (c = 0; c < 4; c++) {
            *shortpnt = -1;
            if (*shortpnt2 >= 0 && numbrushes < 48) {
                memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
                MoveBuf->PortalObj = -1;
                if (LastMoveBuf)
                    LastMoveBuf->Link = MoveBuf;
                else {
                    MoveBuf->Primary = (void (*)())PrimaryDoor2_PDH;
                    if (d == 1) {
                        MoveBuf->PortalObj = ev_door2side->portal;
                        MoveBuf->OtherDoor = OtherDoor + 1;
                    }
                }
                MoveBuf->Axis = ev_door2side->axis;
                MoveBuf->Key = ev_door2side->key;
                LastMoveBuf = MoveBuf;
                if (ev_door2side->axis)
                    MoveBuf->Range = -ev_door2side->range;
                else
                    MoveBuf->Range = ev_door2side->range;
                MoveBuf->Speed = abs(ev_door2side->speed);
                if (d)
                    MoveBuf->Range = -MoveBuf->Range;
                MoveBuf->OpenFlag = 0;
                Obj = *shortpnt2;
                MoveBuf->Obj = Obj;
                Scene2Data[Obj].MoveBufId = numbrushes + 1;
                *SolidBoxPntrPntr = GenSolidBox(&Scene2Data[Obj].VectBox, 0, NewGroup);
                if (*SolidBoxPntrPntr) {
                    SolidBoxPntrPntr = &(*SolidBoxPntrPntr)->LinkedData;
                    NewGroup = 0;
                }
                *shortpnt = numbrushes;
                MoveBuf++;
                numbrushes++;
            }
            shortpnt++;
            shortpnt2++;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVDOOR2SIDE));
}

struct PSXSUBEVENT *PSXEXPLODE(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVEXPLODE *ev_exp;
    short b;
    short c;
    struct SCENE2 *Scene2;
    struct VECT Location;

    if (subevent->evsize != sizeof(struct PSXEVEXPLODE))
        return subevent;
    ev_exp = (struct PSXEVEXPLODE *)subevent;
    subevent = (struct PSXSUBEVENT *)(ev_exp + 1);
    if (damage) {
        ev_exp->shotdamage -= damage;
        SceneObjFragments((int)cre, ev_exp->shotdeb, 0);
        if (ev_exp->shotdamage > 0)
            return 0;
    }
    if (ev_exp->portal != -1) {
        Scene2 = &Scene2Data[ev_exp->portal];
        Scene2->NoDraw = 0;
    }
    for (b = 0; b < 4; b++) {
        if (ev_exp->crossobj[b] >= 0) {
            Scene2 = &Scene2Data[ev_exp->crossobj[b]];
            Location.vx = Scene2->VectBox.Min.vx + (Scene2->VectBox.Max.vx - Scene2->VectBox.Min.vx) / 2;
            Location.vy = Scene2->VectBox.Min.vy + (Scene2->VectBox.Max.vy - Scene2->VectBox.Min.vy) / 2;
            Location.vz = Scene2->VectBox.Min.vz + (Scene2->VectBox.Max.vz - Scene2->VectBox.Min.vz) / 2;
            c = (signed char)ev_exp->expdeb;
            if (!Event_MuteLevFlag) {
                if (c < 0) {
                    QuakeGenExplosion(&Location, Scene2->Area, 0x1000, 0);
                    c = ~c;
                    SoundAtLocation(Sound_Explosion_800B2A68, &Location);
                }
                SceneObjFragments(ev_exp->crossobj[b], c, 0);
            }
            HideObjWithSolid(ev_exp->crossobj[b]);
        }
        if (ev_exp->repobj[b] >= 0)
            Scene2Data[ev_exp->repobj[b]].NoDraw = 0;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVEXPLODE));
}

struct PSXSUBEVENT *PSXEXPLODE_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVEXPLODE *ev_exp;
    struct PSXEVEXPLODE *ev_exp2;
    struct SCENE2 *Scene2;
    short c;

    if (subevent->evsize != sizeof(struct PSXEVEXPLODE))
        return subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    ev_exp2 = (struct PSXEVEXPLODE *)((int)subevent - (int)eventlistbase + (int)neweventlistbase);
    ((struct PSXEVEXPLODE *)subevent)->portal = ev_exp2->portal;
    ev_exp = (struct PSXEVEXPLODE *)subevent;
    if (ev_exp->portal != -1) {
        Scene2 = &Scene2Data[ev_exp->portal];
        Scene2->NoDraw = 1;
    }
    for (c = 0; c < 8; c++)
        ev_exp->crossobj[c] = ev_exp2->crossobj[c];
    if (ev_exp->shotdamage) {
        for (c = 0; c < 4; c++) {
            if (ev_exp->crossobj[c] >= 0 && numbrushes < 48) {
                memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
                Scene2Data[ev_exp->crossobj[c]].NoDraw = 0;
                Scene2Data[ev_exp->crossobj[c]].MoveBufId = numbrushes + 1;
                numbrushes++;
                AttatchSolidBoxForObj(ev_exp->crossobj[c]);
                MoveBuf->DamageFunction = (int (*)())PSXEXPLODE;
                MoveBuf->EvOffset = (char *)ev_exp - (char *)eventlistbase;
                MoveBuf->EvHdrOffset = Event_GlobalEVOFFSET;
                if (MoveBuf->SolidBox)
                    MoveBuf->SolidBox->Flags = 4;
                MoveBuf++;
            }
            if (ev_exp->repobj[c] >= 0)
                Scene2Data[ev_exp->repobj[c]].NoDraw = 1;
        }
    } else {
        for (c = 0; c < 4; c++) {
            if (ev_exp->crossobj[c] >= 0 && numbrushes < 48) {
                Scene2Data[ev_exp->crossobj[c]].NoDraw = 0;
                Scene2Data[ev_exp->crossobj[c]].MoveBufId = numbrushes + 1;
                numbrushes++;
                AttatchSolidBoxForObj(ev_exp->crossobj[c]);
            }
            if (ev_exp->repobj[c] >= 0)
                Scene2Data[ev_exp->repobj[c]].NoDraw = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVEXPLODE));
}

void Event_TransferObjects(void)
{
    int a;
    int b;
    struct PSXSUBEVENT *(*funcaddr)();
    int func;
    struct PSXSUBEVHDR *eventhdr;
    struct PSXSUBEVENT *subevent;
    int numevents;
    int *name;

    numevents = eventlistbase->nummainevents;
    name = (int *)eventlistbase + 1;
    Event_KillTimers();
    numbrushes = 0;
    for (a = 47; a >= 0; a--)
        BrushMoveBufs[a].Primary = 0;
    while (*name)
        name += 4;
    eventhdr = (struct PSXSUBEVHDR *)(name + 1);
    firsteventlist = eventhdr;
    for (a = 0; a < numevents; a++) {
        if (!eventhdr->OnEntry && !eventhdr->OnExit)
            eventhdr->WhileIn = 1;
        Event_GlobalEVOFFSET = (char *)eventhdr - (char *)eventlistbase;
        subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
        for (b = 0; b < eventhdr->numevents; b++) {
            switch (subevent->type) {
            case 3:
                PSXDOOR1UP_T(subevent);
                break;
            case 4:
                PSXDOOR1SIDE_T(subevent);
                break;
            case 5:
                PSXDOOR2SIDE_T(subevent);
                break;
            case 8:
                PSXEXPLODE_T(subevent);
                break;
            case 9:
                if (subevent->evsize == sizeof(struct PSXEVUSRFUNC_TIMER)) {
                    ((int *)subevent)[1] = -1;
                    ((struct PSXEVUSRFUNC_TIMER *)subevent)->repeat = ((struct PSXEVUSRFUNC_TIMER *)subevent)->groupsof;
                }
                break;
            case 22:
                func = ((struct PSXEVUSRFUNC_TIMER *)subevent)->func4;
                if (func >= 0 && func < numuserfunctions) {
                    funcaddr = (struct PSXSUBEVENT *(*)())usrfunclist[func * 3 + 1];
                    if (funcaddr)
                        funcaddr(subevent);
                    MoveBuf = &BrushMoveBufs[numbrushes];
                }
                break;
            }
            subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
        }
        eventhdr = (struct PSXSUBEVHDR *)subevent;
    }
}

struct PSXSUBEVENT *PSXTIMER(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_TIMER *ev;
    int a;
    int time;
    struct PSXSUBEVENT *subevent2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_TIMER))
        return subevent;
    ev = (struct PSXEVUSRFUNC_TIMER *)subevent;
    a = Event_FindTimer(ev);
    if (a >= 0) {
        subevent2 = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
        EventTimers[a].repeat = ev->repeat;
        EventTimers[a].groupsof = ev->groupsof;
        EventTimers[a].timernextsubevent = subevent2;
        EventTimers[a].timersubevent = (struct PSXSUBEVENT *)ev;
        EventTimers[a].eventhdroffset = Event_GlobalEVOFFSET;
        time = ev->random * rand() / 32768 + ev->time;
        EventTimers[a].currtime = time * 30;
    }
    nomoresubevents = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_TIMER));
}

void Event_KillTimers(void)
{
    int a;

    for (a = 7; a >= 0; a--)
        EventTimers[a].timersubevent = 0;
}

int Event_DoTimerSub(struct EVENTTIMER *timer, struct CREATURE *Cre)
{
    int a;
    int b;
    struct PSXSUBEVHDR *eventhdr;
    struct PSXSUBEVENT *subevent;
    struct PSXSUBEVENT *subevent2;

    /* The early `return 0` block is shared with the first loop's `return 0` (bnez + j there,
       not beqz to the epilogue): a goto to one return, presumably as in the original. */
    if (!eventlistbase) {
fail:
        return 0;
    }
    Event_GlobalEVOFFSET = timer->eventhdroffset;
    eventhdr = (struct PSXSUBEVHDR *)(timer->eventhdroffset + (int)eventlistbase);
    if (eventhdr->disabled)
        return 1;
    subevent2 = timer->timernextsubevent;
    eventhdr_start = eventstack;
    eventhdr_end = eventstack;
    eventhdr->disabled = eventhdr->oneshot;
    eventhdr->used = 1;
    a = 0;
    subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
    while (subevent != subevent2) {
        a++;
        if (a >= eventhdr->numevents)
            goto fail;
        subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
    }
    b = timer->groupsof;
    do {
        if (Do_SubEvent(subevent, Cre))
            b = 0;
        a++;
        if (a == eventhdr->numevents) {
            subevent2 = timer->timersubevent;
            subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
            a = 0;
            while (subevent != subevent2) {
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
                a++;
            }
        }
        subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
        b--;
        if (subevent == timer->timernextsubevent)
            break;
    } while (b > 0);
    timer->timernextsubevent = subevent;
    while (eventhdr_start < eventhdr_end) {
        Event_GlobalEVOFFSET = *eventhdr_start;
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + *eventhdr_start++);
        if (!eventhdr->disabled) {
            eventhdr->disabled = eventhdr->oneshot;
            eventhdr->used = 1;
            subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
            nomoresubevents = 0;
            for (a = 0; a < eventhdr->numevents; a++) {
                if (Do_SubEvent(subevent, Cre))
                    break;
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
            }
        }
    }
    return 1;
}

void Event_ProcessTimers(void)
{
    struct PSXSUBEVENT *subevent;
    short a;
    int time;

    for (a = 0; a < 8; a++) {
        if (EventTimers[a].timersubevent) {
            time = EventTimers[a].currtime - NewVcounts;
            EventTimers[a].currtime = time;
            if (time < 0) {
                subevent = EventTimers[a].timersubevent;
                nomoresubevents = 0;
                Event_DoTimerSub(&EventTimers[a], 0);
                if (--EventTimers[a].repeat == 0)
                    EventTimers[a].timersubevent = 0;
                else {
                    if (EventTimers[a].repeat == -1)
                        EventTimers[a].repeat = 0;
                    EventTimers[a].currtime = (((struct PSXEVUSRFUNC_TIMER *)subevent)->random * rand() / 32768 + ((struct PSXEVUSRFUNC_TIMER *)subevent)->time) * 30;
                }
            }
        }
    }
}

int Do_SubEvent(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    int b;
    int func;
    short *shortpnt;
    struct PSXSUBEVENT *(*funcaddr)();
    struct PSXEVIMMEDIATE *ev_imm;
    struct PSXEVEVENABLE *ev_evenab;
    struct PSXEVEVDISABLE *ev_evdisab;
    struct PSXSUBEVHDR *eventhdr2;
    struct IDENTITYNAME zonename;
    struct VECT Location;
    struct VECT Min;
    struct VECT Max;

    if (subevent->disabled)
        return 0;
    subevent->disabled = subevent->oneshot;
    switch (subevent->type) {
    case 2:
        ev_imm = (struct PSXEVIMMEDIATE *)subevent;
        if ((ev_imm->count + (ev_imm->count & 1)) * 2 + 4 != subevent->evsize)
            break;
        shortpnt = (short *)(ev_imm + 1);
        for (b = 0; b < ev_imm->count; b++)
            *eventhdr_end++ = *shortpnt++;
        return 0;
    case 3:
        PSXDOOR1UP_inl(subevent, Cre, 0);
        break;
    case 4:
        PSXDOOR1SIDE(subevent, Cre, 0);
        break;
    case 5:
        PSXDOOR2SIDE(subevent, Cre, 0);
        return 0;
    case 8:
        PSXEXPLODE(subevent, Cre, 0);
        return 0;
    case 9:
        if (subevent->evsize != sizeof(struct PSXEVCOUNTER))
            return 0;
        b = ((struct PSXEVCOUNTER *)subevent)->lastcount;
        ((struct PSXEVCOUNTER *)subevent)->lastcount = ChrisCount + 1;
        if (b == ChrisCount)
            return 1;
        if (--((struct PSXEVCOUNTER *)subevent)->count == 0) {
            if (((struct PSXEVCOUNTER *)subevent)->oneshot)
                ((struct PSXEVCOUNTER *)subevent)->disabled = 1;
            else
                ((struct PSXEVCOUNTER *)subevent)->count = ((struct PSXEVCOUNTER *)subevent)->reset;
            break;
        }
        return 1;
    case 22:
        func = ((struct PSXEVUSRFUNC_TIMER *)subevent)->func4;
        if (func < 0)
            return 0;
        if (func >= numuserfunctions)
            break;
        funcaddr = (struct PSXSUBEVENT *(*)())usrfunclist[func * 3];
        funcaddr(subevent, Cre, 0);
        if (nomoresubevents)
            return 1;
        break;
    case 15:
        if (subevent->evsize != 16)
            return 0;
        if (Event_MuteLevFlag)
            return 0;
        if (MaybeLoadZoneName(*(struct IDENTITYNAME *)((char *)subevent + 2)))
            return 1;
        break;
    case 19:
        if (subevent->evsize != 8)
            return 0;
        DoDamage(0, Cre, ((short *)subevent)[2], ((short *)subevent)[1]);
        return 0;
    case 20:
        ev_evenab = (struct PSXEVEVENABLE *)subevent;
        if ((ev_evenab->count + (ev_evenab->count & 1)) * 2 + 4 != subevent->evsize)
            break;
        shortpnt = (short *)(ev_evenab + 1);
        for (b = 0; b < ev_evenab->count; b++) {
            eventhdr2 = (struct PSXSUBEVHDR *)((char *)eventlistbase + *shortpnt++);
            eventhdr2->disabled = 0;
        }
        return 0;
    case 21:
        ev_evdisab = (struct PSXEVEVDISABLE *)subevent;
        if ((ev_evdisab->count + (ev_evdisab->count & 1)) * 2 + 4 != subevent->evsize)
            return 0;
        shortpnt = (short *)(ev_evdisab + 1);
        for (b = 0; b < ev_evdisab->count; b++) {
            eventhdr2 = (struct PSXSUBEVHDR *)((char *)eventlistbase + *shortpnt++);
            eventhdr2->disabled = 1;
        }
        break;
    }
    return 0;
}

int Event_DoFromSub(int eventhdroffset, struct CREATURE *Cre, struct PSXSUBEVENT *subevent2)
{
    int a;
    struct PSXSUBEVHDR *eventhdr;
    struct PSXSUBEVENT *subevent;

    if (!eventlistbase)
        return 0;
    eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + eventhdroffset);
    eventhdr_end = eventhdr_start = eventstack;
    Event_GlobalEVOFFSET = eventhdroffset;
    if (eventhdr->disabled)
        return 1;
    eventhdr->disabled = eventhdr->oneshot;
    eventhdr->used = 1;
    subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
    a = 0;
    while (subevent != subevent2) {
        a++;
        if (a >= eventhdr->numevents)
            return 0;
        subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
    }
    for (; a < eventhdr->numevents; a++) {
        if (Do_SubEvent(subevent, Cre))
            break;
        subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
    }
    while (eventhdr_start < eventhdr_end) {
        Event_GlobalEVOFFSET = *eventhdr_start;
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + *eventhdr_start++);
        if (!eventhdr->disabled) {
            eventhdr->disabled = eventhdr->oneshot;
            eventhdr->used = 1;
            subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
            nomoresubevents = 0;
            for (a = 0; a < eventhdr->numevents; a++) {
                if (Do_SubEvent(subevent, Cre))
                    break;
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
            }
        }
    }
    return 1;
}

int Event_SetDisableNamed(struct IDENTITYNAME name3, int value)
{
    int offset;
    struct PSXSUBEVHDR *eventhdr;

    offset = Event_GetNamedOffset(name3);
    if (offset) {
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + offset);
        eventhdr->disabled = value;
        return 1;
    }
    return 0;
}

int Event_GetDisableNamed(struct IDENTITYNAME name3)
{
    int offset;
    struct PSXSUBEVHDR *eventhdr;

    offset = Event_GetNamedOffset(name3);
    if (offset) {
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + offset);
        return eventhdr->disabled;
    }
    return -1;
}

int Event_GetNamedOffset(struct IDENTITYNAME name3)
{
    int *name2;
    int *name;
    name2 = (int *)&name3;
    name = (int *)eventlistbase + 1;
    while (*name) {
        if (name[0] == name2[0] && name[1] == name2[1] && name[2] == name2[2])
            return name[3];
        name += 4;
    }
    return 0;
}

int Event_DoNamed(struct IDENTITYNAME name3, struct CREATURE *Cre)
{
    int *name2;
    int *name;

    name2 = (int *)&name3;
    if (eventlistbase) {
        name = (int *)eventlistbase + 1;
        while (*name) {
            if (name[0] == name2[0] && name[1] == name2[1] && name[2] == name2[2]) {
                Event_DoOffset(name[3], Cre);
                return 1;
            }
            name += 4;
        }
    }
    return 0;
}

void Event_ProcessTriggers(struct CREATURE *Cre, int typemask)
{
    int Indx;
    int NumTriggers;
    int a;
    struct COLL3DSPACE *Space;
    int numevents;
    struct PSXSUBEVHDR *eventhdr;
    struct COLLHEADER *Map;  /* not in SYM: an unallocated pointer, rematerialised as lui/addiu $a3 */

    Map = &TrigBoundCollMap;
    numevents = eventlistbase->nummainevents;
    NumTriggers = Map->TotalNumSpaces;
    for (Space = Map->Coll3dSpace, Indx = 0; Indx < NumTriggers; Indx++, Space++) {
        if (typemask & Space->u.trigflags) {
            if ((signed char)Space->Area == -1 || Cre->Box.Area == Space->Area) {
                if (In3dSpaceIndx(Map, &Cre->Box.Location, (int)Space) /* sic: a2 is the Space pointer */) {
                    eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + Space->StartJoin);
                    eventhdr->JustTriggered = 1;
                    if (eventhdr->WhileIn || (eventhdr->OnEntry && !eventhdr->LastTriggered))
                        Event_DoOffset(Space->StartJoin, Cre);
                }
            }
        }
    }
    eventhdr = firsteventlist;
    for (a = 0; a < numevents; a++) {
        if (eventhdr->OnExit) {
            if (eventhdr->LastTriggered && !eventhdr->JustTriggered)
                Event_DoOffset(Space->StartJoin, Cre);
        }
        eventhdr->LastTriggered = eventhdr->JustTriggered;
        eventhdr->JustTriggered = 0;
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventhdr + eventhdr->evsize);
    }
}

struct PSXSUBEVENT *PSXLIFT1EVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_LIFT1 *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LIFT1))
        return subevent;
    ev = (struct PSXEVUSRFUNC_LIFT1 *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    subevent = (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_LIFT1));
    return subevent;
}

/* PSXLIFT1EVENT_T: the original has a dead `lui $v1,%hi(PrimaryDoor2_PDH)` that splat leaves as a
   bare constant (0x8002); cmpfunc compares %hi/%lo of functions by value. */

struct PSXSUBEVENT *PSXLIFT1EVENT_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_LIFT1 *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    struct BRUSHMOVEBUF *LastMoveBuf;
    short *shortpnt;
    short *shortpnt2;
    int c;
    int Obj;
    struct SOLIDBOX **SolidBoxPntrPntr;
    int NewGroup;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LIFT1))
        return subevent;
    ev = (struct PSXEVUSRFUNC_LIFT1 *)subevent;
    shortpnt = ev->crossobj;
    LastMoveBuf = 0;
    NewGroup = 1;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    MoveBuf = &BrushMoveBufs[numbrushes];
    SolidBoxPntrPntr = &MoveBuf->SolidBox;
    for (c = 0; c < 4; c++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            if (LastMoveBuf)
                LastMoveBuf->Link = MoveBuf;
            else
                MoveBuf->Primary = (void (*)())PrimaryDoor2_PDH;
            LastMoveBuf = MoveBuf;
            MoveBuf->PortalObj = -1;
            MoveBuf->Axis = 1;
            MoveBuf->Speed = abs(ev->speed);
            MoveBuf->Range = -ev->range;
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            *SolidBoxPntrPntr = GenSolidBox(&Scene2Data[Obj].VectBox, 0, NewGroup);
            if (*SolidBoxPntrPntr) {
                SolidBoxPntrPntr = &(*SolidBoxPntrPntr)->LinkedData;
                NewGroup = 0;
            }
            *shortpnt = numbrushes;
            MoveBuf++;
            numbrushes++;
        }
        shortpnt++;
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_LIFT1));
}

struct PSXSUBEVENT *PSXSIMROTEVENT2(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT2 *ev;
    int a;

    ev = (struct PSXEVUSRFUNC_SIMROT2 *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->crossobj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->crossobj[a]];
            MoveBuf->Triggered = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_SIMROT2));
}

void PrimarySimRot_PDH2(struct BRUSHMOVEBUF *MoveBuf)
{
    short *ang;
    if (MoveBuf->Triggered) {
        ang = (short *)&MoveBuf->Ang;
        ang += MoveBuf->Axis;
        *ang = (*ang + MoveBuf->Speed) & 0xfff;
        MoveBuf->Triggered = 0;
    }
}

struct PSXSUBEVENT *PSXSIMROTEVENT2_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT2 *ev;
    short *shortpnt;
    short *shortpnt2;
    int c;
    int Obj;

    ev = (struct PSXEVUSRFUNC_SIMROT2 *)subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    shortpnt = ev->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    for (c = 0; c < 4; c++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            MoveBuf->Primary = (void (*)())PrimarySimRot_PDH2;
            MoveBuf->Axis = ev->axis;
            MoveBuf->Speed = ev->speed;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx;
            MoveBuf->RotOff.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy;
            MoveBuf->RotOff.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz;
            *shortpnt = numbrushes;
            MoveBuf++;
            numbrushes++;
        }
        shortpnt++;
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_SIMROT2));
}

struct PSXSUBEVENT *PSXSIMROTEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT *ev;
    int a;

    ev = (struct PSXEVUSRFUNC_SIMROT *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->crossobj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->crossobj[a]];
            MoveBuf->Triggered = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_SIMROT));
}

void PrimarySimRot_PDH(struct BRUSHMOVEBUF *MoveBuf)
{
    short *ang;
    if (MoveBuf->Triggered) {
        ang = (short *)&MoveBuf->Ang;
        ang += MoveBuf->Axis;
        MoveBuf->PrecisionRange += MoveBuf->Speed * NewVcounts;
        *ang = (MoveBuf->PrecisionRange >> 8) & 0xfff;
        MoveBuf->Triggered = 0;
    }
}

struct PSXSUBEVENT *PSXSIMROTEVENT_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SIMROT *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    short *shortpnt;
    short *shortpnt2;
    int c;
    int Obj;

    ev = (struct PSXEVUSRFUNC_SIMROT *)subevent;
    shortpnt = ev->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    MoveBuf = &BrushMoveBufs[numbrushes];
    for (c = 0; c < 4; c++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            MoveBuf->Primary = (void (*)())PrimarySimRot_PDH;
            MoveBuf->PortalObj = -1;
            MoveBuf->Axis = ev->axis;
            MoveBuf->Speed = ev->speed;
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx;
            MoveBuf->RotOff.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy;
            MoveBuf->RotOff.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz;
            *shortpnt = numbrushes;
            MoveBuf++;
            numbrushes++;
        }
        shortpnt++;
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_SIMROT));
}

struct PSXSUBEVENT *PSXFLICKERINGLIGHT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_FLICKERINGLIGHT *ev;

    ev = (struct PSXEVUSRFUNC_FLICKERINGLIGHT *)subevent;
    if (ev->Location.vx & 1) {
        int a;

        for (a = 0; a < MaxWindows; a++) {
            if (AreaValid1(a, ev->Area)) {
                struct POWER Power;
                struct COLOUR Colour;

                Colour.r = ev->Rgb.r;
                Colour.g = ev->Rgb.g;
                Colour.b = ev->Rgb.b;
                Power.Full = rand() * 500 / 32768 + 400;
                Power.Fall = rand() * 500 / 32768 + 1000;
                AddLight(&ev->Location, Colour, Power, (struct SHORTPAIR){ 0, 0 });
                return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT));
            }
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT));
}

struct PSXSUBEVENT *PSXFLICKERINGLIGHT_T(struct PSXSUBEVENT *subevent)
{
    struct VECT *Loc;
    struct VECT *Loc2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT))
        return subevent;
    Loc = &((struct PSXEVUSRFUNC_FLICKERINGLIGHT *)subevent)->Location;
    Loc2 = (struct VECT *)((int)Loc - (int)eventlistbase + (int)neweventlistbase);
    ((struct PSXEVUSRFUNC_FLICKERINGLIGHT *)subevent)->Location = *Loc2;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_FLICKERINGLIGHT));
}

struct PSXSUBEVENT *PSXSETWIBBLE(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SETWIBBLE *SetWibble = (struct PSXEVUSRFUNC_SETWIBBLE *)subevent;
    Scene2Data[SetWibble->PortalNum].SubDiv = SetWibble->WibbleMode;
}

struct PSXSUBEVENT *PSXSETWIBBLE_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_SETWIBBLE *SetWibble;
    short *Obj;
    short *Obj2;
    SetWibble = (struct PSXEVUSRFUNC_SETWIBBLE *)subevent;
    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_SETWIBBLE))
        return subevent;
    Obj = &SetWibble->PortalNum;
    Obj2 = (short *)((int)Obj - (int)eventlistbase + (int)neweventlistbase);
    SetWibble->PortalNum = *Obj2;
}

struct PSXSUBEVENT *PSXTELEPORT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    struct STARTPOSITION *StartPosition;
    struct IDENTITYNAME ZoneName = {"Zone"};
    struct PSXSUBEVHDR *eventhdr;

    eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + Event_GlobalEVOFFSET);
    eventhdr->WhileIn = 1;
    if (!Cre->ModelOpaque) {
        StartPosition = FindStartPosition(*(struct IDENTITYNAME *)((char *)subevent + 4));
        if (StartPosition) {
            ZoneName.Name[4] = StartPosition->Zone + '0';
            if (MaybeLoadZoneName(ZoneName)) {
                Cre->Box.Location = StartPosition->Location;
                Cre->Box.Location.vy -= 0x11e;
                ReCalcCollBox(Cre);
            } else if (SetupBoxDrop(&Cre->Box, &StartPosition->Location, *&(struct SCRPNT){0x11e, 0x23c}, *&(struct SHORTPAIR){-1, 0x400}, PlayerHitRadius))
                Cre->Ang = (struct SVECT){0, StartPosition->AngY, 0};
            else
                while (1)
                    ;
        }
    } else
        Cre->Box.InTeleporter = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

void PSXTELEPORT_T(struct PSXSUBEVENT *subevent)
{
    struct COLLHEADER *Hdr;
    struct COLL3DSPACE *Space;
    short a;    /* never initialised: the original reads it from $a0 */
    short n;    /* not in SYM, but needed so the bound is hoisted past the TelePortData store */

    Hdr = &TrigBoundCollMap;
    Space = Hdr->Coll3dSpace;
    n = Hdr->TotalNumSpaces;
    for (; a < n; a++, Space++) {
        if (*(signed char *)&Space->Area != -1) {
            if (Space->StartJoin == Event_GlobalEVOFFSET) {
                TelePortData[NumTelePorts] = (char *)Space - (char *)Hdr->Coll3dSpace;
                NumTelePorts++;
                return;
            }
        }
    }
}

/* DrawTelePorts: the original has a dead `lui $v1,%hi(TrigBoundCollMap)` (Map's hoisted %hi; its %lo
   use was rematerialised in the loop) that splat leaves as the bare constant 0x800D. */
void DrawTelePorts(void)
{
    short a;
    short i;
    short Directshun;
    struct COLL3DSPACE *Space;
    struct SVECT Vel[15];
    struct VECT PartsOffset;
    struct COLLHEADER *Map;  /* not in SYM: its hoisted %hi is the original's orphan lui $v1 */
    int Timer;

    Map = &TrigBoundCollMap;
    if ((LevelTicks & 127) < 63)
        Directshun = -1;
    else
        Directshun = 1;
    for (a = 0; a < 15; a++) {
        Vel[a].vx = (rand() - 0x4000) >> 13;
        Vel[a].vy = (rand() >> 8) * Directshun;
        Vel[a].vz = (rand() - 0x4000) >> 13;
    }
    for (a = 0; a < NumTelePorts; a++) {
        Space = (struct COLL3DSPACE *)(TelePortData[a] + (int)Map->Coll3dSpace);
        PartsOffset.vx = (Space->MinMax.Min.vx + Space->MinMax.Max.vx) / 2;
        PartsOffset.vz = (Space->MinMax.Min.vz + Space->MinMax.Max.vz) / 2;
        Timer = 24;  /* a separate constant pseudo in the loop: changes which invariants loop.c hoists */
        if (Directshun > 0)
            PartsOffset.vy = Space->MinMax.Min.vy;
        else
            PartsOffset.vy = Space->MinMax.Max.vy;
        PartsOffset.vx += (rand() & 255) - 127;
        PartsOffset.vz += (rand() & 255) - 127;
        AddParticlesSL(&PartsOffset, Vel, ParticleColoursTable[2], ParticleColoursTable[0], 15, (rand() + Timer) & 15,
                       (rand() & 0x2047) + 12000, Space->Area);
        PartsOffset.vy = (Space->MinMax.Min.vy + Space->MinMax.Max.vy) / 2;
        i = LevelTicks & 0x1ff;
        if (i > 256)
            i = 512 - i;
        AddLight(&PartsOffset, *&(struct COLOUR){ 255, 255, 255, 0 }, (struct POWER){ 400, (i * 4) + 800 },
                 *&(struct SHORTPAIR){ 0, 0 });
    }
}

void PSXINWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        cre->Box.InShallowWater = 1;
}

void PSXUNDERWATER(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        cre->Box.InWater = 1;
}

void PSXINCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InCrouch = 1;
}

void PSXINLOWCROUCH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InLowCrouch = 1;
}

void PSXINSECRET(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    char *String;

    if (!Event_MuteLevFlag) {
        String = GetStringFromStringtable((struct IDENTITYNAME){"FoundASecret"});
        if (String) {
            centreprintf(GameWindows, String);
            SoundAtLocation(Sound_Secret, (struct VECT *)&cre->Box);
        }
        GlobalSecretsFound++;
        AddSecretInterMission(GetStringFromStringtable((struct IDENTITYNAME){"MapTitle"}));
    }
}

void PSXINSECRET_T(struct PSXSUBEVENT *subevent)
{
    GlobalSecretsCount++;
}

void PSXINACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre)
        DoDamage(0, cre, 1, 9);
}

void PSXUNDERACID(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre) {
        cre->Box.InWater = 1;
        cre->Box.InAcid = 1;
        DoDamage(0, cre, 1, 9);
    }
}

void PSXINLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    cre->Box.InAcid = 1;
    if (cre)
        DoDamage(0, cre, 20, 10);
}

void PSXUNDERLAVA(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (cre) {
        cre->Box.InWater = 1;
        cre->Box.InAcid = 1;
        DoDamage(0, cre, 20, 10);
    }
}

struct PSXSUBEVENT *PSXLOADMAP(struct PSXSUBEVENT *subevent)
{
    if (ModNameCmp((struct IDENTITYNAME *)((char *)subevent + 4), &ThisLevelName) != 1) {
        NextLevelName = *(struct IDENTITYNAME *)((char *)subevent + 4);
        MainStartName = *(struct IDENTITYNAME *)((char *)subevent + 0x10);
        LevelComplete = 2;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 0x1c);
}

struct PSXSUBEVENT *PSXLOADMAP_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x1c);
}

void DoSolidBoxEventDamage(struct SOLIDBOX *solidbox, short damage, struct VECT *Location)
{
    int a;
    struct PSXSUBEVENT *subevent;

    MoveBuf = BrushMoveBufs;
    for (a = 0; a < numbrushes; a++, MoveBuf++) {
        if (MoveBuf->SolidBox == solidbox) {
            subevent = (struct PSXSUBEVENT *)(BrushMoveBufs[a].EvOffset + (int)eventlistbase);
            MoveBuf->DamageLoc = *Location;
            if (MoveBuf->DamageFunction(subevent, MoveBuf->Obj, damage)) {
                /* reusing subevent (two sets) keeps sched1 from boosting its first set */
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
                Event_GlobalEVOFFSET = BrushMoveBufs[a].EvHdrOffset;
                Event_DoFromSub(Event_GlobalEVOFFSET, 0, subevent);
            }
            return;
        }
    }
}

int StoreEventStates(void)
{
    struct PSXSUBEVHDR *eventhdr;
    struct PSXSUBEVENT *subevent;
    int a;
    int b;
    int numbitsused;
    unsigned char bitval;
    int numevents;
    struct SAVEFILEHDR *FileHdr;
    struct IDENTITYNAME filename;
    char *bits;
    int maxbits;

    numbitsused = 0;
    bitval = 1;
    maxbits = 128;
    if (!eventlistbase)
        return numbitsused;
    strcpy(filename.Name, "EVE_");
    for (a = 0; a < 8; a++)
        filename.Name[a + 4] = ThisLevelName.Name[a];
    FileHdr = OpenSaveMemory(filename, 1);
    if (FileHdr) {
        bits = (char *)(FileHdr + 1);
        numevents = eventlistbase->nummainevents;
        eventhdr = firsteventlist;
        for (a = 0; a < numevents; a++) {
            if (numbitsused < maxbits) {
                if (bitval == 1)
                    *bits = 0;
                if (eventhdr->used && eventhdr->disabled)
                    *bits |= bitval;
                bitval <<= 1;
                if (!bitval) {
                    bitval = 1;
                    bits++;
                }
            }
            numbitsused++;
            subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
            for (b = 0; b < eventhdr->numevents; b++) {
                if (subevent->oneshot) {
                    if (numbitsused < maxbits) {
                        if (bitval == 1)
                            *bits = 0;
                        if (subevent->disabled)
                            *bits |= bitval;
                        bitval <<= 1;
                        if (!bitval) {
                            bitval = 1;
                            bits++;
                        }
                    }
                    numbitsused++;
                }
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
            }
            eventhdr = (struct PSXSUBEVHDR *)((char *)eventhdr + eventhdr->evsize);
        }
        FileHdr->Size = (numbitsused + 31) / 32 * 4;
        CloseWritableSaveMemoryFile(filename, 1);
    }
    return numbitsused;
}

int RestoreEvents2(void)
{
    struct PSXSUBEVHDR *eventhdr;
    struct PSXSUBEVHDR *eventhdr2;
    struct PSXSUBEVENT *subevent;
    short a;
    short b;
    short numbitsused;
    short allsubevents;
    short onesubevent;
    unsigned char bitval;
    short numevents;
    struct SAVEFILEHDR *FileHdr;
    struct IDENTITYNAME filename;
    char *bits;

    numbitsused = 0;
    bitval = 1;
    if (!eventlistbase)
        return 0;
    strcpy(filename.Name, "EVE_");
    for (a = 0; a < 8; a++)
        filename.Name[a + 4] = ThisLevelName.Name[a];
    FileHdr = OpenSaveMemory(filename, 0);
    if (FileHdr) {
        bits = (char *)(FileHdr + 1);
        eventhdr = firsteventlist;
        Event_StartLevFlag = 0;
        numevents = eventlistbase->nummainevents;
        Event_MuteLevFlag = 1;
        for (a = 0; a < numevents; a++) {
            Event_GlobalEVOFFSET = (char *)eventhdr - (char *)eventlistbase;
            allsubevents = 0;
            if (numbitsused < 128) {
                if (*bits & bitval) {
                    eventhdr->used = 1;
                    eventhdr->disabled = 1;
                    allsubevents = 1;
                }
                bitval <<= 1;
                if (!bitval) {
                    bitval = 1;
                    bits++;
                }
            }
            numbitsused++;
            subevent = (struct PSXSUBEVENT *)(eventhdr + 1);
            eventhdr_end = eventhdr_start = eventstack;
            for (b = 0; b < eventhdr->numevents; b++) {
                onesubevent = 0;
                if (subevent->oneshot) {
                    if (numbitsused < 128) {
                        onesubevent = (*bits & bitval) != 0;
                        bitval <<= 1;
                        if (!bitval) {
                            bitval = 1;
                            bits++;
                        }
                    }
                    numbitsused++;
                }
                if (allsubevents || onesubevent) {
                    subevent->disabled = 0;
                    Do_SubEvent(subevent, 0);
                }
                subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
            }
            while (eventhdr_start < eventhdr_end) {
                Event_GlobalEVOFFSET = *eventhdr_start;
                eventhdr2 = (struct PSXSUBEVHDR *)((char *)eventlistbase + *eventhdr_start++);
                if (!eventhdr2->disabled) {
                    eventhdr2->disabled = eventhdr2->oneshot;
                    eventhdr2->used = 1;
                    subevent = (struct PSXSUBEVENT *)(eventhdr2 + 1);
                    nomoresubevents = 0;
                    for (b = 0; b < eventhdr2->numevents; b++) {
                        if (Do_SubEvent(subevent, 0))
                            break;
                        subevent = (struct PSXSUBEVENT *)((char *)subevent + subevent->evsize);
                    }
                }
            }
            eventhdr = (struct PSXSUBEVHDR *)((char *)eventhdr + eventhdr->evsize);
        }
        for (a = 0; a < 7; a++) {
            NewVcounts = 30000;
            LevelTicks += 30000;
            ProcessMoveBufs();
        }
        Event_MuteLevFlag = 0;
        Event_StartLevFlag = 0;
    }
    return numbitsused;
}

int RestoreEvents(void)
{
    usp = SetSp(0x1f8003fc);
    ggg = RestoreEvents2();
    SetSp(usp);
    return ggg;
}

void dofunc_error(void)
{
}

void transfunc_error(void)
{
}

void transfunc_error(void);

void InitUserFuncs(char *addr)
{
    short a;
    short found;
    struct USERFUNCLIST *funclist;
    char Error[13];

    numuserfunctions = 0;
    usrfunclist = 0;
    if (addr) {
        numuserfunctions = *(short *)addr;
        addr += 4;
        usrfunclist = (void **)addr;
        for (a = 0; a < numuserfunctions; a++) {
            found = 0;
            for (funclist = EventUserFuncList; funclist->name.Name[0]; funclist++) {
                if (ModNameCmp(&funclist->name, (struct IDENTITYNAME *)addr)) {
                    ((void **)addr)[0] = funclist->dofunc;
                    ((void **)addr)[1] = funclist->transfunc;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                *(struct IDENTITYNAME *)Error = *(struct IDENTITYNAME *)addr;
                Error[12] = 0;
                ((void **)addr)[0] = dofunc_error;
                ((void **)addr)[1] = transfunc_error;
            }
            addr += 12;
        }
    }
    InitEventSoundsEachLevel();
}

struct PSXSUBEVENT *PSXCAGELIFT1EVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_CAGELIFT1 *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_CAGELIFT1))
        return subevent;
    ev = (struct PSXEVUSRFUNC_CAGELIFT1 *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (!MoveBuf->OpenFlag) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 0xff)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    subevent = (struct PSXSUBEVENT *)(ev + 1);
    return subevent;
}

struct PSXSUBEVENT *PSXCAGELIFT1EVENT_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_CAGELIFT1 *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    struct BRUSHMOVEBUF *LastMoveBuf;
    short *shortpnt;
    short *shortpnt2;
    int c;
    int Obj;
    struct VECTBOX box;
    struct SOLIDBOX *PreviousSolidBox;

    PreviousSolidBox = 0;
    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_CAGELIFT1))
        return subevent;
    box.Max.vx = box.Max.vy = box.Max.vz = -0x7FFFFFFF;
    box.Min.vx = box.Min.vy = box.Min.vz = 0x7FFFFFFF;
    ev = (struct PSXEVUSRFUNC_CAGELIFT1 *)subevent;
    shortpnt = ev->crossobj;
    LastMoveBuf = 0;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    MoveBuf = &BrushMoveBufs[numbrushes];
    for (c = 0; c < 4; c++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            if (LastMoveBuf)
                LastMoveBuf->Link = MoveBuf;
            else
                MoveBuf->Primary = PrimaryDoor2_PDH;
            LastMoveBuf = MoveBuf;
            MoveBuf->PortalObj = -1;
            MoveBuf->Axis = 1;
            MoveBuf->Range = -ev->range;
            MoveBuf->Speed = abs(ev->speed);
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            if (box.Min.vx > Scene2Data[Obj].VectBox.Min.vx)
                box.Min.vx = Scene2Data[Obj].VectBox.Min.vx;
            if (box.Min.vy > Scene2Data[Obj].VectBox.Min.vy)
                box.Min.vy = Scene2Data[Obj].VectBox.Min.vy;
            if (box.Min.vz > Scene2Data[Obj].VectBox.Min.vz)
                box.Min.vz = Scene2Data[Obj].VectBox.Min.vz;
            if (box.Max.vx < Scene2Data[Obj].VectBox.Max.vx)
                box.Max.vx = Scene2Data[Obj].VectBox.Max.vx;
            if (box.Max.vy < Scene2Data[Obj].VectBox.Max.vy)
                box.Max.vy = Scene2Data[Obj].VectBox.Max.vy;
            if (box.Max.vz < Scene2Data[Obj].VectBox.Max.vz)
                box.Max.vz = Scene2Data[Obj].VectBox.Max.vz;
            MoveBuf++;
            *shortpnt = numbrushes;
            numbrushes++;
        }
        shortpnt++;
        shortpnt2++;
    }
    if (shortpnt[-4] >= 0) {
        struct VECTBOX VectBox;

        VectBox = box;
        VectBox.Max.vy = box.Min.vy + ev->CeilingThickness;
        MoveBuf = &BrushMoveBufs[shortpnt[-4]];
        PreviousSolidBox = GenSolidBox(&VectBox, 0, 1);
        MoveBuf->SolidBox = PreviousSolidBox;
    }
    if (shortpnt[-3] >= 0) {
        struct VECTBOX VectBox;

        VectBox = box;
        VectBox.Min.vy = box.Max.vy - ev->FloorThickness;
        if (PreviousSolidBox)
            PreviousSolidBox->LinkedData = GenSolidBox(&VectBox, 0, 0);
        else {
            MoveBuf = &BrushMoveBufs[shortpnt[-3]];
            MoveBuf->SolidBox = GenSolidBox(&VectBox, 0, 1);
        }
    }
    return (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_CAGELIFT1));
}

struct PSXSUBEVENT *PSXMISSIONCOMPLETE(struct PSXSUBEVENT *subevent)
{
    MainStartName = *(struct IDENTITYNAME *)"Default";
    LevelComplete = 7;
    return subevent;
}

struct PSXSUBEVENT *PSXSIMBUTTON(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 8)
        return subevent;
    MissionEvent(*(struct IDENTITYNAME *)"Done Dish");
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

struct PSXSUBEVENT *PSXSIMBUTTON_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 8)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

struct PSXSUBEVENT *PSXBUTTON(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_BUTTON *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_BUTTON))
        return subevent;
    ev = (struct PSXEVUSRFUNC_BUTTON *)subevent;
    if (ev->crossobj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            if (ev->range >= 0) {
                MoveBuf->Range = ev->range;
                MoveBuf->Speed = 1;
            } else {
                MoveBuf->Range = -ev->range;
                MoveBuf->Speed = -1;
            }
            MoveBuf->Time = ev->time * 300;
            if (*(signed char *)&ev->axis)
                MoveBuf->Range = -MoveBuf->Range;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_BUTTON));
}

void PSXBUTTON_M(struct BRUSHMOVEBUF *MoveBuf)
{
    char in;
    short OldXYZ;
    short NewXYZ;
    struct VECT Location;
    int a;
    struct SVECT xyz;

    memset(&xyz, 0, sizeof(struct SVECT));
    switch (MoveBuf->OpenFlag) {
    case 1:
        in = 1;
        break;
    case 2:
        if (!MoveBuf->Time) {
            in = 1;
        } else {
            a = MoveBuf->Time - NewVcounts;
            MoveBuf->Time = a;
            if (a <= 0) {
                MoveBuf->Time = 0xFFFF;
                MoveBuf->OpenFlag = 3;
            }
            in = a > 0;
        }
        break;
    case 0:
    case 3:
        in = 0;
        break;
    }
    OldXYZ = (&MoveBuf->Offset.vx)[MoveBuf->Axis];
    if (in) {
        NewXYZ = OldXYZ + MoveBuf->Speed * NewVcounts;
        if (MoveBuf->Speed > 0) {
            if (NewXYZ > MoveBuf->Range) {
                NewXYZ = MoveBuf->Range;
                if (MoveBuf->OpenFlag == 1) {
                    Location.vx = (Scene2Data[MoveBuf->Obj].VectBox.Min.vx + Scene2Data[MoveBuf->Obj].VectBox.Max.vx) / 2;
                    Location.vy = (Scene2Data[MoveBuf->Obj].VectBox.Min.vy + Scene2Data[MoveBuf->Obj].VectBox.Max.vy) / 2;
                    Location.vz = (Scene2Data[MoveBuf->Obj].VectBox.Min.vz + Scene2Data[MoveBuf->Obj].VectBox.Max.vz) / 2;
                    SoundAtLocation(Sound_Button, &Location);
                    MoveBuf->OpenFlag = 2;
                }
            }
        } else {
            if (NewXYZ < -MoveBuf->Range) {
                NewXYZ = -MoveBuf->Range;
                if (MoveBuf->OpenFlag == 1)
                    MoveBuf->OpenFlag = 2;
            }
        }
    } else {
        NewXYZ = OldXYZ - MoveBuf->Speed * NewVcounts;
        if (MoveBuf->Speed > 0) {
            if (NewXYZ <= 0) {
                NewXYZ = 0;
                MoveBuf->OpenFlag = 0;
            }
        } else {
            if (NewXYZ >= 0) {
                NewXYZ = 0;
                MoveBuf->OpenFlag = 0;
            }
        }
    }
    (&MoveBuf->Offset.vx)[MoveBuf->Axis] += NewXYZ - OldXYZ;
}

struct PSXSUBEVENT *PSXBUTTON_T(struct PSXSUBEVENT *subevent)
{
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_BUTTON))
        return subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    shortpnt = &((struct PSXEVUSRFUNC_BUTTON *)subevent)->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0 && numbrushes < 48) {
        memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
        MoveBuf->Primary = (void (*)())PSXBUTTON_M;
        MoveBuf->Axis = ((struct PSXEVUSRFUNC_BUTTON *)subevent)->axis;
        MoveBuf->OpenFlag = 0;
        Obj = *shortpnt2;
        MoveBuf->Obj = Obj;
        Scene2Data[Obj].MoveBufId = numbrushes + 1;
        *shortpnt = numbrushes;
        MoveBuf++;
        numbrushes++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_BUTTON));
}

struct PSXSUBEVENT *PSXLASERWALL(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    int a;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERWALL))
        return subevent;
    if (((struct PSXEVUSRFUNC_LASERWALL *)subevent)->crossobj >= 0) {
        if (Event_StartLevFlag) {
            if (NumLaserWalls < 8) {
                a = NumLaserWalls++;
                LaserWallEvents[a][0] = Event_GlobalEVOFFSET;
                LaserWallEvents[a][1] = (short)((char *)subevent - (char *)eventlistbase);
            }
        } else {
            DoDamage(Cre, Cre, ((struct PSXEVUSRFUNC_LASERWALL *)subevent)->damage, 11, &Cre->Box.Location);
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERWALL));
}

void DrawLaserWalls(void)
{
    short a;
    short b;
    short Space;
    struct VECT Start;
    struct VECT End;
    struct PSXEVUSRFUNC_LASERWALL *ev;
    long *longpnt;
    short xs;
    short zs;
    struct PSXSUBEVHDR *eventhdr;

    for (a = 0; a < NumLaserWalls; a++) {
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + LaserWallEvents[a][0]);
        if (!eventhdr->disabled) {
            ev = (struct PSXEVUSRFUNC_LASERWALL *)((char *)eventlistbase + LaserWallEvents[a][1]);
            Start.vx = (Scene2Data[ev->crossobj].VectBox.Min.vx + Scene2Data[ev->crossobj].VectBox.Max.vx) / 2;
            Start.vy = (Scene2Data[ev->crossobj].VectBox.Min.vy + Scene2Data[ev->crossobj].VectBox.Max.vy) / 2;
            Start.vz = (Scene2Data[ev->crossobj].VectBox.Min.vz + Scene2Data[ev->crossobj].VectBox.Max.vz) / 2;
            Start.vx += ev->xoff;
            Start.vy -= ev->yoff;
            Start.vz += ev->zoff;
            Space = -1;
            End.vx = Start.vx;
            End.vy = Start.vy;
            End.vz = Start.vz;
            longpnt = &End.vx;
            longpnt += ev->axis;
            *longpnt += ev->length;
            xs = zs = 0;
            if (ev->axis)
                zs = ev->slope;
            else
                xs = ev->slope;
            Space = Find3dSpaceIndxLink(&CollMap, &Start, Space, 1);
            for (b = 0; b < ev->count; b++) {
                AddLaser(&Start, &End, Space, 0, (struct SHORTPAIR){0, 0});
                Start.vx -= xs;
                Start.vz -= zs;
                End.vx += xs;
                End.vz += zs;
                Start.vy -= ev->spacing;
                End.vy -= ev->spacing;
            }
        }
    }
}

struct PSXSUBEVENT *PSXLASERWALL_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    short *shortpnt;
    short *shortpnt2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERWALL))
        return subevent;
    shortpnt = &((struct PSXEVUSRFUNC_LASERWALL *)subevent)->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0)
        *shortpnt = *shortpnt2;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERWALL));
}

struct PSXSUBEVENT *PSXLASERBEAM(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    int a;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERBEAM))
        return subevent;
    if (Event_StartLevFlag && (((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0.vx & 1) && NumLaserBeams < 32) {
        a = NumLaserBeams++;
        LaserBeamEvents[a][0] = Event_GlobalEVOFFSET;
        LaserBeamEvents[a][1] = (short)((char *)subevent - (char *)eventlistbase);
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERBEAM));
}

void DrawLaserBeams(void)
{
    short a;
    struct PSXEVUSRFUNC_LASERBEAM *ev;
    struct PSXSUBEVHDR *eventhdr;

    for (a = 0; a < NumLaserBeams; a++) {
        eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + LaserBeamEvents[a][0]);
        if (!eventhdr->disabled) {
            ev = (struct PSXEVUSRFUNC_LASERBEAM *)((char *)eventlistbase + LaserBeamEvents[a][1]);
            AddLaser(&ev->pos0, &ev->pos1, ev->space, ev->Type, (struct SHORTPAIR){0});
        }
    }
}

struct PSXSUBEVENT *PSXLASERBEAM_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_LASERBEAM *ev2;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_LASERBEAM))
        return subevent;
    ev2 = (struct PSXEVUSRFUNC_LASERBEAM *)((int)subevent - (int)eventlistbase + (int)neweventlistbase);
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0.vx = ev2->pos0.vx;
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos1.vx = ev2->pos1.vx;
    ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->space = Find3dSpaceIndxLink(&CollMap, &((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->pos0, -1, 1);
    if (((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->Type >= 6)
        ((struct PSXEVUSRFUNC_LASERBEAM *)subevent)->Type = 0;
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_LASERBEAM));
}

struct PSXSUBEVENT *PSXGLASS(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct PSXEVUSRFUNC_GLASS *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    struct VECT Location;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_GLASS))
        return subevent;
    ev = (struct PSXEVUSRFUNC_GLASS *)subevent;
    MoveBuf = &BrushMoveBufs[ev->crossobj];
    if (ev->crossobj >= 0)
        SceneObjFragments(MoveBuf->Obj, MoveBuf->Params[0], &MoveBuf->DamageLoc);
    if (damage) {
        ev->damage -= damage;
        if (ev->damage > 0)
            return 0;
    }
    if (ev->crossobj >= 0) {
        if (!Event_MuteLevFlag) {
            SceneObjFragments(MoveBuf->Obj, MoveBuf->Params[1], 0);
            Location.vx = (Scene2Data[MoveBuf->Obj].VectBox.Min.vx + Scene2Data[MoveBuf->Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[MoveBuf->Obj].VectBox.Min.vy + Scene2Data[MoveBuf->Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[MoveBuf->Obj].VectBox.Min.vz + Scene2Data[MoveBuf->Obj].VectBox.Max.vz) / 2;
            SoundAtLocation(Sound_GlassSmash, &Location);
        }
        Scene2Data[MoveBuf->Obj].NoDraw = 1;
        FreeSolidBoxs(MoveBuf->SolidBox);
        MoveBuf->SolidBox = 0;
        MoveBuf->DamageFunction = 0;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_GLASS));
}

struct PSXSUBEVENT *PSXGLASS_T(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != 16)
        return subevent;
    shortpnt = (short *)((char *)subevent + 4);
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0) {
        MoveBuf = &BrushMoveBufs[numbrushes];
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            MoveBuf->DamageFunction = (int (*)())PSXGLASS;
            MoveBuf->EvHdrOffset = Event_GlobalEVOFFSET;
            MoveBuf->EvOffset = (char *)subevent - (char *)eventlistbase;
            MoveBuf->OpenFlag = 0;
            MoveBuf->Params[0] = *((char *)subevent + 10);
            MoveBuf->Params[1] = *((char *)subevent + 12);
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 4, 1);
            *shortpnt = numbrushes;
            numbrushes++;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXDISH(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVDISH *ev = (struct PSXEVDISH *)subevent;

    if (subevent->evsize != sizeof(struct PSXEVDISH))
        return subevent;
    if (ev->obj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->obj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            MoveBuf->Range = ev->speed * 32;
            MoveBuf->Speed = 1;
            MoveBuf->Time = LevelTicks + 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

void PSXDISH_M(struct BRUSHMOVEBUF *MoveBuf)
{
    static struct VECT Location;
    static struct POWER Power;

    if (MoveBuf->Time < LevelTicks) {
        switch (MoveBuf->OpenFlag) {
        case 1:
            MoveBuf->OpenFlag = 2;
            MoveBuf->Time = LevelTicks + 900;
            break;
        case 2:
            MoveBuf->OpenFlag = 3;
            MoveBuf->Time = LevelTicks + 300;
            break;
        case 3:
            MoveBuf->OpenFlag = 4;
            MoveBuf->Time = LevelTicks + 900;
            break;
        case 4:
            MoveBuf->OpenFlag = 5;
            break;
        }
    } else {
        switch (MoveBuf->OpenFlag) {
        case 2:
            MoveBuf->Ang.vx += 8;
            break;
        case 4:
            MoveBuf->Ang.vy += 8;
            break;
        }
    }
    if (!MoveBuf->OpenFlag) {
        short Obj = MoveBuf->Obj;

        Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - 100;
        Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - 900;
        Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - 100;
        Power.Full = 1000;
        Power.Fall = 2000;
    } else {
        AddLight(&Location, *&(struct COLOUR){0xff, 0, 0, 0}, Power, (struct SHORTPAIR){0, 0});
    }
}

struct PSXSUBEVENT *PSXDISH_T(struct PSXSUBEVENT *subevent)
{
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != 8)
        return subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    shortpnt = (short *)((char *)subevent + 6);
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0 && numbrushes < 48) {
        memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
        MoveBuf->Primary = (void (*)())PSXDISH_M;
        MoveBuf->Axis = *((unsigned char *)subevent + 4);
        MoveBuf->OpenFlag = 0;
        Obj = *shortpnt2;
        MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx;
        MoveBuf->RotOff.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy;
        MoveBuf->RotOff.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz;
        MoveBuf->Obj = Obj;
        Scene2Data[Obj].MoveBufId = numbrushes + 1;
        *shortpnt = numbrushes;
        numbrushes++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

struct PSXSUBEVENT *PSXB3ROCKS(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVUSRFUNC_B3ROCKS *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_B3ROCKS))
        return subevent;
    ev = (struct PSXEVUSRFUNC_B3ROCKS *)subevent;
    if (ev->RockObj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->RockObj];
        if (!MoveBuf->OpenFlag) {
            MoveBuf->OpenFlag = 1;
            MoveBuf->Time = LevelTicks + 300;
        }
        GirderFallMB = &BrushMoveBufs[ev->GirderFallObj];
        GirderRotateMB = &BrushMoveBufs[ev->GirderRotateObj];
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_B3ROCKS));
}

void PSXB3ROCKS_ROCK_M(struct BRUSHMOVEBUF *MoveBuf)
{
    short Obj;

    Obj = MoveBuf->Obj;
    if (MoveBuf->Time < LevelTicks) {
        switch (MoveBuf->OpenFlag) {
        case 1:
            MoveBuf->OpenFlag = 2;
            MoveBuf->Time = LevelTicks + 40;
            break;
        case 2: {
            struct VECT Location;

            Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
            MoveBuf->OpenFlag = 3;
            MoveBuf->Time = LevelTicks + 75;
            SoundAtLocation(Sound_Menu2, &Location);
            GirderFallMB->OpenFlag = 2;
            GirderFallMB->Time = LevelTicks + 35;
            GirderRotateMB->OpenFlag = 2;
            GirderRotateMB->Time = LevelTicks + 50;
            break;
        }
        case 3:
            MoveBuf->OpenFlag = 4;
            break;
        }
    }
    switch (MoveBuf->OpenFlag) {
    case 2:
        MoveBuf->Offset.vy += 200;
        break;
    case 3:
        MoveBuf->Offset.vy += 10;
        break;
    }
}

void PSXB3ROCKS_GIRDR_M(struct BRUSHMOVEBUF *MoveBuf)
{
    short Obj;

    Obj = MoveBuf->Obj;
    if (MoveBuf->Time < LevelTicks) {
        switch (MoveBuf->OpenFlag) {
        case 2: {
            struct VECT Location;

            Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
            MoveBuf->OpenFlag = 3;
            MoveBuf->Time = LevelTicks + 75;
            SoundAtLocation(Sound_Menu1, &Location);
            break;
        }
        case 3:
            MoveBuf->OpenFlag = 4;
            break;
        }
    }
    switch (MoveBuf->OpenFlag) {
    case 2:
        MoveBuf->Ang.vz += 32;
        break;
    case 3:
        MoveBuf->Ang.vz -= 2;
        break;
    }
}

void PSXB3ROCKS_GIRDF_M(struct BRUSHMOVEBUF *MoveBuf)
{
    short Obj;
    struct SVECT Vel[15];
    int a;
    struct VECT Location;

    Obj = MoveBuf->Obj;
    if (MoveBuf->Time < LevelTicks) {
        switch (MoveBuf->OpenFlag) {
        case 2:
            Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
            MoveBuf->OpenFlag = 3;
            SoundAtLocation(Sound_Menu2, &Location);
            break;
        case 3:
            Scene2Data[Obj].NoDraw = 1;
            MoveBuf->OpenFlag = 4;
            break;
        }
    }
    switch (MoveBuf->OpenFlag) {
    case 2:
        MoveBuf->Offset.vy += 200;
        for (a = 0; a < 15; a++) {
            Vel[a].vx = (rand() - 0x4000) >> 9;
            Vel[a].vy = ((rand() - 0x4000) >> 9) + 50;
            Vel[a].vz = (rand() - 0x4000) >> 9;
        }
        Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
        Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
        Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
        Location.vy += MoveBuf->Offset.vy;
        Location.vz += (QuickRand() - 0x8000) >> 3;
        break;
    case 3:
        MoveBuf->Ang.vz -= 0x80;
        break;
    }
}

struct PSXSUBEVENT *PSXB3ROCKS_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_B3ROCKS *ev;
    short *shortpnt;
    short *shortpnt2;
    short Obj;
    short Objects;
    void (*Primary)();

    if (subevent->evsize != 12)
        return subevent;
    ev = (struct PSXEVUSRFUNC_B3ROCKS *)subevent;
    for (Objects = 0; Objects < 3; Objects++) {
        switch (Objects) {
        case 0:
            shortpnt = (short *)((char *)ev + 4);
            Primary = PSXB3ROCKS_ROCK_M;
            break;
        case 1:
            shortpnt = (short *)((char *)ev + 6);
            Primary = PSXB3ROCKS_GIRDF_M;
            break;
        case 2:
            shortpnt = (short *)((char *)ev + 8);
            Primary = PSXB3ROCKS_GIRDR_M;
            break;
        }
        MoveBuf = &BrushMoveBufs[numbrushes];
        shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            MoveBuf->Primary = Primary;
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx;
            MoveBuf->RotOff.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy;
            MoveBuf->RotOff.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz;
            if (Objects == 2)
                MoveBuf->RotOff.vx -= 1000;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            *shortpnt = numbrushes;
            numbrushes++;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 12);
}

struct PSXSUBEVENT *PSXROTHATCHEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_ROTHATCH *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_ROTHATCH))
        return subevent;
    ev = (struct PSXEVUSRFUNC_ROTHATCH *)subevent;
    if (ev->crossobj >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay * 300;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_ROTHATCH));
}

void PSXROTHATCH_M(struct BRUSHMOVEBUF *MoveBuf)
{
    short in;
    int a;
    short Obj;
    unsigned short *shortpnt;
    struct VECT Location;

    Obj = MoveBuf->Obj;
    in = ((struct BMBTAIL *)((char *)MoveBuf + 0x50))->Triggered;
    ((struct BMBTAIL *)((char *)MoveBuf + 0x50))->Triggered = 0;
    switch (MoveBuf->OpenFlag) {
    case 0:
        if (in)
            MoveBuf->OpenFlag = 5;
        return;
    case 2:
        if (MoveBuf->Time)
            MoveBuf->OpenFlag = 6;
        return;
    case 3:
        in = 0;
        break;
    case 1:
        in = 1;
        break;
    case 4:
        break;
    case 5:
        a = MoveBuf->StartDelay - NewVcounts;
        if (a > 0) {
            MoveBuf->StartDelay = a;
        } else {

            Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
            MoveBuf->OpenFlag = 1;
            MoveBuf->SolidBox->MinMax.Min.vy -= 10000;
            MoveBuf->SolidBox->MinMax.Max.vy -= 10000;
            SoundAtLocation(Sound_PlatformStart, &Location);
            SoundHere(Sound_PlatformMiddle, &Location, &MoveBuf->SoundInfo);
        }
        return;
    case 6:
        if (MoveBuf->Time != 0xFFFF) {
            a = MoveBuf->Time - NewVcounts;
            if (a > 0)
                MoveBuf->Time = a;
            else if (in)
                MoveBuf->Time = 1;
            else
                MoveBuf->OpenFlag = 3;
        }
        return;
    }
    shortpnt = (unsigned short *)&MoveBuf->Ang;
    shortpnt += MoveBuf->Axis;
    a = *shortpnt;
    if (in) {
        a += MoveBuf->Speed * NewVcounts / 8;
        a &= 0xFFF;
        if (MoveBuf->Speed > 0) {
            if (a > MoveBuf->Range) {
                struct VECT Location;

                Location.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
                Location.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
                Location.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
                MoveBuf->OpenFlag = 2;
                a = MoveBuf->Range;
                SoundAtLocation(Sound_PlatformEnd, &Location);
                SoundStop(&MoveBuf->SoundInfo);
            }
        } else {
            if (a < MoveBuf->Range) {
                a = MoveBuf->Range;
                MoveBuf->OpenFlag = 2;
                SoundStop(&MoveBuf->SoundInfo);
            }
        }
    } else {
        in = a & 0xFFF;
        a -= MoveBuf->Speed * NewVcounts / 8;
        a &= 0xFFF;
        if ((a & 0x800) != (in & 0x800)) {
            if (((a & 0xC00) == 0xC00 && !(in & 0xC00)) || ((in & 0xC00) == 0xC00 && !(a & 0xC00))) {
                MoveBuf->OpenFlag = 0;
                MoveBuf->SolidBox->MinMax.Min.vy += 10000;
                MoveBuf->SolidBox->MinMax.Max.vy += 10000;
                a = 0;
            }
        }
    }
    *shortpnt = a;
}

struct PSXSUBEVENT *PSXROTHATCHEVENT_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_ROTHATCH *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    short *shortpnt;
    short *shortpnt2;
    int Obj;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_ROTHATCH))
        return subevent;
    ev = (struct PSXEVUSRFUNC_ROTHATCH *)subevent;
    shortpnt = &ev->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    MoveBuf = &BrushMoveBufs[numbrushes];
    *shortpnt = -1;
    if (*shortpnt2 >= 0 && numbrushes < 48) {
        memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
        MoveBuf->Primary = (void (*)())PSXROTHATCH_M;
        MoveBuf->PortalObj = -1;
        MoveBuf->Axis = ev->axis;
        if (ev->range < 0x800) {
            MoveBuf->Speed = abs(ev->speed);
            MoveBuf->Range = ev->range;
        } else {
            MoveBuf->Speed = -abs(ev->speed);
            MoveBuf->Range = ev->range;
        }
        MoveBuf->OpenFlag = 0;
        Obj = *shortpnt2;
        MoveBuf->Obj = Obj;
        Scene2Data[Obj].MoveBufId = numbrushes + 1;
        MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx - ev->xoff;
        MoveBuf->RotOff.vy = ev->yoff + ((Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy);
        MoveBuf->RotOff.vz = ev->zoff + ((Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz);
        MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 0, 1);
        *shortpnt = numbrushes;
        numbrushes++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_ROTHATCH));
}

struct PSXSUBEVENT *PSXSTRING(struct PSXSUBEVENT *subevent)
{
    char *String;

    if (!Event_MuteLevFlag) {
        String = GetStringFromStringtable(*(struct IDENTITYNAME *)((char *)subevent + 4));
        if (String)
            centreprintf(GameWindows, String);
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXSTRING_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXCREBATCH(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_MuteLevFlag)
        ActivateCreatureBatch(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXCREBATCH_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 0x10)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXMISEVENT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_StartLevFlag)
        MissionEvent(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXMISEVENT_T(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 0x10)
        return subevent;
    return (struct PSXSUBEVENT *)((char *)subevent + 0x10);
}

struct PSXSUBEVENT *PSXEXPLO(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x18);
}

struct PSXSUBEVENT *PSXEXPLO_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x18);
}

struct PSXSUBEVENT *PSXHELPCOMPUTER(struct PSXSUBEVENT *subevent)
{
    if (!Event_MuteLevFlag)
        HelpComputer_NewMessage(*(struct IDENTITYNAME *)((char *)subevent + 4),
                                *(struct IDENTITYNAME *)((char *)subevent + 16),
                                *(int *)((char *)subevent + 28));
    return (struct PSXSUBEVENT *)((char *)subevent + 32);
}

struct PSXSUBEVENT *PSXHELPCOMPUTER_T(struct PSXSUBEVENT *subevent)
{
    return (struct PSXSUBEVENT *)((char *)subevent + 0x20);
}

struct PSXSUBEVENT *PSXSHOOTTHEN(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
{
    struct BRUSHMOVEBUF *MoveBuf;

    if (subevent->evsize != 8)
        return subevent;
    if (damage == 0)
        return 0;
    MoveBuf = &BrushMoveBufs[((short *)subevent)[2]];
    ((short *)subevent)[3] -= damage;
    if (((short *)subevent)[3] <= 0) {
        FreeSolidBoxs(MoveBuf->SolidBox);
        MoveBuf->SolidBox = 0;
        Event_GlobalEVOFFSET = MoveBuf->EvHdrOffset;
        Event_DoFromSub(MoveBuf->EvHdrOffset, 0, subevent);
    }
    return 0;
}

struct PSXSUBEVENT *PSXSHOOTTHEN_T(struct PSXSUBEVENT *subevent)
{
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != 8)
        return subevent;
    shortpnt = (short *)((char *)subevent + 4);
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0) {
        MoveBuf = &BrushMoveBufs[numbrushes];
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            MoveBuf->EvOffset = (char *)subevent - (char *)eventlistbase;
            MoveBuf->EvHdrOffset = Event_GlobalEVOFFSET;
            MoveBuf->DamageFunction = (int (*)())PSXSHOOTTHEN;
            MoveBuf->OpenFlag = 0;
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 4, 1);
            *shortpnt = numbrushes;
            numbrushes++;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
}

void PSXOBJDRAWOFF(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_OBJDRAWOFF *ev;
    short a;

    ev = (struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent;
    for (a = 0; a < 4; a++) {
        if (ev->obj[a] >= 0) {
            MoveBuf = &BrushMoveBufs[ev->obj[a]];
            HideObjWithSolid(MoveBuf->Obj);
        }
    }
}

struct PSXSUBEVENT *PSXOBJDRAWOFF_T(struct PSXSUBEVENT *subevent)
{
    short a;
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_OBJDRAWOFF))
        return subevent;
    shortpnt = ((struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent)->obj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    for (a = 0; a < 4; a++) {
        *shortpnt = -1;
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            MoveBuf = &BrushMoveBufs[numbrushes];
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            Obj = *shortpnt2;
            MoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 0, 1);
            *shortpnt = numbrushes;
            numbrushes++;
        }
        shortpnt2++;
        shortpnt++;
    }
}

void PSXOBJDRAWON(struct PSXSUBEVENT *subevent)
{
    int a;

    for (a = 0; a < 4; a++) {
        if (((struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent)->obj[a] >= 0)
            Scene2Data[((struct PSXEVUSRFUNC_OBJDRAWOFF *)subevent)->obj[a]].NoDraw = 0;
    }
}

void PSXONKEYDO(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_ONKEYDO *ev;
    struct VECT Location;

    ev = (struct PSXEVUSRFUNC_ONKEYDO *)subevent;
    if (ev->KeyAnd && (GlobalKey & ev->KeyAnd) != ev->KeyAnd) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyOr && !(GlobalKey & ev->KeyOr)) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyNotAnd && (~GlobalKey & ev->KeyNotAnd) != ev->KeyNotAnd) {
        nomoresubevents = 1;
        return;
    }
    if (ev->KeyNotOr && !(~GlobalKey & ev->KeyNotOr)) {
        nomoresubevents = 1;
        return;
    }
}

struct PSXSUBEVENT *PSXROTBUTTON(struct PSXSUBEVENT *subevent)
{
    short time;

    if (subevent->evsize != 12)
        return subevent;
    if (*(short *)((char *)subevent + 10) >= 0) {
        MoveBuf = &BrushMoveBufs[*(short *)((char *)subevent + 10)];
        if (!MoveBuf->Ang.vy) {
            struct VECT Location;

            Location.vx = (Scene2Data[MoveBuf->Obj].VectBox.Max.vx + Scene2Data[MoveBuf->Obj].VectBox.Min.vx) / 2;
            Location.vy = (Scene2Data[MoveBuf->Obj].VectBox.Max.vy + Scene2Data[MoveBuf->Obj].VectBox.Min.vy) / 2;
            Location.vz = (Scene2Data[MoveBuf->Obj].VectBox.Max.vz + Scene2Data[MoveBuf->Obj].VectBox.Min.vz) / 2;
            SoundAtLocation(Sound_Button, &Location);
            MoveBuf->Ang.vy = 0x800;
            time = *(short *)((char *)subevent + 6);
            if (time == -1)
                MoveBuf->Time = 0xFFFF;
            else
                MoveBuf->Time = time * 300;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

void PSXROTBUTTON_M(struct BRUSHMOVEBUF *MoveBuf)
{
    int a;
    struct VECT Location;

    if (MoveBuf->Ang.vy && MoveBuf->Time != 0xFFFF) {
        a = MoveBuf->Time;
        a -= NewVcounts;
        if (a <= 0) {
            MoveBuf->Ang.vy = 0;
            Location.vx = (Scene2Data[MoveBuf->Obj].VectBox.Min.vx + Scene2Data[MoveBuf->Obj].VectBox.Max.vx) / 2;
            Location.vy = (Scene2Data[MoveBuf->Obj].VectBox.Min.vy + Scene2Data[MoveBuf->Obj].VectBox.Max.vy) / 2;
            Location.vz = (Scene2Data[MoveBuf->Obj].VectBox.Min.vz + Scene2Data[MoveBuf->Obj].VectBox.Max.vz) / 2;
            SoundAtLocation(Sound_Button, &Location);
        }
        MoveBuf->Time = a;
    }
}

struct PSXSUBEVENT *PSXROTBUTTON_T(struct PSXSUBEVENT *subevent)
{
    short *shortpnt;
    short *shortpnt2;
    short Obj;

    if (subevent->evsize != 12)
        return subevent;
    MoveBuf = &BrushMoveBufs[numbrushes];
    shortpnt = (short *)((char *)subevent + 10);
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    *shortpnt = -1;
    if (*shortpnt2 >= 0 && numbrushes < 48) {
        memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
        MoveBuf->Primary = (void (*)())PSXROTBUTTON_M;
        Obj = *shortpnt2;
        MoveBuf->RotOff.vx = (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2 - Scene2Data[Obj].Origin.vx;
        MoveBuf->RotOff.vy = (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2 - Scene2Data[Obj].Origin.vy;
        MoveBuf->RotOff.vz = (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2 - Scene2Data[Obj].Origin.vz;
        MoveBuf->Obj = Obj;
        Scene2Data[Obj].MoveBufId = numbrushes + 1;
        *shortpnt = numbrushes;
        MoveBuf++;
        numbrushes++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 12);
}

struct PSXSUBEVENT *PSXPLATFORMEVENT(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PLATFORM *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_PLATFORM))
        return subevent;
    ev = (struct PSXEVUSRFUNC_PLATFORM *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        MoveBuf->Triggered = 1;
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->StartDelay = ev->StartDelay;
            if (ev->EndDelay == 255)
                MoveBuf->Time = 0xffff;
            else
                MoveBuf->Time = ev->EndDelay * 300;
        }
    }
    /* assigning back to subevent keeps ev in its own register ($a2) */
    subevent = (struct PSXSUBEVENT *)(ev + 1);
    return subevent;
}

/* The object centre is computed once per site from a block-local pointer (SYM doesn't list it); the
   index-first integer form gives the original's idx + Scene2Data operand order, as in PSXPISTON_M. */
void PrimaryPlatform_PDH(struct BRUSHMOVEBUF *MoveBuf)
{
    short in;
    int NoProblem;
    struct VECT Location;

    in = -MoveBuf->Triggered;
    if (MoveBuf->Speed < 0)
        MoveBuf->Speed = -MoveBuf->Speed;
    MoveBuf->Triggered = 0;
    switch (MoveBuf->OpenFlag) {
    case 0:
        if (in) {
            if (!MoveBuf->Key || (MoveBuf->Key & GlobalKey)) {
                MoveBuf->OpenFlag = 5;
                if (MoveBuf->Key) {
                    {
                        struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

                        Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2;
                        Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2;
                        Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2;
                    }
                    SoundAtLocation(Sound_KeyUse, &Location);
                    MoveBuf->SoundJustDone = 1;
                }
                return;
            }
            if (!MoveBuf->SoundJustDone) {
                {
                    struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

                    Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2;
                    Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2;
                    Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2;
                }
                SoundAtLocation(Sound_KeyTry, &Location);
                MoveBuf->SoundJustDone = 1;
                QuakeSuggestKey(MoveBuf->Key);
            }
            return;
        }
        MoveBuf->SoundJustDone = 0;
        return;
    case 2:
        MoveBuf->OpenFlag = 6;
        return;
    case 3:
        in = 0;
        break;
    case 1:
        in = 1;
        break;
    case 5:
        if ((short)(MoveBuf->StartDelay - NewVcounts) > 0) {
            MoveBuf->StartDelay -= NewVcounts;
            return;
        }
        MoveBuf->OpenFlag = 1;
        {
            struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

            Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2;
            Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2;
            Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2;
        }
        SoundAtLocation(Sound_PlatformStart, &Location);
        return;
    case 6:
        if (MoveBuf->Time == 0xFFFF)
            return;
        if ((short)(MoveBuf->Time - NewVcounts) > 0) {
            MoveBuf->Time -= NewVcounts;
            return;
        }
        if (in) {
            MoveBuf->Time = 1;
            return;
        }
        MoveBuf->OpenFlag = 3;
        {
            struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

            Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2 + MoveBuf->Offset.vx;
            Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2 + MoveBuf->Offset.vy;
            Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2 + MoveBuf->Offset.vz;
        }
        SoundAtLocation(Sound_PlatformStart, &Location);
        return;
    }
    {
        struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

        Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2 + MoveBuf->Offset.vx;
        Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2 + MoveBuf->Offset.vy;
        Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2 + MoveBuf->Offset.vz;
    }
    SetShockData(4, 13, 0x400, &Location, (struct POWER){0x800, 0x1000});
    {
        unsigned char LastOpenFlag;
        int NewXYZ;
        struct SVECT xyz;

        LastOpenFlag = MoveBuf->OpenFlag;
        NewXYZ = MoveBuf->PrecisionRange;
        MoveBuf->Range = abs(MoveBuf->Range);
        if (MoveBuf->OpenFlag == 4) {
            SeekL((long *)&NewXYZ, MoveBuf->Time, MoveBuf->Speed * NewVcounts);
            if (NewXYZ == MoveBuf->Time)
                MoveBuf->OpenFlag = MoveBuf->Params[0];
        } else if (in) {
            NewXYZ += MoveBuf->Speed * NewVcounts;
            if (NewXYZ > MoveBuf->Range) {
                NewXYZ = MoveBuf->Range;
                MoveBuf->OpenFlag = 2;
                {
                    struct SCENE2 *Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);

                    Location.vx = (Box->VectBox.Min.vx + Box->VectBox.Max.vx) / 2 + MoveBuf->Offset.vx;
                    Location.vy = (Box->VectBox.Min.vy + Box->VectBox.Max.vy) / 2 + MoveBuf->Offset.vy;
                    Location.vz = (Box->VectBox.Min.vz + Box->VectBox.Max.vz) / 2 + MoveBuf->Offset.vz;
                }
                SetShockData(4, 14, 0xC00, &Location, (struct POWER){0x800, 0x1000});
            }
        } else {
            NewXYZ -= MoveBuf->Speed * NewVcounts;
            if (NewXYZ <= 0) {
                NewXYZ = 0;
                MoveBuf->OpenFlag = 0;
            }
        }
        xyz.vx = MoveBuf->DamageLoc.vx * NewXYZ / MoveBuf->Range;
        xyz.vy = MoveBuf->DamageLoc.vy * NewXYZ / MoveBuf->Range;
        xyz.vz = MoveBuf->DamageLoc.vz * NewXYZ / MoveBuf->Range;
        NoProblem = 0;
        if (MoveBuf) {
            if (MoveBuf->SolidBox) {
                struct SVECT xyz2;
                struct SCENE2 *Box;

                Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);
                xyz2.vx = xyz.vx + (Box->VectBox.Min.vx - MoveBuf->SolidBox->MinMax.Min.vx);
                xyz2.vy = xyz.vy + (Box->VectBox.Min.vy - MoveBuf->SolidBox->MinMax.Min.vy);
                xyz2.vz = xyz.vz + (Box->VectBox.Min.vz - MoveBuf->SolidBox->MinMax.Min.vz);
                NoProblem = MoveSolidBoxs(MoveBuf->SolidBox, xyz2);
            }
        }
        if (NoProblem) {
            MoveBuf->PrecisionRange = NewXYZ;
            if (MoveBuf) {
                while (1) {
                    MoveBuf->Offset = xyz;
                    MoveBuf = MoveBuf->Link;
                    if (!MoveBuf)
                        break;
                }
            }
        } else if (LastOpenFlag == 1) {
            MoveBuf->Params[0] = LastOpenFlag;
            MoveBuf->OpenFlag = 4;
            MoveBuf->Time = MoveBuf->PrecisionRange - MoveBuf->Speed * 150;
            if (MoveBuf->Time > MoveBuf->Range)
                MoveBuf->Time = 0;
        } else if (LastOpenFlag == 3) {
            MoveBuf->Params[0] = LastOpenFlag;
            MoveBuf->OpenFlag = 4;
            MoveBuf->Time = MoveBuf->PrecisionRange + MoveBuf->Speed * 150;
            if (MoveBuf->Range < MoveBuf->Time)
                MoveBuf->Time = MoveBuf->Range;
        } else
            MoveBuf->OpenFlag = LastOpenFlag;
    }
}

/* The size check wraps the whole body with `return subevent;` last: that makes subevent outlive ev,
   so cse keeps the ev->dest reads on subevent ($a1) and the xd*xd product lands in $a2. */
struct PSXSUBEVENT *PSXPLATFORMEVENT_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PLATFORM *ev;
    struct BRUSHMOVEBUF *MoveBuf;
    struct BRUSHMOVEBUF *LastMoveBuf;
    struct SOLIDBOX **SolidBoxPntrPntr;
    int NewGroup;
    short *shortpnt;
    short *shortpnt2;
    int c;
    int Obj;
    int range;
    int xd;
    int yd;
    int zd;

    if (subevent->evsize == sizeof(struct PSXEVUSRFUNC_PLATFORM)) {
        shortpnt = ((struct PSXEVUSRFUNC_PLATFORM *)subevent)->crossobj;
        shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
        MoveBuf = &BrushMoveBufs[numbrushes];
        ev = (struct PSXEVUSRFUNC_PLATFORM *)subevent;
        Obj = *shortpnt2;
        if (Obj >= 0) {
            xd = ev->dest.vx - (Scene2Data[Obj].VectBox.Min.vx + Scene2Data[Obj].VectBox.Max.vx) / 2;
            yd = ev->dest.vy - (Scene2Data[Obj].VectBox.Min.vy + Scene2Data[Obj].VectBox.Max.vy) / 2;
            zd = ev->dest.vz - (Scene2Data[Obj].VectBox.Min.vz + Scene2Data[Obj].VectBox.Max.vz) / 2;
            range = xd * xd + yd * yd + zd * zd;
            range = SquareRootNR(range / 4) * 2;
        } else {
            range = 0;
        }
        for (c = 0; c < 4; c++)
            shortpnt[c] = -1;
        LastMoveBuf = 0;
        SolidBoxPntrPntr = &MoveBuf->SolidBox;
        NewGroup = 1;
        for (c = 0; c < 4; c++) {
            if (*shortpnt2 >= 0 && numbrushes < 48) {
                memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
                if (LastMoveBuf)
                    LastMoveBuf->Link = MoveBuf;
                else
                    MoveBuf->Primary = PrimaryPlatform_PDH;
                MoveBuf->PortalObj = -1;
                MoveBuf->Axis = 1;
                MoveBuf->Speed = abs(ev->speed);
                MoveBuf->Range = range;
                MoveBuf->DamageLoc.vx = xd;
                MoveBuf->DamageLoc.vy = yd;
                MoveBuf->DamageLoc.vz = zd;
                MoveBuf->OpenFlag = 0;
                Obj = *shortpnt2;
                MoveBuf->Obj = Obj;
                Scene2Data[Obj].MoveBufId = numbrushes + 1;
                *SolidBoxPntrPntr = GenSolidBox(&Scene2Data[Obj].VectBox, 0, NewGroup);
                if (*SolidBoxPntrPntr) {
                    SolidBoxPntrPntr = &(*SolidBoxPntrPntr)->LinkedData;
                    NewGroup = 0;
                }
                LastMoveBuf = MoveBuf;
                MoveBuf++;
                *shortpnt++ = numbrushes++;
            }
            shortpnt2++;
        }
        return (struct PSXSUBEVENT *)((char *)ev + sizeof(struct PSXEVUSRFUNC_PLATFORM));
        }
        return subevent;
    }

struct PSXSUBEVENT *PSXTLIGHT(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    struct PSXEVTLIGHT *ev = (struct PSXEVTLIGHT *)subevent;

    AddTimedLight(&ev->Loc, ev->Colour, ev->Power, ev->Life * 3);
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVTLIGHT));
}

void PSXPISTON_M(struct BRUSHMOVEBUF *MoveBuf)
{
    short r;
    short a;
    short end;
    struct VECT Location;
    struct SVECT xyz;
    struct SVECT xyz2;
    int speed;
    short old;
    struct SCENE2 *Box;

    end = 0;
    memset(&xyz, 0, sizeof(struct SVECT));
    speed = MoveBuf->Speed * NewVcounts;
    a = MoveBuf->PrecisionRange;
    old = a;
    a = old + speed;
    r = abs(MoveBuf->Range) << 4;
    if (old < r) {
        end = a >= r;
    } else if (a >= r * 2) {
        end = 1;
        a -= r * 2;
    }
    MoveBuf->PrecisionRange = a;
    a = ((rcossin_tbl[((a << 11) / r - 0x400) & 0xFFF][0] + 0x1000) * -MoveBuf->Range) / 8192;
    if (end) {
        if (MoveBuf->Time) {
            MoveBuf->Time--;
            if (!MoveBuf->Time) {
                MoveBuf->Primary = 0;
                MoveBuf->OpenFlag = 0;
            }
        }
    }
    (&xyz.vx)[MoveBuf->Axis] = a;
    while (MoveBuf) {
        (&MoveBuf->Offset.vx)[MoveBuf->Axis] = a;
        if (MoveBuf->SolidBox) {
            /* index first: &Scene2Data[MoveBuf->Obj] puts the base register first and swaps a0/a1 */
            Box = (struct SCENE2 *)(MoveBuf->Obj * sizeof(struct SCENE2) + (int)Scene2Data);
            xyz2.vx = xyz.vx + (Box->VectBox.Min.vx - MoveBuf->SolidBox->MinMax.Min.vx);
            xyz2.vy = xyz.vy + (Box->VectBox.Min.vy - MoveBuf->SolidBox->MinMax.Min.vy);
            xyz2.vz = xyz.vz + (Box->VectBox.Min.vz - MoveBuf->SolidBox->MinMax.Min.vz);
            MoveSolidBoxs(MoveBuf->SolidBox, xyz2);
        }
        MoveBuf = MoveBuf->Link;
    }
}

struct PSXSUBEVENT *PSXPISTON(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PISTON *ev;

    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_PISTON))
        return subevent;
    ev = (struct PSXEVUSRFUNC_PISTON *)subevent;
    if (ev->crossobj[0] >= 0) {
        MoveBuf = &BrushMoveBufs[ev->crossobj[0]];
        if (MoveBuf->OpenFlag == 0) {
            MoveBuf->Primary = PSXPISTON_M;
            MoveBuf->Time = ev->thrusts;
            MoveBuf->Speed = ev->speed;
            MoveBuf->Range = ev->range;
            MoveBuf->OpenFlag = 1;
        }
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_PISTON));
}

struct PSXSUBEVENT *PSXPISTON_T(struct PSXSUBEVENT *subevent)
{
    struct PSXEVUSRFUNC_PISTON *ev;
    short *shortpnt;
    short *shortpnt2;
    short Obj;
    short a;
    struct BRUSHMOVEBUF *LastMoveBuf;

    LastMoveBuf = 0;
    if (subevent->evsize != sizeof(struct PSXEVUSRFUNC_PISTON))
        return subevent;
    ev = (struct PSXEVUSRFUNC_PISTON *)subevent;
    shortpnt = ev->crossobj;
    shortpnt2 = (short *)((int)shortpnt - (int)eventlistbase + (int)neweventlistbase);
    MoveBuf = &BrushMoveBufs[numbrushes];
    for (a = 0; a < 4; a++)
        shortpnt[a] = -1;
    for (a = 0; a < 4; a++) {
        if (*shortpnt2 >= 0 && numbrushes < 48) {
            memset(MoveBuf, 0, sizeof(struct BRUSHMOVEBUF));
            if (LastMoveBuf)
                LastMoveBuf->Link = MoveBuf;
            MoveBuf->Axis = ev->axis;
            LastMoveBuf = MoveBuf;
            Obj = *shortpnt2;
            LastMoveBuf->Obj = Obj;
            Scene2Data[Obj].MoveBufId = numbrushes + 1;
            if (ev->gensolidflag)
                MoveBuf->SolidBox = GenSolidBox(&Scene2Data[Obj].VectBox, 0, 1);
            *shortpnt = numbrushes;
            shortpnt++;
            MoveBuf++;
            numbrushes++;
        }
        shortpnt2++;
    }
    return (struct PSXSUBEVENT *)((char *)subevent + sizeof(struct PSXEVUSRFUNC_PISTON));
}

struct PSXSUBEVENT *PSXDISABLEME(struct PSXSUBEVENT *subevent)
{
    struct PSXSUBEVHDR *eventhdr;

    if (subevent->evsize != 4)
        return subevent;
    eventhdr = (struct PSXSUBEVHDR *)((char *)eventlistbase + Event_GlobalEVOFFSET);
    eventhdr->disabled = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + 4);
}

struct PSXSUBEVENT *PSXDONTJUMP(struct PSXSUBEVENT *subevent, struct CREATURE *cre)
{
    if (subevent->evsize != 4)
        return subevent;
    cre->Box.DontJump = 1;
    return (struct PSXSUBEVENT *)((char *)subevent + 4);
}

struct PSXSUBEVENT *PSXSIMPLESOUND(struct PSXSUBEVENT *subevent)
{
    if (!Event_MuteLevFlag) {
        GetSoundNum(*(struct IDENTITYNAME *)((char *)subevent + 20));
        SoundAtLocation(GetSoundNum(*(struct IDENTITYNAME *)((char *)subevent + 20)), (struct VECT *)((char *)subevent + 4));
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 32);
}

void InitEventSoundsEachLevel(void)
{
    Sound_KeyUse = GetSoundNum((struct IDENTITYNAME){"msc_keyuse"});
    Sound_KeyTry = GetSoundNum((struct IDENTITYNAME){"msc_keytry"});
    Sound_PlatformStart = GetSoundNum((struct IDENTITYNAME){"pt1__strt"});
    Sound_PlatformMiddle = GetSoundNum((struct IDENTITYNAME){"pt1__mid"});
    Sound_PlatformEnd = GetSoundNum((struct IDENTITYNAME){"pt1__end"});
    Sound_Button = GetSoundNum((struct IDENTITYNAME){"amb_butn2"});
    Sound_Menu1 = GetSoundNum((struct IDENTITYNAME){"msc_menu1"});
    Sound_Menu2 = GetSoundNum((struct IDENTITYNAME){"msc_menu2"});
    Sound_Menu3 = GetSoundNum((struct IDENTITYNAME){"msc_menu3"});
    Sound_Secret = GetSoundNum((struct IDENTITYNAME){"msc_secret"});
    Sound_Explosion_800B2A68 = GetSoundNum((struct IDENTITYNAME){"wep_grenlx1a"});
    Sound_GlassSmash = GetSoundNum((struct IDENTITYNAME){"amb_brkglas"});
}
