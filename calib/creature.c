#include "types.h"

extern struct SPAWNDETAILS SpawnTables[];
int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);

struct SPAWNDETAILS *GetSpawnDetails(int TypeIdent)
{
    struct SPAWNDETAILS *Details;

    for (Details = SpawnTables; Details->TypeIdent; Details++) {
        if (Details->TypeIdent == TypeIdent)
            return Details;
    }
    return 0;
}

struct SPAWNDETAILS *GetSpawnDetailsFromIdentityName(struct IDENTITYNAME *Name, struct SPAWNDETAILS *start)
{
    struct SPAWNDETAILS *Details;

    if (!start)
        Details = SpawnTables;
    else
        Details = start;
    for (; Details->TypeIdent; Details++) {
        if (ModNameCmp(&Details->ModelName, Name) == 1)
            return Details;
    }
    return 0;
}

struct CREBATCHNAMES *CreBatchNamesTable;
extern int LevelIsMultiPlayer;
extern int MaxWindows;
extern struct WINDOW GameWindows[];
char *PrintIdentityName(struct IDENTITYNAME Name);
void *FindIdentNameInList(struct IDENTITYNAME *Name, void *VPntr, int NumBytes);

int ActivateCreatureBatch(struct IDENTITYNAME BatchName)
{
    struct CREBATCHNAMES *TableEntry;

    TableEntry = FindIdentNameInList(&BatchName, CreBatchNamesTable, sizeof(struct CREBATCHNAMES));
    if (TableEntry) {
        PrintIdentityName(TableEntry->Name);
        if (LevelIsMultiPlayer)
            TableEntry->Flags = 0;
        if (!(TableEntry->Flags & 2)) {
            TableEntry->Flags |= 1;
            return 1;
        }
    }
    return 0;
}

int BodgySleep(struct CREATURE *Cre)
{
    int i;
    int x;
    int z;
    int Ret;

    if (AreaHidden(Cre->Box.Area)) {
        Ret = 1;
        goto Done;
    }
    for (i = 0; i < MaxWindows; i++) {
        if (AreaValid1(i, Cre->Box.Area)) {
            Ret = 0;
            goto Done;
        }
    }
    x = Cre->Box.Location.vx - GameWindows[0].CamPos.vx;
    if (x < -0x2000 || x > 0x2000) {
        Ret = 1;
        goto Done;
    }
    z = Cre->Box.Location.vz - GameWindows[0].CamPos.vz;
    Ret = (z < -0x2000 || z > 0x2000);
Done:
    if (Ret)
        Cre->Control |= 0x2000000;
    else
        Cre->Control &= ~0x2000000;
    return Ret;
}

void Relocate(void *Pntr, void *Base);

void InitialisePopulation(void *Addr)
{
    struct CREBATCHNAMES *TableEntry;

    CreBatchNamesTable = Addr;
    TableEntry = Addr;
    if (TableEntry) {
        while (*(int *)&TableEntry->Name) {
            if (TableEntry->CreData)
                Relocate(&TableEntry->CreData, CreBatchNamesTable);
            if (TableEntry->PickupData)
                Relocate(&TableEntry->PickupData, CreBatchNamesTable);
            PrintIdentityName(TableEntry->Name);
            TableEntry++;
        }
    }
}
