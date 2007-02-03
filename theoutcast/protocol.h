#ifndef __PROTOCOL_H
#define __PROTOCOL_H


#ifdef WIN32
	#include <windows.h>
	#include <winsock.h>
#else
    #ifndef SOCKET
        typedef int SOCKET;
	#endif
#endif
#include "gm_mainmenu.h"
#include "networkmessage.h"
#include "types.h"
#include "thing.h"
#include "creature.h"
class Protocol {
    public:
        Protocol();
        ~Protocol();
        void SetSocket(SOCKET socket);
        std::string GetMotd() {return motd;}
        std::string GetError() {return errormsg;}
        void SetCharacter(int i) {charlistselected = i;}

        bool CipSoft();
        bool CipSoft(bool cipsoft);

        virtual bool    ParsePacket (NetworkMessage *nm);
        virtual bool    ParseCharlist (NetworkMessage *nm, unsigned char packetid);
        virtual bool    ParseGameworld(NetworkMessage *nm, unsigned char packetid);
        virtual bool    CharlistLogin(const char *username, const char *password);
        virtual bool    GameworldLogin();
        virtual bool    GameworldWork();
        virtual void    Close();

        // transmissives
        virtual void    MoveNorth();
        virtual void    MoveSouth();
        virtual void    MoveWest();
        virtual void    MoveEast();

        virtual void    SetStance(stanceaggression_t aggression, stancechase_t chase);

        // *parse* are "smarter" abstractions
        // *get* are those that only fetch and return

        // abstractions that return directly
        virtual Creature*       GetCreatureByID(NetworkMessage *nm);
        virtual char            GetStackpos(NetworkMessage *nm);
        virtual Thing*          ParseThingDescription(NetworkMessage *nm);

        // abstractions that manipulate directly
        virtual void            GetPlayerStats(NetworkMessage *nm);
        virtual void            GetPlayerSkill(NetworkMessage *nm, skill_t skillid);
        virtual void            GetPlayerSkills(NetworkMessage *nm);
        virtual void            ParseMapDescription (NetworkMessage *nm, int w, int h, int destx, int desty, int destz);
        virtual void            ParseFloorDescription(NetworkMessage *nm, int w, int h, int destx, int desty, int destz, unsigned int *skip);
        virtual void            ParseTileDescription(NetworkMessage *nm, int x, int y, int z);


        // abstractions that store in a struct
        virtual void            GetPosition(NetworkMessage *nm, position_t *pos);
        virtual void            ParseCreatureLook(NetworkMessage *nm, creaturelook_t *crl);

        // internal stuff
        unsigned short GetProtocolVersion ();
    protected:
        SOCKET s;
        unsigned short protocolversion;
        unsigned long fingerprints[3];
        unsigned long key[4];
        connectiontype_t connectiontype;
        std::string motd, errormsg;
        unsigned char charlistcount, charlistselected;
        character_t *charlist;
        unsigned int premiumdays;
        bool logonsuccessful;
        std::string username, password;
        int maxx, maxy, maxz;
        bool active;
        bool cipsoft;


    friend void GM_MainMenu_CharList_Character(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu::CreateCharlist();
    friend void GM_MainMenu::DestroyCharlist();
};

// now include all protocols
// subvariants should be "tabbed"
#include "protocol76.h"
#ifdef USEENCRYPTION
#include "protocol77.h"
#include "protocol79.h"
  #include "protocol792.h"
#endif

extern Protocol* protocol;
bool ProtocolSetVersion (unsigned short protocolversion);

#endif

