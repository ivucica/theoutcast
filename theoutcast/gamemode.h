#ifndef __GAMEMODE_H
#define __GAMEMODE_H

#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
class GameMode {
	public:
		GameMode();
		virtual ~GameMode();

		virtual void Render();
		virtual void ResizeWindow();
		virtual void MouseClick (int button, int shift, int mousex, int mousey);
		virtual void KeyPress (unsigned char key, int x, int y);
		virtual void SpecKeyPress (int key, int x, int y);
};

enum gamemode_t {

    GM_LOGO = 0,
	GM_MAINMENU,
	GM_GAMEWORLD,
	GM_CHARMGR,

	GM_SPRPLAYGROUND = 9000
};

void GameModeEnter(gamemode_t gamemode);
void GameModeInit();
void GameModeDeinit();
extern GameMode* game;
extern gamemode_t gamemode;
#endif
