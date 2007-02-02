#ifndef __PROTOCOL79_H
#define __PROTOCOL79_H

#include "protocol.h"

class Protocol79 : public Protocol {
    public:
        Protocol79();
        ~Protocol79();


        bool CharlistLogin(const char *username, const char *password);
        bool GameworldLogin();
        bool ParseGameworld(NetworkMessage *nm, unsigned char packetid);
        void GetPosition(NetworkMessage *nm, position_t *pos);
        char GetStackpos(NetworkMessage *nm);
};

#endif
