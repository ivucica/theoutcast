#include "item.h"

Item::Item() {

}
Item::~Item() {
}

void Item::Render() {
    Thing::Render();
    moving = false;
//    sprgfx->AnimationSetValue(animation_percent);
}

void Item::Render(position_t *pos) {
    Thing::Render(pos);
    moving = false;
//    sprgfx->AnimationSetValue(animation_percent);
}
