#include "player.h"

Player *player=NULL;
Player::Player(unsigned long creatureid) {
    this->creatureid = creatureid;
}

Player::~Player() {
}

unsigned long Player::GetCreatureID() {
    return creatureid;
}
