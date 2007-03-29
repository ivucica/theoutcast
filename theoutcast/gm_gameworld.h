
#include <GLICT/container.h>
#include <GLICT/window.h>
#include <GLICT/textbox.h>
#include <GLICT/button.h>
#include "gamemode.h"
#include "objspr.h"
#include "types.h"
class GM_Gameworld : public GameMode {
    public:
        GM_Gameworld();
        ~GM_Gameworld();

        void Render();
        void KeyPress (unsigned char key, int x, int y) ;
        void SpecKeyPress(int key, int x, int y);
        void MouseClick (int button, int shift, int mousex, int mousey);
        void ResizeWindow();
    private:
        ObjSpr *g;
        glictContainer desktop;
        glictWindow winWorld, winConsole, winInventory;

        // winconsole
        glictTextbox txtConMessage;
        glictButton btnConSend;

        // wininventory
        glictPanel panInvSlots[10];

    friend void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
};
void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnClick (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void GM_Gameworld_ConsoleOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
