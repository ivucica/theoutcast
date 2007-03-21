#ifndef __PLAYER_H
#define __PLAYER_H

#include "types.h"
#include "creature.h"
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

    private:
        unsigned long creatureid;
        position_t pos;
        unsigned char minz;
};

extern Player *player;

#endif // __PLAYER_H
