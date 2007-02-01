#include "item.h"
#include "glutwin.h"

Item::Item() {

}
Item::~Item() {
}

void Item::Render() {
    Thing::Render();
//    sprgfx->AnimationSetValue(animation_percent);
}

void Item::Render(position_t *pos) {
    Thing::Render(pos);
//    sprgfx->AnimationSetValue(animation_percent);
}
