
#include <GLICT/container.h>
#include <GLICT/window.h>
#include "gamemode.h"
#include "objspr.h"
#include "types.h"
class GM_Gameworld : public GameMode {
    public:
        GM_Gameworld();
        ~GM_Gameworld();

        void Render();
        void KeyPress (unsigned char key, int x, int y) ;
        void MouseClick (int button, int shift, int mousex, int mousey);
        void ResizeWindow();
    private:
        ObjSpr *g;
        glictContainer desktop;
        glictWindow winWorld;


};

void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
