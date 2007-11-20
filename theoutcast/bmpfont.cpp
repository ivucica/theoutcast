#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <stdio.h>
#include "bmpfont.h"

Texture *BMPFontCreate(const char* fontname, char size) {
    Texture *t = new Texture(fontname);

    return t;
}

void BMPFontDelete(void* fontvoid) {
	if (fontvoid) {
		Texture *font = (Texture*)fontvoid;
		delete font;
	}
}

static void BMPFontDrawChar(char t, Texture *tex) {
    register float x,y;

    //printf("%c", t);
    t -= 32;
    x = (int)(t % 32)*16.;
    y = (int)(t / 32)*16.;

    x /= 512.;
    y /= 128.;

    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);
        glTexCoord2f(x,y); glVertex2f(0, 0);
        glTexCoord2f(x + 8./512., y); glVertex2f(.7,0);
        glTexCoord2f(x + 8./512., y + 12./128.); glVertex2f(.7, 1.2);
        glTexCoord2f(x, y + 12./128.); glVertex2f(0, 1.2);
    glEnd();

    glTranslatef(0.7,0,0);
}
void BMPFontDraw(const char* txt, const void* fontvoid, float x, float y) {
    Texture* font = (Texture*)fontvoid;

    glMatrixMode(GL_MODELVIEW);
    glTranslatef(x,y,0);

    glEnable(GL_TEXTURE_2D);
    font->Bind();
    glColor4f(1.,1.,1.,1.);
    volatile register float sizesofar = 0.;
	volatile register float linessofar = 0.;
	for (volatile register unsigned char *t = (unsigned char*)txt; *t; ++t) {
		switch (*t) {
			default:
				BMPFontDrawChar(*t, font);

				sizesofar += 0.7;
				break;
			case '\n':
			case '\r':
				glTranslatef(-sizesofar, 1. ,0);
				linessofar += 1.;
				sizesofar = 0;
				//printf("\n");
				if (*t == '\n' && *(t+1)=='\r' || *t == '\r' && *(t+1)=='\n' ) t++;
                break;

		}
	}
	//printf("\n");
    //glTranslatef(-sizesofar, linessofar ,0);

    glTranslatef(-sizesofar, -linessofar ,0);
    glTranslatef(-x, -y, 0);

    glDisable(GL_TEXTURE_2D);


}


float BMPFontSize(const char* text, const void* fontvoid) {
	//Texture* font = (Texture*)fontvoid;

	float length=0.;
    for (char *t=(char*)text;*t;++t)	// Loop To Find Text Length
	{
		length+=.7;		// Increase Length By Each Characters Width
	}
	return length;
}
