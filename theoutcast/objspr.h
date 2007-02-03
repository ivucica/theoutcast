#ifndef __OBJSPR_H
#define __OBJSPR_H

#include "object.h"
#include "types.h"
#include "texmgmt.h"
class ObjSpr : public Object {
    public:
        ObjSpr();
        ObjSpr(unsigned int itemid, unsigned char type);
        ~ObjSpr();

        bool Render();
        bool Render(position_t *pos);
        bool Render(unsigned char stackcount);

        void LoadItem(unsigned int itemid);
        void LoadCreature(unsigned int creaturetype);
    private:
        spritelist_t sli; // spritelist
        Texture **t; // sprite textures list
        unsigned int itemid;
        unsigned char offsetx, offsety;
};

#endif
