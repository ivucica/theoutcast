#include <stdio.h>
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

int main () {


	BITMAPFILEHEADER bfh;
	BITMAP bmpData;
	BITMAPINFOHEADER bih;
	int i;
	struct { unsigned char b,g,r,a; } rgba;
	char chr;

	FILE * fi = fopen("in.map", "rb");
	FILE * fo = fopen("out.bmp", "wb");

	bfh.bfType = 'MB';
	bfh.bfSize = 512*256 + sizeof(bfh) + sizeof(bih) + sizeof(rgba)*256;
	bfh.bfReserved1 = bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih) + sizeof(rgba) * 256;
	fwrite(&bfh, sizeof(bfh), 1, fo);

	bih.biSize = sizeof(bih);
	bih.biWidth = 256;
	bih.biHeight = 512;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = 0;
	bih.biSizeImage = 512*256;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 256;
	bih.biClrImportant = 256;
	fwrite(&bih, sizeof(bih), 1, fo);

	rgba.a = 255;
	for (i = 0; i < 255; i++) {
		
		
		rgba.b = (i % 6) / 5. * 255;
		rgba.g = ((i / 6) % 6) / 5. * 255;
		rgba.r = (i / 36) / 5. * 255; 
		
		fwrite(&rgba, sizeof(rgba), 1, fo);
	}

	while (!feof(fi)) {
		fread(&chr, 1, 1, fi);
		fwrite(&chr, 1, 1, fo);
	}

	fclose(fi);
	fclose(fo);

	return 0;
}
