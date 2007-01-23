#ifndef __THING_H
#define __THING_H

#include "objspr.h"
#include "types.h"
class Thing {
    public:
        Thing();
        ~Thing();

        // stock item info retrieval
        virtual bool IsGround();
        virtual bool IsStackable();
        unsigned char GetTopIndex();

        // this item info retrieval
        void SetType(unsigned short type); // Move me to ITEM.H!
        unsigned char GetCount();

        virtual void Render();
        virtual void Render(position_t *pos);
        void SetCount(unsigned char count);

    private:
        unsigned short type;
        unsigned char count;
        ObjSpr *sprgfx;
};

#endif
