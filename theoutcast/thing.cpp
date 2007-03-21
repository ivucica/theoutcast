#include "thing.h"
#include "items.h"

#include "creature.h"
#include "item.h"


// FIXME add thread locking in thing and in creature's
Thing::Thing() {
    sprgfx = NULL;
    type = 0;
    //printf("Forged a new item >D\n");
    this->count = 1;
    this->moving = false;
    this->speed = 0;
    this->animationpercent = 0;
    this->preapproved = false;
    ONInitThreadSafe(threadsafe);
}
Thing::~Thing() {
    delete sprgfx;
    ONDeinitThreadSafe(threadsafe);
}
unsigned short Thing::GetType() {
    return type;
}
unsigned short Thing::GetSpeedIndex() {
    return items[type].speedindex;
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
    ONThreadSafe(threadsafe);
    this->type = type;
    sprgfx = new ObjSpr(type, 0);
    ONThreadUnsafe(threadsafe);
}
void Thing::SetDirection(direction_t dir) {
    ONThreadSafe(threadsafe);
    this->direction = dir;
    sprgfx->SetDirection(dir);
    ONThreadUnsafe(threadsafe);
}
void Thing::Render() {
    if (sprgfx) sprgfx->Render();
}
void Thing::Render(position_t *pos) {
    ONThreadSafe(threadsafe);
    if (sprgfx) {
        if (items[type].stackable)
            sprgfx->Render(count);
        else
            sprgfx->Render(pos);
    }
    ONThreadUnsafe(threadsafe);
}
void Thing::AnimationAdvance(float advance) {
    if (sprgfx) animationpercent = sprgfx->AnimationAdvance(advance);
}
bool Thing::IsMoving() {
    return moving;
}
void Thing::StartMoving() {
    moving = true;
    preapproved = true;
}
void Thing::ApproveMove() {
    preapproved = false;
}
void Thing::SetSpeed(unsigned short speed) {
    this->speed = speed;
}
unsigned short Thing::GetSpeed() {
    return speed;
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
