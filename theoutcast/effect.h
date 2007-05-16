#ifndef __EFFECT_H
#define __EFFECT_H

#include "thing.h"
class Tile;

typedef enum {
    MAGICEFFECT=0,
    DISTANCESHOT,
    TEXT,
    ANIMATEDTEXT
} effecttype_t;



class Effect : public Thing {
    public:
        Effect(Tile *parent);
        ~Effect();

        void SetType(unsigned short outfit, void* extra);
        void Render(position_t *pos, bool rendering_overlay);
        bool AnimationAdvance(float advance, bool rendering_overlay);
        void SetText(std::string &text, unsigned char color, bool animated);
    private:
        Tile *parent;
        effecttype_t effecttype;
        color_t textcolor;
        std::string text;

};
#endif
