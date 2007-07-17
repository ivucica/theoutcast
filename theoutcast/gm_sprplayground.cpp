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
#include "windowing.h"
#include "console.h"
extern float ItemAnimationPhase;
extern unsigned int ItemSPRAnimationFrame;

static bool divtest;
static unsigned int counter=167;//102;
static bool creaturetest;
static position_t offset;
static bool splashtest;

#define BIGARRAYSIZE 200
#define BAMULTI 4
Texture *bigarray[BIGARRAYSIZE] = {NULL};
ObjSpr *objbigarray[BIGARRAYSIZE] = {NULL};

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


    // default test
    #if 1
    delete g;
    g = new ObjSpr(counter, 0, 792);
    #endif


    // simple test
    #if 0
    delete g;
    g = new ObjSpr(102, 0, 792);
    #endif

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
    g = new ObjSpr(100, 0, 792);
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
    #if 0
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
    if (creaturetest)
        counter = 1;

    if (!g) {
        printf("No test chosen. Aborting.\n");
        #ifdef USEGLUT
        glutHideWindow();
        #endif
        system("pause");
        exit(1);
    }

    console.insert(" !\"#$%&'()\n", CONLTBLUE);
    console.insert("SPRPlayground mode");
    console.insert("Controls:");
    console.insert(" arrows - change displayed item");
    console.insert(" B - load many sprites");
    console.insert(" M - load half many sprites but two times each");
    console.insert(" U - unload manysprites");
    console.insert(" esc - exit");
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

    g->AnimationAdvance(200./fps);
    glPopMatrix();

    RenderMouseCursor();
}


void GM_SPRPlayground::KeyPress(unsigned char key, int x, int y) {
    printf("%d\n", key);
    switch (key) {
        case 27:
            KeyPress('U', 0, 0);
            exit(0);
            break;
        case 'b':
        case 'B':
            if (bigarray[0]) {
                console.insert("Bigarray already loaded, press U first\n", CONRED);
                break;
            }
            for (int i = 0 ; i < BIGARRAYSIZE; i++) {
                bigarray[i] = new Texture("Tibia792.spr", i+1);
                bigarray[i]->Bind();
            }
            console.insert("Loaded bigarray (press U to unload)", CONBLUE);
            break;
        case 'u':
        case 'U':
            if (!bigarray[0]) {
                console.insert("Bigarray already unloaded, press B or M first\n");
            }
            for (int i = 0 ; i < BIGARRAYSIZE; i++) {
                delete bigarray[i];
                bigarray[i]=NULL;
                TextureReportRemaining();
            }
            console.insert("Unloaded bigarray", CONBLUE);
            break;
        case 'm':
        case 'M':
            if (bigarray[0]) {
                console.insert("Bigarray already loaded, press U first\n", CONRED);
                break;
            }

            for (int i = 0; i < BIGARRAYSIZE / BAMULTI; i++) {
                for (int j = 0 ; j < BAMULTI ; j++) {

                    bigarray[i*BAMULTI+j] = new Texture("Tibia792.spr", i+2);
                    bigarray[i*BAMULTI+j]->Bind();
                }

            }
            console.insert("Loaded multibigarray\n", CONBLUE);
            break;
		case 'o':
		case 'O':
			if (objbigarray[0]) {
				console.insert("Object bigarray already loaded, press P first\n", CONRED);
				break;
			}


            for (int i = 0; i < BIGARRAYSIZE; i++) {
				printf("Loading objbigarray -- %d\n", i);
				objbigarray[i] = new ObjSpr(i+100, 0, 792);
				objbigarray[i]->Render();
            }
            console.insert("Loaded object bigarray\n", CONBLUE);
            break;

        case 'p':
        case 'P':
            if (!bigarray[0]) {
                console.insert("Object bigarray already unloaded, press O first\n");
            }
            for (int i = 0 ; i < BIGARRAYSIZE; i++) {
                delete objbigarray[i];
                objbigarray[i]=NULL;
                TextureReportRemaining();
            }
            console.insert("Unloaded object bigarray", CONBLUE);
            break;
        case 'r':
        case 'R':
            TextureReportRemaining();
            break;
    }
}
void GM_SPRPlayground::SpecKeyPress(int key, int x, int y ) {
    if (!creaturetest) {
        switch (key) {
            case WIN_KEY_UP:
                counter+=100;
                break;
            case WIN_KEY_DOWN:
                if (counter>=200) counter-=100;
                break;
            case WIN_KEY_LEFT:
                if (counter-100) counter--;
                break;
            case WIN_KEY_RIGHT:
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
        #ifdef USEGLUT
        glutPostRedisplay();
        #endif

        char tmp[256];
        sprintf(tmp, "%d", counter);
        console.insert(tmp, CONORANGE);
    } else {
        switch (key) {
            case WIN_KEY_UP:
                offset.y ++;
                g->SetDirection(NORTH);
                break;
            case WIN_KEY_DOWN:
                if (offset.y) offset.y --;
                g->SetDirection(SOUTH);
                break;
            case WIN_KEY_LEFT:
                if (offset.x) offset.x --;
                g->SetDirection(WEST);
                break;
            case WIN_KEY_RIGHT:
                offset.x++;
                g->SetDirection(EAST);
                break;
            case WIN_KEY_PAGE_UP: {
                counter++;

                char tmp[256];
                sprintf(tmp, "%d", counter);
                console.insert(tmp, CONORANGE);

                delete g;


                g = new ObjSpr(counter, 792, 50, 90, 110, 120);
                g->SetDirection(SOUTH);
                break;
            }
            case WIN_KEY_PAGE_DOWN: {
                if (counter==1) return;
                counter--;

                char tmp[256];
                sprintf(tmp, "%d", counter);
                console.insert(tmp, CONORANGE);

                delete g;
                g = new ObjSpr(counter, 792, 50, 90, 110, 120);
                g->SetDirection(SOUTH);

                break;
            }

        }
        char tmp[256];
        sprintf(tmp, "%d %d", offset.x, offset.y);
        console.insert(tmp, CONORANGE);
    }
}
