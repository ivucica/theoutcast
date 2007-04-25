

#ifdef WIN32
	#include <windows.h>
#endif
#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>

#ifdef WIN32
    #include <GL/GRemdeyExtensions.h>
    PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY;
    HINSTANCE glinstance;
    HMODULE glmodule;
#endif

#include "gamemode.h"
#include "gm_mainmenu.h"
#include "glutwin.h"
#include "glutfont.h"
#include "options.h"
#include "skin.h"
#ifdef WIN32
	#include "winfont.h"
#endif
#include "defines.h"
#include "database.h"
#include "sound.h"
#include "debugprint.h"
#include "networkmessage.h" // FIXME remove me
#include "types.h"
version_t glversion;

#define WINFONT
bool fullscreen = false;

// function predeclares
void GameInit();
int main(int argc, char** argv);


void GameInit() {

	glictFont* sysfont = glictCreateFont("system");
	#if !defined(WIN32) || !defined(WINFONT)
		sysfont->SetFontParam(GLUT_STROKE_MONO_ROMAN);
		sysfont->SetRenderFunc(glutxStrokeString);
		sysfont->SetSizeFunc(glutxStrokeSize);
	#else
		//sysfont->SetFontParam(WinFontCreate("Arial", WINFONT_BOLD, 7));
		sysfont->SetFontParam(WinFontCreate("Tahoma", 0 , 7));
		sysfont->SetRenderFunc(WinFontDraw);
		sysfont->SetSizeFunc(WinFontSize);
	#endif

    if (options.intro)
		GameModeEnter(GM_LOGO);
	else
		GameModeEnter(GM_MAINMENU);
//	GameModeEnter(GM_SPRPLAYGROUND);

}



void GLInit() {
	//glClearColor(0., 0., 0., 0.);
	//glClearDepth(200.);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, .5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    	glEnable(GL_CULL_FACE);
	glictGlobals.clippingMode = GLICT_SCISSORTEST;


 #ifdef WIN32
	glinstance = LoadLibrary("opengl32.dll");
	glmodule = GetModuleHandle("opengl32.dll");
	glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) GetProcAddress(glmodule, "glStringMarkerGREMEDY");
	glictGlobals.debugCallback = (GLICTDEBUGCALLBACKPROC)DEBUGMARKER;
#endif


    {

        int x=0,y=0,z=0;
        sscanf((char*)glGetString( GL_VERSION ), "%d.%d.%d", &x, &y, &z);
        glversion.major = x; glversion.minor = y; glversion.revision = z;

    }

    TextureInit();
}

void NetInit() {
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 0 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		MessageBox(HWND_DESKTOP,"You do not have the Windows Sockets. Please update your operating system.","Cannot start network", MB_ICONSTOP);
		return;
	}

	/* Confirm that the WinSock DLL supports 2.0.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */

	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 0 ) {
			MessageBox(HWND_DESKTOP,"You do not have the Windows Sockets DLL that can provide 2.0 functionality. Please update your operating system.","Cannot start network", MB_ICONSTOP);
			WSACleanup( );
		return;
	}

	/* The WinSock DLL is acceptable. Proceed. */
#endif
}



int main(int argc, char** argv) {


#ifdef DEBUGCONSOLE
if(AllocConsole())
{
    freopen("CONOUT$", "wt", stdout);

    SetConsoleTitle("The Outcast : Debug Console");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

}
#endif


	DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "THE OUTCAST 0.3\n");
	DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "===============\n");


	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up net\n");
	NetInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up database\n");
	DBInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up sound system\n");
	SoundInit(NULL);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	//glutInitWindowPosition (0, 0);

    options.Load();

    fullscreen_retry:
    if (options.fullscreen) {
        glutGameModeString("800x600:32");
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



	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up GL\n");
	GLInit();

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading skin\n");
	skin.Load(options.skin.c_str());


	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading mousepointer\n");
	glut_SetMousePointer("DEFAULT");

	// game must be inited LAST.
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up game\n");
	GameInit();



    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up callbacks\n");

	glutDisplayFunc(glut_Display);
	glutReshapeFunc(glut_Reshape);
	glutMouseFunc(glut_Mouse);
	glutIdleFunc(glut_Idle);
	glutPassiveMotionFunc(glut_PassiveMouse);
	glutTimerFunc(1000, glut_FPS, 1000);
	glutTimerFunc(1000, glut_MayAnimateToTrue, 0);
	glutSpecialFunc(glut_SpecKey);
	glutKeyboardFunc(glut_Key);



    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Good to go, proceed\n");
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering mainloop\n");
	glutMainLoop();

	return 0;
}
