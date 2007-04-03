#ifdef _MSC_VER
    #include <stdlib.h> // doesnt like exit() defined elsewhere.
#endif

#include <GL/glut.h>
#include <GLICT/fonts.h>
#include <GLICT/globals.h>

#include "assert.h"
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

#define VISIBLEW 15 // 14
#define VISIBLEH 11 // 10

#define VISIBLEWPIXEL (VISIBLEW*32.)
#define VISIBLEHPIXEL (VISIBLEH*32.)
extern float ItemAnimationPhase;
extern unsigned int ItemSPRAnimationFrame;

static bool useex_item2;
void PaintMap();
ONThreadFuncReturnType ONThreadFuncPrefix GM_Gameworld_Thread(ONThreadFuncArgumentType menuclass_void) {
    while (1) { // while we're in gameworld game mode
        if (!protocol->GameworldWork()) break;
        //if (!(gamemode == GM_GAMEWORLD)) break;
    }
    protocol->Close();
    if (gamemode == GM_GAMEWORLD)
        console.insert("Connection interrupted.");
    else
        MessageBox(0, "Connection interrupted.", 0, 0);
    delete protocol;
    protocol = NULL;


    return 0;
}
GM_Gameworld::GM_Gameworld() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Constructing gameworld\n");

    SoundSetMusic("music/game.mp3");

    //g = new ObjSpr(5022, 0);
    //g = new ObjSpr(4597);

    desktop.AddObject(&winWorld);
        winWorld.SetOnPaint(GM_Gameworld_WorldOnPaint);
        winWorld.SetHeight(370);
        winWorld.SetWidth(370. * VISIBLEWPIXEL/VISIBLEHPIXEL);
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

    desktop.AddObject(&winInventory);
        winInventory.SetWidth(120);
        winInventory.SetHeight(150);
        winInventory.SetCaption("Inventory");
        winInventory.SetPos(410, 0);

        glictPos posInvSlots[10] = {
            {45, 3},  // helmet
            {8, 17},  // necklace
            {83, 17}, // backpack
            {45, 40}, // armor
            {83, 54}, // right weapon
            {8, 54},  // left weapon
            {45, 77}, // legs
            {45, 115},// feet
            {8, 91},  // ring
            {83, 91}};// hand
        for (int i=0;i<10;i++) {
            winInventory.AddObject(&panInvSlots[i]);
            panInvSlots[i].SetBGColor(.2,.2,.2, 1.);
            panInvSlots[i].SetPos(posInvSlots[i]);
            panInvSlots[i].SetHeight(32);
            panInvSlots[i].SetWidth(32);
            panInvSlots[i].SetOnPaint(GM_Gameworld_InvSlotsOnPaint);
            panInvSlots[i].SetOnClick(GM_Gameworld_InvSlotsOnClick);
        }



    //glDisable(GL_CULL_FACE);
    ONNewThread(GM_Gameworld_Thread, NULL);

    glDisable(GL_SCISSOR_TEST);

    useex_item2 = false;
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
    int modifiers = glutGetModifiers();

    switch (key) {
        //case GLUT_KEY_F11: // function key
        case GLUT_KEY_UP:
            if (modifiers & GLUT_ACTIVE_CTRL)
                protocol->Turn(NORTH);
            else
                protocol->Move(NORTH);
            break;
        case GLUT_KEY_DOWN:
            if (modifiers & GLUT_ACTIVE_CTRL)
                protocol->Turn(SOUTH);
            else
                protocol->Move(SOUTH);
            break;

        case GLUT_KEY_LEFT:
            if (modifiers & GLUT_ACTIVE_CTRL)
                protocol->Turn(WEST);
            else
                protocol->Move(WEST);
            break;

        case GLUT_KEY_RIGHT:
            if (modifiers & GLUT_ACTIVE_CTRL)
                protocol->Turn(EAST);
            else
                protocol->Move(EAST);

            break;



        // diagonal moving
        case GLUT_KEY_END: // 1
            protocol->Move(SOUTHWEST);
            break;
        case GLUT_KEY_PAGE_DOWN: // 3
            protocol->Move(SOUTHEAST);
            break;
        case GLUT_KEY_HOME: // 7
            protocol->Move(NORTHWEST);
            break;
        case GLUT_KEY_PAGE_UP: // 9
            protocol->Move(NORTHEAST);
            break;
        // 5 with numlock off is used to stop moving
        // protocol->Move(STOP);
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
            //protocol->Logout();
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





    // Idea for optimization:
    // We introduce a variable called "minfloor". minfloor is initially set to
    // 7 (surface) and through consequent calls to "render" we inspect
    // what's the last tile that contains anything renderable.
    // we also have an auxiliary boolean "needrenewminfloor" which will be
    // initially true, until first inspection of how far do we need to render
    // and then it will be set to false. whenever a gamemap operation is done,
    // needrenewminfloor gets set to true, and the above procedure of
    // "consequent calls to render and finding min floor" is executed.
    // do not store in player->GetMinZ(), because it could be used later on to
    // save recalc time

    ASSERTFRIENDLY(player, "It is possible that server did not send us information about player's creatureID. This is strange.\nAt present stage, it also might be caused by an unpredicted bug.\nThis happens quite often, and a cure for this bug will be sought after.");
    ASSERTFRIENDLY(player->GetCreature(), "Server did not place player on the map at any time. This is strange.\nIf this happened when you died, please inform the development team -- we overlooked this ;)")
    if (player->GetCreature()->IsMoving()) player->GetCreature()->CauseAnimOffset(false);
    static int offset;

    for (int z = 14; z >= min(player->GetPosZ(), (!player->GetMinZ()? 1 : player->GetMinZ()))  ; z--) {
        offset = z - player->GetPosZ();
        for (int layer= 0; layer <= 2; layer++)

            for (int x = -(VISIBLEW/2) - 2; x <= +(VISIBLEW/2) - offset + 2; x++) { // internally "visible" coordinates: -8, +8 and -6, +6
                for (int y = -(VISIBLEH/2) - 2; y <= +(VISIBLEH/2) - offset + 2; y++) { // really visible coordinates: -7, +7 and -5, +5

                    position_t p;
                    p.x = player->GetPosX() + x; p.y = player->GetPosY() + y; p.z = z;//player->GetPos()->z;

                    glPushMatrix();
                    glTranslatef((x+8 - (player->GetPosZ() - z))*32, (14-(y+6 - (player->GetPosZ() - z)))*32, 0);

                    Tile *t;
                    Thing *g;

                    if (x+offset < -9 || x+offset > 9 || y+offset < -7 || y+offset > 7)
                        glColor4f(.5,.5,.5,1.);
                    else
                        glColor4f(1., 1., 1., 1.);

                    if (t=gamemap.GetTile(&p))
                            //t->Render(0);
                            t->Render(layer);


                    glMatrixMode(GL_MODELVIEW);
                    glPopMatrix();
                }
            }
    }

// now we just render the overlay
// keep code in sync with above
    int z = player->GetPosZ();
    int layer = 3;
    for (int x = -(VISIBLEW/2) - 1; x <= +(VISIBLEW/2) - offset + 1; x++) { // internally "visible" coordinates: -8, +8 and -6, +6
        for (int y = -(VISIBLEH/2) - 1; y <= +(VISIBLEH/2) - offset + 1; y++) { // really visible coordinates: -7, +7 and -5, +5
            position_t p;
            p.x = player->GetPosX() + x; p.y = player->GetPosY() + y; p.z = z;//player->GetPos()->z;

            glPushMatrix();
            glTranslatef((x+8 - (player->GetPosZ() - z))*32, (14-(y+6 - (player->GetPosZ() - z)))*32, 0);

            Tile *t;
            Thing *g;

            if (x+offset < -7 || x+offset > 7 || y+offset < -5 || y+offset > 5)
                glColor4f(.5,.5,.5,1.);
            else
                glColor4f(1., 1., 1., 1.);

            if (t=gamemap.GetTile(&p))
                    //t->Render(0);
                    t->Render(layer);


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
    winWorld.SetWidth((float)(winh > 100 ? winh - 100 : 0) * VISIBLEWPIXEL/VISIBLEHPIXEL);
    winWorld.SetPos(0, 0);

    winConsole.SetWidth(winw-100);
    winConsole.SetPos(0, winh-100+14);
        txtConMessage.SetWidth(winw-100-50);
        btnConSend.SetPos(winw-100-50, 52);

    winInventory.SetPos(winw-120, 0);


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

    glOrtho(0, VISIBLEWPIXEL, 0, VISIBLEHPIXEL, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(-1*32. + (VISIBLEW - 15)/2. * 32., -3*32. + (VISIBLEH - 11)/2. * 32., 0);
    //glTranslatef(-1*32., -4*32., 0);
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

    glictSize size;
    position_t pos2;

    caller->GetSize(&size);
    pos->x *= VISIBLEWPIXEL / size.w;
    pos->y *= VISIBLEHPIXEL / size.h;

    pos->x /= 32;
    pos->y /= 32;


    pos2.x = pos->x - (VISIBLEW / 2) + player->GetPosX();
    pos2.y = pos->y - (VISIBLEH / 2) + player->GetPosY();
    pos2.z = player->GetPosZ();


    GM_Gameworld_ClickExec(&pos2);

}
void GM_Gameworld_ClickExec(position_t *pos) {
    static int modifiers;
    modifiers = glutGetModifiers();

    if (useex_item2) {
        useex_item2 = false;
        Tile *t;
        if (pos->x!=0xFFFF) t = gamemap.GetTile(pos);

        protocol->Use(&(((GM_Gameworld*)game)->useex_item1_pos), ((GM_Gameworld*)game)->useex_item1_stackpos, pos, pos->x!=0xFFFF ? t->GetTopUsableStackpos() : 0);
        glut_SetMousePointer("mousepointer.bmp");
        return;
    }

    if (modifiers & GLUT_ACTIVE_SHIFT)
        protocol->LookAt(pos);
    else if (modifiers & GLUT_ACTIVE_ALT) {
        if (pos->x==0xFFFF) return;
        Tile *t = gamemap.GetTile(pos);

        if (Creature *c = t->GetCreature())
            protocol->Attack(c->GetCreatureID());
    } else if (modifiers & GLUT_ACTIVE_CTRL) {
        Tile *t;
        Thing *th;
        if (pos->x!=0xFFFF) {
            t = gamemap.GetTile(pos);
            th = t->GetStackPos(t->GetTopUsableStackpos());
        } else {
            th = player->inventory[pos->y-1];
        }
        if (th) {
            unsigned short itemid = th->GetType();
            if (items[itemid].usable) {
                // extended usable
                console.insert("Specify where do you want to use this item", CONLTBLUE);
                glut_SetMousePointer(new ObjSpr(itemid,0));
                ((GM_Gameworld*)game)->useex_item1_pos = *pos;
                ((GM_Gameworld*)game)->useex_item1_stackpos = pos->x != 0xFFFF ? t->GetTopUsableStackpos() : 0;
                useex_item2 = true;
            } else {
                // simple usable
                protocol->Use(pos, pos->x!=0xFFFF ? t->GetTopUsableStackpos() : 0);
            }
        } else {
            if (pos->x!=0xFFFF) {
                char tmp[256];
                sprintf(tmp, "There's a bug in GetTopUsableStackpos() routine or the tile is empty. %d/%d", t->GetTopUsableStackpos(), t->GetItemCount() );

                console.insert(tmp, CONRED);
            }
        }
    } else {
        char tmp [256];
        sprintf(tmp, "You clicked on location (%d, %d, %d).", pos->x, pos->y, pos->z);
        console.insert(tmp, CONWHITE);
        gamemap.GetTile(pos)->ShowContents();
    }


}
void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {
    /*char tmp[256];
    sprintf(tmp, "%d", (glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots);
    caller->SetCaption(tmp);

    printf("%s\n", tmp);
    return;*/

    glViewport(clipped->left, glictGlobals.h - clipped->bottom, clipped->right - clipped->left, clipped->bottom - clipped->top);
    glClearColor(.1, .1, .1, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0., 0., 0., 1.);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 32, 0, 32, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    player->RenderInventory((glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);


    glViewport(0,0,glictGlobals.w,glictGlobals.h);
}
void GM_Gameworld_InvSlotsOnClick(glictPos* pos, glictContainer* caller) {
    //char tmp[256];
    //sprintf(tmp, "Clicked on %d", (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots));
    //console.insert(tmp, CONYELLOW);
    int slot = (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots) + 1;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = slot;
    pos2.z = 0;

    GM_Gameworld_ClickExec(&pos2);

}
