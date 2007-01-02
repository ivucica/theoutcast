#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/globals.h>
#include "gamemode.h"
#include "texmgmt.h"
#include "defines.h"
#include "simple_effects.h"
int winw=0, winh=0;
int ptrx=0, ptry=0;
Texture* mousepointer=NULL;
int frames=0;
float fps=0;
bool mayanimate=false;
float cursoraniangle=0.;
void glut_Display() {
	game->Render();

	frames++;
	glutSwapBuffers();
}

void glut_Reshape (int w, int h) {
	glViewport(0,0,w,h);
	glClearColor(0., 0., 0. , 1.);

	winw = w; winh = h;
	glictGlobals.w = w; glictGlobals.h = h;

	game->ResizeWindow();
}

void glut_Mouse (int button, int shift, int mousex, int mousey) {
	game->MouseClick (button, shift, mousex, mousey);
	glutPostRedisplay();
}

void glut_Idle () {
	#ifdef WIN32
	POINT mouse;
	GetCursorPos(&mouse);
	POINT offset;
	GetDCOrgEx(wglGetCurrentDC(), &offset);
	mouse.x -= offset.x;
	mouse.y -= offset.y;
	ptrx = mouse.x;
	ptry = mouse.y;
	#endif

	glutPostRedisplay();
}

void glut_PassiveMouse (int mousex, int mousey) {
	ptrx = mousex;
	ptry = mousey;
}
void glut_SetMousePointer(std::string texturefile) {
	if (mousepointer) delete mousepointer;
	mousepointer = new Texture(texturefile);
}
void glut_FPS (int param) {
	//fps = 1000./(float)param * frames;
	if (fps > 5.)
		fps = fps * ((1000./(float)param - 5)/(1000./(float)param)) + frames*5;
	else
		fps = fps * ((1000./(float)param - 1)/(1000./(float)param)) + frames;
	frames = 0;
	glutTimerFunc(100, glut_FPS, 100);

	char tmp[256];
	sprintf(tmp, "%s / FPS: %c%c%.02f", APPTITLE, fps <= 10.009 ? '<' : ' ', fps <= 10.009 ? '=' : ' ', fps);
	glutSetWindowTitle(tmp);

}

void glut_MayAnimateToTrue (int param) {
	mayanimate = true;
}

void glut_Key(unsigned char key, int x, int y) {
	game->KeyPress(key, x, y);
}

void RenderMouseCursor() {
	////////////////////////////CURSOR RENDERING/////////////////////
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.,winw,0.,winh);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);

	mousepointer->Bind();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	FlagEffect(ptrx-32., ptry-32., ptrx+32., ptry+32., 10, 10, cursoraniangle, 360., 2.	);

	glDisable(GL_TEXTURE_2D);
	if (fps && mayanimate) cursoraniangle += 180. / fps;
	if (cursoraniangle > 360.) cursoraniangle -= 360.;

	//////////////////////END CURSOR RENDERING//////////////////////


}
