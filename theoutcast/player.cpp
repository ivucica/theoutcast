#include "creature.h"
#include "player.h"
#include "tile.h"
#include "map.h"
#include "debugprint.h"

Player *player=0; // it says NULL not declared?!
Player::Player(unsigned long creatureid) {
    this->creatureid = creatureid;
    this->minz = 14;
    for (int i = 0; i < 10; i++) {
        inventory[i] = NULL;
    }
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
        if (t && t->GetItemCount()) {
            minz = z+1;

            return;
        }
    }
}
unsigned int Player::GetMinZ() {

    return minz;
}



void Player::SetHP(unsigned short hp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "HP: %d\n", hp);
}
void Player::SetMaxHP(unsigned short maxhp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MaxHP: %d\n", maxhp);
}
void Player::SetCap(unsigned short cap) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Cap: %d\n", cap);
}
void Player::SetExp(unsigned long exp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Exp: %d\n", exp);
}
void Player::SetLevel(unsigned short lvl) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Level: %d\n", lvl);
}
void Player::SetLevelPercent(unsigned char lvlpercent) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Level%%: %d\n", lvlpercent);
}
void Player::SetMP(unsigned short mp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MP: %d\n", mp);
}
void Player::SetMaxMP(unsigned short maxmp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MaxMP: %d\n", maxmp);
}
void Player::SetMLevel(unsigned char mlvl) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MLevel: %d\n", mlvl);
}
void Player::SetMLevelPercent(unsigned char mlvlpercent) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MLevel%%: %d\n", mlvlpercent);
}
void Player::SetSoulPoints(unsigned char soul) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Soul: %d\n", soul);
}

/* dunno where this was added but it is there in 792 */
void Player::SetStamina(unsigned short stamina) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Stamina: %d\n", stamina);
}

void Player::SetInventorySlot(unsigned int slot, Thing *item) {
    if (inventory[slot-1]) delete inventory[slot-1];
    inventory[slot-1] = item;
}
void Player::RenderInventory(unsigned int slot) {
    if (!inventory[slot]) return;
    inventory[slot]->Render();
}
