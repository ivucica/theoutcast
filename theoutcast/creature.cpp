#include "creature.h"

Creature::Creature() {

}
Creature::~Creature() {
}
void Creature::SetType(unsigned short outfit) {
    printf("Settype\n");
    this->type = outfit;
    sprgfx = new ObjSpr(outfit-1, 1);

}
void Creature::SetCreatureID(unsigned long creatureid) {
    this->id = creatureid;
}
bool Creature::IsGround() {
    return false;
}
