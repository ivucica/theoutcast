#include <string>
#include <SDL/SDL_keysym.h>
#include "object.h"
void sdlw_Display();
void sdlw_Reshape (int w, int h);
void sdlw_Mouse (int button, int shift, int mousex, int mousey);
void sdlw_Idle ();
void sdlw_PassiveMouse (int mousex, int mousey);
void sdlw_SetMousePointer(std::string texturefile);
void sdlw_SetMousePointer(Object *obj);
void sdlw_FPS (int param);
void sdlw_MayAnimateToTrue(int param);
void sdlw_Key(unsigned char key, int x, int y);
void sdlw_SpecKey(int key, int x, int y);
void sdlw_Init(int *argc, char**argv);
void sdlw_CreateDisplay();
void sdlw_MainLoop();
void sdlw_Timer(int ms, void(*func)(int) , int arg);
extern Object *mousepointer_object;
extern int winw, winh;
extern float fps;
extern bool mayanimate;
extern int sdl_WindowHandle;

// FIXME (Khaos#1#) To be moved
void RenderMouseCursor();


#define win_Display sdlw_Display
#define win_Reshape sdlw_Reshape
#define win_Mouse sdlw_Mouse
#define win_Idle sdlw_Idle
#define win_PassiveMouse sdlw_PassiveMouse
#define win_SetMousePointer sdlw_SetMousePointer
#define win_FPS sdlw_FPS
#define win_MayAnimateToTrue sdlw_MayAnimateToTrue
#define win_Key sdlw_Key
#define win_SpecKey sdlw_SpecKey
#define win_Init sdlw_Init
#define win_CreateDisplay sdlw_CreateDisplay
#define win_MainLoop sdlw_MainLoop
#define win_Timer sdlw_Timer

#define WIN_PRESS 0
#define WIN_RELEASE 1

#define WIN_KEY_UP SDLK_UP
#define WIN_KEY_DOWN SDLK_DOWN
#define WIN_KEY_LEFT SDLK_LEFT
#define WIN_KEY_RIGHT SDLK_RIGHT
#define WIN_KEY_HOME SDLK_HOME
#define WIN_KEY_END SDLK_END
#define WIN_KEY_PAGE_DOWN SDLK_PAGEDOWN
#define WIN_KEY_PAGE_UP SDLK_PAGEUP




#define WIN_ACTIVE_CTRL 1
#define WIN_ACTIVE_ALT 2
#define WIN_ACTIVE_SHIFT 4




