#ifndef __THING_H
#define __THING_H

#include "objspr.h"
#include "types.h"
#include "threads.h"
class Thing {
    public:
        Thing();
        ~Thing();

        // stock item info retrieval
        virtual bool IsGround();
        virtual bool IsStackable();
        unsigned char GetTopIndex();

        // this item's info retrieval
        unsigned short GetType();
        unsigned char GetCount();

        virtual void SetType(unsigned short type, unsigned short extendedtype);

        virtual void Render();
        virtual void Render(position_t *pos);
        virtual void AnimationAdvance(float percent);

        void SetCount(unsigned char count);

    private:
        unsigned char count;

    protected:
        ObjSpr *sprgfx;
        unsigned short type;
        ONCriticalSection threadsafe;
};
Thing *ThingCreate(unsigned int type);
#endif
