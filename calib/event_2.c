#include "types.h"
extern struct BRUSHMOVEBUF BrushMoveBufs[48];
/* defined in Event.Obj, so $gp-relative */
struct PSXMAINEVENTHDR *eventlistbase;

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

void ProcessMoveBufs(void)
{
    struct BRUSHMOVEBUF *MoveBuf;
    for (MoveBuf = BrushMoveBufs; MoveBuf < &BrushMoveBufs[48]; MoveBuf++)
        if (MoveBuf->Primary)
            MoveBuf->Primary(MoveBuf);
}

/* Sound_Explosion: splat names Event.Obj's copy Sound_Explosion_800B2A68 (quakecre.obj has another) */
int Sound_KeyUse;
int Sound_KeyTry;
int Sound_PlatformStart;
int Sound_PlatformMiddle;
int Sound_PlatformEnd;
int Sound_Button;
int Sound_Menu1;
int Sound_Menu2;
int Sound_Menu3;
int Sound_Secret;
int Sound_Explosion_800B2A68;
int Sound_GlassSmash;
int GetSoundNum(struct IDENTITYNAME Name);

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
