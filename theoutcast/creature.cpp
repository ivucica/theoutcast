#include "creature.h"

Creature::Creature() {

}
Creature::~Creature() {
}
void Creature::SetType(unsigned short outfit, unsigned short extendedtype) {
    ONThreadSafe(threadsafe);
    printf("Creature::SetType to %d %d\n", outfit, extendedtype);
    this->type = outfit;
    if (outfit != 0)
        sprgfx = new ObjSpr(outfit, 1);
    else
        sprgfx = new ObjSpr(extendedtype, 0);
    ONThreadUnsafe(threadsafe);
}
void Creature::SetCreatureID(unsigned long creatureid) {
    this->id = creatureid;
}
bool Creature::IsGround() {
    return false;
}
void Creature::SetName(std::string creaturename) {
    this->name = creaturename;
}
std::string Creature::GetName() {
    return this->name;
}
void Creature::AnimationAdvance(float advance) {
    float oldanimationpercent = this->animationpercent;
    if (moving && this->animationpercent != 100.) Thing::AnimationAdvance(advance);
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
        this->animationpercent = 0;
        if (sprgfx) sprgfx->AnimationSetValue(0);
        if (!preapproved)
            this->moving = false;
        else
            this->animationpercent = 100.;
    }
}
void Creature::CauseAnimOffset(bool individual) {   // if we're rendering an individual, default is ok
                                                    // if we're rendering the screen, we need to invert

    switch (direction) {
        case NORTH:
            glTranslatef(0, ((preapproved ? 0 : -32.)  + 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case SOUTH:
            glTranslatef(0, ((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case WEST:
            glTranslatef(((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0, 0);
            break;
        case EAST:
            glTranslatef(((preapproved ? 0 : -32) + 32.*animationpercent/100.) * (individual ? 1. : -1), 0, 0);
            break;
    }
}
void Creature::Render(position_t *pos) {
    if (moving) {
        glPushMatrix();
        CauseAnimOffset(true);

    }
    Thing::Render(pos);
    if (moving) glPopMatrix();
}
void Creature::Render() {
    position_t p = {0, 0, 0};
    Render(&p);
}
