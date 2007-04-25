#ifndef __SKIN_H
#define __SKIN_H

#include "texmgmt.h"
class Skin {
    public:
        Skin();
        ~Skin();

        void Load(const char *what);
        void Unload();
	void AssureLoadedness(); 
    private:
        Texture *wintl, *wint, *wintr, *winl, *winc, *winr, *winbl, *winb, *winbr;
        glictSkinner win;
        Texture *btntl, *btnt, *btntr, *btnl, *btnc, *btnr, *btnbl, *btnb, *btnbr;
        glictSkinner btn;
        Texture *bthtl, *btht, *bthtr, *bthl, *bthc, *bthr, *bthbl, *bthb, *bthbr;
        glictSkinner bth;
        Texture *txttl, *txtt, *txttr, *txtl, *txtc, *txtr, *txtbl, *txtb, *txtbr;
        glictSkinner txt;

};
extern Skin skin;

#endif
