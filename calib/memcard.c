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

struct SAVEFILEHDR *OpenSaveMemory(struct IDENTITYNAME Name, int Writeable);
void CloseWritableSaveMemoryFile(struct IDENTITYNAME Name, short Flags);
void FrontEndResetPlayerDefaults(void);
void FrontEndResetSoundDefaults(void);
void FrontEndResetVideoDefaults(void);
void FrontEndResetMultiplayerDefaults(void);
void Config_InstallVideo(void);

void LoadSaveGameConfigData(int Write)
{
    struct SAVEFILEHDR *FileHdr;
    struct QGAMECONFIG *ThisGameConfig;

    FileHdr = OpenSaveMemory(*&(struct IDENTITYNAME){"Game Config"}, Write);
    if (FileHdr) {
        ThisGameConfig = (struct QGAMECONFIG *)(FileHdr + 1);
        if (Write) {
            *ThisGameConfig = Engine.GameConfig;
            FileHdr->Size = sizeof(struct QGAMECONFIG);
            CloseWritableSaveMemoryFile(*&(struct IDENTITYNAME){"Game Config"}, 4);
            return;
        }
        Engine.GameConfig = *ThisGameConfig;
    } else {
        FrontEndResetPlayerDefaults();
        FrontEndResetSoundDefaults();
        FrontEndResetVideoDefaults();
        FrontEndResetMultiplayerDefaults();
    }
    Config_InstallVideo();
}

extern short MultiTapExists[];
/* "SLOT 1" sits in .sdata right after coop, and splat names it coop+0xC */
extern char coop[];
int sprintf(char *s, const char *format, ...);

void MemoryCardGetCardnameFromPortNum(long index, char *buf2)
{
    char buf[12];

    switch (index) {
    case 0:
        if (MultiTapExists[0])
            sprintf(buf, "SLOT 1-A");
        else
            sprintf(buf, coop + 0xc);
        break;
    case 1:
        sprintf(buf, "SLOT 1-B");
        break;
    case 2:
        sprintf(buf, "SLOT 1-C");
        break;
    case 3:
        sprintf(buf, "SLOT 1-D");
        break;
    case 4:
        if (MultiTapExists[1])
            sprintf(buf, "SLOT 2-A");
        else
            sprintf(buf, "SLOT 2");
        break;
    case 5:
        sprintf(buf, "SLOT 2-B");
        break;
    case 6:
        sprintf(buf, "SLOT 2-C");
        break;
    case 7:
        sprintf(buf, "SLOT 2-D");
        break;
    }
    sprintf(buf2, "MEMORY CARD %s", buf);
}

/* ---- MemoryCardUpdate2 and the inline helpers it uses ---- */
extern unsigned short memcard_slotname[];
extern char tempDescriptionBuffer[];
extern struct _MEMCARD_HEADER MemoryCardHeader;
extern unsigned long MemoryCardIcon[];
long MemCardExist(long chan);
long MemCardDeleteFile(long chan, char *name);
char *GetSaveMemoryAddr(void);
int GetSaveMemorySize(void);
int GetCurrentLevelNumber(void);
void *memset(void *s, int c, int n);

/* memcard_data as the original declared it: its char fields are signed (they load with lb) */
struct MEMCARD_DATA_S {
    long cmds;
    long result;
    long ret;
    long pendingStatus;
    struct DIRENTRY fileName[15];
    long cardInfo[8];
    char fileDescription[15][32];
    signed char numCards;
    signed char activeCardIndex;
    signed char numFiles;
    signed char numMpFiles;
    signed char numFreeFiles;
    signed char activeFileIndex;
    signed char testCardIndex;
    signed char testDescriptionIndex;
    char activeFileName[32];
};
#define MCD (*(struct MEMCARD_DATA_S *)&memcard_data)
#define SLOT(n) memcard_slotname[n]

/* Remove the card list entry OldCard (names made up: SYM only shows the inlined blocks) */
static void RemoveCardEntryUnchecked(int OldCard)
{
    int i;

    if (OldCard == -1)
        Engine.PrintF("Serious problem in memcard removal detection from choose file - Tell Ray\n");
    for (i = OldCard; i < MCD.numCards - 1; i++)
        MCD.cardInfo[i] = MCD.cardInfo[i + 1];
    MCD.numCards--;
}

static void RemoveCardEntry(int OldCard)
{
    int i;

    if (OldCard != -1) {
        for (i = OldCard; i < MCD.numCards - 1; i++)
            MCD.cardInfo[i] = MCD.cardInfo[i + 1];
        MCD.numCards--;
    }
}

static void RemoveCard(int card)
{
    int OldCard;
    int i;

    OldCard = -1;
    for (i = 0; i < MCD.numCards; i++) {
        if (MCD.cardInfo[i] == card) {
            OldCard = i;
            break;
        }
    }
    RemoveCardEntry(OldCard);
}

static void HeaderTitleFromDescription(char *buf)
{
    char *ascp;
    unsigned short *jisp;

    jisp = (unsigned short *)MemoryCardHeader.Title;
    ascp = buf;
    while (*ascp) {
        int sj = ASCII2SJIS(*ascp++);
        *jisp++ = (sj << 8) | ((unsigned short)sj >> 8);
    }
    *jisp = 0;
}

static char SJIS2ASCII(unsigned short JIS)
{
    static char table[] = " \?\?,.\?:;\?!\"\?''\?^\?_\?\?\?\?\?\?\?\?\?\?\?-/\\~\?|\?\?`'\"\"()[][]{}<>\?\?\?\?\?\?\?\?+-\?x\?\?=\?<>\?\?\?\?\?\?\?\?\?\?\?$\?\?%#&*@";

    if (!(JIS & 0xff))
        return 0;
    if (JIS >= 0x824f && JIS <= 0x8258)
        return JIS - 0x821f;
    if (JIS >= 0x8260 && JIS <= 0x827a)
        return JIS - 0x821f;
    if (JIS >= 0x8281 && JIS <= 0x829b)
        return JIS - 0x8220;
    if (JIS >= 0x8140 && JIS <= 0x8197)
        return table[JIS - 0x8140];
    return '?';
}

static unsigned short SwapShort(unsigned short x)
{
    return (x << 8) | ((x >> 8) & 0xff);
}

static inline void DescriptionFromHeaderTitle(void)
{
    char *ascp;
    unsigned short *jisp;
    unsigned short swapshort;

    ascp = MCD.fileDescription[MCD.testDescriptionIndex];
    jisp = (unsigned short *)(tempDescriptionBuffer + 4);
    while (*jisp) {
        swapshort = SwapShort(*jisp);
        *ascp++ = SJIS2ASCII(swapshort);
        jisp++;
    }
    *ascp = 0;
}

static void MakeSaveFileName(char *dst)
{
    char tempBuf[64];

    GetSaveDescription(tempBuf);
    Engine.sprintf(tempBuf, "%sSAVE%02d%c", "BASLUS-00757", (unsigned char)GetCurrentLevelNumber(), gFDescVersionNum + 'A');
    tempBuf[13] = (Engine.GameConfig.cCheats & 3) + '0';
    tempBuf[14] = (Engine.GameConfig.cContinues & 3) + '0';
    tempBuf[15] = (Engine.GameConfig.sDifficulty & 3) + '0';
    Engine.sprintf(dst, tempBuf);
}

void MemoryCardUpdate2(void)
{
    int i;
    int sync;
    int OldCard;
    int j;
    int BackUpCursorPos;
    char buf[32];
    char *memoryBuffer;
    long numFiles;

    sync = MemCardSync(1, &MCD.cmds, &MCD.result);
    switch (sync) {
    case 0:
        break;
    case -1:
        switch (MCD.pendingStatus) {
        case 1:
        case 2:
            Engine.MemCardAccept(SLOT(MCD.testCardIndex));
            break;
        case 3:
            Engine.MemCardAccept(SLOT(MCD.testCardIndex));
            break;
        case 13:
        case 18:
            MemCardExist(SLOT(MCD.activeCardIndex));
            break;
        case 4:
        case 6:
        case 20:
        case 21:
        case 22:
            Engine.MemCardAccept(SLOT(MCD.activeCardIndex));
            break;
        case 10:
            Engine.sprintf(buf, "%s", "*");
            if (!Engine.MemCardGetDirentry(SLOT(MCD.activeCardIndex), buf, MCD.fileName, &numFiles, 0, 15)) {
                MCD.numFiles = numFiles;
                MemoryCardPostProcessDir();
            } else
                MemoryCardSetStatus(17);
            MCD.testDescriptionIndex = 0;
            if (MCD.numFiles)
                MemoryCardSetStatus(11);
            else if (Engine.gFrontEndLoadType == 1) {
                if (MCD.numFreeFiles <= 0)
                    MemoryCardSetStatus(18);
                else
                    MemoryCardSetStatus(13);
            } else
                MemoryCardSetStatus(12);
            break;
        case 11:
            Engine.MemCardReadFile(SLOT(MCD.activeCardIndex), MCD.fileName[MCD.testDescriptionIndex].name, tempDescriptionBuffer, 0, 0x80);
            break;
        case 7:
            if (!Engine.MemCardFormat(SLOT(MCD.activeCardIndex)))
                MemoryCardSetStatus(10);
            else
                MemoryCardSetStatus(9);
            break;
        case 14:
            memoryBuffer = GetSaveMemoryAddr();
            Engine.MemCardReadFile(SLOT(MCD.activeCardIndex), MCD.fileName[MCD.activeFileIndex].name, memoryBuffer, 0x200, GetSaveMemorySize());
            break;
        case 15:
            memset(MemoryCardHeader.pad, 0, 0x1c);
            MemoryCardHeader.Magic[0] = 'S';
            MemoryCardHeader.Magic[1] = 'C';
            MemoryCardHeader.Type = 0x11;
            MemoryCardHeader.BlockEntry = 1;
            GetSaveDescription(buf);
            HeaderTitleFromDescription(buf);
            memcpy(MemoryCardHeader.Clut, (char *)MemoryCardIcon + 0x14, 32);
            memcpy(MemoryCardHeader.Icon, (char *)MemoryCardIcon + 0x40, 128);
            LoadSaveGameConfigData(1);
            MCD.activeFileName[13] = (Engine.GameConfig.cCheats & 3) + '0';
            MCD.activeFileName[14] = (Engine.GameConfig.cContinues & 3) + '0';
            MCD.activeFileName[15] = (Engine.GameConfig.sDifficulty & 3) + '0';
            Engine.MemCardWriteFile(SLOT(MCD.activeCardIndex), MCD.activeFileName, &MemoryCardHeader, 0, GetSaveMemorySize() + 0x200);
            break;
        }
        break;
    case 1:
        switch (MCD.pendingStatus) {
        case 1:
            switch (MCD.result) {
            case 4:
                if (Engine.gFrontEndLoadType != 1)
                    break;
            case 0:
            case 3:
                MCD.cardInfo[MCD.numCards] = MCD.testCardIndex;
                MCD.numCards++;
                break;
            case 1:
            case 2:
                break;
            }
            if (MultiTapExists[MCD.testCardIndex >> 2])
                MCD.testCardIndex++;
            else
                MCD.testCardIndex += 4;
            if (MCD.testCardIndex >= 8) {
                MCD.testCardIndex = 0;
                if (MCD.numCards) {
                    MemoryCardSetStatus(3);
                    MCD.testCardIndex = 0;
                    break;
                }
                MemoryCardSetStatus(2);
                break;
            }
            Engine.MemCardAccept(SLOT(MCD.testCardIndex));
            break;
        case 13:
            switch (MCD.result) {
            case 0:
                break;
            case 1:
                OldCard = -1;
                for (j = 0; j < MCD.numCards; j++) {
                    if (MCD.cardInfo[j] == MCD.activeCardIndex) {
                        OldCard = j;
                        break;
                    }
                }
                RemoveCardEntryUnchecked(OldCard);
                MemoryCardSetStatus(1);
                break;
            }
            break;
        case 2:
        case 3:
            OldCard = -1;
            for (j = 0; j < MCD.numCards; j++) {
                if (MCD.cardInfo[j] == MCD.testCardIndex) {
                    OldCard = j;
                    break;
                }
            }
            switch (MCD.result) {
            case 0:
            case 3:
            case 4:
                if (OldCard == -1) {
                    MCD.cardInfo[MCD.numCards] = MCD.testCardIndex;
                    MCD.numCards++;
                }
                break;
            case 1:
            case 2:
            default:
                RemoveCardEntry(OldCard);
                break;
            }
            if (!MultiTapExists[0]) {
                RemoveCard(1);
                RemoveCard(2);
                RemoveCard(3);
            }
            if (!MultiTapExists[1]) {
                RemoveCard(5);
                RemoveCard(6);
                RemoveCard(7);
            }
            if (MultiTapExists[MCD.testCardIndex >> 2])
                MCD.testCardIndex++;
            else
                MCD.testCardIndex += 4;
            BackUpCursorPos = Engine.cursorpos;
            if (MCD.pendingStatus == 3)
                Engine.InGame_PrintMemCardMessage(3);
            else
                MemoryCardSetStatus(2);
            if (BackUpCursorPos >= Engine.cursorupper)
                BackUpCursorPos = Engine.cursorupper - 1;
            Engine.cursorpos = BackUpCursorPos;
            if (MCD.numCards) {
                if (MCD.pendingStatus == 2)
                    MemoryCardSetStatus(3);
            } else {
                if (MCD.pendingStatus == 3)
                    MemoryCardSetStatus(2);
            }
            if (MCD.testCardIndex >= 8)
                MCD.testCardIndex = 0;
            Engine.MemCardAccept(SLOT(MCD.testCardIndex));
            break;
        case 11:
            if (MCD.result == 0)
                DescriptionFromHeaderTitle();
            else {
                MemoryCardSetStatus(17);
                break;
            }
            if (MCD.testDescriptionIndex < MCD.numFiles - 1) {
                MCD.testDescriptionIndex++;
                Engine.MemCardReadFile(SLOT(MCD.activeCardIndex), MCD.fileName[MCD.testDescriptionIndex].name, tempDescriptionBuffer, 0, 0x80);
                break;
            } else {
                int nummpfiles;
                int numspfiles;

                numspfiles = 0;
                nummpfiles = 0;
                for (i = 0; i < MCD.numFiles; i++) {
                    int IsMulti;

                    if (MCD.fileDescription[i][3] == 'M' && MCD.fileDescription[i][4] == 'U' &&
                        MCD.fileDescription[i][5] == 'L' && MCD.fileDescription[i][6] == 'T' &&
                        MCD.fileDescription[i][7] == 'I') {
                        IsMulti = 1;
                        nummpfiles++;
                    } else {
                        IsMulti = 0;
                        numspfiles++;
                    }
                    if (!Engine.gFrontEndLoadType) {
                        if (IsMulti) {
                            if (!(*(int *)&Engine.gFrontEndPadShot & 0x7f000000))
                                FileRemaps[nummpfiles - 1] = i;
                        } else if ((*(int *)&Engine.gFrontEndPadShot & 0x7f000000) == 0x1000000)
                            FileRemaps[numspfiles - 1] = i;
                    }
                }
                MCD.numMpFiles = nummpfiles;
                if (!Engine.gFrontEndLoadType) {
                    if ((!numspfiles && (*(int *)&Engine.gFrontEndPadShot & 0x7f000000) == 0x1000000) ||
                        (!MCD.numMpFiles && !(*(int *)&Engine.gFrontEndPadShot & 0x7f000000)))
                        MemoryCardSetStatus(12);
                    else
                        MemoryCardSetStatus(13);
                } else {
                    if (!MCD.numFiles && !MCD.numFreeFiles)
                        MemoryCardSetStatus(18);
                    else
                        MemoryCardSetStatus(13);
                }
            }
            break;
        case 14:
            if (MCD.result == 0) {
                LoadSaveGameConfigData(0);
                MemoryCardSetStatus(16);
            } else {
                LoadSaveGameConfigData(0);
                MemoryCardSetStatus(17);
            }
            break;
        case 15:
            if (MCD.result == 0)
                MemoryCardSetStatus(16);
            else
                MemoryCardSetStatus(17);
            break;
        case 4:
            switch (MCD.result) {
            case 4:
                MemoryCardSetStatus(5);
                break;
            case 0:
                MemoryCardSetStatus(10);
                break;
            case 3:
            default:
                MemoryCardSetStatus(17);
                break;
            }
            break;
        case 22:
            switch (MCD.result) {
            case 4:
                MemoryCardSetStatus(5);
                break;
            case 0:
                MakeSaveFileName(MCD.activeFileName);
                if (!Engine.MemCardCreateFile(SLOT(MCD.activeCardIndex), MCD.activeFileName, 1))
                    MemoryCardSetStatus(15);
                else
                    MemoryCardSetStatus(17);
                break;
            case 3:
            default:
                MemoryCardSetStatus(17);
                break;
            }
            break;
        case 6:
            if (MCD.result == 4)
                MemoryCardSetStatus(7);
            else
                MemoryCardSetStatus(9);
            break;
        case 21:
            switch (MCD.result) {
            case 4:
                MemoryCardSetStatus(5);
                break;
            case 0:
                MemoryCardSetStatus(14);
                break;
            case 3:
            default:
                MemoryCardSetStatus(17);
                break;
            }
            break;
        case 20:
            switch (MCD.result) {
            case 4:
                MemoryCardSetStatus(5);
                break;
            case 0:
                Engine.sprintf(MCD.activeFileName, MCD.fileName[MCD.activeFileIndex].name);
                switch (MemCardDeleteFile(SLOT(MCD.activeCardIndex), MCD.activeFileName)) {
                case 0:
                    MakeSaveFileName(MCD.activeFileName);
                    if (!Engine.MemCardCreateFile(SLOT(MCD.activeCardIndex), MCD.activeFileName, 1))
                        MemoryCardSetStatus(15);
                    else
                        MemoryCardSetStatus(17);
                    break;
                case 1:
                case 2:
                case 4:
                case 5:
                    MemoryCardSetStatus(17);
                    break;
                }
                break;
            case 3:
            default:
                MemoryCardSetStatus(17);
                break;
            }
            break;
        }
        break;
    }
}
