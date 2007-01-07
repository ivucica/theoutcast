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
	if (game) delete game;
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

