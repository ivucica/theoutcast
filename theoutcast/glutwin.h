#include <string>
#include "object.h"
void glut_Display();
void glut_Reshape (int w, int h);
void glut_Mouse (int button, int shift, int mousex, int mousey);
void glut_Idle ();
void glut_PassiveMouse (int mousex, int mousey);
void glut_SetMousePointer(std::string texturefile);
void glut_SetMousePointer(Object *obj);
void glut_FPS (int param);
void glut_MayAnimateToTrue(int param);
void glut_Key(unsigned char key, int x, int y);
void glut_SpecKey(int key, int x, int y);
void glut_Init(int *argc, char**argv);
void glut_CreateDisplay();
void glut_MainLoop();
extern Object *mousepointer_object;
extern int winw, winh;
extern float fps;
extern bool mayanimate;
extern int glut_WindowHandle;

// FIXME (Khaos#1#) To be moved
void RenderMouseCursor();


#define win_Display glut_Display
#define win_Reshape glut_Reshape
#define win_Mouse glut_Mouse
#define win_Idle glut_Idle
#define win_PassiveMouse glut_PassiveMouse
#define win_SetMousePointer glut_SetMousePointer
#define win_FPS glut_FPS
#define win_MayAnimateToTrue glut_MayAnimateToTrue
#define win_Key glut_Key
#define win_SpecKey glut_SpecKey
#define win_Init glut_Init
#define win_CreateDisplay glut_CreateDisplay
#define win_MainLoop glut_MainLoop

#define win_Timer glutTimerFunc
#define win_GetModifiers glutGetModifiers

#define WIN_PRESS GLUT_DOWN
#define WIN_RELEASE GLUT_UP


#define WIN_KEY_UP GLUT_KEY_UP
#define WIN_KEY_DOWN GLUT_KEY_DOWN
#define WIN_KEY_LEFT GLUT_KEY_LEFT
#define WIN_KEY_RIGHT GLUT_KEY_RIGHT
#define WIN_KEY_HOME GLUT_KEY_HOME
#define WIN_KEY_END GLUT_KEY_END
#define WIN_KEY_PAGE_DOWN GLUT_KEY_PAGE_DOWN
#define WIN_KEY_PAGE_UP GLUT_KEY_PAGE_UP




#define WIN_ACTIVE_CTRL GLUT_ACTIVE_CTRL
#define WIN_ACTIVE_ALT GLUT_ACTIVE_ALT
#define WIN_ACTIVE_SHIFT GLUT_ACTIVE_SHIFT

