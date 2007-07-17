
#ifndef _imgfmts_
#define _imgfmts_




#include <stdlib.h>
#ifndef PNGSUPPORT
    // png.h already includes setjmp.h and forbids us to include it again
    #include <setjmp.h>
#else
    // this will provide setjmp.h in case we use png
    #include <png.h>
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



//////////////////////////////////// BMP STUFF /////////////////////////////////////////////////

#if defined(WIN32)
    #include <windows.h>
#endif

#ifndef BI_RGB // if windows.h is not included



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


bool LoadBitmapFromFile2RGBA(FILE* fjl, /*BITMAP *bd, */ int* w, int* h, RGBA **data);


#ifdef JPEGSUPPORT
    #include <jpeglib.h>
    struct my_error_mgr {
      struct jpeg_error_mgr pub;	/* "public" fields */
      jmp_buf setjmp_buffer;	/* for return to caller */
    };
    typedef struct my_error_mgr * my_error_ptr;

    bool LoadJPEGFromFile2RGBA(FILE* fjl, unsigned int *width, unsigned int *height, RGBA **data);
#endif


#ifdef PNGSUPPORT
    #include <png.h>
    bool LoadPNGFromFile2RGBA(FILE* fjl, unsigned int *width, unsigned int *height, RGBA **data);
#endif


#endif

