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
        winWorld.SetHeight(370);
        winWorld.SetWidth(370. * 640./480.);
        winWorld.SetCaption("World");
        winWorld.SetOnClick(GM_Gameworld_WorldOnClick);

    desktop.AddObject(&winConsole);
        winConsole.SetOnPaint(GM_Gameworld_ConsoleOnPaint);
        winConsole.SetHeight(70);
        winConsole.SetWidth(400);
        winConsole.SetCaption("Console");
        winConsole.SetPos(0,390);
        winConsole.AddObject(&txtConMessage);
            txtConMessage.SetCaption("");
            txtConMessage.SetPos(0,52);
            txtConMessage.SetHeight(15);
            txtConMessage.SetWidth(350);
        winConsole.AddObject(&btnConSend);
            btnConSend.SetCaption("Send");
            btnConSend.SetPos(350,52);
            btnConSend.SetHeight(15);
            btnConSend.SetWidth(50);
            btnConSend.SetBGColor(.6,.6,.6,1.);
            btnConSend.SetOnClick(GM_Gameworld_ConSendOnClick);


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

//    glColor4f(1., 1., 1., 1.);
    //console.draw(10);

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



	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glEnable(GL_SCISSOR_TEST);
	//glTranslatef(winw/2, winh/2,0);
	//glRotatef( sin( bg_move_angle * PI / 180. )*2., 0., 0, 1);
	//glTranslatef(-winw/2, -winh/2,0);
	//desktop.RememberTransformations();
	desktop.Paint();

	glDisable(GL_SCISSOR_TEST);


    RenderMouseCursor();
}

void GM_Gameworld::SpecKeyPress(int key, int x, int y ) {
    if (!protocol) {
        printf("No protocol!");

        GameModeEnter(GM_MAINMENU);
        return;
    }
    switch (key) {
        //case GLUT_KEY_F11: // function key
        case GLUT_KEY_UP:
            protocol->MoveNorth();
            break;
        case GLUT_KEY_DOWN:
            protocol->MoveSouth();
            break;
        case GLUT_KEY_LEFT:
            protocol->MoveWest();
            break;
        case GLUT_KEY_RIGHT:
            protocol->MoveEast();
            break;
    }
}
void GM_Gameworld::KeyPress (unsigned char key, int x, int y) {
    if (!protocol) {
        printf("No protocol!");

        GameModeEnter(GM_MAINMENU);
        return;
    }

    unsigned char lckey = tolower(key);

    switch (lckey) {
        case 13: // enter ... ONLY TMP!! should be managed by the GUI
            GM_Gameworld_ConSendOnClick (NULL, NULL);
            break;
        case 27: // esc
            protocol->Close();
            GameModeEnter(GM_MAINMENU);
            break;

        default:
            desktop.CastEvent(GLICT_KEYPRESS, &key, 0);

    }
}
void PaintMap() {
    gamemap.Lock();

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    //glTranslatef(100, 100, 0);
    //glScalef(0.5, 0.5, 0.5);

    //for (int z = 14; z >= min((player->GetPosZ()>7 ? player->GetPosZ() : 0), player->GetMinZ())  ; z--)
    for (int z = 14; z >= min(player->GetPosZ(), (!player->GetMinZ()? 1 : player->GetMinZ()))  ; z--)
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

    glPopMatrix();
    //printf("%d\n", player->GetMinZ());
    gamemap.Unlock();
}
void GM_Gameworld::ResizeWindow() {
	desktop.SetHeight(winh);
	desktop.SetWidth(winw);

    winWorld.SetHeight(winh > 100 ? winh - 100 : 0);
    winWorld.SetWidth((float)(winh > 100 ? winh - 100 : 0) * 640./480.);
    winWorld.SetPos(0, 0);

    winConsole.SetWidth(winw-100);
    winConsole.SetPos(0, winh-100+14);

    txtConMessage.SetWidth(winw-100-50);
    btnConSend.SetPos(winw-100-50, 52);




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

void GM_Gameworld_ConsoleOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {

    glViewport(clipped->left, glictGlobals.h - clipped->bottom, clipped->right - clipped->left, clipped->bottom - clipped->top);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, winw-100, 0, 70, -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 10, 0);
    glColor4f(1., 1., 1., 1.);
    console.draw(10);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);

    glViewport(0,0,glictGlobals.w,glictGlobals.h);

}
void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {

    glViewport(clipped->left, glictGlobals.h - clipped->bottom, clipped->right - clipped->left, clipped->bottom - clipped->top);
    glClearColor(.1, .1, .1, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0., 0., 0., 1.);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 640, 0, 480, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    PaintMap();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);


    glViewport(0,0,glictGlobals.w,glictGlobals.h);

}

void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller) {
    protocol->Speak(NORMAL, ((GM_Gameworld*)game)->txtConMessage.GetCaption().c_str() );
    ((GM_Gameworld*)game)->txtConMessage.SetCaption("");
}

void GM_Gameworld_WorldOnClick (glictPos* pos, glictContainer* caller) {
    char tmp[256];
    sprintf(tmp, "Clicked on %d %d", pos->x, pos->y);
    console.insert(tmp, CONORANGE);
}
