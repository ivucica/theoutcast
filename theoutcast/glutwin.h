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
void RenderMouseCursor();
extern Object *mousepointer_object;
extern int winw, winh;
extern float fps;
extern bool mayanimate;
extern int glut_WindowHandle;
