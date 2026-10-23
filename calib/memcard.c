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

long MemoryCardGetStatus(void)
{
    return memcard_data.pendingStatus;
}

long MemoryCardGetNumFreeBlocks(void)
{
    return (signed char)memcard_data.numFreeFiles;
}

long MemoryCardGetNumCards(void)
{
    return (signed char)memcard_data.numCards;
}

int ASCII2SJIS(unsigned char ascii_code)
{
    static unsigned short ascii_k_table[33] = {
        0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166,
        0x8169, 0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144, 0x815E,
        0x8146, 0x8147, 0x8171, 0x8181, 0x8172, 0x8148, 0x8197,
        0x816D, 0x815F, 0x816E, 0x814F, 0x8151, 0x8165,
        0x816F, 0x8162, 0x8170, 0x8160,
    };
    unsigned char t;

    if (ascii_code >= 0x20 && ascii_code <= 0x2f)
        t = 0x20;
    else if (ascii_code >= 0x30 && ascii_code <= 0x39)
        return ascii_code + 0x821f;
    else if (ascii_code >= 0x3a && ascii_code <= 0x40)
        t = 0x2a;
    else if (ascii_code >= 0x41 && ascii_code <= 0x5a)
        return ascii_code + 0x821f;
    else if (ascii_code >= 0x5b && ascii_code <= 0x60)
        t = 0x44;
    else if (ascii_code >= 0x61 && ascii_code <= 0x7a)
        return ascii_code + 0x8220;
    else if (ascii_code >= 0x7b && ascii_code <= 0x7e)
        t = 0x5e;
    else
        return 0;
    return ascii_k_table[ascii_code - t];
}

char gFDescVersionNum;
int sprintf(char *s, const char *format, ...);
int strcmp(const char *a, const char *b);
void GetSaveLevelName(char *buf);

/* The helper's name is made up: SYM shows its inlined locals as nested blocks of GetSaveDescription. */
static int DescInUse(char *buf, short j)
{
    short i;
    char testBuf[32];

    if (j)
        sprintf(testBuf, "%s (%d)", buf, j);
    else
        sprintf(testBuf, "%s", buf, j);
    for (i = 0; i < (signed char)memcard_data.numFiles; i++) {
        if (i != (signed char)memcard_data.activeFileIndex && !strcmp(testBuf, memcard_data.fileDescription[i]))
            return 1;
    }
    return 0;
}

void GetSaveDescription(char *buf)
{
    short j = 0;

    GetSaveLevelName(buf);
    buf[31] = 0;
    while (DescInUse(buf, j))
        j++;
    if (j)
        sprintf(buf, "%s (%d)", buf, j);
    gFDescVersionNum = j;
}

char *GetStringFromStringtable(struct IDENTITYNAME StringName);

/* Engine+0x2bc is read as a 32-bit word (lw, sign test): types.h has a short there */
void GetSaveLevelName(char *buf)
{
    char *pChar;
    short i;

    sprintf(buf, "Q2_");
    i = 3;
    if (!(*(int *)&Engine.gFrontEndPadShot & 0x7f000000)) {
        if (*(int *)&Engine.gFrontEndPadShot < 0)
            sprintf(buf + 3, "MULTI UNLOCK");
        else
            sprintf(buf + 3, "MULTI DATA");
        return;
    }
    for (pChar = GetStringFromStringtable(*(struct IDENTITYNAME *)"MapTitle"); *pChar; pChar++) {
        if (*pChar == ' ' || (*pChar >= '0' && *pChar <= '9'))
            buf[i++] = *pChar;
        else
            buf[i++] = *pChar & 0xdf;
    }
    buf[i] = 0;
}

void MemoryCardPostProcessDir(void)
{
    char buf[16];
    char *pCharA;
    char *pCharB;
    char q2fileindeces[16];
    int i;
    int totalsize;
    int totalq2files;
    int samelettercount;

    totalq2files = 0;
    totalsize = 0;
    sprintf(buf, "%s", "BASLUS-00757");
    for (i = 0; i < (signed char)memcard_data.numFiles; i++) {
        totalsize += memcard_data.fileName[i].size;
        pCharA = buf;
        pCharB = memcard_data.fileName[i].name;
        samelettercount = 0;
        while (*pCharA && *pCharB) {
            if (*pCharA++ != *pCharB++)
                break;
            samelettercount++;
        }
        if (samelettercount >= 12)
            q2fileindeces[totalq2files++] = i;
    }
    for (i = 0; i < totalq2files; i++)
        memcard_data.fileName[i] = memcard_data.fileName[q2fileindeces[i]];
    memcard_data.numFiles = totalq2files;
    totalsize /= 8192;
    memcard_data.numFreeFiles = 15 - totalsize;
}
