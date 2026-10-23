#include "types.h"

extern struct DB *olddb;
extern struct DB *currdb;
extern struct DB db[];
extern unsigned char DbFrame[];
extern char FinishedCutScene;
extern char WaitingCutScene;
extern char InitialisedCutScene;
extern int UseCdData;
extern short SceneValid;
extern struct GLOBMODHDR *CastList;
void InitialiseVBlankLoading(void);


void TestIt(char *Name)
{
    char Path[64];

    FinishedCutScene = 0;
    WaitingCutScene = 0;
    InitialisedCutScene = 0;
    DrawSync(0);
    ClearOTag(db[0].Ot[0], 51);
    ClearOTag(db[1].Ot[0], 51);
    DbFrame[0] = 1;
    currdb = &db[1];
    VSync(0);
    VSync(0);
    PutDispEnv(&currdb->DispEnv);
    SceneValid = 0;
    if (UseCdData)
        strcpy(Path, "\\Q2DATA\\");
    else
        strcpy(Path, "c:\\PsxData\\Q2Data\\");
    strcat(Path, "LEVELS\\TITLE\\");
    CastList = 0;
    SetDispMask(1);
    InitLoadingAnim();
    InitialiseVBlankLoading();
}

void VBlankLoading(void)
{
    DoubleTrebleBuffer();
    PutDispEnv(&olddb->DispEnv);
    DrawOTag(olddb->Ot[0]);
    ClearOTag(currdb->Ot[0], 50);
    VBlankMainLoop();
}
