#ifndef __OBJECT_H
#define __OBJECT_H

#include <vector>
typedef enum {
    ANI_STAND = 0,
    ANI_WALK
} animation_e;
// these are derived into obj3ds and similar, into rendering objects
// these are NOT ingame objects
class Object {
    public:
        Object();
        virtual ~Object() {}

        virtual bool Render() {return true;}
        virtual float AnimationAdvance(float percent);
        virtual void AnimationSetValue(float percent);
        virtual void AnimationSetType(animation_e a);

    protected:
        float animation_percent;
        animation_e animation_type;
        std::vector<int> animation_framelist_stand;
        std::vector<int> animation_framelist_move;
};
#endif
