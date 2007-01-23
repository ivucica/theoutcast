#include "player.h"

Player *player=0; // it says NULL not declared?!
Player::Player(unsigned long creatureid) {
    this->creatureid = creatureid;
}

Player::~Player() {
}

unsigned long Player::GetCreatureID() {
    return creatureid;
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
