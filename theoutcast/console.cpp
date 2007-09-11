

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
    return con[id].text;
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
        consoleentry x;

        x.text = txt.c_str();
        if (x.text[0] == 13) x.text = x.text.substr(1, x.text.size()-1);
        if (x.text[0] == 10) x.text = x.text.substr(1, x.text.size()-1);
        if (x.text[0] == 13) x.text = x.text.substr(1, x.text.size()-1);
        if ((x.text[x.text.size()-2]==13 && x.text[x.text.size()-1]==10) || (
             x.text[x.text.size()-2]==10 && x.text[x.text.size()-1]==13))
            x.text = x.text.substr(0, x.text.size()-2);

        if (x.text[x.text.size()-1]==13 || x.text[x.text.size()-1]==10)
            x.text = x.text.substr(0, x.text.size()-1);
        x.color = col;


        ONThreadSafe(threadsafe);
        con.insert(con.begin(), x);
        ONThreadUnsafe(threadsafe);
        ASSERTFRIENDLY(TextureIntegrityTest(), "Console::insert(): Texture integrity test failed");
    }
}
void Console::clear() {
    ONThreadSafe(threadsafe);
    std::vector<consoleentry>::iterator it;
    for (it=con.begin(); it!=con.end();) {


		con.erase(it);
    }
    ONThreadUnsafe(threadsafe);
}
void Console::draw(char count) {

    int p=1;

    float oldcolor[4];
    glGetFloatv(GL_CURRENT_COLOR, oldcolor);

    glColor3f(1.0f,1.0f,1.0f);
    ONThreadSafe(threadsafe);

    for (std::vector<consoleentry>::iterator it=con.begin(); it!=con.end() && p<count; it++) {


        glColor4fv (&consolecolors[it->color * 4]);


            p += glictFontNumberOfLines(it->text.c_str());

            glDisable(GL_SCISSOR_TEST);
            glDisable(GL_CULL_FACE );
            glRotatef(180,1,0,0);
            glictFontRender(it->text.c_str(), "system", 0, -p*12 );
            glRotatef(180,1,0,0);
            glEnable(GL_SCISSOR_TEST);
			glEnable(GL_CULL_FACE );



    }
    glColor4fv(oldcolor);
    ONThreadUnsafe(threadsafe);
}


