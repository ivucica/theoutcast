#include <GLICT/fonts.h>
#include <string>
#include "effect.h"
#include "tile.h"

extern  float fps;
Effect::Effect(Tile *parent) {
    this->parent = parent;
}

Effect::~Effect() {
}

bool Effect::AnimationAdvance(float advance, bool overlayed) {
    float oldanimationpercent = this->animationpercent;
//    printf("Advancing effect animation - %g, type %s, overlayed %s\n", animationpercent, effecttype==MAGICEFFECT ? "magiceffect" : "text", overlayed ? "true" : "false");


    if (this->animationpercent != 100.) {
        if ((effecttype==MAGICEFFECT || effecttype==DISTANCESHOT)) {
//            printf("It's an effect\n");
            if (!overlayed) {
                Thing::AnimationAdvance(advance);
            }

        } else {
            if (overlayed) {
//                printf("Ani advanced\n");
                animationpercent += 50./fps;
                if (animationpercent > 100.) animationpercent = 100.; // FIXME isnan?
            } else {
//                printf("Not advancing ani\n");
            }
        }
    }
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
        parent->Remove(this);
//        printf("REMOVING!!!!!!!!!\n");
        return false;
    }
    return true;
}

void Effect::SetType(unsigned short outfit, void* extra) {
    sprgfx = new ObjSpr(outfit, 2);
    this->effecttype = MAGICEFFECT;
}
void Effect::SetText(std::string &text, unsigned char color, bool animated) {
    textcolor.b = (color % 6) / 5.;
    textcolor.g = ((color / 6) % 6) / 5.;
    textcolor.r = (color / 36) / 5.;

    printf("%d - %02x %02x %02x - %g %g %g\n", color, (char)(textcolor.r * 256), (char)(textcolor.g * 256), (char)(textcolor.b * 256), textcolor.r, textcolor.g, textcolor.b);

    textcolor.a = 1.;

    this->text = text;

    this->effecttype = (animated ? ANIMATEDTEXT : TEXT);

}

void Effect::Render(position_t *pos, bool overlayed) {
    switch (effecttype) {
        case MAGICEFFECT:
            if (!overlayed) Thing::Render();
            break;
        case TEXT:
            glColor4f(textcolor.r, textcolor.g, textcolor.b, textcolor.a);
            if (overlayed) glictFontRender(text.c_str(), "system", 16 - glictFontSize(text.c_str(), "system")/2., 64);
            glColor4f(1., 1., 1., 1.);
            break;
        case ANIMATEDTEXT:
            glColor4f(textcolor.r, textcolor.g, textcolor.b, textcolor.a);
            if (overlayed) glictFontRender(text.c_str(), "system", 0, 64+animationpercent/3.);
            glColor4f(1., 1., 1., 1.);
            break;
        default:
            printf("Still don't know how to render this magic effect\n");
    }
}

bool Effect::IsGround() {
    return false;
}
