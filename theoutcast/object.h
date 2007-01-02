#ifndef __OBJECT_H
#define __OBJECT_H

typedef enum {
    ANI_STAND = 0,
    ANI_WALK
} animation_e;

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
