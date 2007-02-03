#ifndef __PROTOCOL76_H
#define __PROTOCOL76_H

#include "protocol.h"

class Protocol76 : public Protocol {
    public:
        Protocol76();
        ~Protocol76();

        bool CharlistLogin(const char *username, const char *password);
        bool GameworldLogin ();
        //bool ParseGameworld(NetworkMessage *nm, unsigned char packetid);
        /*void GetPosition(NetworkMessage *nm, position_t *pos);
        char GetStackpos(NetworkMessage *nm);*/

};

#endif
