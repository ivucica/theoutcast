#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>

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

void PaintMap();
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

    desktop.AddObject(&winWorld);
    winWorld.SetOnPaint(GM_Gameworld_WorldOnPaint);
    winWorld.SetHeight(400);
    winWorld.SetWidth(400);

    //glDisable(GL_CULL_FACE);
    ONNewThread(GM_Gameworld_Thread, NULL);

    glDisable(GL_SCISSOR_TEST);
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
    glOrtho(0, 640, 0, 480, -100, 100);

    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#1 %s\n", gluErrorString(er));
    }

    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#2 %s\n", gluErrorString(er));
    }
    glColor4f(1., 1., 1., 1.);
    console.draw(10);

    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#3 %s\n", gluErrorString(er));
    }

    /*glPushMatrix();
    glTranslatef(216, 216, 0);
    position_t p = {0,0,0};
    g->Render(&p);
    g->AnimationAdvance(25./fps);
    glPopMatrix();
*/


    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);


    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#3.5 %s\n", gluErrorString(er));
    }


	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glEnable(GL_SCISSOR_TEST);
	//glTranslatef(winw/2, winh/2,0);
	//glRotatef( sin( bg_move_angle * PI / 180. )*2., 0., 0, 1);
	//glTranslatef(-winw/2, -winh/2,0);
	//desktop.RememberTransformations();
	desktop.Paint();

	glDisable(GL_SCISSOR_TEST);

    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#4 %s\n", gluErrorString(er));
    }


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
void PaintMap() {
    gamemap.Lock();

    glMatrixMode(GL_MODELVIEW);

    {
    int er;
    if ((er = glGetError()) != GL_NO_ERROR) printf("#6 %s\n", gluErrorString(er));
    }
    glPushMatrix();
    //glTranslatef(100, 100, 0);
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

                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
            }
        }

    {
    int er;
    if ((er=glGetError()) != GL_NO_ERROR) printf("#5 %s\n", gluErrorString(er));
    }
    glPopMatrix();

    gamemap.Unlock();
}
void GM_Gameworld::ResizeWindow() {
	desktop.SetHeight(winh);
	desktop.SetWidth(winw);

	glutPostRedisplay();
}



void GM_Gameworld::MouseClick (int button, int shift, int mousex, int mousey) {
	if (shift == GLUT_UP) SoundPlay("sounds/mouse.wav");

	glictPos pos;
	pos.x = mousex;
	pos.y = mousey;
	desktop.TransformScreenCoords(&pos);
	if (shift==GLUT_DOWN) desktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0);
	if (shift==GLUT_UP) desktop.CastEvent(GLICT_MOUSEUP, &pos, 0);

}

void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {

    {
        int er;
		if ((er = glGetError()) != GL_NO_ERROR) {printf("#7 %s\n", gluErrorString(er)); system("pause");}
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 400, 0, 400, -100, 100);

		{
        int er;
		if ((er = glGetError()) != GL_NO_ERROR) {printf("#8 %s\n", gluErrorString(er)); system("pause");}
		}

    //return;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    {
        int er;
        if ((er = glGetError()) != GL_NO_ERROR) {printf("#9 %s\n", gluErrorString(er)); system("pause");}
    }

    //glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(clipped->left, glictGlobals.h - clipped->bottom, clipped->right - clipped->left, clipped->bottom - clipped->top);
    glClear(GL_COLOR_BUFFER_BIT);


    PaintMap();

    //glPopAttrib();
    glViewport(0,0,glictGlobals.w,glictGlobals.h);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);


}
