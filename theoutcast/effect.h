#ifndef __EFFECT_H
#define __EFFECT_H

#include "thing.h"
class Tile;
class Effect : public Thing {
    public:
        Effect(Tile *parent);
        ~Effect();

        void SetType(unsigned short outfit, void* extra);
        void Render(position_t *pos);
        bool AnimationAdvance(float advance);
    private:
        Tile *parent;
};
#endif
