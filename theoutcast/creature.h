#ifndef __CREATURE_H
#define __CREATURE_H

#include "thing.h"

class Creature : public Thing {
    public:
        Creature();
        ~Creature();

        void SetType(unsigned short outfit);
        bool IsGround();
        void SetCreatureID(unsigned long creatureid);
    private:
        unsigned long id;
};

#endif // __CREATURE_H
