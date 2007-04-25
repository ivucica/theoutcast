#include <stdlib.h>
#include <GL/glut.h>
#include <GLICT/globals.h>
#include <GLICT/fonts.h>
#include <math.h>
#include "texmgmt.h"
#include "glutwin.h"
#include "gm_mainmenu.h"
#include "defines.h"
#include "simple_effects.h"
#include "protocol.h"
#include "database.h"
#include "gwlogon.h"
#include "sprfmts.h"
#include "sound.h"
#include "debugprint.h"
#include "options.h"
#include "skin.h"
#include "charlist.h"
int currentspr;

GM_MainMenu::GM_MainMenu() {


    SoundSetMusic("music/logon.mp3");

//    SPRLoader("tibia76.spr");
    currentspr = 1;

    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    city = new Obj3ds("outcastcity.3ds");
    if (!flythrough.load("outcastcity.fly")) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_WARNING, "Failed to load flythrough file!\n");
    }







	desktop.AddObject(&mainmenu);
	mainmenu.SetWidth(200);
	mainmenu.SetHeight(232);


	mainmenu.AddObject(&btnLogIn);
	//mainmenu.AddObject(&btnTutorial);
	mainmenu.AddObject(&btnOptions);
	//mainmenu.AddObject(&btnToS);
	mainmenu.AddObject(&btnAbout);
	mainmenu.AddObject(&btnExit);
	mainmenu.SetCaption("Main Menu");



	btnLogIn.SetBGColor(.6,.6,.6,1.);
	btnLogIn.SetWidth(120);
	btnLogIn.SetCaption("Log In");
	btnLogIn.SetOnClick(GM_MainMenu_LogIn);
	btnLogIn.SetPos(200/2 - 120/2,  32*1 );

	btnTutorial.SetBGColor(.6,.6,.6,1.);
	btnTutorial.SetWidth(120);
	btnTutorial.SetCaption("Tutorial");
	//btnTutorial.SetOnClick(GM_MainMenu_Tutorial);
	btnTutorial.SetPos(200/2 - 120/2,  32*2 );

	btnOptions.SetBGColor(.6,.6,.6,1.);
	btnOptions.SetWidth(120);
	btnOptions.SetCaption("Options");
    btnOptions.SetOnClick(GM_MainMenu_Options);
  //  btnOptions.SetOnClick(GM_MainMenu_NextSprite);
	btnOptions.SetPos(200/2 - 120/2,  32*3 );

	btnToS.SetBGColor(.6, .6, .6, 1.);
	btnToS.SetWidth(120);
	btnToS.SetCaption("Terms of Service");
	btnToS.SetOnClick(GM_MainMenu_ToS);
	btnToS.SetPos(200/2 - 120/2, 232 - 32*3);

	btnAbout.SetBGColor(.6, .6, .6, 1.);
	btnAbout.SetWidth(120);
	btnAbout.SetCaption("About");
	btnAbout.SetOnClick(GM_MainMenu_About);
	btnAbout.SetPos(200/2 - 120/2, 232 - 32*2);

	btnExit.SetBGColor(.6,.6,.6,1.);
	btnExit.SetWidth(120);
	btnExit.SetCaption("Exit");
	btnExit.SetOnClick(GM_MainMenu_Exit);
	btnExit.SetPos(200/2 - 120/2, 232 - 32);


	desktop.AddObject(&login);
	login.SetWidth(300);
	login.SetHeight(180);
	login.SetCaption("Log in");
	login.AddObject(&pnlLogin);
	pnlLogin.SetCaption("Please enter the username, password and \n"
	                    "server address of the server to connect to.\n"
	                    "\n"
	                    "We advise you not to connect to CipSoft's\n"
	                    "servers using this client, as this is a \n"
	                    "breach of Tibia Rules.");
	pnlLogin.SetWidth(300);
	pnlLogin.SetPos(0,0);
	pnlLogin.SetHeight(92);
	pnlLogin.SetBGActiveness(false);

    char tmp[256];

	login.AddObject(&pnlLoginProtocol);
	pnlLoginProtocol.SetCaption("Protocol:");
	pnlLoginProtocol.SetPos(0, 5*15);
	pnlLoginProtocol.SetHeight(14);
	pnlLoginProtocol.SetWidth(70);
	pnlLoginProtocol.SetBGActiveness(false);
	login.AddObject(&txtLoginProtocol);
	txtLoginProtocol.SetPos(100, 5*15);
	txtLoginProtocol.SetHeight(14);
	txtLoginProtocol.SetWidth(150);
	dbLoadSetting("protocol", tmp, 256, "760");
    txtLoginProtocol.SetCaption(tmp);

	login.AddObject(&pnlLoginServer);
	pnlLoginServer.SetCaption("Server:");
	pnlLoginServer.SetPos(0, 6*15);
	pnlLoginServer.SetHeight(14);
	pnlLoginServer.SetWidth(70);
	pnlLoginServer.SetBGActiveness(false);
	login.AddObject(&txtLoginServer);
	txtLoginServer.SetPos(100, 6*15);
	txtLoginServer.SetHeight(14);
	txtLoginServer.SetWidth(150);
	dbLoadSetting("server", tmp, 256, "localhost");
	txtLoginServer.SetCaption( tmp );

	login.AddObject(&pnlLoginUsername);
	pnlLoginUsername.SetCaption("Username:");
	pnlLoginUsername.SetPos(0, 7*15);
	pnlLoginUsername.SetHeight(14);
	pnlLoginUsername.SetWidth(70);
	pnlLoginUsername.SetBGActiveness(false);
	login.AddObject(&txtLoginUsername);
	txtLoginUsername.SetPos(100, 7*15);
	txtLoginUsername.SetHeight(14);
	txtLoginUsername.SetWidth(150);
	dbLoadSetting("username", tmp, 256, "111111");
	txtLoginUsername.SetCaption(tmp);

	login.AddObject(&pnlLoginPassword);
	pnlLoginPassword.SetCaption("Password:");
	pnlLoginPassword.SetPos(0, 8*15);
	pnlLoginPassword.SetHeight(14);
	pnlLoginPassword.SetWidth(70);
	pnlLoginPassword.SetBGActiveness(false);
	login.AddObject(&txtLoginPassword);
	txtLoginPassword.SetPos(100, 8*15);
	txtLoginPassword.SetHeight(14);
	txtLoginPassword.SetWidth(150);
	txtLoginPassword.SetPassProtectCharacter('*');
	dbLoadSetting("password", tmp, 256, "tibia");
	txtLoginPassword.SetCaption(tmp);

	login.AddObject(&btnLoginLogin);
	btnLoginLogin.SetPos(170, 14 + 9*15);
	btnLoginLogin.SetWidth(130);
	btnLoginLogin.SetCaption("Log in");
	btnLoginLogin.SetBGColor(.6,.6,.6,1.);
	btnLoginLogin.SetOnClick(&GM_MainMenu_LoginLogin);
	login.AddObject(&btnLoginCancel);
	btnLoginCancel.SetPos(0, 14 + 9*15);
	btnLoginCancel.SetWidth(130);
	btnLoginCancel.SetCaption("Cancel");
	btnLoginCancel.SetBGColor(.6,.6,.6,1.);
	btnLoginCancel.SetOnClick(&GM_MainMenu_LoginCancel);

	login.SetVisible(false);


	charlist.SetCaption("Logging in...");
	charlist.SetHeight(28);
	charlist.SetWidth(400);
	charlist.SetBGColor(.3,.3,.3,1.);
	charlist.SetVisible(false);

	desktop.AddObject(&characterlist);
	characterlist.SetCaption("Choose a character");
	characterlist.SetVisible(false);

	about.SetCaption("About");
	//about.SetBGColor(.5,.5,.5,1.);
	about.SetOnDismiss(&GM_MainMenu_AboutOnDismiss);
	char abouttext[8192];
	char extensions[7000];
	strcpy(extensions, (char*)glGetString(GL_EXTENSIONS));
	int lenext = strlen(extensions);
	int linelen = 0;
	char outext[7000];
	strcpy(outext,"");
	char outextension[7000];
	for (int i = 0 ; i < lenext ; ) {
		char extension[100];

		int j;
		for (j = i; j < lenext && extensions[j]!=' ' ; ++j);

		extensions[j] = 0;
		strcpy(extension, extensions+i);

		linelen += (int)glictFontSize(extension, "system") + (int)glictFontSize(", ", "system");
		if (linelen < 500) {
			if (i==0) {
				sprintf(outextension, "%s", extension);
			} else {
				sprintf(outextension, "%s, %s", outext, extension);
//				linelen += glictFontSize(",  ", "system");
			}
		}
		else {
			sprintf(outextension, "%s\n%s", outext, extension);
			linelen = (int)glictFontSize(extension, "system");
		}
		strcpy(outext, outextension);

		i = j+1;
	}
	sprintf(abouttext, "%s 0.3.9\n\nCopyright (c) 2005-2007 OBJECT Networks.\nAll rights reserved.\n\nThis software comes with no warranty; authors cannot be held responsible\nfor any kind of data, financial or any other kind of loss,\nnor with any breach of copyright at hands of the end users.\n\nGL vendor: %s\nGL renderer: %s\nGL version: %s\nGL extensions:\n%s", APPTITLE, glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION), outextension);
	about.SetMessage(abouttext);
//    about.SetMessage("oi");
	about.SetHeight(390);
	about.SetWidth(500);

	tos.SetCaption("Terms of Service");
	//about.SetBGColor(.5,.5,.5,1.);
	tos.SetOnDismiss(&GM_MainMenu_ToSOnDismiss);
	char tostext[] = "Terms of usage of the client software is stated in the License Agreement. On \nthe other hand, rights of use of the service are stated here, and by using\nthe service, you also agree to these Terms of Service (\"ToS\" and \"Terms\").\n\nThese Terms are stated here:\n\n"
		"1. OBJECT Networks (\"we\") reserve the right to change this ToS at any time.\n"
		"We shall notify you of each ToS appropriately, upon next login since the\n"
		"change of the ToS.";
	tos.SetMessage(tostext);
	tos.SetHeight(390);
	tos.SetWidth(500);






	desktop.AddObject(&options);
	options.SetVisible(false);
	options.SetCaption("Options");
	options.SetWidth(220);
	options.SetHeight(200);

    options.AddObject(&pnlOptionsRestartWarning);
	pnlOptionsRestartWarning.SetPos(0, 200-16-16);
	pnlOptionsRestartWarning.SetCaption("(*) Won't apply until restart");
	pnlOptionsRestartWarning.SetHeight(16);
	pnlOptionsRestartWarning.SetWidth(220);
	pnlOptionsRestartWarning.SetBGActiveness(false);


    options.AddObject(&pnlOptionsMaptrack);
    pnlOptionsMaptrack.SetPos(0, 20);
    pnlOptionsMaptrack.SetHeight(16);
    pnlOptionsMaptrack.SetWidth(100);
    pnlOptionsMaptrack.SetCaption("Map storing:");
    pnlOptionsMaptrack.SetBGActiveness(false);

    options.AddObject(&btnOptionsMaptrack);
    btnOptionsMaptrack.SetPos(100, 20);
    btnOptionsMaptrack.SetHeight(16);
    btnOptionsMaptrack.SetWidth(16);
    btnOptionsMaptrack.SetOnClick(GM_MainMenu_OptionsCheckbox);
    btnOptionsMaptrack.SetBGColor(.6,.6,.6,1.);

    options.AddObject(&pnlOptionsFullscreen);
    pnlOptionsFullscreen.SetPos(0, 40);
    pnlOptionsFullscreen.SetHeight(16);
    pnlOptionsFullscreen.SetWidth(100);
    pnlOptionsFullscreen.SetCaption("Fullscreen: (*)");
    pnlOptionsFullscreen.SetBGActiveness(false);

    options.AddObject(&btnOptionsFullscreen);
    btnOptionsFullscreen.SetPos(100, 40);
    btnOptionsFullscreen.SetHeight(16);
    btnOptionsFullscreen.SetWidth(16);
    btnOptionsFullscreen.SetOnClick(GM_MainMenu_OptionsCheckbox);
    btnOptionsFullscreen.SetBGColor(.6,.6,.6,1.);

	options.AddObject(&pnlOptionsIntro);
    pnlOptionsIntro.SetPos(0, 60);
    pnlOptionsIntro.SetHeight(16);
    pnlOptionsIntro.SetWidth(100);
    pnlOptionsIntro.SetCaption("Show intro: (*)");
    pnlOptionsIntro.SetBGActiveness(false);

    options.AddObject(&btnOptionsIntro);
    btnOptionsIntro.SetPos(100, 60);
    btnOptionsIntro.SetHeight(16);
    btnOptionsIntro.SetWidth(16);
    btnOptionsIntro.SetOnClick(GM_MainMenu_OptionsCheckbox);
    btnOptionsIntro.SetBGColor(.6,.6,.6,1.);

	options.AddObject(&pnlOptionsOSCursor);
    pnlOptionsOSCursor.SetPos(0, 80);
    pnlOptionsOSCursor.SetHeight(16);
    pnlOptionsOSCursor.SetWidth(100);
	pnlOptionsOSCursor.SetCaption("Use OS cursor:");
    pnlOptionsOSCursor.SetBGActiveness(false);


    options.AddObject(&btnOptionsOSCursor);
    btnOptionsOSCursor.SetPos(100, 80);
    btnOptionsOSCursor.SetHeight(16);
    btnOptionsOSCursor.SetWidth(16);
    btnOptionsOSCursor.SetOnClick(GM_MainMenu_OptionsCheckbox);
    btnOptionsOSCursor.SetBGColor(.6,.6,.6,1.);

	options.AddObject(&pnlOptionsSkin);
    pnlOptionsSkin.SetPos(0, 100);
    pnlOptionsSkin.SetHeight(16);
    pnlOptionsSkin.SetWidth(100);
    pnlOptionsSkin.SetCaption("Skin: ");
    pnlOptionsSkin.SetBGActiveness(false);

    options.AddObject(&txtOptionsSkin);
    txtOptionsSkin.SetPos(100, 100);
    txtOptionsSkin.SetHeight(16);
    txtOptionsSkin.SetWidth(100);
    txtOptionsSkin.SetBGColor(.6,.6,.6,1.);

	options.AddObject(&btnOptionsOk);
	btnOptionsOk.SetPos(220 - 64 - 64 - 5, 200-16);
	btnOptionsOk.SetCaption("Ok");
	btnOptionsOk.SetHeight(16);
	btnOptionsOk.SetWidth(64);
    btnOptionsOk.SetOnClick(GM_MainMenu_OptionsOk);
	btnOptionsOk.SetBGColor(.6,.6,.6,1.);

	options.AddObject(&btnOptionsCancel);
	btnOptionsCancel.SetPos(220 - 64, 200-16);
	btnOptionsCancel.SetCaption("Cancel");
	btnOptionsCancel.SetHeight(16);
	btnOptionsCancel.SetWidth(64);
    btnOptionsCancel.SetOnClick(GM_MainMenu_OptionsCancel);
	btnOptionsCancel.SetBGColor(.6,.6,.6,1.);







	logo = new Texture("logo.bmp");

	bg = new Texture("bg.bmp");

	sine_flag_angle = 0.;
	bg_move_angle = 0.;
	fadein = 1.;
	fadeout = 0.;

	aboutIsOpen = false;
	tosIsOpen = false;

	glDisable(GL_DEPTH_TEST);
}

GM_MainMenu::~GM_MainMenu() {
	delete logo;
	delete bg;
	delete city;

	SoundSetMusic(NULL);
}

void GM_MainMenu::Render() {

    //GM_MainMenu_NextSprite(NULL, NULL);

	//glClear(GL_STENCIL_BUFFER_BIT); <-- already done in glict
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//system("pause");
    if (false) {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.,640.,0.,480.);
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glTranslatef(0,-480.,0.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        bg->Bind();
        glPushMatrix();
        glTranslatef( sin( bg_move_angle * PI / 180. )*5., 0, 0);
        StillEffect(-20, 0, 660., 480., 10, 10, false, true);
        glPopMatrix();


    } else {
//system("pause");
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        GLfloat lightpos[] = { 1., 1., 1., 0. }; // 0 = directional light, 1 = positional light

        if (!flythrough.loaded) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.,640.,0.,480.,-400.,400.);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


            glTranslatef(300., 200., 0.);
            glRotatef(90., -1., 0., 0.);
            glScalef(5.,5.,5.);

        } else {

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, (float)winw/(float)winh, 0.1, 900.0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }


//system("pause");

        glEnable(GL_DEPTH_TEST);

        if (!flythrough.loaded) {
            glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
            city->Render();

        } else {

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, (float)winw/(float)winh, 0.1, 900.0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


            glPushMatrix();
            flythrough.set_cam_pos(fps);
            glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
            city->Render();
            glPopMatrix();

        }


        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);



    }

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.,640.,0.,480.);
    glRotatef(180.0, 1.0, 0.0, 0.0);
    glTranslatef(0,-480.,0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glAlphaFunc(GL_GEQUAL, 0.99);
    logo->Bind();
    StillEffect(200, 0, 425, 100, 2, 2, false, true); // divisions were 40 10








	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,winw,0,winh);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glEnable(GL_SCISSOR_TEST);
	//glTranslatef(winw/2, winh/2,0);
	//glRotatef( sin( bg_move_angle * PI / 180. )*2., 0., 0, 1);
	//glTranslatef(-winw/2, -winh/2,0);
	//desktop.RememberTransformations();
	desktop.Paint();
	glDisable(GL_SCISSOR_TEST);
	glPopMatrix();


    glColor4f(1,1,1,1);
	RenderMouseCursor();

	if (fadein || fadeout) {
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBegin(GL_QUADS);
		if (fadein) {
		    SoundSetMusicVolume((1.-fadein) * 128);
			glColor4f(0,0,0, fadein);
		}
		else {
		    SoundSetMusicVolume((fadeout) * 128);
			glColor4f(0,0,0,1. - fadeout);
		}
		glVertex2f(0, 0);
		glVertex2f(winw, 0);
		glVertex2f(winw, winh);
		glVertex2f(0, winh);


		glColor4f(1.,1.,1.,1.);
		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
	}



	if (fps && mayanimate) sine_flag_angle += 180. / fps;
	if (sine_flag_angle > 360.) sine_flag_angle -= 360.;

	if (fps && mayanimate) bg_move_angle += 45. / fps;
	if (bg_move_angle > 360.) bg_move_angle -= 360.;

	if (fps && mayanimate && fadein > 0.) fadein -= 2. / fps;//.5 / fps;
	if (fadein < 0) fadein = 0.;

	if (fps && mayanimate && fadeout > 0.) fadeout -= 2. / fps;//.5 / fps;
	if (fadeout < 0) {

	    fadeout = 0.;
	    //GameModeEnter(GM_LOADING);
        OnFadeout();
	}

    glColor4f(1,1,1,1);
}

void GM_MainMenu::ResizeWindow() {
	desktop.SetHeight(winh);
	desktop.SetWidth(winw);

    glictGlobals.w = winw;
    glictGlobals.h = winh;
	glictSize s;

	mainmenu.SetPos(winw / 2 - 200 / 2, winh / 2 - 200 / 2);
	about.SetPos(winw / 2 - 500 / 2, winh / 2 - 350 / 2);


	login.GetSize(&s);
	login.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	charlist.GetSize(&s);
	charlist.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	characterlist.GetSize(&s);
	characterlist.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	tos.GetSize(&s);
	tos.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	options.GetSize(&s);
	options.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	glutPostRedisplay();
}

void GM_MainMenu::MouseClick (int button, int shift, int mousex, int mousey) {
	if (shift == GLUT_UP) SoundPlay("sounds/mouse.wav");

	glictPos pos;
	pos.x = mousex;
	pos.y = mousey;
	desktop.TransformScreenCoords(&pos);
	if (shift==GLUT_DOWN) desktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0);
	if (shift==GLUT_UP) desktop.CastEvent(GLICT_MOUSEUP, &pos, 0);

}

void GM_MainMenu::KeyPress (unsigned char key, int x, int y) {
    SoundPlay("sounds/key.wav");
	desktop.CastEvent(GLICT_KEYPRESS, &key, 0);
	glutPostRedisplay();
}

void GM_MainMenu::MsgBox (const char* mbox, const char* title) {
	glictSize s;
	glictMessageBox *mb;
	desktop.AddObject(mb = new glictMessageBox);

	mb->GetSize(&s);

	mb->SetCaption(title);
	mb->SetMessage(mbox);

	mb->SetPos(winw / 2 - s.w / 2, winh / 2 - s.h / 2);

	mb->SetOnDismiss(GM_MainMenu_MBOnDismiss);

}

void GM_MainMenu::CreateCharlist() {
    characterlist.SetHeight(18 * (protocol->charlistcount+1) + 6 + 2);
    characterlist.SetWidth(300);
    characterlist.SetBGColor(.85,.85,.85,1.);
    for (int i = 0 ; i < protocol->charlistcount  ; i++) {
        characterlist.AddObject(protocol->charlist[i].button = new glictButton);
        char tmp[256];
        sprintf(tmp, "%s (%s)", protocol->charlist[i].charactername, protocol->charlist[i].worldname);
        protocol->charlist[i].button->SetCaption(tmp);
        protocol->charlist[i].button->SetWidth(296);
        protocol->charlist[i].button->SetHeight(16);
        protocol->charlist[i].button->SetBGColor(.6,.6,.6,1.);
        protocol->charlist[i].button->SetPos(2, i*18 + 2);
        protocol->charlist[i].button->SetOnClick(GM_MainMenu_CharList_Character);
    }
    characterlist.AddObject(&btnCharlistCancel);
    btnCharlistCancel.SetPos(1, protocol->charlistcount * 18 + 6);
    btnCharlistCancel.SetWidth(298);
    btnCharlistCancel.SetHeight(16);
    btnCharlistCancel.SetBGColor(.8,.6,.6,1.);
    btnCharlistCancel.SetCaption("Cancel");
    btnCharlistCancel.SetOnClick(GM_MainMenu_CharList_Cancel);

    characterlist.SetVisible(true);
    characterlist.Focus(NULL);

    glictSize s;
	characterlist.GetSize(&s);
	characterlist.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);
}
void GM_MainMenu::DestroyCharlist() {
    for (int i = 0 ; i < protocol->charlistcount  ; i++) {
        characterlist.RemoveObject(protocol->charlist[i].button);
        delete protocol->charlist[i].button;
    }
    characterlist.RemoveObject(&btnCharlistCancel);


}

void GM_MainMenu::GoToGameworld() {
    ((GM_MainMenu*)game)->OnFadeout = GM_MainMenu_GoToGameworldDo;
    ((GM_MainMenu*)game)->fadeout = 1.;

}
void GM_MainMenu_GoToGameworldDo() {

	GameModeEnter(GM_GAMEWORLD);
}

void GM_MainMenu_LogIn(glictPos* pos, glictContainer* caller) {

	// when we want to do fadeout:
	//((GM_MainMenu*)game)->fadeout = 1.;
	//((GM_MainMenu*)game)->mainmenu.SetEnabled(0);

	((GM_MainMenu*)game)->login.SetVisible(true);
	((GM_MainMenu*)game)->login.Focus(NULL);

}

void GM_MainMenu_ToS(glictPos* pos, glictContainer* caller) {
	if (!((GM_MainMenu*)game)->tosIsOpen) {
		((GM_MainMenu*)game)->desktop.AddObject(&((GM_MainMenu*)game)->tos);
		((GM_MainMenu*)game)->tosIsOpen = true;
	}
}

void GM_MainMenu_About(glictPos* pos, glictContainer* caller) {
	if (!((GM_MainMenu*)game)->aboutIsOpen) {
		((GM_MainMenu*)game)->desktop.AddObject(&((GM_MainMenu*)game)->about);
		((GM_MainMenu*)game)->aboutIsOpen = true;

	}
}

void GM_MainMenu_Exit(glictPos* pos, glictContainer* caller) {
	((GM_MainMenu*)game)->OnFadeout = GM_MainMenu_ExitDo;
	((GM_MainMenu*)game)->fadeout = 1.;
}
void GM_MainMenu_ExitDo() {
	exit(0);
}
void GM_MainMenu_LoginLogin(glictPos* pos, glictContainer* caller) {
	//((GM_MainMenu*)game)->MsgBox("Logging in soon!", "So long, sire");

	if (((GM_MainMenu*)game)->txtLoginProtocol.GetCaption()=="" ||  ((GM_MainMenu*)game)->txtLoginServer.GetCaption()=="" || ((GM_MainMenu*)game)->txtLoginPassword.GetCaption()=="" || ((GM_MainMenu*)game)->txtLoginUsername.GetCaption()=="") {
		((GM_MainMenu*)game)->MsgBox("Please fill out all the fields.\n\nOr press Cancel if you don't want\nto log in.", "No more secrets");
		return;
	}

    if (!ProtocolSetVersion(atoi(((GM_MainMenu*)game)->txtLoginProtocol.GetCaption().c_str()))) {
        ((GM_MainMenu*)game)->MsgBox("This protocol version is not supported.", "Sorry");
        return;
    }

    FILE *f=NULL;
    char sprfilename[256] = {0};
    char protocolstr[10] = {0};
    switch (protocol->GetProtocolVersion()) {
        case 750:
            strcpy(sprfilename, "tibia75.spr");
            strcpy(protocolstr, "7.5");
            break;
        case 760:
        case 770:
            strcpy(sprfilename, "tibia76.spr");
            strcpy(protocolstr, "7.6");
            break;
        case 790:
            strcpy(sprfilename, "tibia79.spr");
            strcpy(protocolstr, "7.9");
            break;
        case 792:
            strcpy(sprfilename, "tibia792.spr");
            strcpy(protocolstr, "7.92");
            break;
        default:
            strcpy(sprfilename, "ERROR");
            strcpy(protocolstr, "<<INTERNAL ERROR>>");

    }
    if (strlen(sprfilename)) f = fopen(sprfilename, "rb");
    if (!f ) {
        char sprerr[256];
        sprintf(sprerr, "For this protocol,\nyou must copy Tibia.spr from Tibia\n%s client's folder to The\nOutcast's folder and rename it into \n%s.", protocolstr, sprfilename);
        ((GM_MainMenu*)game)->MsgBox(sprerr, "Sorry");
        return;
    } else fclose(f);

    ((GM_MainMenu*)game)->ResizeWindow();
	((GM_MainMenu*)game)->login.SetVisible(false);
	((GM_MainMenu*)game)->desktop.AddObject(&((GM_MainMenu*)game)->charlist);
	((GM_MainMenu*)game)->charlist.SetEnabled(false);
	((GM_MainMenu*)game)->charlist.SetVisible(true);
	((GM_MainMenu*)game)->charlist.Focus(NULL);
	((GM_MainMenu*)game)->mainmenu.SetEnabled(false);

    ((GM_MainMenu*)game)->charlist.SetCaption("Please wait...");
	((GM_MainMenu*)game)->charlist.SetMessage("Initializing...");
	((GM_MainMenu*)game)->thrCharList = ONNewThread(Thread_CharList, game); //CreateThread(NULL, 0, Thread_CharList, ((GM_MainMenu*)game), 0, &((GM_MainMenu*)game)->thrCharListId);
	//((GM_MainMenu*)game)->MsgBox("Loading","oi");

}

void GM_MainMenu_LoginCancel(glictPos* pos, glictContainer* caller) {
	((GM_MainMenu*)game)->login.SetVisible(false);
	((GM_MainMenu*)game)->mainmenu.Focus(NULL);
}

void GM_MainMenu_CharList_LogonOK(glictPos* pos, glictContainer* caller) {
    ((GM_MainMenu*)game)->CreateCharlist();

    ((GM_MainMenu*)game)->login.SetEnabled(true);
    ((GM_MainMenu*)game)->login.SetVisible(false);
    ((GM_MainMenu*)game)->charlist.SetHeight(28);
    ((GM_MainMenu*)game)->charlist.SetVisible(false);
}

void GM_MainMenu_CharList_LogonError(glictPos* pos, glictContainer* caller) {
    ((GM_MainMenu*)game)->login.SetEnabled(true);
    ((GM_MainMenu*)game)->login.SetVisible(false);
    ((GM_MainMenu*)game)->charlist.SetHeight(28);
    ((GM_MainMenu*)game)->charlist.SetVisible(false);

    ((GM_MainMenu*)game)->mainmenu.SetEnabled(true);
}

void GM_MainMenu_CharList_Character(glictPos* pos, glictContainer* caller) {
    for (int i = 0 ; i < protocol->charlistcount ; i++) {
        if (caller == protocol->charlist[i].button) {
            //((GM_MainMenu*)game)->MsgBox(protocol->charlist[i].charactername, protocol->charlist[i].worldname);
            /*char tmp [1024];
            sprintf(tmp, "%s:\n%d.%d.%d.%d\n%d\n\nNothing else is done, move along now... :)", protocol->charlist[i].worldname,
                                            ((unsigned char*)&protocol->charlist[i].ipaddress)[0],
                                            ((unsigned char*)&protocol->charlist[i].ipaddress)[1],
                                            ((unsigned char*)&protocol->charlist[i].ipaddress)[2],
                                            ((unsigned char*)&protocol->charlist[i].ipaddress)[3],
                                            protocol->charlist[i].port);
            ((GM_MainMenu*)game)->MsgBox(tmp, protocol->charlist[i].charactername);*/
            protocol->SetCharacter(i);
            ((GM_MainMenu*)game)->characterlist.SetVisible(false);
            ((GM_MainMenu*)game)->charlist.SetVisible(true);
            ((GM_MainMenu*)game)->charlist.SetCaption("Entering game...");
            ((GM_MainMenu*)game)->charlist.SetMessage("Initializing...");
            ((GM_MainMenu*)game)->charlist.SetEnabled(false);
            ((GM_MainMenu*)game)->desktop.AddObject(&((GM_MainMenu*)game)->charlist);
            ((GM_MainMenu*)game)->thrGWLogon = ONNewThread(Thread_GWLogon, game);
            return;
        }
    }
}

void GM_MainMenu_CharList_Cancel(glictPos* pos, glictContainer* caller) {
    ((GM_MainMenu*)game)->DestroyCharlist();

    ((GM_MainMenu*)game)->characterlist.SetVisible(false);

    ((GM_MainMenu*)game)->mainmenu.SetEnabled(true);
}

void GM_MainMenu_AboutOnDismiss(glictPos* pos, glictContainer* caller) {
	((GM_MainMenu*)game)->aboutIsOpen = false;
}

void GM_MainMenu_ToSOnDismiss(glictPos* pos, glictContainer* caller) {
	((GM_MainMenu*)game)->tosIsOpen = false;
}
void GM_MainMenu_MBOnDismiss(glictPos* pos, glictContainer* caller) {
	((GM_MainMenu*)game)->desktop.RemoveObject(caller);
	delete caller;
}


void GM_MainMenu_Options(glictPos* pos, glictContainer *caller) {
    ((GM_MainMenu*)game)->mainmenu.SetEnabled(false);
    ((GM_MainMenu*)game)->options.SetVisible(true);
    ((GM_MainMenu*)game)->options.Focus(NULL);

    if (options.maptrack) ((GM_MainMenu*)game)->btnOptionsMaptrack.SetCaption("X"); else ((GM_MainMenu*)game)->btnOptionsMaptrack.SetCaption("");
    if (options.fullscreen) ((GM_MainMenu*)game)->btnOptionsFullscreen.SetCaption("X"); else ((GM_MainMenu*)game)->btnOptionsFullscreen.SetCaption("");
	if (options.intro) ((GM_MainMenu*)game)->btnOptionsIntro.SetCaption("X"); else ((GM_MainMenu*)game)->btnOptionsIntro.SetCaption("");
	if (options.os_cursor) ((GM_MainMenu*)game)->btnOptionsOSCursor.SetCaption("X"); else ((GM_MainMenu*)game)->btnOptionsOSCursor.SetCaption("");

	((GM_MainMenu*)game)->txtOptionsSkin.SetCaption(options.skin);
}
void GM_MainMenu_OptionsOk(glictPos* pos, glictContainer *caller) {



	// first, do some argument check

	{// skin
		char tmp[256];
		sprintf(tmp, "skins/%s/window/tl.bmp", ((GM_MainMenu*)game)->txtOptionsSkin.GetCaption().c_str());
		FILE *f = fopen(tmp,"r");
		if (!f) {
			sprintf(tmp, "You have entered invalid skin name,\n '%s'.\n\nPlease check that the skin exists.\n(To see available skins, open subfolder 'skins/'.)", ((GM_MainMenu*)game)->txtOptionsSkin.GetCaption().c_str());
			((GM_MainMenu*)game)->MsgBox(tmp, "Skin name invalid");
			return;
		} else {
			fclose(f);
		}
	}

    GM_MainMenu_OptionsCancel(pos, caller);
    game->Render();
    glutSwapBuffers();



    options.maptrack = ((GM_MainMenu*)game)->btnOptionsMaptrack.GetCaption() == "X";
    options.fullscreen = ((GM_MainMenu*)game)->btnOptionsFullscreen.GetCaption() == "X";
	options.intro = ((GM_MainMenu*)game)->btnOptionsIntro.GetCaption() == "X";
	options.os_cursor = ((GM_MainMenu*)game)->btnOptionsOSCursor.GetCaption() == "X";

	options.skin = ((GM_MainMenu*)game)->txtOptionsSkin.GetCaption();

	glut_SetMousePointer("DEFAULT");
	skin.Load(options.skin.c_str());
	((GM_MainMenu*)game)->ResizeWindow();
    options.Save();
}
void GM_MainMenu_OptionsCancel(glictPos* pos, glictContainer *caller) {
    ((GM_MainMenu*)game)->mainmenu.SetEnabled(true);
    ((GM_MainMenu*)game)->options.SetVisible(false);
    ((GM_MainMenu*)game)->mainmenu.Focus(NULL);
}
void GM_MainMenu_OptionsCheckbox(glictPos* pos, glictContainer *caller) {
    if (caller->GetCaption() == "X")
        caller->SetCaption("");
    else
        caller->SetCaption("X");
}
void GM_MainMenu_NextSprite(glictPos* pos, glictContainer* caller) {

    delete ((GM_MainMenu*)game)->logo;
    currentspr ++;
    int w = currentspr;
    switch (currentspr) {
        case 0:
            w = 514;
            break;
        case 1:
            w = 320;
            break;
        case 2:
            w = 260;
            break;
        case 3:
            w = 769;
            break;
        case 4:
            w = 5470;
            break;
        case 5:
            w = 0;
            break;
        case 6:
            w = 5469;
            break;
        case 7:
            w = 0;
            break;
        case 8:
            w = 5452;
            break;
        case 9:
            w = 5451;
            break;
        case 10:
            w = 0;
            break;
        case 11:
            w = 0;
            break;
        case 12:
            w = 5464;
            break;
        case 13:
            w = 0;

        default:
        break;
    }
    ((GM_MainMenu*)game)->logo = new Texture ("tibia76.spr", w);
    //((GM_MainMenu*)game)->logo = new Texture ("tibia76.spr", currentspr++);
}

void GM_MainMenu::SetLoginStatus(const char *loginstatus) {
    charlist.SetMessage(loginstatus);
}
