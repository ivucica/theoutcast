#ifndef __PROTOCOLSP_H
#define __PROTOCOLSP_H

#include "protocol.h"

class ProtocolSP : public Protocol {
    public:
        ProtocolSP();
        ~ProtocolSP();

        void CharlistConnect();
        bool CharlistLogin(const char *username, const char *password);
        void GameworldConnect();
        bool GameworldLogin ();
        void Close();

        // transmissives

        // transmissives EXTENSIONS (not that it matters for SP :P)
        void OCMCreateCharacter();
        void OCMCharlist();
};

#endif

