
#include <GLICT/container.h>
#include <GLICT/textbox.h>
#include "gamemode.h"
#include "objspr.h"
#include "types.h"
class GM_Gameworld : public GameMode {
    public:
        GM_Gameworld();
        ~GM_Gameworld();

        void Render();
        void KeyPress (unsigned char key, int x, int y) ;
    private:
        ObjSpr *g;


};
