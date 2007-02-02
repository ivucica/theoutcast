#ifndef __CREATURE_H
#define __CREATURE_H

#include <string>
#include "thing.h"

class Creature : public Thing {
    public:
        Creature();
        ~Creature();

        // setting info
        void SetType(unsigned short outfit, unsigned short extendedlook);
        void SetCreatureID(unsigned long creatureid);
        void SetName(std::string creaturename);

        // obtaining info
        bool IsGround();
        std::string GetName();
    private:
        unsigned long id;
        std::string name;
};

#endif // __CREATURE_H
