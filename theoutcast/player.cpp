#include "gamemode.h"
#include "gm_gameworld.h"
#include "creature.h"
#include "player.h"
#include "tile.h"
#include "map.h"
#include "debugprint.h"
#include "sound.h"
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

extern float fps;

Player *player=0; // it says NULL not declared?!
Player::Player(unsigned long creatureid) {
    this->creatureid = creatureid;
    this->minz = 14;
    for (int i = 0; i < 10; i++) {
        inventory[i] = NULL;
    }

    hp = 0;
    maxhp = 0;
    cap = 0;
    mp = 0;
    maxmp = 0;
    level = 0;
    levelpercent = 0;
    mlevel = 0;
    mlevelpercent = 0;
    exp = 0;
    for (int i = 0; i < 7; i++) {
    	playerskills[i] = 0;
    	playerskillspcnt[i] = 0;
    }
    icons = 0;
    pos.x = 0; pos.y = 0; pos.z = 0;
}

Player::~Player() {
    for (int i = 0; i < 10; i++) {
        if (inventory[i]) delete inventory[i];
        inventory[i] = NULL;
    }
}

unsigned long Player::GetCreatureID() {
    return creatureid;
}
void Player::SetCreatureID(unsigned long crid) {
	creatureid = crid;
}
Creature *Player::GetCreature() {
	if (!creatureid) {
		DEBUGPRINT(DEBUGPRINT_WARNING, DEBUGPRINT_LEVEL_DEBUGGING, "If it crashed below this, the cause is surely that player has no assigned creature\n");
		return NULL;
	}
    return gamemap.GetCreature(creatureid, NULL);
}
void Player::GetPos(position_t *p) {
    *p = pos;
}
position_t& Player::GetPos() {
    return pos;
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
    if (gamemode==GM_GAMEWORLD) ((GM_Gameworld*)game)->UpdateStats();
}
void Player::SetPos(unsigned short x, unsigned short y, unsigned char z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
    if (gamemode==GM_GAMEWORLD) ((GM_Gameworld*)game)->UpdateStats();
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
    this->hp = hp;
}
void Player::SetMaxHP(unsigned short maxhp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MaxHP: %d\n", maxhp);
    this->maxhp = maxhp;
}
void Player::SetCap(unsigned short cap) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Cap: %d\n", cap);
    this->cap = cap;
}
void Player::SetExp(unsigned long exp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Exp: %d\n", exp);
    this->exp = exp;
}
void Player::SetLevel(unsigned short lvl) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Level: %d\n", lvl);
    this->level = lvl;
}
void Player::SetLevelPercent(unsigned char lvlpercent) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Level%%: %d\n", lvlpercent);
    this->levelpercent = lvlpercent;
}
void Player::SetMP(unsigned short mp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MP: %d\n", mp);
    this->mp = mp;
}
void Player::SetMaxMP(unsigned short maxmp) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MaxMP: %d\n", maxmp);
    this->maxmp = maxmp;
}
void Player::SetMLevel(unsigned char mlvl) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MLevel: %d\n", mlvl);
    this->mlevel = mlvl;
}
void Player::SetMLevelPercent(unsigned char mlvlpercent) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "MLevel%%: %d\n", mlvlpercent);
    this->mlevelpercent = mlvlpercent;
}
void Player::SetSoulPoints(unsigned char soul) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Soul: %d\n", soul);
}

/* dunno where this was added but it is there in 792 */
void Player::SetStamina(unsigned short stamina) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Stamina: %d\n", stamina);
}


unsigned short Player::GetHP() {
    return hp;
}
unsigned short Player::GetMaxHP() {
    return maxhp;
}
unsigned short Player::GetCap() {
    return cap;
}
unsigned short Player::GetMP() {
    return mp;
}
unsigned short Player::GetMaxMP() {
    return maxmp;
}
unsigned long Player::GetExp() {
    return exp;
}
unsigned short Player::GetLevel() {
    return level;
}
unsigned short Player::GetMLevel() {
    return mlevel;
}
unsigned char Player::GetLevelPercent() {
    return levelpercent;
}
unsigned char Player::GetMLevelPercent() {
    return mlevelpercent;
}


#include "console.h"
void Player::SetInventorySlot(unsigned int slot, Thing *item) {
    if (inventory[slot-1]) {
    	console.insert("Deleted an item from inventory.\n");
    	delete inventory[slot-1];
    }
    inventory[slot-1] = item;
}
void Player::RenderInventory(unsigned int slot) {
    if (!inventory[slot]) return;
    inventory[slot]->Render();
    inventory[slot]->AnimationAdvance(100./fps);
}

void Player::SetContainer(unsigned char cid, Container *container) {
    containers[cid] = container;
}
unsigned char Player::GetFreeContainer() {
    for (unsigned char i = 0; i < 16; i++) {
        if ((containers.find(i))==containers.end())
            return i;
    }
    return 0;
}
void Player::RemoveContainer(unsigned int cid) {
    ContainerMap::iterator it;

    if ((it = containers.find(cid))==containers.end()) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Could not find a container to remove");
    } else {
        containers.erase(it);
    }
}


void Player::SetSkill(skill_t skill, unsigned char level, unsigned char percent) {
    this->playerskillspcnt[skill] = percent;
    this->playerskills[skill] = level;
}
unsigned char Player::GetSkillLevel(skill_t skill) {
    return this->playerskills[skill];
}
unsigned char Player::GetSkillPercent(skill_t skill) {
    return this->playerskillspcnt[skill];
}


Container *Player::GetContainer(unsigned char container) {
    ContainerMap::iterator it;
    if ((it = containers.find(container))==containers.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

void Player::SetIcons(unsigned int icons) {
    this->icons = icons;
}

bool Player::GetIcon(statusicons_t icon) {
    return icons & icon;
}
#include "assert.h"
void Player::Die(){
	free((void*)1);
	this->creatureid = 0;
	SoundPlay("sounds/death.wav");
}
