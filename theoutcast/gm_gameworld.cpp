#include <GL/glut.h>
#include "gm_gameworld.h"
#include "items.h"
#include "console.h"
#include "objspr.h"
#include "sprfmts.h"
#include "glutwin.h"
GM_Gameworld::GM_Gameworld() {
    SPRLoader("tibia76.spr");

    g = new ObjSpr(1952);
}


GM_Gameworld::~GM_Gameworld() {
    printf("Destructing gameworld\n");
    SPRUnloader();
}

void GM_Gameworld::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 0, 480);
    console.draw(10);
    glPushMatrix();
    //glTranslatef(200, 200, 0);
    g->Render();
    glPopMatrix();
    RenderMouseCursor();
}

void GM_Gameworld::KeyPress (unsigned char key, int x, int y) {
    console.insert("Entering mainmenu\n");
    printf("Entering mainmenu\n");
    GameModeEnter(GM_MAINMENU);
}

