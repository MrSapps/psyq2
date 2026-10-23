#include "types.h"

extern char SaveMemory[7168];
extern char LevelRestartMemory[7168];

/* the save area is copied as a whole */
struct MEMBLK {
    char m[7168];
};

void DeleteSaveMemory(struct IDENTITYNAME Name);
void Relocate(void *Pntr, void *Base);

void DeleteAllMatchingMemory(short Mask)
{
    struct SAVEFILEHDR *FileHdr;

    FileHdr = (struct SAVEFILEHDR *)SaveMemory;
    while (*(int *)FileHdr) {
        if (FileHdr->Flags & Mask)
            DeleteSaveMemory(FileHdr->Name);
        else
            Relocate(&FileHdr, (void *)(FileHdr->Size + sizeof(struct SAVEFILEHDR)));
    }
}

void ClearSaveMemory(void)
{
    /* empty name terminates the file list */
    memcpy(SaveMemory, "\0\0\0\0\0\0\0\0\0\0\0", 12);
}

void *GetSaveMemoryAddr(void)
{
    return SaveMemory;
}

int GetSaveMemorySize(void)
{
    return sizeof(SaveMemory);
}

void StoreLevelStartState(void)
{
    *(struct MEMBLK *)LevelRestartMemory = *(struct MEMBLK *)SaveMemory;
}

void RestoreLevelStartState(void)
{
    *(struct MEMBLK *)SaveMemory = *(struct MEMBLK *)LevelRestartMemory;
}
