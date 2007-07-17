#include <time.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/globals.h>
#include <math.h>

#include "windowing.h"
#include "gamemode.h"
#include "texmgmt.h"
#include "defines.h"
#include "options.h"
#include "threads.h"
#include "debugprint.h"
#include "simple_effects.h"
#ifdef _MSC_VER
    #include <float.h>
    #define isnan _isnan
    #define isinf _isnan
#endif


int winw=0, winh=0;
int ptrx=0, ptry=0;
Texture* mousepointer=NULL;
int frames=0;
clock_t lasttime;
float fps=0;
bool mayanimate=false;
float cursoraniangle=0.;
int glut_WindowHandle;
Object *mousepointer_object;

extern ONCriticalSection gmthreadsafe;

//#define FPSMETHOD1
#define FPSMETHOD2
//#define FPSMETHOD3

extern int texcount;
void glut_FPS(int param);


void OnExit(int exitcondition, void* arg);
void AtExit();

void glut_Display() {
    ONThreadSafe(gmthreadsafe);
	game->Render();
	ONThreadUnsafe(gmthreadsafe);

	frames++;
//	glut_FPS(0);
	glutSwapBuffers();
}

void glut_Reshape (int w, int h) {
	glViewport(0,0,w,h);
	glClearColor(0., 0., 0. , 1.);

	winw = w; winh = h;
	glictGlobals.w = w; glictGlobals.h = h;

    ONThreadSafe(gmthreadsafe);
	game->ResizeWindow();
	ONThreadUnsafe(gmthreadsafe);
}

void glut_Mouse (int button, int shift, int mousex, int mousey) {
    ONThreadSafe(gmthreadsafe);
	game->MouseClick (button, shift, mousex, mousey);
	ONThreadUnsafe(gmthreadsafe);
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
	#else
/*
        //printf("Getting root window.\n");
        Window root_win = RootWindow(display,SCREEN);


        int root_x_return, root_y_return;
        int win_x_return, win_y_return;

        //printf("Getting mouse position.");
        bool xqueryResult = XQueryPointer(display, root_win, &root_return, &child_return, &root_x_return, &root_y_return, &win_x_return,
    &win_y_return, &mask_return);

        printf("%d %d ; %d %d\n", root_x_return, root_y_return, win_x_return, win_y_return);
*/
	#endif

	glutPostRedisplay();
}

void glut_PassiveMouse (int mousex, int mousey) {
	ptrx = mousex;
	ptry = mousey;
}
void glut_SetMousePointer(std::string texturefile) {
	ONThreadSafe(gmthreadsafe);
    if (mousepointer_object) {
        delete mousepointer_object;
        mousepointer_object = NULL;
    }
	if (mousepointer) delete mousepointer;

	if (texturefile=="DEFAULT") {
		if (options.os_cursor) texturefile = "WINDOWS"; else texturefile = "mousepointer.bmp";
	}

	if (texturefile == "WINDOWS") {
		glutSetCursor(GLUT_CURSOR_INHERIT );
		mousepointer = NULL;
	}
	else {
		glutSetCursor(GLUT_CURSOR_NONE);
		mousepointer = new Texture(texturefile);
	}
	ONThreadUnsafe(gmthreadsafe);
}
void glut_SetMousePointer(Object *obj) {
	ONThreadSafe(gmthreadsafe);
	if (mousepointer_object) {
	    delete mousepointer_object;
	}
	mousepointer_object = obj;
	#ifdef WIN32
	glutSetCursor(GLUT_CURSOR_NONE);
	#endif
	ONThreadUnsafe(gmthreadsafe);
}

void glut_FPS (int param) {


	#ifdef FPSMETHOD1
    /*
    // Method 1
    // Every FRAMECONST frames, calculate FPS
    */

    #define FRAMECONST 14
    //printf("clock ticks: %d, frames: %d\n", clock(), frames);
    if (frames == FRAMECONST) {
        fps = (float)FRAMECONST / ((clock() - lasttime)/1000.);
        lasttime = clock();
        frames = 0;
    }

	#endif

	#ifdef FPSMETHOD2

	// Method 2
	// Every 1 second, calculate FPS
	glutTimerFunc(1000, glut_FPS, 1000);
	fps = 1000./(float)param * frames;
	frames = 0;

	#endif


	#ifdef FPSMETHOD3

    // Method 3
    // Every arbitrary num of seconds, calculate FPS
    glutTimerFunc(100, glut_FPS, 100);
	if (fps > 5.)
		fps = fps * ((1000./(float)param - 5)/(1000./(float)param)) + frames*5;
	else
		fps = fps * ((1000./(float)param - 1)/(1000./(float)param)) + frames;



    frames = 0;


	#endif



	if (isnan(fps) || isinf(fps)) {
		printf("FPS is NAN!!\n");
		fps = 5.;
	}


	char tmp[256];
	sprintf(tmp, "%s / FPS: %c%c%.02f, TexCount: %d", APPTITLE, fps <= 10.009 ? '<' : ' ', fps <= 10.009 ? '=' : ' ', fps, texcount);
	glutSetWindowTitle(tmp);

}

void glut_MayAnimateToTrue (int param) {
	mayanimate = true;
}

void glut_Key(unsigned char key, int x, int y) {
    ONThreadSafe(gmthreadsafe);
	game->KeyPress(key, x, y);
	ONThreadUnsafe(gmthreadsafe);
}

void glut_SpecKey(int key, int x, int y) {
    ONThreadSafe(gmthreadsafe);
    game->SpecKeyPress(key, x, y);
    ONThreadUnsafe(gmthreadsafe);
}


// FIXME (Khaos#1#) Move into more appropriate file (that'll apply for both sdl and glut)
void RenderMouseCursor() {
	////////////////////////////CURSOR RENDERING/////////////////////
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



void glut_Init(int *argc, char**argv) {

    glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (640, 480);

	glutInitWindowPosition (200, 100); //FIXME remove this

}

void glut_CreateDisplay() {
    fullscreen_retry:
    if (options.fullscreen) {
        glutGameModeString("320x240:32");
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering fullscreen\n");
        glut_WindowHandle = glutEnterGameMode();
	if (!glut_WindowHandle) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_ERROR, "There was an error entering fullscreen. Retrying in windowed mode.\n");
		options.fullscreen = false;
		options.Save();
		goto fullscreen_retry;
	}
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Done\n");
    } else {
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Creating window\n");
        glut_WindowHandle = glutCreateWindow (APPTITLE);
        glutSetWindow(glut_WindowHandle);
        glutShowWindow();
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Done\n");
    }

	if (!glut_WindowHandle) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "There was a problem initializing OpenGL render area. Giving up and exiting.\n");
		exit(1);
	}

}
void glut_MainLoop() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up callbacks\n");

	glutDisplayFunc(glut_Display);
	glutReshapeFunc(glut_Reshape);
	glutMouseFunc(glut_Mouse);
	glutIdleFunc(glut_Idle);
	glutPassiveMotionFunc(glut_PassiveMouse);
	glutSpecialFunc(glut_SpecKey);
	glutKeyboardFunc(glut_Key);
	glutTimerFunc(1000, glut_FPS, 1000);
	glutTimerFunc(1000, glut_MayAnimateToTrue, 0);

    #ifndef WIN32
        on_exit(OnExit, NULL);
    #else
        atexit(AtExit);
    #endif

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering mainloop\n");
	glutMainLoop();
}

