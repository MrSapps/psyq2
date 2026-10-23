#include "types.h"

extern int MusicCheck;
extern int ReadError;

struct CdlFILE *CdSearchFile(struct CdlFILE *fp, char *name);
int CdControlB(unsigned char com, unsigned char *param, unsigned char *result);
int CdRead(int sectors, unsigned long *buf, int mode);
int CdReadSync(int mode, unsigned char *result);

long GetFLen(char *name)
{
    int retval;

    for (;;) {
        {
            struct CdlFILE fp;

            if (CdSearchFile(&fp, name))
                retval = fp.size;
            else
                retval = -1;
        }
        if (retval >= 0)
            return retval;
    }
}

long LoadFile(char *name, char *buffer)
{
    int retval;

    for (;;) {
        MusicCheck = 0;
        ReadError = 0;
        {
            int fileLength;
            struct CdlFILE fp;

            if (CdSearchFile(&fp, name)) {
                fileLength = fp.size;
                if (CdControlB(0x15, (unsigned char *)&fp.pos, 0)
                    && CdRead((short)((fileLength + 2047) / 2048), (unsigned long *)buffer, 0x80)
                    && CdReadSync(0, 0) >= 0)
                    retval = fileLength;
                else
                    retval = -2;
            } else {
                retval = -1;
            }
        }
        if (retval >= 0)
            return retval;
    }
}
