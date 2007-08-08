#ifndef __UTIL_H
#define __UTIL_H

void NativeGUIError(const char* text, const char *title);
void RenderMouseCursor();

class Texture; class Object;
extern Texture* mousepointer;
extern Object *mousepointer_object;

#endif
