#include <string>
#include "gamemode.h"
#include "gm_mainmenu.h"
#include "gm_logo.h"
#include "gm_gameworld.h"
#include "glutwin.h"
#include "debugprint.h"
GameMode* game=NULL;
gamemode_t gamemode;
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

void GameModeEnter(gamemode_t gm) {
	DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "SWITCHING GAME MODES\n");
	if (game) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "First deleting old one ...\n");
	    // FIXME is there a way to "virtualise" destructor? meaning, when deleting ptr to parent class, i want to call child's destructor
	if (dynamic_cast<GM_Logo*>(game)) delete (GM_Logo*)game; else
		if (dynamic_cast<GM_MainMenu*>(game)) delete (GM_MainMenu*)game; else
		if (dynamic_cast<GM_Gameworld*>(game)) delete (GM_Gameworld*)game; else
		delete game;
	}

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
		default:
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Invalid gamemode %d.\n", gamemode);
			MessageBox(0, "Invalid gamemode\n", 0, 0);
			exit(1);
	}
	game->ResizeWindow();
	gamemode = gm;

}

