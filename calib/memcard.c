#include "types.h"
extern struct QENGINELEVINTERFACE Engine;
extern unsigned int usp;
char FileRemaps[16];
struct _MEMCARD_DATA memcard_data;
unsigned long fastusp;
int MemoryCardRemapCardIndex(int index);
void MemoryCardGetCardnameFromPortNum(long index, char *buf2);
void MemoryCardUpdate2(void);

void MemoryCardInit(void)
{
    memcard_data.ret = -1;
    memcard_data.cmds = 0;
    memcard_data.result = 0;
    memcard_data.pendingStatus = 1;
    memcard_data.numCards = 0;
    memcard_data.activeCardIndex = 0;
    memcard_data.numFiles = 0;
    memcard_data.activeFileIndex = 0;
}

void MemoryCardSetStatus(long desiredStatus)
{
    if (desiredStatus == 1) {
        memcard_data.numCards = 0;
        memcard_data.activeCardIndex = 0;
        memcard_data.testCardIndex = 0;
    }
    if (desiredStatus == 4 || desiredStatus == 1) {
        MemCardSync(0, &memcard_data.cmds, &memcard_data.result);
    }
    memcard_data.pendingStatus = desiredStatus;
    Engine.InGame_PrintMemCardMessage(desiredStatus);
}

char *MemoryCardGetFilename(long index)
{
    if (Engine.gFrontEndLoadType == 1)
        return memcard_data.fileDescription[index];
    return memcard_data.fileDescription[FileRemaps[index]];
}

long MemoryCardGetNumFiles(void)
{
    if (Engine.gFrontEndLoadType == 1)
        return (signed char)memcard_data.numFiles;
    if (!Engine.gFrontEndLoadSubType)
        return (signed char)memcard_data.numMpFiles;
    return (signed char)memcard_data.numFiles - (signed char)memcard_data.numMpFiles;
}

void MemoryCardGetCardname(long index, char *buf)
{
    MemoryCardGetCardnameFromPortNum(MemoryCardRemapCardIndex(index), buf);
}

void MemoryCardSetActiveCard(long index)
{
    memcard_data.activeCardIndex = MemoryCardRemapCardIndex(index);
}

void MemoryCardSetActiveFile(long activeFile)
{
    if (Engine.gFrontEndLoadType == 1)
        memcard_data.activeFileIndex = activeFile;
    else
        memcard_data.activeFileIndex = FileRemaps[activeFile];
}

void MemoryCardUpdate(void)
{
    fastusp = SetSp(usp);
    MemoryCardUpdate2();
    SetSp(fastusp);
}

int MemoryCardRemapCardIndex(int index)
{
    char sortarray[8];
    int AllInOrder;
    int i;
    for (i = 0; i < (signed char)memcard_data.numCards; i++)
        sortarray[i] = memcard_data.cardInfo[i];
    do {
        AllInOrder = 1;
        for (i = 0; i < (signed char)memcard_data.numCards - 1; i++) {
            if (sortarray[i] > sortarray[i + 1]) {
                sortarray[i] ^= sortarray[i + 1];
                sortarray[i + 1] ^= sortarray[i];
                sortarray[i] ^= sortarray[i + 1];
                AllInOrder = 0;
            }
        }
    } while (!AllInOrder);
    return sortarray[index];
}
