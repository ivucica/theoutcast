#include <windows.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>
#include "glutfont.h"
#include "gamemode.h"
#include "gm_mainmenu.h"
#include "glutwin.h"
#include "winfont.h"
#include "defines.h"
#include "database.h"
#include "sound.h"
#include "debugprint.h"

bool fullscreen = false;

// function predeclares
void GameInit();
int main(int argc, char** argv);


void GameInit() {

	glictFont* sysfont = glictCreateFont("system");
	/*
	sysfont->SetFontParam(GLUT_STROKE_MONO_ROMAN);
	sysfont->SetRenderFunc(glutxStrokeString);
	sysfont->SetSizeFunc(glutxStrokeSize);
	*/
	sysfont->SetFontParam(WinFontCreate("Arial", WINFONT_BOLD, 7));
	sysfont->SetRenderFunc(WinFontDraw);
	sysfont->SetSizeFunc(WinFontSize);

	GameModeEnter(GM_LOGO);

}

void GLInit() {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, .5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glictGlobals.clippingMode = GLICT_SCISSORTEST;

}

void NetInit() {
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





	printf("THE OUTCAST 0.3\n");
	printf("===============\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize (640, 480);
	//glutInitWindowPosition (0, 0);

    if (fullscreen) {
        glutGameModeString("640x480:32");
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering fullscreen\n");
        glut_WindowHandle = glutEnterGameMode();
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Done\n");
    } else {
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Creating window\n");
        glut_WindowHandle = glutCreateWindow (APPTITLE);
        glutSetWindow(glut_WindowHandle);
        glutShowWindow();
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Done\n");
    }

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading mousepointer\n");
	glut_SetMousePointer("mousepointer.bmp");

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up GL\n");
	GLInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up net\n");
	NetInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up database\n");
	DBInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up sound system\n");
	SoundInit(NULL);

	// game must be inited LAST.
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up game\n");
	GameInit();

	glutDisplayFunc(glut_Display);
	glutReshapeFunc(glut_Reshape);
	glutMouseFunc(glut_Mouse);
	glutIdleFunc(glut_Idle);
	glutPassiveMotionFunc(glut_PassiveMouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutTimerFunc(500, glut_FPS, 500);
	glutTimerFunc(1000, glut_MayAnimateToTrue, 0);
	glutKeyboardFunc(glut_Key);


	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering mainloop\n");
	glutMainLoop();

	return 0;
}
