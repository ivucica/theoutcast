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
void Console::insert(std::string txt, bool debug) {
    this->insert(txt, CONWHITE, debug);
}
void Console::insert(std::string txt, consolecolors_t col) {
    this->insert(txt, col, false);
}
#include "assert.h" // remove me -- just for texture integrity test
#include "texmgmt.h" // remove me - just for texture integrity test
void Console::insert(std::string txt, consolecolors_t col, bool debug) {
//    DEBUGPRINT("Inserting new console entry, color %d\n", col);

    if (debug && DEBUGLEVEL_BUILDTIME >= 0 || !debug) {
        consoleentry* x = new consoleentry;
        x->text = (char*)malloc(txt.size() + 1);
        strcpy(x->text, txt.c_str());
        if (x->text[0] == 13) memmove(x->text, x->text+1, strlen(x->text)-1);
        if (x->text[0] == 10) memmove(x->text, x->text+1, strlen(x->text)-1);
        if (x->text[0] == 13) memmove(x->text, x->text+1, strlen(x->text)-1);
        if ((x->text[strlen(x->text)-2]==13 && x->text[strlen(x->text)-1]==10) || (
             x->text[strlen(x->text)-2]==10 && x->text[strlen(x->text)-1]==13))
            x->text[strlen(x->text)-2] = 0;

        if (x->text[strlen(x->text)-1]==13 || x->text[strlen(x->text)-1]==10)
            x->text[strlen(x->text)-1] = 0;
        x->color = col;

        //DEBUGPRINT("Adding %s to console\n", x->text);
        ONThreadSafe(threadsafe);
        con.insert(con.begin(), x);
        ONThreadUnsafe(threadsafe);
        ASSERTFRIENDLY(TextureIntegrityTest(), "Console::insert(): Texture integrity test failed");
    }
}
void Console::clear() {
    ONThreadSafe(threadsafe);
    consolecontainer::iterator it;
    for (it=con.begin(); it!=con.end();) {

        free((*it)->text);
		con.erase(it);
    }
    ONThreadUnsafe(threadsafe);
}
void Console::draw(char count) {
    consolecontainer::iterator it;
    int p=1;

    float oldcolor[4];
    glGetFloatv(GL_CURRENT_COLOR, oldcolor);

    glColor3f(1.0f,1.0f,1.0f);
    //DEBUGPRINT("Beginning console render...\n");
    ONThreadSafe(threadsafe);

    for (it=con.begin(); it!=con.end() && p<count; it++) {
        //p++;
        //printf("Line %d / %d\n", p, count);
        //DEBUGPRINT("%s\n", (*it)->text);


        glColor4fv (&consolecolors[(*it) ->color * 4]);


        if ((*it)->text) {
            p += glictFontNumberOfLines((*it)->text);

            //glPushMatrix();
            glDisable(GL_SCISSOR_TEST);
            glDisable(GL_CULL_FACE );
            glRotatef(180,1,0,0);
            glictFontRender(((*it) ->text), "system", 0, -p*12 );
            //glTranslatef(-glictFontSize((*it)->text, "system"), 0, 0);
            glRotatef(180,1,0,0);
            glEnable(GL_SCISSOR_TEST);
			glEnable(GL_CULL_FACE );

            //glPopMatrix();


        }

    }
    glColor4fv(oldcolor);
    ONThreadUnsafe(threadsafe);
}


