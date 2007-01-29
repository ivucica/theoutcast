#include <string>
#include "gamemode.h"
#include "gm_mainmenu.h"
#include "gm_logo.h"
#include "gm_gameworld.h"
#include "glutwin.h"
GameMode* game=NULL;

GameMode::GameMode() {
}

GameMode::~GameMode() {
    printf("Destructing game\n");
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

void GameModeEnter(gamemode_t gamemode) {
	printf("SWITCHING GAME MODES\n");
	if (game) {
		printf("First deleting old one ...\n");
	    // FIXME is there a way to "virtualise" destructor? meaning, when deleting ptr to parent class, i want to call child's destructor
	if (dynamic_cast<GM_Logo*>(game)) delete (GM_Logo*)game; else
		if (dynamic_cast<GM_MainMenu*>(game)) delete (GM_MainMenu*)game; else
		if (dynamic_cast<GM_Gameworld*>(game)) delete (GM_Gameworld*)game; else
		delete game;
	}

	switch (gamemode) {
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
			printf("Invalid gamemode.\n");
			MessageBox(0, "Invalid gamemode\n", 0, 0);
			exit(1);
	}
	game->ResizeWindow();

}

