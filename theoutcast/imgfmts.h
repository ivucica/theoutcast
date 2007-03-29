
#ifndef _imgfmts_
#define _imgfmts_




#include <stdlib.h>
#ifdef JPEGSUPPORT
#include <jpeglib.h>
#endif
#include <setjmp.h>

#if defined(WIN32)
#include <windows.h>
#endif

#ifndef BI_RGB // is windows.h included



#pragma pack(push)
#pragma pack(1)


typedef struct tagBITMAP {  // bm
   long   bmType;
   long   bmWidth;
   long   bmHeight;
   long   bmWidthBytes;
   unsigned short int bmPlanes;
   unsigned short int bmBitsPixel;
   void*  bmBits;
} BITMAP;
typedef struct tagBITMAPFILEHEADER { // bmfh
        unsigned short bfType;
        unsigned int   bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned int   bfOffBits;
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{ // bmih
   unsigned int   biSize;
   long           biWidth;
   long           biHeight;
   unsigned short biPlanes;
   unsigned short biBitCount;
   unsigned int   biCompression;
   unsigned int   biSizeImage;
   long           biXPelsPerMeter;
   long           biYPelsPerMeter;
   unsigned int   biClrUsed;
   unsigned int   biClrImportant;
} BITMAPINFOHEADER;
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define GetRValue(rgb)      ((unsigned char)(rgb))
#define GetGValue(rgb)      ((unsigned char)(((unsigned short)(rgb)) >> 8))
#define GetBValue(rgb)      ((unsigned char)((rgb)>>16))



#pragma pack(pop)

#endif

typedef struct {
   unsigned char r,g,b,a;
} RGBA;


typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} T_RGB;

typedef T_RGB * RGBArray;

#ifdef JPEGSUPPORT
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;
#endif

bool LoadBitmapFromFile2RGBA(FILE* fjl, /*BITMAP *bd, */ int* w, int* h, RGBA **data);
#ifdef JPEGSUPPORT
bool LoadJPEGFromFile2RGBA(FILE* fjl, unsigned int *width, unsigned int *height, RGBA **data);
#endif


typedef struct {
    unsigned int size;
    unsigned char* dump;
    unsigned int internal;
} Sprite;
#include <map>
typedef std::map<unsigned long, Sprite*> SpriteMap;
typedef SpriteMap::iterator SpriteIterator;
extern SpriteMap sprites;
extern bool sprLoaded;

bool LoadSprites(const char *filename);

#endif

