

#ifdef WIN32
	#include <windows.h>
#endif
#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>

#ifdef WIN32
	#ifdef USEGREMEDY
		#include <GL/GRemdeyExtensions.h>
		PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY;
		HINSTANCE glinstance;
		HMODULE glmodule;
	#endif
#endif

#include "gamemode.h"
#include "gm_mainmenu.h"

#include "windowing.h"

#include "options.h"
#include "skin.h"
#include "glutfont.h"
#include "bmpfont.h"
#ifdef WIN32
	#include "winfont.h"
#endif
#include "defines.h"
#include "database.h"
#include "sound.h"
#include "debugprint.h"

#include "types.h"

#include "assert.h"

#include "util.h"

version_t glversion;


bool fullscreen = false;

bool sprplayground = false;

glictFont* sysfont = NULL;
Texture *fonttexture = NULL;

// function predeclares
void GameInit();
void ObjSprInit();
void ObjSprDeinit();
int main(int argc, char** argv);


void GameInit() {

	sysfont = glictCreateFont("system");
	#if (!defined(WINFONT) && !defined(BMPFONT))
        #ifndef USEGLUT
        #error If you dont use GLUT, you need to use either WINFONT or BMPFONT
		#else
		sysfont->SetFontParam(GLUT_STROKE_MONO_ROMAN);
		sysfont->SetRenderFunc(glutxStrokeString);
		sysfont->SetSizeFunc(glutxStrokeSize);
		#endif
	#elif defined(BMPFONT)
        sysfont->SetFontParam(fonttexture = BMPFontCreate("fontbordered.bmp", 8));
        sysfont->SetRenderFunc(BMPFontDraw);
        sysfont->SetSizeFunc(BMPFontSize);
	#else // winfont is defined
		//sysfont->SetFontParam(WinFontCreate("Arial", WINFONT_BOLD, 7));
		sysfont->SetFontParam(WinFontCreate("Tahoma", 0 , 7));
		sysfont->SetRenderFunc(WinFontDraw);
		sysfont->SetSizeFunc(WinFontSize);
    #endif


    ObjSprInit();
	GameModeInit();

    if (!sprplayground)
        if (options.intro)
            GameModeEnter(GM_LOGO);
        else
            GameModeEnter(GM_MAINMENU);
    else
        GameModeEnter(GM_SPRPLAYGROUND);

}

void GameDeinit() {
    ObjSprDeinit();
    glictDeleteFont("system");
    delete fonttexture;
    GameModeDeinit();
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
	#ifdef USEGREMEDY
		glinstance = LoadLibrary("opengl32.dll");
		glmodule = GetModuleHandle("opengl32.dll");
		glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) GetProcAddress(glmodule, "glStringMarkerGREMEDY");
		glictGlobals.debugCallback = (GLICTDEBUGCALLBACKPROC)DEBUGMARKER;
	#endif
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


void OnExit(int exitcondition, void* arg) {
    GameDeinit();
	delete skin;
	delete game;

    printf("Remaining textures: \n");
    TextureReportRemaining();
    printf("Expunging remaining textures: \n");
    TextureExpungeRemaining();

    printf("GAME DEINIT SUCCESSFUL!\n");
}
void AtExit() {
    OnExit(0, NULL);
}
#include <math.h> // remove me
int main(int argc, char** argv) {


#ifdef DEBUGCONSOLE
if(AllocConsole())
{
    freopen("CONOUT$", "wt", stdout);

    SetConsoleTitle("The Outcast : Debug Console");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

}
#endif




	//TextureIntegrityTest();

	DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "THE OUTCAST 0.4\n");
	DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "===============\n");


    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "Reading cmd line arguments\n");

    for (int i=1;i<argc;i++) {
        if (!strcmp(argv[i], "sprplayground")) {
        	sprplayground = true;
        }
        #ifndef WIN32
        // linux only arguments:
        if (!strcmp(argv[i], "softwarerenderer")) {
            setenv("LIBGL_ALWAYS_INDIRECT", "1", 1);
        }
        #endif
    }

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up net\n");
	NetInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up database\n");
	DBInit();
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up sound system\n");
	SoundInit(NULL);

	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up windowing system\n");

    win_Init(&argc, argv);
    options.Load();

    win_CreateDisplay();



	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up GL\n");
	GLInit();

	if (!sprplayground) {
	    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading skin\n");
	    skin = new Skin;
        skin->Load(options.skin.c_str());
	}


	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Loading mousepointer\n");
	win_SetMousePointer("DEFAULT");

	// game must be inited LAST.
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Setting up game\n");
	GameInit();



    win_MainLoop();

	return 0;
}
