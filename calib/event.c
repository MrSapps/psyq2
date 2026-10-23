#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
/* defined in Event.Obj, so $gp-relative */
struct BRUSHMOVEBUF *MoveBuf;

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

struct PSXMAINEVENTHDR *eventlistbase;
int Event_GetNamedOffset(struct IDENTITYNAME name3);

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

extern struct EVENTTIMER EventTimers[];
extern int NewVcounts;
char nomoresubevents;
short Event_GlobalEVOFFSET;
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

extern long *DummyMemory;
extern struct WINDOW GameWindows[];
int sprintf(char *, const char *, ...);
void centreprintf();

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

extern int MaxWindows;
int rand(void);
int AreaValid1(int Win, int Area);
int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);

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

short *eventhdr_start;
short *eventhdr_end;
extern short eventstack[];
int Do_SubEvent(struct PSXSUBEVENT *subevent, struct CREATURE *Cre);

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

short Event_MuteLevFlag;
static int Sound_Secret;
extern int GlobalSecretsFound;
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
char AddSecretInterMission(char *CurrentMapTitle);
int SoundAtLocation(int SFXNumber, struct VECT *loc);

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

struct PSXMAINEVENTHDR *neweventlistbase;
short numbrushes;
extern struct SCENE2 *Scene2Data;
void *memset(void *, int, int);
void PrimarySimRot_PDH2(struct BRUSHMOVEBUF *MoveBuf);

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

void PrimarySimRot_PDH(struct BRUSHMOVEBUF *MoveBuf);

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


struct PSXSUBEVHDR *firsteventlist;
void **usrfunclist;
short numuserfunctions;
void Event_KillTimers(void);
struct PSXSUBEVENT *PSXDOOR1UP_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXDOOR1SIDE_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXDOOR2SIDE_T(struct PSXSUBEVENT *subevent);
struct PSXSUBEVENT *PSXEXPLODE_T(struct PSXSUBEVENT *subevent);

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


/* Sound_Explosion: splat names Event.Obj's copy Sound_Explosion_800B2A68 (quakecre.obj has another) */
static int Sound_Explosion_800B2A68;
void SceneObjFragments(int ObjIndx, int NumBits, struct VECT *Loc);
void QuakeGenExplosion(struct VECT *Location, int Area, int Scale, int Type);
void HideObjWithSolid(int Obj);

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

struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
int abs(int);
void PrimaryDoor2_PDH(struct BRUSHMOVEBUF *MoveBuf);


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


extern struct SHORTPAIR PlayerHitRadius;
struct STARTPOSITION *FindStartPosition(struct IDENTITYNAME Name);
int MaybeLoadZoneName(struct IDENTITYNAME name);
int SetupBoxDrop(struct COLLBOX3D *Box, struct VECT *Location, struct SCRPNT CollSize, struct SHORTPAIR MainSpaceDrop, struct SHORTPAIR HitSize);
void ReCalcCollBox(struct CREATURE *Cre);

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


extern struct IDENTITYNAME ThisLevelName;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
char *strcpy(char *, const char *);

extern struct IDENTITYNAME ThisLevelName;
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
char *strcpy(char *, const char *);

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


int AttatchSolidBoxForObj(int Obj);

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


short Event_StartLevFlag;
extern unsigned int LevelTicks;

static void ProcessMoveBufs(void)
{
    {
        struct BRUSHMOVEBUF *MoveBuf;

        for (MoveBuf = BrushMoveBufs; MoveBuf < &BrushMoveBufs[48]; MoveBuf++)
            if (MoveBuf->Primary)
                MoveBuf->Primary(MoveBuf);
    }
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


extern int ChrisCount;
void DoDamage(struct CREATURE *Cre, struct CREATURE *Target, int Damage, int DamageType);

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

/* inlined into Do_SubEvent (PSXDOOR1UP's out-of-line copy is in event_9) */
static struct PSXSUBEVENT *PSXDOOR1UP(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage)
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
        PSXDOOR1UP(subevent, Cre, 0);
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
