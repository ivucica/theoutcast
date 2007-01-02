#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <ctype.h>

#include "glutwin.h"
#include "gm_logo.h"
#include "flythrough.h"
int poiksu=0, poipsilonu=0, pozeu=0;




GM_Logo::GM_Logo () {
    logo = new Obj3ds("objnet.3ds");
    if (!flythrough.load("objnet.fly")) printf("Failed to load flythrough file!\n");
    flythrough.set_on_finish(GM_Logo_OnFinish, this);
    done = false;
}
void GM_Logo::Render() {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    if (!flythrough.loaded) {

        logo->Render();

    } else {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (float)winw/(float)winh, 0.1, 900.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glPushMatrix();
        flythrough.set_cam_pos(mayanimate ? fps : 90000);
        if (!done) logo->Render();
        glPopMatrix();

        if (done) GameModeEnter(GM_MAINMENU);
    }


}

void GM_Logo::KeyPress (unsigned char key, int x, int y) {
    if (tolower(key) == 'd')
        poiksu++;//=1;
    if (tolower(key) == 'a')
        poiksu--;//=1;
    if (tolower(key) == 's')
        poipsilonu--;//=1;
    if (tolower(key) == 'w')
        poipsilonu++;//=1;
    if (tolower(key) == 'r')
        pozeu++;//=1;
    if (tolower(key) == 'f')
        pozeu--;//=1;


    this->OnFinish();

}
void GM_Logo::ResizeWindow (int w, int h) {
    if (!flythrough.loaded) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.,640.,0.,480.,-400.,400.);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glTranslatef(300., 200., 0.);
        glTranslatef(poiksu*10, poipsilonu*20, pozeu*20);
        glRotatef(90., -1., 0., 0.);
        glScalef(5.,5.,5.);

    } else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (float)winw/(float)winh, 0.1, 900.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}


void GM_Logo::OnFinish() {
    done = true;
}


void GM_Logo_OnFinish (void *arg) {
    GM_Logo* logo = (GM_Logo*)arg;

    logo->OnFinish();
}
