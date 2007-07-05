#include <string>
#include "gamemode.h"
#include "gm_mainmenu.h"
#include "gm_logo.h"
#include "gm_gameworld.h"
#include "gm_charmgr.h"
#include "gm_sprplayground.h"
#include "windowing.h"
#include "debugprint.h"
GameMode* game=NULL;
gamemode_t gamemode;

ONCriticalSection gmthreadsafe;

GameMode::GameMode() {
}

GameMode::~GameMode() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destructing game\n");
}

void GameMode::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void GameMode::ResizeWindow() {
}

void GameMode::MouseClick (int button, int shift, int mousex, int mousey) {
}

void GameMode::KeyPress (unsigned char key, int x, int y) {

}
void GameMode::SpecKeyPress (int key, int x, int y) {

}
void GameModeInit() {
    ONInitThreadSafe(gmthreadsafe);
}
void GameModeDeinit() {
    ONDeinitThreadSafe(gmthreadsafe);

    if (dynamic_cast<GM_Logo*>(game)) delete (GM_Logo*)game; else
		if (dynamic_cast<GM_MainMenu*>(game)) delete (GM_MainMenu*)game; else
		if (dynamic_cast<GM_Gameworld*>(game)) delete (GM_Gameworld*)game; else
		if (dynamic_cast<GM_CharMgr*>(game)) delete (GM_CharMgr*)game; else
		if (dynamic_cast<GM_SPRPlayground*>(game)) delete (GM_SPRPlayground*)game; else
		delete game;

}


void GameModeEnter(gamemode_t gm) {
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "SWITCHING GAME MODES\n");
	//ONThreadSafe(gmthreadsafe);
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Thread safe\n");
	if (game) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "First deleting old one ...\n");
	    // FIXME (Khaos#1#) is there a way to "virtualise" destructor? meaning, when deleting ptr to parent class, i want to call child's destructor
	if (dynamic_cast<GM_Logo*>(game)) delete (GM_Logo*)game; else
		if (dynamic_cast<GM_MainMenu*>(game)) delete (GM_MainMenu*)game; else
		if (dynamic_cast<GM_Gameworld*>(game)) delete (GM_Gameworld*)game; else
		if (dynamic_cast<GM_CharMgr*>(game)) delete (GM_CharMgr*)game; else
		if (dynamic_cast<GM_SPRPlayground*>(game)) delete (GM_SPRPlayground*)game; else
		delete game;
	}

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Forming new gamemode\n");

	switch (gm) {

		case GM_LOGO:
			game = new GM_Logo;
			break;
		case GM_MAINMENU:
			game = new GM_MainMenu;
			break;
		case GM_GAMEWORLD:
			game = new GM_Gameworld;
			break;
        case GM_CHARMGR:
            game = new GM_CharMgr;
            break;

        case GM_SPRPLAYGROUND:
            game = new GM_SPRPlayground;
            break;
		default:
			printf("GAMEMODE INVALID\n");
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Invalid gamemode %d.\n", gamemode);

			//MessageBox(0, "Invalid gamemode\n", 0, 0);
			exit(1);
	}

	game->ResizeWindow();
	gamemode = gm;
	mayanimate = false;
	win_Timer(500, win_MayAnimateToTrue, 0);
//    ONThreadUnsafe(gmthreadsafe);
}

