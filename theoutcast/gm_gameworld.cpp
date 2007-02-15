#include <GL/glut.h>
#include <GLICT/fonts.h>
#include "gm_gameworld.h"
#include "items.h"
#include "creatures.h"
#include "console.h"
#include "objspr.h"
#include "sprfmts.h"
#include "glutwin.h"
#include "player.h"
#include "map.h"
#include "threads.h"
#include "protocol.h"
#include "sound.h"
#include "debugprint.h"
extern float ItemAnimationPhase;
extern unsigned int ItemSPRAnimationFrame;
ONThreadFuncReturnType ONThreadFuncPrefix GM_Gameworld_Thread(ONThreadFuncArgumentType menuclass_void) {
    while (gamemode == GM_GAMEWORLD) { // while we're in gameworld game mode
        if (!protocol->GameworldWork()) break;
    }
    protocol->Close();
    if (gamemode == GM_GAMEWORLD)
        console.insert("Connection interrupted.");
    delete protocol;
    protocol = NULL;
}
GM_Gameworld::GM_Gameworld() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Constructing gameworld\n");

    SoundSetMusic("music/game.mp3");

    g = new ObjSpr(5022, 0);
    //g = new ObjSpr(4597);

    //glDisable(GL_CULL_FACE);
    ONNewThread(GM_Gameworld_Thread, NULL);
}


GM_Gameworld::~GM_Gameworld() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destructing gameworld\n");
    ItemsUnload();
    CreaturesUnload();
    SPRUnloader();
}

void GM_Gameworld::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 0, 480);

    gamemap.Lock();
    {
        char tmp[256];
        sprintf(tmp, "%d %d %d", player->GetPosX(), player->GetPosY(), player->GetPosZ());

        glPushMatrix();
        glictFontRender(tmp, "system", 0, 600 );
        glPopMatrix();

    }

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

    /*glPushMatrix();
    glTranslatef(216, 216, 0);
    position_t p = {0,0,0};
    g->Render(&p);
    g->AnimationAdvance(25./fps);
    glPopMatrix();
*/

    RenderMouseCursor();
}

void GM_Gameworld::KeyPress (unsigned char key, int x, int y) {
    key = tolower(key);
    if (!protocol) {
        printf("No protocol!");

        GameModeEnter(GM_MAINMENU);
        return;
    }
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
            protocol->Close();
            GameModeEnter(GM_MAINMENU);
    }
}

