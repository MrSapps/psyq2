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

int GetDifficultySetting(void);

int CreatureCount(void *Addr)
{
    struct CREBATCHNAMES *TableEntry;
    struct CREDATA *CreatureData;
    int TotalCreatures;
    int a;

    TotalCreatures = 0;
    a = GetDifficultySetting();
    if (a == 0)
        a = 0x800;
    else if (a == 1)
        a = 0x1000;
    else
        a = 0x2000;
    CreBatchNamesTable = Addr;
    TableEntry = Addr;
    if (!TableEntry)
        return TotalCreatures;
    for (; *(long *)&TableEntry->Name; TableEntry++) {
        if (!ModNameCmp(&TableEntry->Name, (struct IDENTITYNAME *)"PathCorner")) {
            PrintIdentityName(TableEntry->Name);
            CreatureData = TableEntry->CreData;
            if (CreatureData) {
                for (; CreatureData->TypeIdent; CreatureData++) {
                    if ((CreatureData->TypeIdent < 0x22 || CreatureData->TypeIdent > 0x24) && !(CreatureData->SpawnFlags & a))
                        TotalCreatures++;
                }
            }
        }
    }
    return TotalCreatures;
}

struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);

void RestoreCreatures(struct IDENTITYNAME LevelName, struct IDENTITYNAME ZoneName)
{
    struct CRESAVE *CreSave;
    long BytesToGo;
    long ManyBytes;
    struct SAVEFILEHDR *FileHdr;
    struct CRESAVE *(*Routine)();
    struct IDENTITYNAME FileName;
    int a;

    FileName = LevelName;
    for (a = 0; a < 11; a++) {
        if (!FileName.Name[a])
            break;
    }
    FileName.Name[a] = ZoneName.Name[4];
    FileHdr = OpenSaveMemory(FileName, 0);
    if (!FileHdr)
        return;
    BytesToGo = FileHdr->Size;
    CreSave = (struct CRESAVE *)(FileHdr + 1);
    while (BytesToGo) {
        Routine = GetSpawnDetails(CreSave->u.Packed.TypeIdent)->LoadStoreRoutine;
        Routine(0, 0, CreSave);
        if (CreSave->u.Packed.Packed)
            ManyBytes = sizeof(struct CRESAVEPACKED);
        else
            ManyBytes = sizeof(struct CRESAVEUNPACKED);
        BytesToGo -= ManyBytes;
        CreSave = (struct CRESAVE *)((char *)CreSave + ManyBytes);
    }
}

struct CREATURE *SpawnGeneral(struct CREDATA *CreData, struct RESPAWNDATA *RespawnData);
void InitialiseQuakePickups(struct PICKUPENTRY *Pickups);

void ProcessCreatureBatches(void)
{
    struct CREBATCHNAMES *TableEntry;

    TableEntry = CreBatchNamesTable;
    if (!TableEntry)
        return;
    for (; *(long *)&TableEntry->Name; TableEntry++) {
        if (TableEntry->Flags & 2)
            continue;
        if (TableEntry->Flags & 1) {
            if (TableEntry->CreData) {
                struct CREDATA *CreData;

                for (CreData = TableEntry->CreData; CreData->TypeIdent; )
                    SpawnGeneral(CreData++, 0);
                PrintIdentityName(TableEntry->Name);
            }
            if (TableEntry->PickupData) {
                InitialiseQuakePickups(TableEntry->PickupData);
                PrintIdentityName(TableEntry->Name);
            }
            TableEntry->Flags |= 2;
        }
    }
}

struct CREATURE *GetNextCre(int UseExpendable);
int GetFloorOffset(struct CREATURE *Cre);
int GetCeilingOffset(struct CREATURE *Cre);
int GameSpawn(struct CREATURE *Cre, struct CREDATA *CreData, struct SPAWNDETAILS *Details, struct RESPAWNDATA *RespawnData);
struct MATRIX *RotMatrixYXZ(struct SVECT *r, struct MATRIX *m);

struct CREATURE *SpawnGeneral(struct CREDATA *CreData, struct RESPAWNDATA *RespawnData)
{
    struct SPAWNDETAILS *Details;
    struct CREATURE *Cre;

    switch (GetDifficultySetting()) {
    case 0:
        if (CreData->SpawnFlags & 0x800)
            return 0;
        break;
    case 1:
        if (CreData->SpawnFlags & 0x1000)
            return 0;
        break;
    case 2:
        if (CreData->SpawnFlags & 0x2000)
            return 0;
        break;
    }
    Cre = GetNextCre(1);
    if (Cre) {
        Details = GetSpawnDetails(CreData->TypeIdent);
        if (!Details)
            return 0;
        if ((Cre->ModelHdr = Details->Hdr)) {
            Cre->TypeIdent = CreData->TypeIdent;
            Cre->SpawnFlags = CreData->SpawnFlags;
            Cre->Ang.vy = CreData->AngY;
            Cre->IdealYaw = CreData->AngY;
            Cre->Control = 1;
            Cre->HeightOffset = GetFloorOffset(Cre);
            Cre->TotalHeight = Cre->HeightOffset + GetCeilingOffset(Cre);
            RotMatrixYXZ(&Cre->Ang, &Cre->Matrix);
            PrintIdentityName(Cre->ModelHdr->ModelName);
            if (GameSpawn(Cre, CreData, Details, RespawnData))
                return Cre;
        }
    }
    return 0;
}

extern struct CRESAVE *SaveMemoryWriteAddress;
extern struct CREATURE Creatures[48];
extern struct CREATURE *CreFreeList[48];
int BytesBetween(void *Pntr0, void *Pntr1);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);

void SaveCreatures(struct IDENTITYNAME LevelName, struct IDENTITYNAME ZoneName)
{
    struct SPAWNDETAILS *SpawnDetails;
    struct CREATURE *Cre;
    struct SAVEFILEHDR *FileHdr;
    struct IDENTITYNAME FileName;
    int a;

    FileName = LevelName;
    for (a = 0; a < 11; a++) {
        if (!FileName.Name[a])
            break;
    }
    FileName.Name[a] = ZoneName.Name[4];
    FileHdr = OpenSaveMemory(FileName, 1);
    if (!FileHdr)
        return;
    SaveMemoryWriteAddress = (struct CRESAVE *)(FileHdr + 1);
    /* the end of Creatures[]; &Creatures[48] would be natural, but cmpfunc's address check
       sign-extends the 0x9000 %lo addend (reads 0x800c4c98), so name the next symbol instead */
    for (Cre = Creatures; Cre < &Creatures[48]; Cre++) {
        if (Cre->Life) {
            SpawnDetails = GetSpawnDetails(Cre->TypeIdent);
            if (SpawnDetails && SpawnDetails->OnReloadRoutine)
                SpawnDetails->OnReloadRoutine(Cre);
        }
    }
    FileHdr->Size = BytesBetween(FileHdr + 1, SaveMemoryWriteAddress);
    CloseWritableSaveMemoryFile(FileName, 1);
}

extern struct LEVELDATA *ThisLevelData;
struct IDENTITYNAME CreBatchSaveName;
void dprintf(struct WINDOW *Window, char *_format, ...);
void SpawnPathCorners(struct PATHCORNER *PathList);

void LoadSaveCreatureBatches(struct IDENTITYNAME *SaveName, int Save)
{
    struct SAVEFILEHDR *FileHdr;
    long *DataPntr;
    struct CREBATCHNAMES *Table;

    if (!ThisLevelData->MaxPlayers)
        return;
    Table = CreBatchNamesTable;
    if (!Table)
        return;
    if (SaveName)
        CreBatchSaveName = *SaveName;
    if (Save) {
        if (!LevelIsMultiPlayer) {
            FileHdr = OpenSaveMemory(CreBatchSaveName, 1);
            if (FileHdr) {
                PrintIdentityName(CreBatchSaveName);
                for (DataPntr = (long *)(FileHdr + 1); *(long *)&Table->Name; Table++)
                    *DataPntr++ = Table->Flags;
                FileHdr->Size = BytesBetween(FileHdr + 1, DataPntr);
                CloseWritableSaveMemoryFile(CreBatchSaveName, 1);
            } else {
                PrintIdentityName(CreBatchSaveName);
                dprintf(GameWindows, "Error, cant save batch info\n");
            }
        }
    } else {
        int FlagInit;

        FlagInit = 1;
        if (!LevelIsMultiPlayer) {
            FileHdr = OpenSaveMemory(CreBatchSaveName, 0);
            if (FileHdr) {
                FlagInit = 0;
                PrintIdentityName(CreBatchSaveName);
                for (DataPntr = (long *)(FileHdr + 1); *(long *)&Table->Name; Table++)
                    Table->Flags = *DataPntr++;
            }
        }
        if (FlagInit) {
            PrintIdentityName(CreBatchSaveName);
            for (; *(long *)&Table->Name; Table++)
                Table->Flags &= ~3;
        }
    }
    for (Table = CreBatchNamesTable; *(long *)&Table->Name; Table++)
        ;
    if (SaveName) {
        struct CREBATCHNAMES *TableEntry;

        TableEntry = FindIdentNameInList((struct IDENTITYNAME *)"PathCorner", CreBatchNamesTable, sizeof(struct CREBATCHNAMES));
        if (TableEntry)
            SpawnPathCorners((struct PATHCORNER *)TableEntry->CreData);
    }
}
