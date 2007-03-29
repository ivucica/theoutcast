#ifndef __PROTOCOL75_H
#define __PROTOCOL75_H

#include "protocol.h"

class Protocol75 : public Protocol {
    public:
        Protocol75();
        ~Protocol75();

        bool CharlistLogin(const char *username, const char *password);
        bool GameworldLogin ();

        // overridden data types that should behave differently
        void GetPlayerStats(NetworkMessage *nm);

};

#endif
