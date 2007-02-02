#include "creature.h"

Creature::Creature() {

}
Creature::~Creature() {
}
void Creature::SetType(unsigned short outfit, unsigned short extendedtype) {
    printf("Creature::SetType to %d %d\n", outfit, extendedtype);
    this->type = outfit;
    if (outfit != 0)
        sprgfx = new ObjSpr(outfit-1, 1);
    else
        sprgfx = new ObjSpr(extendedtype, 0);
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
