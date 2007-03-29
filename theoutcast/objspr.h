#ifndef __OBJSPR_H
#define __OBJSPR_H

#include "texmgmt.h"
#include "object.h"
#include "types.h"
class ObjSpr : public Object {
    public:
        ObjSpr();
        ObjSpr(unsigned int itemid, unsigned char type);
        ObjSpr(unsigned int itemid, unsigned char type, unsigned int protocolversion);
        ~ObjSpr();

        bool Render();
        bool Render(position_t *pos);
        bool Render(unsigned char stackcount);

        void LoadItem(unsigned int itemid);
        void LoadItem(unsigned int itemid, unsigned int protocolversion);
        void LoadCreature(unsigned int creaturetype);
        void LoadCreature(unsigned int creaturetype, unsigned int protocolversion);
        void SetDirection(direction_t dir);
    private:
        spritelist_t sli; // spritelist
        Texture **t; // sprite textures list
        unsigned int itemid;
        unsigned char offsetx, offsety;
        unsigned int type; // item, creature, ...
        direction_t direction;
};

#endif
