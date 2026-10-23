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
