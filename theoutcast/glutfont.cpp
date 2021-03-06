// objnet glut helper
// (c) 2005 object networks
//
// contains some helper code for glut
// e.g. entire string printing

#include <GL/glut.h>
#include <string.h>

char* glutxStrokeStringExpert(const char* txt, const void* font) {
    glPushMatrix();

		char *p;
		for (p = (char*)txt; *p && *p!='\n'; p++) {
			glutStrokeCharacter((void*)font,*p);
		}

    glPopMatrix();
    return p;
}
#include <stdio.h>
void glutxStrokeString(const char* txt, const void* font, float x, float y) {
    char* fromwhere=(char*)txt;
    do {
        if (fromwhere!=txt) fromwhere++;
        glPushMatrix();
			glRotatef(180.,1.,0.,0.);
			glTranslatef(x,-y - 1,0);
			//if (font==GLUT_STROKE_ROMAN)
			//    glScalef(0.1,0.075,0.075);
			//else
			//    glScalef(0.075,0.075,0.075);
			glScalef(.0075, .0075, .0075);
			fromwhere = glutxStrokeStringExpert(fromwhere,font);
			glScalef(1/.0075, 1/.0075, 1/.0075);
			glTranslatef(-x,y + 1,0);
			glRotatef(180.,1.,0.,0.);

		glPopMatrix();
        y += 1;
    } while (*fromwhere);
}

void glutxBitmapString(char* txt, const void* font,int x,int y) {
    int len, i;



    glRasterPos2f(x, y);
    len = (int) strlen(txt);
    for (i = 0; i < len; i++) {
      glutBitmapCharacter((void*)font, txt[i]);
    }

}
float glutxBitmapSize(char* txt, const void* font) {
    int size=0, len=strlen(txt);
    for (int i=0;i<len;i++) {
        size+=glutBitmapWidth((void*)font,txt[i]);
    }
    return (float)size;
}


float glutxStrokeSize(const char* txt, const void* font) {



    int size=0, len=strlen(txt);
    int maxsize=0;
    for (int i=0;i<len;i++) {
        size+=glutStrokeWidth((void*)font,txt[i]);
        if (txt[i]=='\n') {
            if (size>maxsize) maxsize=size;
            size=0;
        }
    }
    if (size>maxsize) maxsize=size;
    size = maxsize;


    // scale it down appropriately (like we've done before)
    //if (font != GLUT_STROKE_ROMAN)
    //    return (float)size*0.075;
    //else
    //    return (float)size*0.1;//0.12
    return (float)size * .0075;

}
int glutxNumberOfLines(const char* txt) {
    int count=1; // at least 1 line
    //DEBUGPRINT("NUMBEROFLINES: ");
    for (unsigned int i=0;i<strlen(txt);i++) {
        if (txt[i]=='\n') {
            count++;
        } //else DEBUGPRINT("%d ",txt[i]);
    }

    return count;
}
