#ifndef __PLAYER_H
#define __PLAYER_H

class Player {
    public:
        Player(unsigned long creatureid);
        ~Player();

        unsigned long GetCreatureID();
};

extern Player *player;

#endif // __PLAYER_H
