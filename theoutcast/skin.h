#ifndef __SKIN_H
#define __SKIN_H

#include "texmgmt.h"
class Skin {
    public:
        Skin();
        ~Skin();

        void Load(const char *what);
        void Unload();

    private:
        Texture *wintl, *wint, *wintr, *winl, *winc, *winr, *winbl, *winb, *winbr;
        glictSkinner win;
        Texture *btntl, *btnt, *btntr, *btnl, *btnc, *btnr, *btnbl, *btnb, *btnbr;
        glictSkinner btn;

};
extern Skin skin;

#endif
