#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <ctype.h>

#include "glutwin.h"
#include "gm_logo.h"
#include "flythrough.h"
#include "sound.h"
#include "debugprint.h"
int poiksu=0, poipsilonu=0, pozeu=0;




GM_Logo::GM_Logo () {
    logo = new Obj3ds("objnet.3ds");
    if (!flythrough.load("objnet.fly")) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_WARNING, "Failed to load flythrough file!\n");
    }
    flythrough.set_on_finish(GM_Logo_OnFinish, this);
    done = false;
    fadein = 1.;
    fadeout = 0;
    oldmayanimate = false;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

	glut_SetMousePointer(NULL);


}
GM_Logo::~GM_Logo() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destructing logo\n");
	glut_SetMousePointer("DEFAULT");
    if (logo) delete logo;
    SoundSetMusic(NULL);
}
void GM_Logo::Render() {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

//    printf("Clearing\n");
    if (!oldmayanimate && mayanimate) {
        oldmayanimate = true;
        SoundSetMusic("music/logo.mp3");
    }
    if (!flythrough.loaded) {

        logo->Render();

    } else {

//	printf("Drawing\n");
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (float)winw/(float)winh, 1., 900.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


//	printf("Setting campos\n");
        glPushMatrix();
        flythrough.set_cam_pos(mayanimate && fps > 2. ? fps : 90000);

        static GLfloat lightpos[] = { 1., 1., 1., 0. }; // 0 = directional light, 1 = positional light
//	printf("Setting lightpos\n");
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

//	printf("Rendering\n");
        if (!done)
            logo->Render();
        glPopMatrix();

//	printf("Checking doneness\n");
        if (done)
            GameModeEnter(GM_MAINMENU);

    }


    if (false) if (fadein>0 || fadeout>0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
//	printf("Switching to ortho\n");
//        glOrtho(0.,640.,0.,480.,-400.,400.);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
//

		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBegin(GL_QUADS);
		if (fadein) {
		    SoundSetMusicVolume((int)((1.-fadein) * 128));
			glColor4f(0,0,0, fadein);
		}
		else {
		    SoundSetMusicVolume((int)((fadeout) * 128));
			glColor4f(0,0,0,1. - fadeout);
		}
		glVertex2f(0, 0);
		glVertex2f(0, winh);
		glVertex2f(winw, winh);
		glVertex2f(winw, 0);
		glColor4f(1.,1.,1.,1.);
		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);

	}

	if (fps && mayanimate && fadein > 0.) fadein -= 2. / fps;//.5 / fps;
	if (fadein < 0) fadein = 0.;

	if (fps && mayanimate && fadeout > 0.) fadeout -= 2. / fps;//.5 / fps;
	//if (mayanimate) printf("fadein %f fadeout %f step %f fps %f\n", fadein, fadeout, 2./fps, fps);

	if (fadeout < 0) {

          //  printf("DONE!!!!!\n");
	    fadeout = 0.;
	    OnFinish();

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

    fadeout = 1.;

//    this->OnFinish();
    printf("Stisnuto\n");

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

//    fadeout = 1.;
}


void GM_Logo_OnFinish (void *arg) {
    printf("FINISH!\n");
    GM_Logo* logo = (GM_Logo*)arg;

    logo->OnFinish();

}
