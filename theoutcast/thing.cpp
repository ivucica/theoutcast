#include "thing.h"
#include "items.h"

#include "creature.h"
#include "item.h"
Thing::Thing() {
    sprgfx = NULL;
    type = 0;
    //printf("Forged a new item >D\n");
    this->count = 1;
}
Thing::~Thing() {
    delete sprgfx;
}
unsigned short Thing::GetType() {
    return type;
}
bool Thing::IsGround() {
    //printf("Is ground %d: %s\n", type, items[type].ground ? "yes" : "no");
    return items[type].ground;
}
bool Thing::IsStackable() {
    return items[type].stackable;
}
unsigned char Thing::GetTopIndex() {
    return items[type].topindex;
}
unsigned char Thing::GetCount() {
    if (items[type].stackable) return count; else return 1;
}
void Thing::SetCount(unsigned char count) {
    this->count = count;
}
void Thing::SetType(unsigned short type, unsigned short extendedtype) {
    this->type = type;
    //printf("NEW ITEM OF TYPE %d\n", type);
    sprgfx = new ObjSpr(type, 0);
}
void Thing::Render() {
    //printf("Wendewing\n");
    //printf(" %d!\n", type);
    if (sprgfx) sprgfx->Render();
}
void Thing::Render(position_t *pos) {
    if (sprgfx) {
        if (items[type].stackable)
            sprgfx->Render(count);
        else
            sprgfx->Render(pos);
    }
}
void Thing::AnimationAdvance(float advance) {
    if (sprgfx) sprgfx->AnimationAdvance(advance);
}

Thing *ThingCreate(unsigned int type) {
    switch (type) {
        case 0x61:
        case 0x62:
        case 0x63:
            return new Creature;
        default:
            return new Item;
    }
}
