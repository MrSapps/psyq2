#include "types.h"

/* defined below; gcc defers these to the end of the file in this order */
long GetFLen(char *name);
long LoadFile(char *name, char *buffer);

extern int MusicCheck;
extern int ReadError;


long GetFLen(char *name)
{
    int retval;

    for (;;) {
        {
            CdlFILE fp;

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
            CdlFILE fp;

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
