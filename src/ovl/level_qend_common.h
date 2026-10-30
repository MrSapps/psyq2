/* Shared by level_qendmis1, level_qdummy and level_qmagextr (one program built three times):
 * types and the library functions linked into each module. */
/* libgpu's DR_MODE (not in types.h) */
struct DR_MODE {
    unsigned long tag;
    unsigned long code[2];
};

/* libgpu.h */
#define setRECT(r, _x, _y, _w, _h) ((r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h))

/* the strplay sample's macros and decode environment */
#define bound(val, n) ((((val) - 1) / (n) + 1) * (n))
#define bound16(val) bound(val, 16)
#define bound24(val) bound(val, 24)

struct DECENV {
    unsigned long *vlcbuf[2];   /* 0x0 */
    unsigned short *imgbuf;     /* 0x8 */
    int vlcid;                  /* 0xc */
    struct RECT rect[2];        /* 0x10 */
    int rectid;                 /* 0x20 */
    struct RECT slice;          /* 0x24 */
    int isdone;                 /* 0x2c */
};

/* library functions linked into the module */
void *memset(void *s, int c, int n);
unsigned short GetTPage(int tp, int abr, int x, int y);
void SetDrawTPage(struct DR_TPAGE *p, int dfe, int dtd, int tpage);
void DecDCTReset(int mode);
void DecDCTin(unsigned long *buf, int mode);
void DecDCTout(unsigned long *buf, int size);
int DecDCToutCallback(void (*func)());
int DecDCTvlc2(unsigned long *bs, unsigned long *buf, unsigned short *table);
void DecDCTvlcBuild(unsigned short *table);
int DecDCTBufSize(unsigned long *bs);

