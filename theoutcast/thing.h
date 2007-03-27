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
        // FIXME these functions should check if it's an item or not ...
        virtual bool IsGround();
        virtual bool IsStackable();
        unsigned char GetTopIndex();
        unsigned short GetSpeedIndex();

        // this item's info retrieval
        unsigned short GetType();
        unsigned char GetCount();
        unsigned short GetSpeed();

        // this item info setup
        virtual void SetType(unsigned short type, unsigned short extendedtype);
        virtual void SetSpeed(unsigned short speed);
        void SetCount(unsigned char count);
        void SetDirection(direction_t dir);

        // others
        virtual void Render();
        virtual void Render(position_t *pos);
        virtual void AnimationAdvance(float percent);


        virtual bool IsMoving();
        virtual void StartMoving();

        void ApproveMove();
        void CancelMoving();

    private:
        unsigned char count;

    protected:
        ObjSpr *sprgfx;
        unsigned short type;
        ONCriticalSection threadsafe;
        direction_t direction;
        unsigned short speed;
        bool moving, preapproved; // preapproved == we're moving before we were confirmed move by the server.
        float animationpercent;
};
Thing *ThingCreate(unsigned int type);
#endif
