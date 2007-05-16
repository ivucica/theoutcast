#ifdef _MSC_VER
    #include <stdlib.h> // doesnt like exit() defined elsewhere.
#endif

#include <GL/glut.h>

#include "gm_sprplayground.h"
#include "items.h"
#include "creatures.h"
#include "effects.h"

#include "objspr.h"
#include "sprfmts.h"
#include "debugprint.h"
#include "glutwin.h"
#include "console.h"
extern float ItemAnimationPhase;
extern unsigned int ItemSPRAnimationFrame;

static bool divtest;
static unsigned int counter=100;
static bool creaturetest;
static position_t offset;
static bool splashtest;
GM_SPRPlayground::GM_SPRPlayground() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Constructing SPR playground\n");


    SPRLoader("Tibia792.spr");
    ItemsLoad_NoUI(792);
    CreaturesLoad_NoUI(792);
    EffectsLoad_NoUI(792);

    offset.x = 0; offset.y = 0;
    g = NULL;
    creaturetest = false;
    splashtest = false;
    divtest = false;

    // animated example
    #if 0
    delete g;
    g = new ObjSpr(5022, 0, 792);
    #endif

    // blendframes
    #if 0
    delete g;
    g = new ObjSpr(149, 0, 792);
    #endif

    // blendframes + xdivydiv
    #if 0
    delete g;
    g = new ObjSpr(173, 0, 792);
    divtest = true;
    #endif

    // xdivydiv
    #if 0
    delete g;
    g = new ObjSpr(107, 0, 792);
    divtest = true;
    #endif

    // creature, without suit (human, oldstyle)
    #if 0
    delete g;
    g = new ObjSpr(127, 1, 792);
    creaturetest = true;
    #endif

    // creature, without suit 2 (demon)
    #if 0
    delete g;
    g = new ObjSpr(35, 1, 792);
    creaturetest = true;
    #endif

    // creature, with suit (human paladin)
    #if 0
    delete g;
    g = new ObjSpr(128, 792, 50, 90, 110, 120);
    creaturetest = true;
    #endif


    // splash test
    #if 0
    delete g;
    g = new ObjSpr(2889, 0, 792);
    splashtest = true;
    #endif

    // fluid container test
    #if 1
    delete g;
    g = new ObjSpr(2524, 0, 792);
    splashtest = true;
    #endif

    // effect
    #if 0
    delete g;
    g = new ObjSpr(2, 2, 792);
    #endif

    if (splashtest)
        counter = 0;

    if (!g) {
        printf("No test chosen. Aborting.\n");
        glutHideWindow();
        system("pause");
        exit(1);
    }

    console.insert(" !\"#$%&'()");
}


GM_SPRPlayground::~GM_SPRPlayground() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destructing SPR playground\n");
    ItemsUnload();
    CreaturesUnload();
    SPRUnloader();
}

void GM_SPRPlayground::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -100, 100);




    console.draw(10);


    glPushMatrix();
    glTranslatef(216, 216, 0);

    position_t p = {0,0,0};

    if (divtest) {
        for (int i = 0; i < 4; i++) {
            p.x=i;
            for (int j = 0; j < 4; j++) {
                p.y=j;
                glPushMatrix();
                glTranslatef(32 * (i), 32 * (3-j), 0);
                g->Render(&p);
                glPopMatrix();
            }
        }
    } else
        if (splashtest)
            g->Render(counter);
        else
            g->Render(&p);

    g->AnimationAdvance(400./fps);
    glPopMatrix();

    RenderMouseCursor();
}

void GM_SPRPlayground::SpecKeyPress(int key, int x, int y ) {
    if (!creaturetest) {
        switch (key) {
            case GLUT_KEY_UP:
                counter+=100;
                break;
            case GLUT_KEY_DOWN:
                if (counter>=200) counter-=100;
                break;
            case GLUT_KEY_LEFT:
                if (counter-100) counter--;
                break;
            case GLUT_KEY_RIGHT:
                counter++;
                break;
        }
        if (splashtest) {
            // leave item as is, on next render use the counter for subtype ...
        } else {
            delete g;
            g = new ObjSpr(counter, 0, 792);
            if (items[counter]->sli.xdiv>1 || items[counter]->sli.ydiv>1 ) divtest = true; else divtest = false;
        }
        glutPostRedisplay();

        char tmp[256];
        sprintf(tmp, "%d", counter);
        console.insert(tmp, CONORANGE);
    } else {
        switch (key) {
            case GLUT_KEY_UP:
                offset.y ++;
                g->SetDirection(NORTH);
                break;
            case GLUT_KEY_DOWN:
                if (offset.y) offset.y --;
                g->SetDirection(SOUTH);
                break;
            case GLUT_KEY_LEFT:
                if (offset.x) offset.x --;
                g->SetDirection(WEST);
                break;
            case GLUT_KEY_RIGHT:
                offset.x++;
                g->SetDirection(EAST);
                break;

        }
        char tmp[256];
        sprintf(tmp, "%d %d", offset.x, offset.y);
        console.insert(tmp, CONORANGE);
    }
}
