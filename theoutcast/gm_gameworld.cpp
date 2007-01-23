#include <GL/glut.h>
#include "gm_gameworld.h"
#include "items.h"
#include "console.h"
#include "objspr.h"
#include "sprfmts.h"
#include "glutwin.h"
#include "player.h"
#include "map.h"
#include "threads.h"
#include "protocol.h"
ONThreadFuncReturnType ONThreadFuncPrefix GM_Gameworld_Thread(ONThreadFuncArgumentType menuclass_void) {
    while (dynamic_cast<GM_Gameworld*>(game)) { // while we're in gameworld game mode
        protocol->GameworldWork();
    }
    protocol->Close();
    delete protocol;
}
GM_Gameworld::GM_Gameworld() {
    printf("Constructing gameworld\n");
    SPRLoader("tibia76.spr");

    //g = new ObjSpr(1952);
    //g = new ObjSpr(4597);

    ONNewThread(GM_Gameworld_Thread, NULL);
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
    gamemap.Lock();


    glPushMatrix();
    glTranslatef(100, 100, 0);
    glScalef(0.5, 0.5, 0.5);
    for (int z = 14; z >= (player->GetPosZ()>7 ? player->GetPosZ() : 0) ; z--)

        for (int x = -16; x <= +16; x++) {
            for (int y = -12; y <= +12; y++) {

                position_t p;
                p.x = player->GetPos()->x + x; p.y = player->GetPos()->y + y; p.z = z;//player->GetPos()->z;
                glPushMatrix();
                glTranslatef((x+8 - (player->GetPos()->z - z))*32, (14-(y+6 - (player->GetPos()->z - z)))*32, 0);

                Tile *t;
                Thing *g;

                if (x < -7 || x > 7 || y < -5 || y > 5) glColor4f(.5,.5,.5,1.); else glColor4f(1., 1., 1., 1.);

                if (t=gamemap.GetTile(&p))
                    t->render();
                glPopMatrix();
            }
        }

    glPopMatrix();


    glColor4f(1., 1., 1., 1.);
    gamemap.Unlock();
    console.draw(10);


    //glPushMatrix();

    //glTranslatef(200, 200, 0);
    //g->Render();
    //glPopMatrix();


    RenderMouseCursor();
}

void GM_Gameworld::KeyPress (unsigned char key, int x, int y) {
    //console.insert("Entering mainmenu\n");
    //printf("Entering mainmenu\n");

    switch (key) {
        case 'w':
            protocol->MoveNorth();
            break;
        case 's':
            protocol->MoveSouth();
            break;
        case 'a':
            protocol->MoveWest();
            break;
        case 'd':
            protocol->MoveEast();
            break;
        default:
            GameModeEnter(GM_MAINMENU);
    }
}

