#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <sstream>
#include "util.h"
#include "texmgmt.h"
#include "object.h"
#include "simple_effects.h"
extern int winw, winh;
extern int ptrx, ptry;
extern float fps;
extern bool mayanimate;

Texture* mousepointer=NULL;
Object *mousepointer_object;



void str_replace(std::string &s, const std::string& what, const std::string& with) {
    std::string::size_type p, l;
    for (l = 0; (p = s.find(what, l)) != std::string::npos; l = p + with.size()) {
        s.replace(p, what.size(), with);
    }
}


void NativeGUIError(const char* text, const char *title) {
	#ifdef WIN32
		MessageBox(HWND_DESKTOP, text, title, MB_ICONSTOP);
	#else
		std::string texts=text, titles=title;
		str_replace(texts, std::string("("), std::string("\\("));
		str_replace(texts, ")", "\\)");
		str_replace(texts, "\"", "\\\"");
		std::stringstream cmd;
		cmd << "xmessage -center \"" << title << "\n------------\n" << text << "\"";

		system(cmd.str().c_str());
	#endif
}


void RenderMouseCursor() {
	////////////////////////////CURSOR RENDERING/////////////////////
	static float cursoraniangle=0.;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.,winw,0.,winh);
	//glRotatef(180.0, 1.0, 0.0, 0.0);
	//glTranslatef(0,-winh,0.0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (!mousepointer_object && mousepointer) {
        mousepointer->Bind();
        glEnable(GL_TEXTURE_2D);

        //FlagEffect(ptrx-32., ptry-32., ptrx+32., ptry+32., 10, 10, cursoraniangle, 360., 2.	);
        StillEffect(ptrx-32., winh-ptry-32., ptrx+32., winh-ptry+32., 10, 10, false, false, true);

    } else if (mousepointer_object) {

        glPushMatrix();
			glTranslatef(ptrx - 16., winh-ptry - 16., 0);
			mousepointer_object->Render();
			mousepointer_object->AnimationAdvance(100./fps);
        glPopMatrix();

    }
	glDisable(GL_TEXTURE_2D);
	if (fps && mayanimate) cursoraniangle += 180. / fps;
	if (cursoraniangle > 360.) cursoraniangle -= 360.;

	//////////////////////END CURSOR RENDERING//////////////////////


}

