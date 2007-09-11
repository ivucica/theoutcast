#ifndef __PLAYER_H
#define __PLAYER_H

#include <map>
#include "types.h"
#include "creature.h"
#include "tile.h"
#include "container.h"


typedef std::map <unsigned char, Container*> ContainerMap;

class Player {
    public:
        Player(unsigned long creatureid);
        ~Player();

        unsigned long   GetCreatureID();
        void            SetCreatureID(unsigned long crid);
        Creature*       GetCreature();
        void            GetPos(position_t *p);
        position_t&     GetPos();
        unsigned short  GetPosX(); unsigned short GetPosY(); unsigned char GetPosZ();
        void            SetPos(position_t *p);
        void            SetPos(unsigned short x, unsigned short y, unsigned char z);

        void            FindMinZ();
        unsigned int    GetMinZ();


        unsigned short  GetHP();
        unsigned short  GetMaxHP();
        unsigned short  GetCap();
        unsigned short  GetMP();
        unsigned short  GetMaxMP();
        unsigned short  GetLevel();
        unsigned short  GetMLevel();
        unsigned char   GetLevelPercent();
        unsigned char   GetMLevelPercent();
        unsigned long   GetExp();


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


        void            SetSkill(skill_t skill, unsigned char level, unsigned char percent);
        unsigned char   GetSkillLevel(skill_t skill);
        unsigned char   GetSkillPercent(skill_t skill);




        void            SetIcons(unsigned int icons);
        bool            GetIcon(statusicons_t icon);



        void            SetInventorySlot(unsigned int slot, Thing *item);
        void            RenderInventory(unsigned int slot);
        void            SetContainer(unsigned char cid, Container *container);
        Container *     GetContainer(unsigned char container);
        void            RemoveContainer(unsigned int cid);
        unsigned char   GetFreeContainer();


        void            Die();
// public variables
        Thing*          inventory[10];

    private:
        unsigned long   creatureid;
        position_t      pos;
        unsigned char   minz;


        // stats
        unsigned short  hp;
        unsigned short  maxhp;
        unsigned short  cap;
        unsigned short  mp;
        unsigned short  maxmp;
        unsigned short  level;
        unsigned short  mlevel;
        unsigned char   levelpercent;
        unsigned char   mlevelpercent;
        unsigned long   exp;


        ContainerMap containers;
        unsigned char playerskills[7];
        unsigned char playerskillspcnt[7];
        unsigned int icons;

    friend void Tile::Render(int layer);
    friend void Tile::RenderStrayCreatures(const position_t *p);
};

extern Player *player;

#endif // __PLAYER_H
