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
#include "windowing.h"
#include "player.h"
#include "map.h"
#include "threads.h"
#include "protocol.h"
#include "sound.h"
#include "debugprint.h"
#include "defines.h" // min and max
#include "skin.h"
#include "types.h"

#define VISIBLEW 15 // 14
#define VISIBLEH 11 // 10

#define VISIBLEWPIXEL (VISIBLEW*32.)
#define VISIBLEHPIXEL (VISIBLEH*32.)


#ifndef WIN32
    #define MessageBox(x,y,z,u)
#endif
extern float ItemAnimationPhase;
extern unsigned int ItemSPRAnimationFrame;

static bool useex_item2;
void PaintMap();

Texture* texSkull;

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

    ONInitThreadSafe(desktopthreadsafe);




    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Constructing gameworld\n");

    SoundSetMusic("music/game.mp3");

    texSkull = new Texture("skull.bmp");
    chase = STAND;
    stance = BALANCED;
    invitingparty = false;
    revokingparty = false;
    joiningparty = false;
    passingparty = false;

    desktop.AddObject(&winWorld);
        winWorld.SetOnPaint(GM_Gameworld_WorldOnPaint);
        winWorld.SetHeight(370);
        winWorld.SetWidth((int)(370. * VISIBLEWPIXEL/VISIBLEHPIXEL));
        winWorld.SetCaption("World");
        winWorld.SetOnClick(GM_Gameworld_WorldOnClick);
        winWorld.SetOnMouseDown(GM_Gameworld_WorldOnMouseDown);
        winWorld.SetOnMouseUp(GM_Gameworld_WorldOnMouseUp);

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
            panInvSlots[i].SetOnMouseDown(GM_Gameworld_InvSlotsOnMouseDown);
            panInvSlots[i].SetOnMouseUp(GM_Gameworld_InvSlotsOnMouseUp);
        }



    desktop.AddObject(&winStats);
        winStats.SetWidth(120);
        winStats.SetHeight(250);
        winStats.SetCaption("Stats");
        winStats.SetPos(410, 240);
        winStats.AddObject(&panStaStats);
            panStaStats.SetBGActiveness(false);
            panStaStats.SetHeight(220);
            panStaStats.SetWidth(120);
            panStaStats.SetPos(0, 20);
       winStats.AddObject(&btnStaStance1);
            btnStaStance1.SetHeight(16);
            btnStaStance1.SetWidth(16);
            btnStaStance1.SetPos(0,0);
            btnStaStance1.SetCaption("O");
            btnStaStance1.SetCustomData((void*)1);
            btnStaStance1.SetOnClick( GM_Gameworld_StaStanceOnClick );
        winStats.AddObject(&btnStaStance2);
            btnStaStance2.SetHeight(16);
            btnStaStance2.SetWidth(16);
            btnStaStance2.SetPos(20,0);
            btnStaStance2.SetCaption("B");
            btnStaStance2.SetCustomData((void*)2);
            btnStaStance2.SetOnClick( GM_Gameworld_StaStanceOnClick );
        winStats.AddObject(&btnStaStance3);
            btnStaStance3.SetHeight(16);
            btnStaStance3.SetWidth(16);
            btnStaStance3.SetPos(40,0);
            btnStaStance3.SetCaption("D");
            btnStaStance3.SetCustomData((void*)3);
            btnStaStance3.SetOnClick( GM_Gameworld_StaStanceOnClick );
        winStats.AddObject(&chkStaChase);
            chkStaChase.SetHeight(16);
            chkStaChase.SetWidth(16);
            chkStaChase.SetPos(70,0);
            chkStaChase.SetCaption(" ");
            chkStaChase.SetOnClick(GM_Gameworld_StaChaseOnClick);
        winStats.AddObject(&btnInviteParty);
            btnInviteParty.SetWidth(60);
            btnInviteParty.SetHeight(16);
            btnInviteParty.SetPos(0,206);
            btnInviteParty.SetCaption("Invite");
            btnInviteParty.SetOnClick(GM_Gameworld_StaInviteParty);
        winStats.AddObject(&btnJoinParty);
            btnJoinParty.SetWidth(60);
            btnJoinParty.SetHeight(16);
            btnJoinParty.SetPos(60,206);
            btnJoinParty.SetCaption("Join");
            btnJoinParty.SetOnClick(GM_Gameworld_StaJoinParty);
        winStats.AddObject(&btnRevokeParty);
            btnRevokeParty.SetWidth(60);
            btnRevokeParty.SetHeight(16);
            btnRevokeParty.SetPos(0,222);
            btnRevokeParty.SetCaption("Revoke");
            btnRevokeParty.SetOnClick(GM_Gameworld_StaRevokeParty);
        winStats.AddObject(&btnLeaveParty);
            btnLeaveParty.SetWidth(60);
            btnLeaveParty.SetHeight(16);
            btnLeaveParty.SetPos(60,222);
            btnLeaveParty.SetCaption("Leave");
            btnLeaveParty.SetOnClick(GM_Gameworld_StaLeaveParty);
        winStats.AddObject(&btnPassParty);
            btnPassParty.SetWidth(120);
            btnPassParty.SetHeight(16);
            btnPassParty.SetPos(0,238);
            btnPassParty.SetCaption("Pass Leadership");
            btnPassParty.SetOnClick(GM_Gameworld_StaPassParty);

    UpdateStats();

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

    delete texSkull;

    ONDeinitThreadSafe(desktopthreadsafe);
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
//	glEnable(GL_SCISSOR_TEST);
	//glTranslatef(winw/2, winh/2,0);
	//glRotatef( sin( bg_move_angle * PI / 180. )*2., 0., 0, 1);
	//glTranslatef(-winw/2, -winh/2,0);
	//desktop.RememberTransformations();
	ONThreadSafe(desktopthreadsafe);
    glEnable(GL_SCISSOR_TEST);
	skin.AssureLoadedness();
	desktop.Paint();
	containerdesktop.Paint();
	glDisable(GL_SCISSOR_TEST);
	ONThreadUnsafe(desktopthreadsafe);


    glColor4f(1,1,1,1);

    RenderMouseCursor();
}

void GM_Gameworld::SpecKeyPress(int key, int x, int y ) {
    if (!protocol) {
        printf("No protocol!");

        GameModeEnter(GM_MAINMENU);
        return;
    }
    #ifdef USEGLUT
    int modifiers = glutGetModifiers();
    #else
    int modifiers = 0;
    #endif

    switch (key) {
        //case GLUT_KEY_F11: // function key
        case WIN_KEY_UP:
            if (modifiers & WIN_ACTIVE_CTRL)
                protocol->Turn(NORTH);
            else
                protocol->Move(NORTH);
            break;
        case WIN_KEY_DOWN:
            if (modifiers & WIN_ACTIVE_CTRL)
                protocol->Turn(SOUTH);
            else
                protocol->Move(SOUTH);
            break;

        case WIN_KEY_LEFT:
            if (modifiers & WIN_ACTIVE_CTRL)
                protocol->Turn(WEST);
            else
                protocol->Move(WEST);
            break;

        case WIN_KEY_RIGHT:
            if (modifiers & WIN_ACTIVE_CTRL)
                protocol->Turn(EAST);
            else
                protocol->Move(EAST);

            break;



        // diagonal moving
        case WIN_KEY_END: // 1
            protocol->Move(SOUTHWEST);
            break;
        case WIN_KEY_PAGE_DOWN: // 3
            protocol->Move(SOUTHEAST);
            break;
        case WIN_KEY_HOME: // 7
            protocol->Move(NORTHWEST);
            break;
        case WIN_KEY_PAGE_UP: // 9
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
            //containerdesktop.CastEvent(GLICT_KEYPRESS, &key, 0);

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
			//for (int layer= 0; layer <= 2; layer++)

				for (int x = -(VISIBLEW/2) - 1; x <= +(VISIBLEW/2) - offset + 1; x++) { // internally "visible" coordinates: -8, +8 and -6, +6
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
									t->Render(0);
									//t->Render(layer);


							glMatrixMode(GL_MODELVIEW);
						glPopMatrix();
					}
				}
		}

	// now we just render the overlay
	// keep code in sync with above
		int z = player->GetPosZ();
		int layer = 3;
        for (int z = 14; z >= min(player->GetPosZ(), (!player->GetMinZ()? 1 : player->GetMinZ()))  ; z--) {
            offset = z - player->GetPosZ();
            for (int x = -(VISIBLEW/2) - 1; x <= +(VISIBLEW/2) - offset + 1; x++) { // internally "visible" coordinates: -8, +8 and -6, +6
                for (int y = -(VISIBLEH/2) - 2; y <= +(VISIBLEH/2) - offset + 2; y++) { // really visible coordinates: -7, +7 and -5, +5
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
        }


    glPopMatrix();
    //printf("%d\n", player->GetMinZ());
    gamemap.Unlock();
}
void GM_Gameworld::ResizeWindow() {

	desktop.SetHeight(winh);
	desktop.SetWidth(winw);

    winWorld.SetHeight((winh > 100 ? winh - 100 : 0) - 16 - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetTopSize()->h : 0));
    winWorld.SetWidth((int)((float)((winh > 100 ? winh - 100 : 0) - 16 - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetTopSize()->h : 0)) * VISIBLEWPIXEL/VISIBLEHPIXEL));
    winWorld.SetPos(0, 0);

    winConsole.SetWidth(winw-100);
    winConsole.SetPos(0, winh-100+32 - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetTopSize()->h : 0) - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetBottomSize()->h : 0));
        txtConMessage.SetWidth(winw-100-50);
        btnConSend.SetPos(winw-100-50, 52);

    winInventory.SetPos(winw-120 - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetLeftSize()->w : 0) - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetRightSize()->w : 0), 0);

    winStats.SetPos(winw-120 - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetLeftSize()->w : 0) - (glictGlobals.windowBodySkin ? glictGlobals.windowBodySkin->GetRightSize()->w : 0), winInventory.GetHeight());


	containerdesktop.SetHeight(winh);
	containerdesktop.SetWidth(winw);

#ifdef USEGLUT
	glutPostRedisplay();
#endif
}



void GM_Gameworld::MouseClick (int button, int shift, int mousex, int mousey) {
	if (shift == WIN_RELEASE) SoundPlay("sounds/mouse.wav");

	glictPos pos;
	pos.x = mousex;
	pos.y = mousey;
	desktop.TransformScreenCoords(&pos);
	if (shift==WIN_PRESS)
        if (!containerdesktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0))
            desktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0);
	if (shift==WIN_RELEASE) {
        if (!containerdesktop.CastEvent(GLICT_MOUSEUP, &pos, 0))
            desktop.CastEvent(GLICT_MOUSEUP, &pos, 0);
	}

}

void GM_Gameworld::UpdateStats() {
    static char tmp[2048];
    sprintf(tmp,"HP: %d/%d\n"
                "MP: %d/%d\n"
                "---------\n"
                "Exp: %d\n"
                "Level: %d (%d%%)\n"
                "Magic: %d (%d%%)\n"
                "Cap: %d\n"
                "---------\n"
                "Fist: %d (%d%%)\n"
                "Club: %d (%d%%)\n"
                "Sword: %d (%d%%)\n"
                "Axe: %d (%d%%)\n"
                "Distance: %d (%d%%)\n"
                "Shielding: %d (%d%%)\n"
                "Fishing: %d (%d%%)\n"
                "---------\n"
                "(%d,%d,%d)\n"
                "%s%s%s%s%s%s%s%s%s\n",
                player->GetHP(), player->GetMaxHP(),
                player->GetMP(), player->GetMaxMP(),
                player->GetExp(),
                player->GetLevel(), player->GetLevelPercent(),
                player->GetMLevel(), player->GetMLevelPercent(),
                player->GetCap(),
                player->GetSkillLevel(FIST), player->GetSkillPercent(FIST),
                player->GetSkillLevel(CLUB), player->GetSkillPercent(CLUB),
                player->GetSkillLevel(SWORD), player->GetSkillPercent(SWORD),
                player->GetSkillLevel(AXE), player->GetSkillPercent(AXE),
                player->GetSkillLevel(DISTANCE), player->GetSkillPercent(DISTANCE),
                player->GetSkillLevel(SHIELDING), player->GetSkillPercent(SHIELDING),
                player->GetSkillLevel(FISHING), player->GetSkillPercent(FISHING),

                player->GetPosX(),player->GetPosY(),player->GetPosZ(),

                player->GetIcon(ICON_POISON) ? "P" : "", player->GetIcon(ICON_BURN) ? "B" : "", player->GetIcon(ICON_ENERGY) ? "E" : "", player->GetIcon(ICON_DRUNK) ? "D" : "", player->GetIcon(ICON_MANASHIELD) ? "S" : "", player->GetIcon(ICON_PARALYZE) ? "p" : "", player->GetIcon(ICON_HASTE) ? "H" : "", player->GetIcon(ICON_SWORDS) ? "X" : "", player->GetIcon(ICON_DROWNING) ? "d" : ""
                 );

    panStaStats.SetCaption(tmp);

}

void GM_Gameworld::AddContainer(Container *c, unsigned int x, unsigned int y) {
    printf("Locking\n");
    ONThreadSafe(desktopthreadsafe);
    printf("okies\n");
    if (c->GetWindow()) {
        containerdesktop.AddObject(c->GetWindow());
        c->GetWindow()->SetPos(x, y);
    }
    ONThreadUnsafe(desktopthreadsafe);
    printf("Unlocked\n");
}
void GM_Gameworld::RemoveContainer(Container *c) {
    ONThreadSafe(desktopthreadsafe);
    if (c->GetWindow()) {
        containerdesktop.RemoveObject(c->GetWindow());
    }
    ONThreadUnsafe(desktopthreadsafe);
}

unsigned int GM_Gameworld::GetContainersX() {
    return winw-166;
}
unsigned int GM_Gameworld::GetContainersY() {
    ONThreadSafe(desktopthreadsafe);
    glictPos p; unsigned int h;
    panStaStats.GetPos(&p);
    h = panStaStats.GetHeight();
    ONThreadUnsafe(desktopthreadsafe);
    return p.y + h;
}
void GM_Gameworld::MsgBox (const char* mbox, const char* title) {
	glictSize s;
	glictMessageBox *mb;

	ONThreadSafe(desktopthreadsafe);

	mb = new glictMessageBox;
	mb->GetSize(&s);

	mb->SetCaption(title);
	mb->SetMessage(mbox);

	mb->SetPos(winw / 2 - s.w / 2, winh / 2 - s.h / 2);

	mb->SetOnDismiss(GM_Gameworld_MBOnDismiss);

	desktop.AddObject(mb);

	ONThreadUnsafe(desktopthreadsafe);

}
void GM_Gameworld_MBOnDismiss(glictPos* pos, glictContainer* caller) {
	((GM_Gameworld*)game)->desktop.RemoveObject(caller);
	delete caller;
}

void GM_Gameworld_ConsoleOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {


    if (clipped->bottom <= clipped->top) return;
    if (clipped->right <= clipped->left) return;

    glViewport((int)clipped->left, (int)(glictGlobals.h - clipped->bottom), (int)(clipped->right - clipped->left), (int)(clipped->bottom - clipped->top));
//    glClear(GL_COLOR_BUFFER_BIT);
    float ProjectionMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    glOrtho(0, winw-100, 0, 70, -100, 100);

    glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0, 10, 0);
		glColor4f(1., 1., 1., 1.);
		console.draw(10);


    glMatrixMode(GL_PROJECTION);
	/*glLoadIdentity();
	glOrtho(0,winw,0,winh, -100, 100);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);*/
	glLoadMatrixf(ProjectionMatrix);

    glViewport(0,0,(int)glictGlobals.w,(int)glictGlobals.h);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	skin.AssureLoadedness();
}
void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {

    if (clipped->bottom <= clipped->top) return;
    if (clipped->right <= clipped->left) return;


    glViewport((int)clipped->left, (int)(glictGlobals.h - clipped->bottom), (int)(clipped->right - clipped->left), (int)(clipped->bottom - clipped->top));
  //  glClearColor(.1, .1, .1, 1.);
//    glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0., 0., 0., 1.);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, VISIBLEWPIXEL, 0, VISIBLEHPIXEL, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
		glLoadIdentity();

		glTranslatef(-1*32. + (VISIBLEW - 15)/2. * 32., -3*32. + (VISIBLEH - 11)/2. * 32., 0);
		//glTranslatef(-1*32., -4*32., 0);
		PaintMap();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,winw,0,winh, -100, 100);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		glTranslatef(0,-winh,0.0);


		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glViewport(0,0,(int)glictGlobals.w,(int)glictGlobals.h);
    skin.AssureLoadedness();

}

void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller) {
    std::string entry = ((GM_Gameworld*)game)->txtConMessage.GetCaption();
    if (entry[0] == '/') { // it's a command
        if (entry=="/logout") {
            protocol->Logout();
            return;
        }
        //console.insert("Only '/logout' command supported so far. Rela", CONRED);
        //((GM_Gameworld*)game)->txtConMessage.SetCaption("");
        //return;
    }
    if (entry[0] == '*') {

        char recv[256], *recvend;
        strcpy(recv, entry.c_str()+1);
        recvend = strchr(recv, '*');
        if (recvend) {
            *recvend = 0;
            protocol->Speak(PRIVATE, ((GM_Gameworld*)game)->txtConMessage.GetCaption().c_str()+(recvend-recv)+2, recv );
            ((GM_Gameworld*)game)->txtConMessage.SetCaption(std::string("*") + recv + "*");
            return;
        }


    }
    protocol->Speak(NORMAL, ((GM_Gameworld*)game)->txtConMessage.GetCaption().c_str() );
    ((GM_Gameworld*)game)->txtConMessage.SetCaption("");

}

void GM_Gameworld_WorldOnClick (glictPos* pos, glictContainer* caller) {
    glictSize size;
    position_t pos2;

    caller->GetSize(&size);
    pos->x = (int)(pos->x * VISIBLEWPIXEL / size.w);
    pos->y = (int)(pos->y * VISIBLEHPIXEL / size.h);

    pos->x /= 32;
    pos->y /= 32;

    pos2.x = pos->x - (VISIBLEW / 2) + player->GetPosX();
    pos2.y = pos->y - (VISIBLEH / 2) + player->GetPosY();
    pos2.z = player->GetPosZ();


    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSECLICK);
}

void GM_Gameworld_WorldOnMouseDown (glictPos* pos, glictContainer* caller) {
    glictSize size;
    position_t pos2;

    caller->GetSize(&size);
    pos->x = (int)(pos->x * VISIBLEWPIXEL / size.w);
    pos->y = (int)(pos->y * VISIBLEHPIXEL / size.h);

    pos->x /= 32;
    pos->y /= 32;

    pos2.x = pos->x - (VISIBLEW / 2) + player->GetPosX();
    pos2.y = pos->y - (VISIBLEH / 2) + player->GetPosY();
    pos2.z = player->GetPosZ();

    //printf("%d %d %d\n", pos2.x, pos2.y, pos2.z);
    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEDOWN);
}
void GM_Gameworld_WorldOnMouseUp (glictPos* pos, glictContainer* caller) {
    glictSize size;
    position_t pos2;

    caller->GetSize(&size);
    pos->x = (int)(pos->x * VISIBLEWPIXEL / size.w);
    pos->y = (int)(pos->y * VISIBLEHPIXEL / size.h);

    pos->x /= 32;
    pos->y /= 32;

    pos2.x = pos->x - (VISIBLEW / 2) + player->GetPosX();
    pos2.y = pos->y - (VISIBLEH / 2) + player->GetPosY();
    pos2.z = player->GetPosZ();

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEUP);
}

void GM_Gameworld_ClickExec(position_t *pos, glictEvents evttype ) {
    // This function decides what happens when player clicks on a location.
    // FIXME (Khaos#1#) Rework from scratch
    static int modifiers;
    static bool moving=false;
    #ifdef USEGLUT
    modifiers = glutGetModifiers();
    #else
    modifiers = 0;
    #endif

    //printf("%d %d %d\n", pos->x, pos->y, pos->z);
    //console.insert(moving ? "moving" : "not moving");
    //console.insert(((GM_Gameworld*)game)->desktop.EvtTypeDescriptor(evttype));

    GM_Gameworld *gw = (GM_Gameworld*)game;
    Creature *cr;
    if (gw->invitingparty) {
        if (evttype == GLICT_MOUSECLICK) {
            gw->invitingparty = false;
            gw->passingparty = false;
            gw->revokingparty = false;
            gw->joiningparty = false;
            win_SetMousePointer("DEFAULT");
            if (cr = gamemap.GetTile(pos)->GetCreature())
                protocol->InviteParty(cr);
        }
        return;
    }
    if (gw->passingparty) {
        if (evttype == GLICT_MOUSECLICK) {
            gw->invitingparty = false;
            gw->passingparty = false;
            gw->revokingparty = false;
            gw->joiningparty = false;
            win_SetMousePointer("DEFAULT");
            if (cr = gamemap.GetTile(pos)->GetCreature())
                protocol->PassLeadershipParty(cr);
        }
        return;
    }
    if (gw->revokingparty) {
        if (evttype == GLICT_MOUSECLICK) {
            gw->invitingparty = false;
            gw->passingparty = false;
            gw->revokingparty = false;
            gw->joiningparty = false;
            win_SetMousePointer("DEFAULT");
            if (cr = gamemap.GetTile(pos)->GetCreature())
                protocol->RevokeInviteParty(cr);
        }
        return;
    }
    if (gw->joiningparty) {
        if (evttype == GLICT_MOUSECLICK) {
            gw->invitingparty = false;
            gw->passingparty = false;
            gw->revokingparty = false;
            gw->joiningparty = false;
            win_SetMousePointer("DEFAULT");
            if (cr = gamemap.GetTile(pos)->GetCreature())
                protocol->JoinParty(cr);
        }
        return;
    }


    if (useex_item2 && (!(
        pos->x == gw->useex_item1_pos.x &&
        pos->y == gw->useex_item1_pos.y &&
        pos->z == gw->useex_item1_pos.z) &&
        evttype == GLICT_MOUSEUP || !moving && evttype == GLICT_MOUSECLICK)

    ) {
        useex_item2 = false;
        Tile *t;
        if (pos->x!=0xFFFF) t = gamemap.GetTile(pos);

        if (moving) {
//            console.insert("SET UP DESTINATION SUCCESSFULLY\n");
            protocol->Move(&(((GM_Gameworld*)game)->useex_item1_pos), ((GM_Gameworld*)game)->useex_item1_stackpos, pos, pos->x!=0xFFFF ? t->GetTopUsableStackpos() : 0, 1);
        } else {
            protocol->Use(&(((GM_Gameworld*)game)->useex_item1_pos), ((GM_Gameworld*)game)->useex_item1_stackpos, pos, pos->x!=0xFFFF ? t->GetTopUsableStackpos() : 0);
        }

        win_SetMousePointer("DEFAULT");
        return;
    }

    if (modifiers & WIN_ACTIVE_SHIFT) {
        //console.insert("SHIFT\n");
        if (evttype == GLICT_MOUSECLICK) protocol->LookAt(pos);
    }
    else if (modifiers & WIN_ACTIVE_ALT) {
        if (pos->x==0xFFFF) return;
        Tile *t = gamemap.GetTile(pos);
        //console.insert("ALT\n");

        if (Creature *c = t->GetCreature())
            if (evttype == GLICT_MOUSECLICK) protocol->Attack(c->GetCreatureID());
    } else if ((modifiers & WIN_ACTIVE_CTRL) || evttype == GLICT_MOUSEDOWN) {
        Tile *t;
        Thing *th;
        //console.insert("CTRL\n");
        if (pos->x!=0xFFFF) {
            t = gamemap.GetTile(pos);
            th = t->GetStackPos(t->GetTopUsableStackpos());
        } else {
            if (!(pos->y & 0x40))
                th = player->inventory[pos->y-1];
            else {
                Container *c = player->GetContainer(pos->y & 0x0F);
                if (!c) return;
                th = c->GetItem(pos->z);
            }

        }
        if (th) {
            unsigned short itemid = th->GetType();


            if (((items[itemid]->usable || items[itemid]->rune) && evttype == GLICT_MOUSECLICK ) || (evttype == GLICT_MOUSEDOWN && !useex_item2 )) {
                // extended usable or move

                if (dynamic_cast<Creature*>(th)) {
                    ObjSpr *s;
                    if (itemid == 0)
                        s = new ObjSpr(th->GetLook().extendedlook, 0);
                    else
                        s = new ObjSpr(itemid, th->GetLook().head,th->GetLook().body,th->GetLook().legs,th->GetLook().feet );
                    s->SetDirection(th->GetDirection());
                    win_SetMousePointer(s);
                } else
                    win_SetMousePointer(new ObjSpr(itemid,0));
                ((GM_Gameworld*)game)->useex_item1_pos = *pos;
                ((GM_Gameworld*)game)->useex_item1_stackpos = pos->x != 0xFFFF ? t->GetTopUsableStackpos() : 0;

                if (!(modifiers & WIN_ACTIVE_CTRL) && evttype == GLICT_MOUSEDOWN) {
                    console.insert("Specify where do you want to move this item", CONLTBLUE);
                    moving  = true;
                    useex_item2 = true;
                } else if (modifiers & WIN_ACTIVE_CTRL && evttype == GLICT_MOUSECLICK) {
                    console.insert("Specify where do you want to use this item", CONLTBLUE);
                    useex_item2 = true;
                }

            } else if (evttype == GLICT_MOUSECLICK) {
                // simple usable
                protocol->Use(pos, pos->x!=0xFFFF ? t->GetTopUsableStackpos() : 0);
                moving = false;
                win_SetMousePointer("DEFAULT");
                useex_item2 = false;
            }



        } else {
            if (pos->x!=0xFFFF) {
                char tmp[256];
                sprintf(tmp, "There's a bug in GetTopUsableStackpos() routine or the tile is empty. %d/%d", t->GetTopUsableStackpos(), t->GetItemCount() );

                console.insert(tmp, CONRED);
            }
        }


    } else if (evttype == GLICT_MOUSECLICK) {
        char tmp [256];
        sprintf(tmp, "You clicked on location (%d, %d, %d)", pos->x, pos->y, pos->z );
        console.insert(tmp, CONWHITE);
        gamemap.GetTile(pos)->ShowContents();
        moving = false;
        win_SetMousePointer("DEFAULT");
        useex_item2 = false;
    }


}
void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {
/*    char tmp[256];
    sprintf(tmp, "%d", (glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots);
    caller->SetCaption(tmp);

    //printf("%s\n", tmp);

    ((glictPanel*)caller)->SetBGColor(
        0,
        0,
        ((glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots)  / 20. + 0.25,
        1. );
*/

    if (clipped->bottom <= clipped->top) return;
    if (clipped->right <= clipped->left) return;
    glViewport((int)clipped->left, (int)(glictGlobals.h - clipped->bottom), (int)(clipped->right - clipped->left), (int)(clipped->bottom - clipped->top));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 32, 0, 32, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
		glLoadIdentity();

		player->RenderInventory((glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots);


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,winw,0,winh, -100, 100);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		glTranslatef(0,-winh,0.0);

		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glViewport(0,0,(int)glictGlobals.w,(int)glictGlobals.h);
    skin.AssureLoadedness();
}
void GM_Gameworld_InvSlotsOnClick(glictPos* pos, glictContainer* caller) {
    char tmp[256];
    sprintf(tmp, "Clicked on %d", (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots));
    console.insert(tmp, CONYELLOW);
    int slot = (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots) + 1;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = slot;
    pos2.z = 0;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSECLICK);


}

void GM_Gameworld_InvSlotsOnMouseDown(glictPos* pos, glictContainer* caller) {
    char tmp[256];
    sprintf(tmp, "MouseDown on %d", (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots));
    console.insert(tmp, CONYELLOW);
    int slot = (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots) + 1;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = slot;
    pos2.z = 0;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEDOWN);


}


void GM_Gameworld_InvSlotsOnMouseUp(glictPos* pos, glictContainer* caller) {
    char tmp[256];
    sprintf(tmp, "MouseUp on %d", (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots));
    console.insert(tmp, CONYELLOW);
    int slot = (glictPanel*)caller - (((GM_Gameworld*)game)->panInvSlots) + 1;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = slot;
    pos2.z = 0;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEUP);


}
void GM_Gameworld_StaStanceOnClick(glictPos* pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;
    gw->btnStaStance1.SetHeight(15);
    gw->btnStaStance2.SetHeight(15);
    gw->btnStaStance3.SetHeight(15);

    caller->SetHeight(20);

    int stance = ((int)caller->GetCustomData());

    gw->stance = (stanceaggression_t)stance;
    protocol->SetStance(gw->stance, gw->chase);

}
void GM_Gameworld_StaChaseOnClick(glictPos* pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;
    bool ov;
    if (ov = (caller->GetCaption() == "X"))
        caller->SetCaption("");
    else
        caller->SetCaption("X");

    if (ov)
        gw->chase = STAND;
    else
        gw->chase = CHASE;

    protocol->SetStance(gw->stance, gw->chase);
}


void GM_Gameworld_StaInviteParty(glictPos *pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;

    gw->invitingparty = true;
    console.insert("Select which player you want to invite to party...", CONLTBLUE);
    win_SetMousePointer("mousequestion.bmp");
}
void GM_Gameworld_StaLeaveParty(glictPos *pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;

    protocol->LeaveParty();
}

void GM_Gameworld_StaRevokeParty(glictPos *pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;

    gw->revokingparty = true;
    console.insert("Select whose invitation you want to revoke...", CONLTBLUE);
    win_SetMousePointer("mousequestion.bmp");
}
void GM_Gameworld_StaJoinParty(glictPos *pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;

    gw->joiningparty = true;
    console.insert("Select whose invitation you want to accept...", CONLTBLUE);
    win_SetMousePointer("mousequestion.bmp");
}
void GM_Gameworld_StaPassParty(glictPos *pos, glictContainer* caller) {
    GM_Gameworld *gw = (GM_Gameworld*)game;

    gw->passingparty = true;
    console.insert("Select who do you want to pass leadership to...", CONLTBLUE);
    win_SetMousePointer("mousequestion.bmp");
}
