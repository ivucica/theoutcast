/* Need to make it so the second chunk gets properly decoded with 0,1,8,2 xtea

00000000  85 00                                            ..
00000002  0a 02 00 18 03 51 9a 1d  b7 e9 ad b5 9b 49 e8 26 .....Q.. .....I.&
00000012  a2 6d b0 dc 88 bb 98 f4  84 8d 19 a4 99 1f e7 1c .m...... ........
00000022  d8 99 89 c0 4c 70 f7 85  3f 90 ca a8 ae d1 5e 15 ....Lp.. ?.....^.
00000032  df 6e f8 03 76 ed fe 6b  b4 3b e2 2c 1a 93 59 3b .n..v..k .;.,..Y;
00000042  1b 97 c2 30 1e c4 98 95  ce 9b c2 45 bd d8 02 7f ...0.... ...E....
00000052  ce b7 13 ce 14 26 c9 cd  d4 63 1a 5e 69 3b 02 55 .....&.. .c.^i;.U
00000062  da 0a 86 dc 0d ce fd 89  d8 09 c7 b4 3e f2 00 cb ........ ....>...
00000072  d4 e2 99 30 f8 ca f7 2b  de 01 ca 7b 99 75 59 88 ...0...+ ...{.uY.
00000082  02 99 e2 47 71                                   ...Gq
                                                                              00000000  28 00 b1 81 07 e5 22 65  62 49 a2 33 3b 61 0c 08 (....."e bI.3;a..
                                                                              00000010  06 45 22 7e d9 7a 1f 6f  de a0 75 56 a8 f6 af 72 .E"~.z.o ..uV...r
                                                                              00000020  8b 72 af 7b 9b dd c7 8d  91 65                   .r.{.... .e
 */


#ifdef WIN32
	#include <windows.h>
#endif
#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>

#include <GL/GRemdeyExtensions.h>
PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY;
HINSTANCE glinstance;
HMODULE glmodule;

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
		sysfont->SetFontParam(WinFontCreate("Arial", WINFONT_BOLD, 7));
		sysfont->SetRenderFunc(WinFontDraw);
		sysfont->SetSizeFunc(WinFontSize);
	#endif

    GameModeEnter(GM_LOGO);
	//GameModeEnter(GM_MAINMENU);
	//GameModeEnter(GM_SPRPLAYGROUND);

}


void GLInit() {

    // FIXME gotta fix

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, .5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
//	glictGlobals.clippingMode = GLICT_SCISSORTEST;


	glinstance = LoadLibrary("opengl32.dll");
	glmodule = GetModuleHandle("opengl32.dll");
	glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) GetProcAddress(glmodule, "glStringMarkerGREMEDY");
	glictGlobals.debugCallback = (GLICTDEBUGCALLBACKPROC)DEBUGMARKER;

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


	printf("THE OUTCAST 0.3\n");
	printf("===============\n");


	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up net\n");
	NetInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up database\n");
	DBInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up sound system\n");
	SoundInit(NULL);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize (640, 480);
	//glutInitWindowPosition (0, 0);

    options.Load();

    if (options.fullscreen) {
        glutGameModeString("800x600:32");
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

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up GL\n");
	GLInit();

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading mousepointer\n");
	glut_SetMousePointer("mousepointer.bmp");

	// game must be inited LAST.
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up game\n");
	GameInit();

    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up callbacks\n");

	glutDisplayFunc(glut_Display);
	glutReshapeFunc(glut_Reshape);
	glutMouseFunc(glut_Mouse);
	glutIdleFunc(glut_Idle);
	glutPassiveMotionFunc(glut_PassiveMouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutTimerFunc(500, glut_FPS, 500);
	glutTimerFunc(1000, glut_MayAnimateToTrue, 0);
	glutSpecialFunc(glut_SpecKey);
	glutKeyboardFunc(glut_Key);

    skin.Load("default");

    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Good to go, proceed\n");
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Entering mainloop\n");
	glutMainLoop();

	return 0;
}
