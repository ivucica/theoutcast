


#include <stdlib.h>
#include <GL/glut.h>

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <setjmp.h>
#include <GL/glu.h>
#include "imgfmts.h"

bool invertcolors = false; //default value in client

unsigned int  getbitsfrompos(unsigned char *arr, unsigned int pos, unsigned short int bpp) {
	unsigned int bitaddr = pos * bpp;
	unsigned char *bp = arr + (bitaddr>>3);
	unsigned char leftinbyte;
	unsigned char b = *bp;
	unsigned int r = 0;

	b<<=bitaddr&7;
	leftinbyte = 8-(bitaddr&7);
	for (unsigned short int a=0;a<bpp;++a) {
		if (!leftinbyte) {
			b=*(++bp);
			leftinbyte=8;
		}
		r<<=1;
		r|=(b&0x80)>>7;
		b<<=1;
		--leftinbyte;
	}

	return r;
}


bool LoadBitmapFromFile2RGBA(FILE* fjl, /*BITMAP *bd, */ int* w, int* h, RGBA **data) {
	BITMAPFILEHEADER bfh;
	BITMAP bmpData;
	BITMAPINFOHEADER bih;
	//FILE *fjl;
	unsigned int palitems = 0;
	RGBA *pal;
	unsigned int sz;

/*	fjl = fopen(filename, "rb");
	if (!fjl)
		return false;
*/
	// uèita heder od bitmape
	fread (&bfh, sizeof(BITMAPFILEHEADER), 1, fjl);
	if (bfh.bfType != 'MB') {		// da li je to zbilja bitmapa
		//fclose (fjl);
		return false;
	}

	// uèita informacije o bitmapi
	fread (&bih, sizeof(BITMAPINFOHEADER), 1, fjl);

	// ako je kompresirano, onda returnaj fols...
	if (bih.biCompression != BI_RGB && bih.biCompression != BI_BITFIELDS) {
		//fclose(fjl);
		return false;
	}

	// popuni podatke o bitmapi koji æe se poslati juzeru ako ih bude htio
	bmpData.bmType = 0;
	bmpData.bmWidth = bih.biWidth;
	bmpData.bmPlanes = bih.biPlanes;
	bmpData.bmHeight = bih.biHeight;
	bmpData.bmBitsPixel = bih.biBitCount;
	bmpData.bmBits = NULL;

	// ako je definirana velièina slike, onda od tamo izraèuna šrinu retka, inaèe proba sam
	if (bih.biSizeImage) {
		bmpData.bmWidthBytes = bih.biSizeImage / bih.biHeight;
		sz = bih.biSizeImage;
	} else {
		unsigned int bpl = bmpData.bmWidth * bmpData.bmBitsPixel;
		bmpData.bmWidthBytes = (bpl & 3) ? ((bpl >> 3) + 1) : (bpl >> 3);
		bmpData.bmWidthBytes+=(bmpData.bmWidthBytes & 1);// if (bmpData.bmWidthBytes & 1) bmpData.bmWidthBytes++;

		sz = bmpData.bmWidthBytes * abs(bmpData.bmHeight);
	}

	// ako je bitmapa bitna manje od 16 bita, onda uèita paletu
	if (bmpData.bmBitsPixel < 16) {
		palitems = 1 << bmpData.bmBitsPixel;
		pal = (RGBA*)malloc(sizeof(RGBA) * palitems);
		fread(pal, sizeof(RGBA), palitems, fjl);
	}

	// uèita sliku
	fseek (fjl, bfh.bfOffBits, SEEK_SET);
	bmpData.bmBits = malloc(sz);
	if (!bmpData.bmBits) {
		//fclose(fjl);
		return false;
	}
	fread(bmpData.bmBits, sz, 1, fjl);
	//fclose(fjl);

	// popuni RGBA
	*data = (RGBA*)malloc(sizeof(RGBA) * bmpData.bmWidth * abs(bmpData.bmHeight));
	for (int y=0; y<bmpData.bmHeight; ++y) {
		unsigned char *ptr = ((unsigned char *)bmpData.bmBits) + y*bmpData.bmWidthBytes;
		for (int x=0; x<bmpData.bmWidth; ++x) {
			unsigned int bits = getbitsfrompos(ptr, x, bmpData.bmBitsPixel);	// izvuèe komad bitstringa
			//RGBA *tmp = &(*data)[(bmpData.bmHeight-y-1)*bmpData.bmWidth+x];	// izokrene
			RGBA *tmp = &(*data)[(y)*bmpData.bmWidth+x];	// izokrene

			// ako je bitmapa bitna manje od 16, boju uzme iz palete
			if (bmpData.bmBitsPixel < 16) {
			        if (invertcolors) {
					    *(unsigned int *)tmp=~(*(unsigned int *)&pal[bits]);
					} else
					    *(unsigned int *)tmp= (*(unsigned int *)&pal[bits]);
			} else {
				// inaèe je boja dobra...
				if (invertcolors) {
    				tmp->r =	~GetRValue(bits);
	    			tmp->g =	~GetGValue(bits);
		    		tmp->b =	~GetBValue(bits);
		        } else {
    				tmp->r =	GetRValue(bits);
	    			tmp->g =	GetGValue(bits);
		    		tmp->b =	GetBValue(bits);
		        }
			}
			//tmp->a = 255;
			if ((!invertcolors && tmp->r == 255 && tmp->b == 255 && tmp->g==0)
                        ||
                (invertcolors && tmp->r == 0 && tmp->b == 0 && tmp->g == 255))
                    // ljubicasta je prozirna
                    tmp->a = 0;
                else
                    tmp->a = 255;
		}
	}

	free(bmpData.bmBits);	// bitmapa kao takva više nije potrebna jer imamo arej RGBA-ova
	if (palitems) free(pal);	// paleta takoðer ne treba

	// skopira bitmapdata ako je moguæe
	/*if (bd) {
		bmpData.bmBits = NULL;		// da ne bi netko malo pametniji išao to iskorištavati, a frijana je memorija...
		memcpy(bd, &bmpData, sizeof(BITMAP));
	}*/
	if (w) {
		*w = bmpData.bmWidth;
	}
	if (h) {
		*h = abs(bmpData.bmHeight);
	}

	return true;
}












#ifdef JPEGSUPPORT
///////////////////////ÐEJPEGLIB!!
#pragma comment(lib,"jpeglib.lib")

METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Always display the message. */
	MessageBox(NULL,buffer,"JPEG Fatal Error",MB_ICONSTOP);


	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

//
//	stash a scanline
//
void j_putRGBScanline(unsigned char *jpegline, int widthPix, unsigned char *outBuf, int row) {
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}

//
//	stash a gray scanline
//
void j_putGrayScanlineToRGB(unsigned char *jpegline, int widthPix, unsigned char *outBuf, int row) {
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {

		unsigned char iGray;

		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}

RGBArray JpegFileToRGB(FILE* infile, unsigned int *width, unsigned int *height) {

	// basic code from IJG Jpeg Code v6 example.c

	*width=0;
	*height=0;

	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct my_error_mgr jerr;
	/* More stuff */
/*	FILE * infile=NULL;*/		/* source file */

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
//	char buf[250];

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	if (!(infile /*= fopen(fileName, "rb")*/)) {
		return NULL;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_decompress(&cinfo);

		/*if (infile!=NULL)
			fclose(infile);*/
		return NULL;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/

	// get our buffer set to hold data
	unsigned char *dataBuf;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * sizeof(T_RGB));
	if (!dataBuf) {
		jpeg_destroy_decompress(&cinfo);
		//fclose(infile);
		return NULL;
	}

	// how big is this thing gonna be?
	*width = cinfo.output_width;
	*height = cinfo.output_height;

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */

		// asuumer all 3-components are RGBs
		if (cinfo.out_color_components==3) {

			j_putRGBScanline(buffer[0],
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		} else if (cinfo.out_color_components==1) {

			// assume all single component images are grayscale
			j_putGrayScanlineToRGB(buffer[0],
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		}

	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	//fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	return (RGBArray)dataBuf;
}

/*typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} RGBA;*/

bool LoadJPEGFromFile2RGBA(FILE* fjl, unsigned int *width, unsigned int *height, RGBA **data) {
	unsigned int w, h;

	RGBArray buf = JpegFileToRGB(fjl, &w, &h);
	if (!buf) {

     return false;
    }

	RGBA *rez = (RGBA*)malloc(w * h * sizeof(RGBA));
	for (unsigned int y=0;y<h;++y) {
		RGBA *trgba = &rez[(h-y-1)*w];
		T_RGB *trgb = &buf[(y)*w];
		for (unsigned int x=0;x<w;++x) {
		    if (invertcolors) {
		        trgba[x].r = ~trgb[x].red;
			    trgba[x].g = ~trgb[x].green;
			    trgba[x].b = ~trgb[x].blue;
			    trgba[x].a = 255;
			} else {
		        trgba[x].r = trgb[x].red;
			    trgba[x].g = trgb[x].green;
			    trgba[x].b = trgb[x].blue;
			    trgba[x].a = 255;
			}
		}
	}
	free(buf);

	if (data) *data = rez;
	if (width) *width = w;
	if (height) *height = h;

	return true;
}
#endif





