#ifndef __BMPFONT_H
#define __BMPFONT_H


#include <GL/gl.h>

#include "texmgmt.h"


Texture *BMPFontCreate(const char* fontname, char size);
void BMPFontDelete(void* font);
float BMPFontSize(const char* txt, const void* fontvoid);
void BMPFontDraw(const char* txt, const void* fontvoid, float x, float y);

#endif
