#ifndef __PLAYER_H
#define __PLAYER_H

#include "types.h"
#include "creature.h"
#include "tile.h"
class Player {
    public:
        Player(unsigned long creatureid);
        ~Player();

        unsigned long   GetCreatureID();
        Creature*       GetCreature();
        void            GetPos(position_t *p);
        position_t*     GetPos();
        unsigned short  GetPosX(); unsigned short GetPosY(); unsigned char GetPosZ();
        void            SetPos(position_t *p);
        void            SetPos(unsigned short x, unsigned short y, unsigned char z);

        void            FindMinZ();
        unsigned int    GetMinZ();


        unsigned short  GetHP();
        unsigned short  GetMaxHP();
        unsigned short  GetMP();
        unsigned short  GetMaxMP();


        void            SetHP(unsigned short hp); // hp
        void            SetMaxHP(unsigned short maxhp); // max hp
        void            SetCap(unsigned short cap); // cap
        void            SetExp(unsigned long exp); // exp
        void            SetLevel(unsigned short lvl); // lvl
        void            SetLevelPercent(unsigned char lvlpercent); // level percent
        void            SetMP(unsigned short mp); // mp
        void            SetMaxMP(unsigned short maxmp); // mmp
        void            SetMLevel(unsigned char mlvl); // mag lvl
        void            SetMLevelPercent(unsigned char mlvlpercent); // maglvl percent
        void            SetSoulPoints(unsigned char soul); // soul
        /* dunno where this was added but it is there in 792 */
        void            SetStamina(unsigned short stamina); // stamina (minutes)

        void            SetInventorySlot(unsigned int slot, Thing *item);
        void            RenderInventory(unsigned int slot);

// public variables
        Thing           *inventory[10];

    private:
        unsigned long   creatureid;
        position_t      pos;
        unsigned char   minz;


        // stats
        unsigned short  hp;
        unsigned short  maxhp;
        unsigned short  mp;
        unsigned short  maxmp;


    friend void Tile::Render(int layer);

};

extern Player *player;

#endif // __PLAYER_H
