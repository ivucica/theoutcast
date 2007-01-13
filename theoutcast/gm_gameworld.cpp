#include <GL/glut.h>
#include "gm_gameworld.h"
#include "items.h"
#include "console.h"
GM_Gameworld::GM_Gameworld() {

}


GM_Gameworld::~GM_Gameworld() {
}

void GM_Gameworld::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 0, 480);
    console.draw(10);
}

void GM_Gameworld::KeyPress (unsigned char key, int x, int y) {
    console.insert("Entering mainmenu\n");
    printf("Entering mainmenu\n");
    GameModeEnter(GM_MAINMENU);
}
