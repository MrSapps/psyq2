#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
void ClearSaveMemory(void);
void *GetSaveMemoryAddr(void);
int GetSaveMemorySize(void);
void StoreLevelStartState(void);
void RestoreLevelStartState(void);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void DeleteSaveMemory(struct IDENTITYNAME Name);
void DeleteAllMatchingMemory(short Mask);

extern char SaveMemory[7168];
extern char LevelRestartMemory[7168];

/* the save area is copied as a whole */
struct MEMBLK {
    char m[7168];
};

void DeleteSaveMemory(struct IDENTITYNAME Name);
void Relocate(void *Pntr, void *Base);

int ModNameCmp(struct IDENTITYNAME *Name1, struct IDENTITYNAME *Name2);
char *PrintIdentityName(struct IDENTITYNAME Name);

static struct SAVEFILEHDR *FindSaveFile(struct IDENTITYNAME Name)
{
    {
        struct SAVEFILEHDR *File;

        File = (struct SAVEFILEHDR *)SaveMemory;
        while (1) {
            switch (ModNameCmp(&Name, &File->Name)) {
            case 0:
                Relocate(&File, (void *)(File->Size + sizeof(struct SAVEFILEHDR)));
                break;
            case 1:
                return File;
            case 2:
                if (*(int *)&Name)
                    return 0;
                return File;
            }
        }
    }
}

int BytesBetween(void *a, void *b);
void *memmove(void *dst, const void *src, unsigned int n);



void *memcpy(void *dst, const void *src, unsigned int n);

void StoreLevelStartState(void)
{
    *(struct MEMBLK *)LevelRestartMemory = *(struct MEMBLK *)SaveMemory;
}

void RestoreLevelStartState(void)
{
    *(struct MEMBLK *)SaveMemory = *(struct MEMBLK *)LevelRestartMemory;
}

void *GetSaveMemoryAddr(void)
{
    return SaveMemory;
}

int GetSaveMemorySize(void)
{
    return sizeof(SaveMemory);
}

void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags)
{
    struct SAVEFILEHDR *FileHdr;

    PrintIdentityName(Name);
    PrintIdentityName(Name);
    FileHdr = FindSaveFile(Name);
    if (FileHdr) {
        struct SAVEFILEHDR *EndOfFile;

        FileHdr->Flags = Flags;
        EndOfFile = FileHdr + 1;
        Relocate(&EndOfFile, (void *)FileHdr->Size);
        EndOfFile->Name = (struct IDENTITYNAME){""};
    }
}

struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable)
{
    struct SAVEFILEHDR *FileName;

    /* dead store, deleted by flow after jump opt: the (then empty) branch
       survives until combine and shapes the arg packing below */
    if (Writeable)
        FileName = 0;
    PrintIdentityName(Name);
    PrintIdentityName(Name);
    FileName = FindSaveFile(Name);
    if (FileName) {
        if (Writeable) {
            struct SAVEFILEHDR *LastFile;
            int BytesCanMove;
            int BytesToMove;
            int BytesThisPass;

            LastFile = FindSaveFile((struct IDENTITYNAME){""});
            BytesCanMove = BytesBetween(LastFile, SaveMemory + sizeof(SaveMemory)) - sizeof(struct SAVEFILEHDR);
            BytesToMove = FileName->Size + sizeof(struct SAVEFILEHDR);
            while (BytesToMove) {
                if (BytesToMove <= BytesCanMove)
                    BytesThisPass = BytesToMove;
                else
                    BytesThisPass = BytesCanMove;
                BytesToMove -= BytesThisPass;
                memcpy(LastFile, FileName, BytesThisPass);
                memmove(FileName, (char *)FileName + BytesThisPass, BytesBetween(FileName, LastFile) - BytesToMove);
            }
            LastFile->Name = (struct IDENTITYNAME){""};
            FileName = FindSaveFile(Name);
        }
        return FileName;
    }
    if (Writeable) {
        {
            FileName = FindSaveFile((struct IDENTITYNAME){""});
            FileName->Name = Name;
            FileName->Size = 0;
            CloseWritableSaveMemoryFile(Name, 0);
        }
        return FileName;
    }
    return 0;
}

void DeleteSaveMemory(struct IDENTITYNAME Name)
{
    struct SAVEFILEHDR *File;
    struct SAVEFILEHDR *NextFile;
    struct SAVEFILEHDR *LastFile;

    PrintIdentityName(Name);
    PrintIdentityName(Name);
    File = FindSaveFile(Name);
    if (File) {
        NextFile = File + 1;
        Relocate(&NextFile, (void *)File->Size);
        LastFile = FindSaveFile((struct IDENTITYNAME){""});
        memmove(File, NextFile, BytesBetween(NextFile, LastFile) + sizeof(struct SAVEFILEHDR));
    }
}

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
