
#ifdef _MSC_VER
    #include <float.h>
    #define isnan _isnan
    #define isinf _isnan
#endif

#include <stdio.h>

#include <math.h>
#include "object.h"
Object::Object () {
    animation_percent = 0;
    animation_type = ANI_STAND;
}
float Object::AnimationAdvance(float percent) {
    int i;
    if (isnan(percent) && isinf(percent)) {
        animation_percent = 0;
        return 0;
    }


    animation_percent += percent / (animation_type == 0 ? animation_framelist_stand.size() : animation_framelist_move.size());
    if (isnan(animation_percent) || isinf(animation_percent)) animation_percent = 0;

    i=0;
    while (animation_percent >= 100. && i++<20) {
        animation_percent -= 100.;
    }
    i=0;
    while (animation_percent < 0. && i++<20) {
        animation_percent += 100.;
    }
    return animation_percent;
    //printf("new animation frame %g\n", animation_percent);
}
void Object::AnimationSetValue(float percent) {
    int i ;
    if (isnan(percent)) return;

    animation_percent = percent / (animation_type == 0 ? animation_framelist_stand.size() : animation_framelist_move.size());

    i=0;
    while (animation_percent >= 100. && i++<20) {
        animation_percent -= 100.;
    }
    i=0;
    while (animation_percent < 0. && i++<20) {
        animation_percent += 100.;
    }

    //printf("new animation frame %g\n", animation_percent);
}
void Object::AnimationSetType(animation_e a) {
    animation_type = a;
}
