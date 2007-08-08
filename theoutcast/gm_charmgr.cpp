#ifdef WIN32
    #include <windows.h>
#endif
#include <stdio.h>
#include <math.h>
#ifndef _MSC_VER
	#include <unistd.h> // unlink
#else
	#define unlink _unlink
#endif
#include <GL/gl.h>
#include <GLICT/globals.h>
#include <GLICT/messagebox.h>
#include "gm_charmgr.h"
#include "simple_effects.h"
#include "windowing.h"
#include "protocol.h"
#include "sound.h"
#include "defines.h"
void GM_CharMgr_CharList_Character(glictPos* pos, glictContainer *caller);
static void GM_CharMgr_CharList_Cancel(glictPos* pos, glictContainer *caller);
void GM_CharMgr_MBOnDismiss(glictPos* pos, glictContainer* caller);
void GM_CharMgr_CreateCharacter_Ok(glictPos* pos, glictContainer *caller);
void GM_CharMgr_CreateCharacter_Cancel(glictPos* pos, glictContainer *caller);
void GM_CharMgr_CreateCharacter_Vocation(glictPos* pos, glictContainer *caller);
GM_CharMgr::GM_CharMgr() {
    ONInitThreadSafe(threadsafe);
    bg = new Texture("charmgr.bmp" );


    desktop.AddObject(&winCharacterList);
    winCharacterList.SetCaption("Character Manager");
    winCharacterList.SetWidth(200);
    winCharacterList.SetHeight(50);

    desktop.AddObject(&winCreateCharacter);
    winCreateCharacter.SetCaption("Create Character");
    winCreateCharacter.SetWidth(200);
    winCreateCharacter.SetHeight(150);
    winCreateCharacter.SetVisible(false);

        winCreateCharacter.AddObject(&btnCreateCharacterOk);
        btnCreateCharacterOk.SetCaption("Ok");
        btnCreateCharacterOk.SetOnClick(GM_CharMgr_CreateCharacter_Ok);
        btnCreateCharacterOk.SetHeight(20);
        btnCreateCharacterOk.SetWidth(50);
        btnCreateCharacterOk.SetPos(135, 130);

        winCreateCharacter.AddObject(&btnCreateCharacterCancel);
        btnCreateCharacterCancel.SetCaption("Cancel");
        btnCreateCharacterCancel.SetOnClick(GM_CharMgr_CreateCharacter_Cancel);
        btnCreateCharacterCancel.SetHeight(20);
        btnCreateCharacterCancel.SetWidth(50);
        btnCreateCharacterCancel.SetPos(35, 130);

        winCreateCharacter.AddObject(&pnlCreateCharacterName);
        pnlCreateCharacterName.SetCaption("Character name:");
        pnlCreateCharacterName.SetWidth(130);
        pnlCreateCharacterName.SetHeight(16);
        pnlCreateCharacterName.SetPos(0,0);
        pnlCreateCharacterName.SetBGActiveness(false);

        winCreateCharacter.AddObject(&txtCreateCharacterName);
        txtCreateCharacterName.SetCaption("");
        txtCreateCharacterName.SetPos(0,14);

        winCreateCharacter.AddObject(&pnlCreateCharacterVocation);
        pnlCreateCharacterVocation.SetCaption("...");
        pnlCreateCharacterVocation.SetPos(9, 36);
        pnlCreateCharacterVocation.SetHeight(16);
        pnlCreateCharacterVocation.SetWidth(150);
        pnlCreateCharacterVocation.SetBGActiveness(false);

        winCreateCharacter.AddObject(&scbCreateCharacterVocation);
        scbCreateCharacterVocation.SetPos(0, 28);
        scbCreateCharacterVocation.SetWidth(9);
        scbCreateCharacterVocation.SetHeight(9*3);
        scbCreateCharacterVocation.SetOnClick(GM_CharMgr_CreateCharacter_Vocation);

    CreateCharlist();

    vocationlist.insert(vocationlist.end(), std::string("Knight"));
    vocationlist.insert(vocationlist.end(), std::string("Paladin"));
    vocationlist.insert(vocationlist.end(), std::string("Druid"));
    vocationlist.insert(vocationlist.end(), std::string("Sorcerer"));

    scbCreateCharacterVocation.SetMin(0);
    scbCreateCharacterVocation.SetMax(vocationlist.size()-1);
    scbCreateCharacterVocation.SetValue(0);

    pnlCreateCharacterVocation.SetCaption(vocationlist[scbCreateCharacterVocation.GetValue()]);

}
GM_CharMgr::~GM_CharMgr() {
    ONDeinitThreadSafe(threadsafe);
    delete bg;
}

void GM_CharMgr::Render() {

    #ifdef WALLHACK
    glClearColor(0.,0.,0.,1.);
    glAlphaFunc(GL_GREATER, 0.2);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    #endif

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
    StillEffect(0, 0, 640., 480., 10, 10, false, true);
    glPopMatrix();

///////// ui

//DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Painting UI\n");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,winw,0,winh);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glTranslatef(0,-winh,0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glEnable(GL_SCISSOR_TEST);
    ONThreadSafe(threadsafe);
	desktop.Paint();
	ONThreadUnsafe(threadsafe);
	glDisable(GL_SCISSOR_TEST);
	glPopMatrix();

    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Painting cursor\n");

    glColor4f(1,1,1,1);
	RenderMouseCursor();



}
void GM_CharMgr::SpecKeyPress(int key, int x, int y ) {
    printf("SKP\n");
    //GameModeEnter(GM_MAINMENU);
}

void GM_CharMgr::ResizeWindow() {
    printf("Resize\n");
    desktop.SetHeight(winh);
	desktop.SetWidth(winw);

    glictGlobals.w = winw;
    glictGlobals.h = winh;
	glictSize s;

	winCharacterList.GetSize(&s);
	winCharacterList.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

	winCreateCharacter.GetSize(&s);
	winCreateCharacter.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);

}


void GM_CharMgr::MouseClick (int button, int shift, int mousex, int mousey) {

	glictPos pos;
	pos.x = mousex;
	pos.y = mousey;
	desktop.TransformScreenCoords(&pos);
	if (shift==WIN_PRESS) desktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0);
	if (shift==WIN_RELEASE) {
	    if (desktop.CastEvent(GLICT_MOUSEUP, &pos, 0))
	        SoundPlay("sounds/mouse.wav");

	}

}
void GM_CharMgr::KeyPress (unsigned char key, int x, int y) {
    SoundPlay("sounds/key.wav");
    if (key == 27)
        GameModeEnter(GM_MAINMENU);
    else
        desktop.CastEvent(GLICT_KEYPRESS, &key, 0);
	#ifdef USEGLUT
	glutPostRedisplay();
	#endif
}


void GM_CharMgr::CreateCharlist() {
    winCharacterList.SetHeight(18 * (protocol->charlistcount+1) + 6 + 2);
    winCharacterList.SetWidth(300);
    winCharacterList.SetBGColor(.85,.85,.85,1.);
    static int added;

    added = 0;

    winCharacterList.AddObject(&btnCharlistCreate);
    btnCharlistCreate.SetCaption("Create Character");
    btnCharlistCreate.SetWidth(296);
    btnCharlistCreate.SetHeight(16);
    btnCharlistCreate.SetBGColor(.6,.6,.6,1.);
    btnCharlistCreate.SetPos(2, added*18 + 2);
    btnCharlistCreate.SetOnClick(GM_CharMgr_CharList_Character);
    btnCharlistCreate.SetCustomData((void*)added);
    added++;

    for (int i = 0 ; i < protocol->charlistcount  ; i++) {

        char tmp[256];
        if (strcmp(protocol->charlist[i]->charactername, "Character Manager")) {
            winCharacterList.AddObject(protocol->charlist[i]->button = new glictButton);
            sprintf(tmp, "%s (%s)", protocol->charlist[i]->charactername, protocol->charlist[i]->worldname);
            protocol->charlist[i]->button->SetCaption(tmp);
            protocol->charlist[i]->button->SetWidth(296);
            protocol->charlist[i]->button->SetHeight(16);
            protocol->charlist[i]->button->SetBGColor(.6,.6,.6,1.);
            protocol->charlist[i]->button->SetPos(2, added*18 + 2);
            protocol->charlist[i]->button->SetOnClick(GM_CharMgr_CharList_Character);
            protocol->charlist[i]->button->SetCustomData((void*)added);
            added++;
        }
    }


    winCharacterList.AddObject(&btnCharlistCancel);
    btnCharlistCancel.SetPos(1, added * 18 + 6);
    btnCharlistCancel.SetWidth(298);
    btnCharlistCancel.SetHeight(16);
    btnCharlistCancel.SetBGColor(.8,.6,.6,1.);
    btnCharlistCancel.SetCaption("Done");
    btnCharlistCancel.SetOnClick(GM_CharMgr_CharList_Cancel);

    winCharacterList.SetVisible(true);
    winCharacterList.Focus(NULL);


    glictSize s;
	winCharacterList.GetSize(&s);
	winCharacterList.SetPos(winw / 2 - s.w / 2, winh/2 - s.h / 2);
}
void GM_CharMgr::DestroyCharlist() {
    ONThreadSafe(threadsafe);

    for (int i = 0 ; i < protocol->charlistcount  ; i++) {
        winCharacterList.RemoveObject(protocol->charlist[i]->button);
        if (protocol->charlist[i]->button) delete protocol->charlist[i]->button;
        protocol->charlist[i]->button = NULL;
    }
    winCharacterList.RemoveObject(&btnCharlistCreate);
    winCharacterList.RemoveObject(&btnCharlistCancel);
    winCharacterList.DelayedRemove();

    ONThreadUnsafe(threadsafe);

}

void GM_CharMgr::MsgBox (const char* mbox, const char* title) {
	glictSize s;
	glictMessageBox *mb;
	desktop.AddObject(mb = new glictMessageBox);

	mb->GetSize(&s);

	mb->SetCaption(title);
	mb->SetMessage(mbox);

	mb->SetPos(winw / 2 - s.w / 2, winh / 2 - s.h / 2);

	mb->SetOnDismiss(GM_CharMgr_MBOnDismiss);
}
void GM_CharMgr::ShowCharList() {
    CreateCharlist();
    winCharacterList.SetVisible(true);
    winCreateCharacter.SetVisible(false);
}

void GM_CharMgr::ShowCreateCharacter() {
    DestroyCharlist();
    winCharacterList.SetVisible(false);
    winCreateCharacter.SetVisible(true);
}

void GM_CharMgr_MBOnDismiss(glictPos* pos, glictContainer* caller) {
	((GM_CharMgr*)game)->desktop.RemoveObject(caller);
	delete caller;
}

void GM_CharMgr_CharList_Character(glictPos* pos, glictContainer *caller) {
    glictButton* b = (glictButton*)caller;
    GM_CharMgr *clc = (GM_CharMgr*)game;
    if (((int)b->GetCustomData()) == 0) {

        clc->winCharacterList.SetVisible(false);
        protocol->OCMCreateCharacter();
    } else {

    }
}


void GM_CharMgr_CreateCharacter_Ok(glictPos* pos, glictContainer *caller) {

    GM_CharMgr *clc = (GM_CharMgr*)game;

    printf("Creating %s\n", (std::string("save/") + protocol->username + ".ous").c_str());

    if (fileexists((std::string("save/") + protocol->username + "_" + clc->txtCreateCharacterName.GetCaption() + ".ocs").c_str())) {
        clc->MsgBox("Character already exists", "Error");
        return;
    }

    FILE *f = fopen( (std::string("save/") + protocol->username + "_" + clc->txtCreateCharacterName.GetCaption() + ".ocs").c_str(), "w");
    if (!f) {
        clc->MsgBox("Failed to open character file for writing", "Error");
        return;
    }
    fclose(f);


    f = fopen( (std::string("save/") + protocol->username + ".ous").c_str(), "a");
    if (!f) {
        unlink((std::string("save/") + protocol->username + "_" + clc->txtCreateCharacterName.GetCaption() + ".ocs").c_str());
        clc->MsgBox("Failed to open account file for appending", "Error");
        return;
    }
    fprintf(f, "%s\n", clc->txtCreateCharacterName.GetCaption().c_str());

    fclose(f);


    clc->winCreateCharacter.SetVisible(false);
    protocol->OCMCharlist();
}
void GM_CharMgr_CreateCharacter_Cancel(glictPos* pos, glictContainer *caller) {
    GM_CharMgr *clc = (GM_CharMgr*)game;
    clc->winCreateCharacter.SetVisible(false);
    protocol->OCMCharlist();
}

static void GM_CharMgr_CharList_Cancel(glictPos* pos, glictContainer *caller) {
    GameModeEnter(GM_MAINMENU);
}

void GM_CharMgr_CreateCharacter_Vocation(glictPos* pos, glictContainer *caller) {
    GM_CharMgr *clc = (GM_CharMgr*)game;

    if ((unsigned)clc->scbCreateCharacterVocation.GetValue() >= clc->vocationlist.size()) {
        printf("Problem.\n");
        return;
    }
    clc->pnlCreateCharacterVocation.SetCaption(clc->vocationlist[clc->scbCreateCharacterVocation.GetValue()]);
}
