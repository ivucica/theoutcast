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
#include "gm_charmgr.h"
#include "networkmessage.h"
#include "types.h"
#include "thing.h"
#include "creature.h"
#include "threads.h"
class Protocol {
    public:
        Protocol();
        virtual ~Protocol();
        void SetSocket(SOCKET socket);
        std::string GetMotd() {return motd;}
        std::string GetError() {return errormsg;}
        void SetCharacter(int i) {charlistselected = i;}
        void SetProtocolStatus(const char *protostat);


        bool CipSoft();
        bool CipSoft(bool cipsoft);

        virtual bool    ParsePacket (NetworkMessage *nm);
        virtual bool    ParseCharlist (NetworkMessage *nm, unsigned char packetid);
        virtual bool    ParseGameworld(NetworkMessage *nm, unsigned char packetid);
        virtual void    CharlistConnect();
        virtual bool    CharlistLogin(const char *username, const char *password);
        virtual void    GameworldConnect();
        virtual bool    GameworldLogin();
        virtual bool    GameworldWork();
        virtual void    Close();




        // transmissives
        virtual void    Move(direction_t dir);
        virtual void    Turn(direction_t dir);

        virtual void    SetStance(stanceaggression_t aggression, stancechase_t chase);
        void            Speak(speaktype_t sp, const char *message);
        void            Speak(speaktype_t sp, const char *message, const char *destination);
        void            Speak(speaktype_t sp, const char *message, unsigned long destination);
        virtual void    Speak(speaktype_t sp, const char *message, const char *deststr, unsigned long destlong);

        virtual void    LookAt(position_t *pos);
        virtual void    Attack(unsigned long creatureid);
        virtual void    Use(position_t *pos, unsigned char stackpos);
        virtual void    Use(position_t *pos1, unsigned char stackpos1, position_t *pos2, unsigned char stackpos2);
        virtual void    Move(position_t *pos1, unsigned char stackpos1, position_t *pos2, unsigned char stackpos2, unsigned char amount);
        virtual void    CloseContainer(unsigned char cid);

        virtual void    InviteParty(Creature*c);
        virtual void    JoinParty(Creature *c);
        virtual void    RevokeInviteParty(Creature *c);
        virtual void    PassLeadershipParty(Creature *c);
        virtual void    LeaveParty();

        virtual void    Logout();


        // protocol properties
        virtual bool    CanCreateCharacter();

        // transmissives EXTENSIONS

        virtual void    OCMCreateCharacter();
        virtual void    OCMCharlist();


        // *parse* are "smarter" abstractions
        // *get* are those that only fetch and return

        // abstractions that return directly
        virtual Creature*       GetCreatureByID(NetworkMessage *nm);
        virtual char            GetStackpos(NetworkMessage *nm);
        virtual unsigned short  GetItemTypeID(NetworkMessage *nm);
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


        // transmited abstractions
        virtual void            AddPosition(NetworkMessage *nm, const position_t *pos);

        // internal stuff
        unsigned short          GetProtocolVersion ();

		// threads
        ONThreadId thrCharList, thrGWLogon;



        // public variables
        std::string charlistserver;
        unsigned short charlistport;
        std::string gameworldserver;
        unsigned short gameworldport;
        std::string username, password;

        gamemode_t newgamemode; // to which gamemode shall we be switching?
    protected:
        SOCKET s;
        unsigned short protocolversion;
        unsigned long fingerprints[3];
        unsigned long key[4];
        connectiontype_t connectiontype;
        std::string motd, errormsg;
        unsigned char charlistcount, charlistselected;
        character_t **charlist;
        unsigned int premiumdays;
        bool logonsuccessful;
        int maxx, maxy, maxz;
        bool active;
        bool cipsoft;
        unsigned short lastsuccessfulitem;
        ONCriticalSection threadsafe;


    friend void GM_MainMenu_CharList_Character(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu::CreateCharlist();
    friend void GM_MainMenu::DestroyCharlist();
    friend void GM_CharMgr::CreateCharlist();
    friend void GM_CharMgr::DestroyCharlist();
    friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon(ONThreadFuncArgumentType menuclass_void);
    friend class Map;



};

// now include all protocols
// subvariants should be "tabbed"
class Protocol75;
class Protocol76;
#ifdef USEENCRYPTION
	class Protocol77;
	class Protocol79;
	class Protocol792;
	class Protocol80;
#endif
#ifdef INCLUDE_SP
	class ProtocolSP;
#endif
#ifdef INCLUDE_ME
	class ProtocolME0;
#endif


extern Protocol* protocol;
bool ProtocolSetVersion (unsigned short protocolversion);

#endif

