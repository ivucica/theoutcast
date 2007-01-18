#ifndef __OBJSPR_H
#define __OBJSPR_H

#include "object.h"
#include "types.h"
#include "texmgmt.h"
class ObjSpr : public Object {
    public:
        ObjSpr();
        ObjSpr(unsigned int itemid);
        ~ObjSpr();

        bool Render();

        void LoadItem(unsigned int itemid);
    private:
        spritelist_t sli; // spritelist
        Texture **t; // sprite textures list
};

#endif
