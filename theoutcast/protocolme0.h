#ifndef __PROTOCOLME0_H
#define __PROTOCOLME0_H

#include "protocol.h"

class ProtocolME0 : public Protocol {
    public:
        ProtocolME0();
        ~ProtocolME0();

        void CharlistConnect();
        bool CharlistLogin(const char *username, const char *password);
        void GameworldConnect();
        bool GameworldLogin ();
        void Close();

        // protocol properties
        bool CanCreateCharacter();
        // transmissives

        // transmissives EXTENSIONS
        void OCMCreateCharacter();
        void OCMCharlist();
};
#define ME0PROTOCOLVERSION 5000
#endif

