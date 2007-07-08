#ifndef __GM_SPRPLAYGROUND
#define __GM_SPRPLAYGROUND

#include "gamemode.h"
#include "objspr.h"

class GM_SPRPlayground : public GameMode {
    public:
        GM_SPRPlayground();
        ~GM_SPRPlayground();


        void Render();
        void KeyPress(unsigned char key, int x, int y);
        void SpecKeyPress(int key, int x, int y );

    private:
        ObjSpr *g;


};


#endif // __GM_SPRPLAYGROUND
