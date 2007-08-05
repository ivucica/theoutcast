#ifndef __PROTOCOLSP_H
#define __PROTOCOLSP_H

#include "protocol.h"
#include "tile.h"
class ProtocolSP : public Protocol {
    public:
        ProtocolSP();
        ~ProtocolSP();

        void CharlistConnect();
        bool CharlistLogin(const char *username, const char *password);
        void GameworldConnect();
        bool GameworldLogin ();
        bool GameworldWork();
        void Close();

        // transmissives
        void Move(direction_t dir);
		void LookAt(position_t *pos);
        // transmissives EXTENSIONS (not that it matters for SP :P)
        void OCMCreateCharacter();
        void OCMCharlist();

        // helper
        void SPAddTile(Tile*t, NetworkMessage *nm);
        void SPFillTile(Tile &t, std::vector<Thing*>&remover, int i, int j, int k);
        void SPAddCreature(NetworkMessage *nm, Creature* cr);
};

#endif

