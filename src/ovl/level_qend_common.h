/* Shared by level_qendmis1, level_qdummy and level_qmagextr (one program built three times):
 * the strplay sample's macros and types. */
#include <memory.h>

/* the strplay sample's macros and decode environment */
#define bound(val, n) ((((val) - 1) / (n) + 1) * (n))
#define bound16(val) bound(val, 16)
#define bound24(val) bound(val, 24)

struct DECENV {
    unsigned long *vlcbuf[2];   /* 0x0 */
    unsigned short *imgbuf;     /* 0x8 */
    int vlcid;                  /* 0xc */
    RECT rect[2];        /* 0x10 */
    int rectid;                 /* 0x20 */
    RECT slice;          /* 0x24 */
    int isdone;                 /* 0x2c */
};


