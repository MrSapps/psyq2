#include "types.h"
extern struct EVENTTIMER EventTimers[];

void Event_KillTimers(void)
{
    int a;

    for (a = 7; a >= 0; a--)
        EventTimers[a].timersubevent = 0;
}

extern struct USERFUNCLIST EventUserFuncList[];
short numuserfunctions;
void **usrfunclist;
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
void InitEventSoundsEachLevel(void);
void dofunc_error(void);
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
