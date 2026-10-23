#include "types.h"
extern struct WINDOW GameWindows[];
void MissionEvent(struct IDENTITYNAME Name);
char *GetStringFromStringtable(struct IDENTITYNAME StringName);
int ActivateCreatureBatch(struct IDENTITYNAME BatchName);
void centreprintf(struct WINDOW *Window, char *_format, ...);
/* defined in Event.Obj, so $gp-relative */
short Event_MuteLevFlag;
short Event_StartLevFlag;

struct PSXSUBEVENT *PSXSIMBUTTON(struct PSXSUBEVENT *subevent)
{
    if (subevent->evsize != 8)
        return subevent;
    MissionEvent(*(struct IDENTITYNAME *)"Done Dish");
    return (struct PSXSUBEVENT *)((char *)subevent + 8);
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

struct PSXSUBEVENT *PSXCREBATCH(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_MuteLevFlag)
        ActivateCreatureBatch(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}

struct PSXSUBEVENT *PSXMISEVENT(struct PSXSUBEVENT *subevent, struct CREATURE *Cre)
{
    if (subevent->evsize != 16)
        return subevent;
    if (!Event_StartLevFlag)
        MissionEvent(*(struct IDENTITYNAME *)((char *)subevent + 4));
    return (struct PSXSUBEVENT *)((char *)subevent + 16);
}
extern int NewVcounts;
extern struct SCENE2 *Scene2Data;
int SoundAtLocation(int SFXNumber, struct VECT *loc);
static int Sound_Button;

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

extern struct BRUSHMOVEBUF BrushMoveBufs[];
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;
short Event_GlobalEVOFFSET;
short numbrushes;
struct BRUSHMOVEBUF *MoveBuf;
void *memset(void *s, int c, unsigned int n);
struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
struct PSXSUBEVENT *PSXSHOOTTHEN(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);

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

void HelpComputer_NewMessage(struct IDENTITYNAME Orders, struct IDENTITYNAME Objective, int FrameDelay);

struct PSXSUBEVENT *PSXHELPCOMPUTER(struct PSXSUBEVENT *subevent)
{
    if (!Event_MuteLevFlag)
        HelpComputer_NewMessage(*(struct IDENTITYNAME *)((char *)subevent + 4),
                                *(struct IDENTITYNAME *)((char *)subevent + 16),
                                *(int *)((char *)subevent + 28));
    return (struct PSXSUBEVENT *)((char *)subevent + 32);
}

void PSXBUTTON_M(struct BRUSHMOVEBUF *MoveBuf);

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

struct PSXSUBEVENT *PSXGLASS(struct PSXSUBEVENT *subevent, struct CREATURE *cre, short damage);

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

int GetSoundNum(struct IDENTITYNAME name);
int SoundAtLocation(int SFXNumber, struct VECT *loc);

struct PSXSUBEVENT *PSXSIMPLESOUND(struct PSXSUBEVENT *subevent)
{
    if (!Event_MuteLevFlag) {
        GetSoundNum(*(struct IDENTITYNAME *)((char *)subevent + 20));
        SoundAtLocation(GetSoundNum(*(struct IDENTITYNAME *)((char *)subevent + 20)), (struct VECT *)((char *)subevent + 4));
    }
    return (struct PSXSUBEVENT *)((char *)subevent + 32);
}

extern unsigned int LevelTicks;
static int Sound_Menu1;

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

void PSXDISH_M(struct BRUSHMOVEBUF *MoveBuf);

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

static int Sound_Menu2;
static struct BRUSHMOVEBUF *GirderFallMB;
static struct BRUSHMOVEBUF *GirderRotateMB;

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

void SceneObjFragments(int ObjIndx, int NumBits, struct VECT *Loc);
void FreeSolidBoxs(struct SOLIDBOX *SolidBox);
static int Sound_GlassSmash;

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

int AddLight(struct VECT *Location, struct COLOUR Colour, struct POWER Power, struct SHORTPAIR FlareTypeSize);

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

void PSXB3ROCKS_ROCK_M(struct BRUSHMOVEBUF *MoveBuf);
void PSXB3ROCKS_GIRDF_M(struct BRUSHMOVEBUF *MoveBuf);
void PSXB3ROCKS_GIRDR_M(struct BRUSHMOVEBUF *MoveBuf);

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

int rand(void);
unsigned short QuickRand(void);

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

extern short LaserWallEvents[][2];
extern struct COLLHEADER CollMap;
short NumLaserWalls;
int Find3dSpaceIndxLink(struct COLLHEADER *Map, struct VECT *Loc, short Main, int CheckAll);
struct CREATURE *AddLaser(struct VECT *Start, struct VECT *Stop, short Space, short LaserType, struct SHORTPAIR Parts);

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
static int Sound_PlatformStart;
static int Sound_PlatformMiddle;
static int Sound_PlatformEnd;

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

extern struct BRUSHMOVEBUF BrushMoveBufs[];
extern struct SCENE2 *Scene2Data;
struct PSXMAINEVENTHDR *eventlistbase;
struct PSXMAINEVENTHDR *neweventlistbase;
short Event_GlobalEVOFFSET;
short numbrushes;
void *memset(void *s, int c, unsigned int n);
struct SOLIDBOX *GenSolidBox(struct VECTBOX *MinMax, int Flags, int NewGroup);
int abs(int);
void PrimaryDoor2_PDH(struct BRUSHMOVEBUF *MoveBuf);

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
