#include "effect.h"
#include "tile.h"
Effect::Effect(Tile *parent) {
    this->parent = parent;
}

Effect::~Effect() {
}

void Effect::AnimationAdvance(float advance) {
    float oldanimationpercent = this->animationpercent;
    if (moving && this->animationpercent != 100.) Thing::AnimationAdvance(advance);
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
        parent->Remove(this);
    }
}
