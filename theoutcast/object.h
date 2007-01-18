#ifndef __OBJECT_H
#define __OBJECT_H

typedef enum {
    ANI_STAND = 0,
    ANI_WALK
} animation_e;
// these are derived into obj3ds and similar, into rendering objects
// these are NOT ingame objects
class Object {
    public:
        Object();
        ~Object() {}

        virtual bool Render() {return true;}

    private:
        float animation_percent;
        animation_e animation_type;
};
#endif
