#ifndef __GM_LOGO_H
#define __GM_LOGO_H
#include "flythrough.h"
#include "gamemode.h"
#include "obj3ds.h"
class GM_Logo : public GameMode {
    public:
        GM_Logo();
        ~GM_Logo();

        void Render();
        void KeyPress (unsigned char key, int x, int y);
        void ResizeWindow (int w, int h);

    private:
        Obj3ds *logo;
        flythrough_c flythrough;
        bool done;

        void OnFinish();
        float fadein, fadeout;

    friend void GM_Logo_OnFinish (void *arg);

};

void GM_Logo_OnFinish (void *arg);

#endif
