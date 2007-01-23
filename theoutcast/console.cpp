// REWRITE THIS FILE FROM SCRATCH.


#include <string>
#include <GL/glut.h>
#include <GLICT/fonts.h>
#include "console.h"
#include "glutfont.h"
#include "debugprint.h"
#include "threads.h"

using namespace std;
Console console;

Console::Console() {
    ONInitThreadSafe(threadsafe);
}
Console::~Console() {
    ONDeinitThreadSafe(threadsafe);
}
const std::string Console::operator[](int id) {
    return con[id]->text;
}
void Console::insert(std::string txt) {
    this->insert(txt, CONWHITE);
}
void Console::insert(std::string txt, consolecolors col) {
//    DEBUGPRINT("Inserting new console entry, color %d\n", col);

    consoleentry* x = new consoleentry;
    x->text = (char*)malloc(strlen(txt.c_str()) + 1);
    strcpy(x->text, txt.c_str());
    x->color = col;

    //DEBUGPRINT("Adding %s to console\n", x->text);
    ONThreadSafe(threadsafe);
    con.insert(con.begin(), x);
    ONThreadUnsafe(threadsafe);
}
void Console::clear() {
    ONThreadSafe(threadsafe);
    consolecontainer::iterator it;
    for (it=con.begin(); it!=con.end();) {
        con.erase(it);
        free((*it)->text);
    }
    ONThreadUnsafe(threadsafe);
}
void Console::draw(char count) {
    consolecontainer::iterator it;
    int p=0;

    float oldcolor[4];
    glGetFloatv(GL_CURRENT_COLOR, oldcolor);

    glColor3f(1.0f,1.0f,1.0f);
    //DEBUGPRINT("Beginning console render...\n");
    ONThreadSafe(threadsafe);
    for (it=con.begin(); it!=con.end() && p<count; it++) {
        //p++;
        //DEBUGPRINT("Line %d / %d\n", p, count);
        //DEBUGPRINT("%s\n", (*it)->text);
        switch ((*it) ->color) {
            /* TODO (Khaos#3#): These colors should perhaps use colors.h? Not sure
                                about this xD */
            case CONRED:
                glColor3f(1.0f, 0.0f, 0.0f);
                break;
            case CONBLUE:
                glColor3f(0.0f, 0.0f, 1.0f);
                break;
            case CONYELLOW:
                glColor3f(1.0f, 1.0f, 0.0f);
                break;
            case CONORANGE:
                glColor3f(1.0f, 0.5f, 0.0f);
                break;
            case CONLTBLUE:
                glColor3f(0.5f, 0.5f, 1.0f);
                break;
            case CONGREEN:
                glColor3f(0.0f, 1.0f, 0.0f);
                break;
            case CONWHITE:
            default:
                glColor3f(1.0f, 1.0f, 1.0f);
                break;
        }
        if ((*it)->text) {
            p += glutxNumberOfLines((*it)->text);
            glPushMatrix();
            glictFontRender((*it)->text, "system", 0, p*12 );
            glPopMatrix();

        }
    }
    glColor4fv(oldcolor);
    ONThreadUnsafe(threadsafe);
}


