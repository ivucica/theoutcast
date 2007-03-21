#include "creature.h"
#include "player.h"
#include "tile.h"
#include "map.h"
Player *player=0; // it says NULL not declared?!
Player::Player(unsigned long creatureid) {
    this->creatureid = creatureid;
    this->minz = 14;
}

Player::~Player() {
}

unsigned long Player::GetCreatureID() {
    return creatureid;
}
Creature *Player::GetCreature() {
    return gamemap.GetCreature(creatureid, NULL);
}
void Player::GetPos(position_t *p) {
    p = &pos;
}
position_t *Player::GetPos() {
    return &pos;
}
unsigned short Player::GetPosX() {
    return pos.x;
}
unsigned short Player::GetPosY() {
    return pos.y;
}
unsigned char Player::GetPosZ() {
    return pos.z;
}
void Player::SetPos(position_t *p) {
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
}
void Player::SetPos(unsigned short x, unsigned short y, unsigned char z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
}
void Player::FindMinZ() { // finds highest place that we may render on
    Tile *t;
    position_t postmp;

    minz = 0;
    for (int z = pos.z-1; z>=0; z--) {

        postmp.x = pos.x-(z-pos.z);
        postmp.y = pos.y-(z-pos.z);
        postmp.z = z;
        t = gamemap.GetTile(&postmp);
        if (t && t->getitemcount()) {
            minz = z+1;

            return;
        }
    }
}
unsigned int Player::GetMinZ() {

    return minz;
}
