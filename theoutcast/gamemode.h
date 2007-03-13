#ifndef __GAMEMODE_H
#define __GAMEMODE_H

#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
class GameMode {
	public:
		GameMode();
		~GameMode();

		virtual void Render();
		virtual void ResizeWindow();
		virtual void MouseClick (int button, int shift, int mousex, int mousey);
		virtual void KeyPress (unsigned char key, int x, int y);
		virtual void SpecKeyPress (int key, int x, int y);
};

enum gamemode_t {
    GM_LOGO = 0,
	GM_MAINMENU = 1,
	GM_GAMEWORLD = 2
};

void GameModeEnter(gamemode_t gamemode);

extern GameMode* game;
extern gamemode_t gamemode;
#endif
