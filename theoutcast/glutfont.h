// Glut-helper.cpp
// contains glut helper

// objnet glut helper
// (c) 2005 object networks
//
// contains some helper code for glut
// e.g. entire string printing

#ifndef __GLUTFONT_H
#define __GLUTFONT_H
void glutxStrokeString(const char* txt, const void* font, float x, float y) ;
char* glutxStrokeStringExpert(const char* txt, const void* font);
void glutxBitmapString(char* txt, const void* font,int x, int y) ;
float glutxBitmapSize(char* txt, const void* font);
float glutxStrokeSize(const char* txt, const void* font);
int glutxNumberOfLines(const char* txt);
#endif
