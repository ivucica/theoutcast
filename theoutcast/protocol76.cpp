
#include "protocol.h"
#include "protocol77.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
#include "console.h"
Protocol76::Protocol76 () {
    protocolversion = 760;
    // FIXME Put CORRECT fingerprints
    fingerprints[FINGERPRINT_TIBIADAT] = 0x439D5A33;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x439852BE;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x4450C8D8;

    maxx = 18; maxy = 14; maxz = 14;
}

Protocol76::~Protocol76() {
}

bool Protocol76::CharlistLogin(const char *username, const char *password) {

    NetworkMessage nm;

    connectiontype = CHARLIST;

    nm.AddU8(0x01); // protocol id
    nm.AddU16(0x02); // client OS
    nm.AddU16(protocolversion);

    nm.AddU32(fingerprints[FINGERPRINT_TIBIADAT]); // tibia.dat
    nm.AddU32(fingerprints[FINGERPRINT_TIBIASPR]); // tibia.spr
    nm.AddU32(fingerprints[FINGERPRINT_TIBIAPIC]); // tibia.pic


    // account number and password
    nm.AddU32(atol((this->username = username).c_str()));
    nm.AddString(this->password = password);


    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s);
    //nm.ShowContents();

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    return logonsuccessful;
}

bool Protocol76::GameworldLogin () {
    NetworkMessage nm;

    console.clear();

    connectiontype = GAMEWORLD;

    nm.AddU8(0x0A); // protocol id

    nm.AddU16(0x02); // client OS
    nm.AddU16(protocolversion);


    // are we a gamemaster
    nm.AddChar(0);

    // account number and password
    nm.AddU32(atol(this->username.c_str())); // this does NOT exist before 7.4
    nm.AddString(this->charlist[this->charlistselected].charactername);
    nm.AddString(this->password);



    // FIXME inside dump, we should check whether or not socket is still open
    // or after dump, at least
    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s );

    //nm.ShowContents();

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");
    return logonsuccessful;
}
// FIXME abstract the position retrieval
bool Protocol76::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    switch (packetid) {
        case 0x0A: // Creature ID

            player = new Player(nm->GetU32());
            printf("Own creature ID: %d\n", player->GetID(););
            return true;
        case 0x0B: // GM Actions
            nm->Trim(32); // unknown
            return true;
        case 0x14: // Generic login error message
            errormsg = nm->GetString();
            logonsuccessful = false;
            return false;
        case 0x15: // Messagebox Popup ("for your information")
            nm->GetString(); // message
            return true;
        case 0x16: // Too Many Players login error message
            errormsg = nm->GetString();
            {
                char tmp[256];
                sprintf(tmp, "\nYou can retry in %d seconds", (unsigned short)nm->GetU8());
                errormsg = errormsg + tmp;
            }
            logonsuccessful = false;
            return false;
        case 0x1E: // Ping Message
            // TODO Respond to the ping
            return true;
        case 0x32: // Something Else, Bug Report
            printf("Unknown value: %d\n", nm->GetU8());
            printf("Can report bugs: %d\n", nm->GetU8());
            return true;
        case 0x64: // Player Location Setup
            {
                position_t pos;
                GetPosition(nm, &pos);

                printf("Player location: %d %d %d\n", pos.x, pos.y, pos.z);

                // only if we're in main menu then let's do the item loading
                if (dynamic_cast<GM_MainMenu*>(game)) ItemsLoad();



                ParseMapDescription(nm, maxx, maxy, x - (maxx-1)/2, y - (maxy-1)/2, z);
                return true;
            }
        /*case 0x65: // Move Player North
            ParseMapDescription(nm, maxx, 1, x - (maxx-1)/2, y - (maxy - 1)/2, z);
            return true;
        case 0x66: // Move Player East
            ParseMapDescription(nm, 1, maxy, x - (maxx-1)/2, y + (maxy - 1)/2, z);
            return true;
        case 0x67: // Move Player South
            ParseMapDescription(nm, maxx, 1, x - (maxx-1)/2, y + (maxy - 1)/2, z);
            return true;
        case 0x68: // Move Player West
            ParseMapDescription(nm, maxx, 1, x + (maxx-1)/2, y - (maxy - 1)/2, z);
            return true;*/
        case 0x69: // Tile Update
        {
            position_t pos;
            GetPosition(nm, &pos);
            ParseTileDescription(nm, pos.x,pos.y,pos.z);
            return true;
        }
        case 0x6A: {// Add Item
            position_t pos;
            GetPosition(nm, &pos);

            ParseThingDescription(nm, NULL);
            return true;
        }
        case 0x6B: {// Replace Item
            position_t pos;
            GetPosition(nm, &pos);

            GetStackpos(nm);

            ParseThingDescription(nm, NULL);
            return true;
        }
        case 0x6C: {// Remove Item
            position_t pos;
            GetPosition(nm, &pos);

            GetStackpos(nm);

            return true;
        }
        case 0x6D: {// Move Item
            position_t src;
            position_t dst;
            GetPosition(nm, &src);
            GetStackpos(nm);
            GetPosition(nm, &dst);
            return true;
        }
        case 0x6E: // Container Open
            nm->GetU8(); // container id
            nm->GetU16(); // container icon
            nm->GetString(); // container title
            nm->GetU8(); // capacity
            nm->GetU8(); // hasparent
            {
                int itemcount = nm->GetU8(); // item count
                for (int i = 0 ; i < itemcount ; i++) {
                    ParseThingDescription(nm, NULL);
                }
            }
            return true;
        case 0x6F: // Container Close
            nm->GetU8();
            return true;
        case 0x70: // Add Container Item
            nm->GetU8(); // container id
            ParseThingDescription(nm, NULL);
            return true;
        case 0x71: // Replace Container Item
            nm->GetU8(); // container id
            nm->GetU8(); // slot
            ParseThingDescription(nm, NULL);

            return true;
        case 0x72: // Remove Container Item
            nm->GetU8(); // container id
            nm->GetU8(); // slot
            return true;
        case 0x78: // Inventory Item
        case 0x79: // Inventory Empty
            nm->GetU8(); // item slot
            if (packetid == 0x78) {
                ParseThingDescription(nm, NULL);
            }
            return true;
        case 0x7D: // Trade Request
        case 0x7E: // Trade Ack
            nm->GetString(); // Other player
            {
                int itemcount = nm->GetU8();
                for (int i = 0; i < itemcount; i++) {
                    ParseThingDescription(nm, NULL);
                }
            }
            return true;
        case 0x7F: // Trade Close
            return true;
        case 0x82: // World Light
            nm->GetU8(); // Light Level
            nm->GetU8(); // Light Color
            return true;
        case 0x83: // Magic Effect
            nm->GetU16(); // position
            nm->GetU16();
            nm->GetU8();

            nm->GetU8(); // mageffect type
            return true;
        case 0x84: // Animated Text
            nm->GetU16(); // position
            nm->GetU16();
            nm->GetU8();

            nm->GetU8(); // color
            nm->GetString(); // message
            return true;
        case 0x85: // Distance Shot
            nm->GetU16(); // position FROM
            nm->GetU16();
            nm->GetU8();

            nm->GetU16(); // position TO
            nm->GetU16();
            nm->GetU8();

            nm->GetU8(); // type of shot
            return true;
        case 0x86: // Creature Square
            nm->GetU32(); // around which creature
            nm->GetU8(); // square color
            return true;
        case 0x8C: // Creature HP
            nm->GetU32(); // creature
            nm->GetU8(); // health percent
            return true;
        case 0x8D: // Creature Light
            nm->GetU32();//creature id
            nm->GetU8(); //lightradius
            nm->GetU8(); //lightcolor
            return true;
        case 0x8E: // Set Creature Outfit
        // FIXME abstract this through "getcreaturelook" or somethinglikehtat
            nm->GetU32(); // creature id
            nm->GetU8(); // look type
            nm->GetU8(); // look head
            nm->GetU8(); // look body
            nm->GetU8(); // look legs
            nm->GetU8(); // look feet

            return true;
        case 0x8F: // Creature Speed
            nm->GetU32(); // creature id
            nm->GetU16(); // speed index
            return true;
        case 0x90: // Creature Skull
            nm->GetU32(); // creature id
            nm->GetU8(); // skull type
            return true;
        case 0x91: // Creature Shield
            nm->GetU32(); // creature id
            nm->GetU8(); // shield type
            return true;
        case 0x96: // Text Window
            nm->GetU32(); // window id
            nm->GetU16(); // itemid -- for icon?
            nm->GetU16(); // max length
            nm->GetString();
            return true;
        case 0x97: // House Window
            nm->GetU8(); // 0?
            nm->GetU32(); // creature id
            nm->GetString(); // dunno, probably house name?
            return true;
        case 0xA0: // Player Stats
        // 7.6 version
        // for more check out old outcast :/

// FIXME abstract this with getplayerstats
            nm->GetU16(); // hp
            nm->GetU16(); // max hp
            nm->GetU16(); // cap
            nm->GetU32(); // exp
            nm->GetU16(); // lvl
            nm->GetU8(); // level percent
            nm->GetU16(); // mp
            nm->GetU16(); // mmp
            nm->GetU8(); // mag lvl
            nm->GetU8(); // maglvl percent
            nm->GetU8(); // soul

            break;
        case 0xA1: // Player Skills
            // FIXME Make a new function "getskill" which will fetch both level and percent of one particular skill
            // FIXME Make a new function "getskills" which will fetch all skills

            nm->GetU8(); // Fist Level
            nm->GetU8(); // Fist Percent
            nm->GetU8(); // Club Level
            nm->GetU8(); // Club Percent
            nm->GetU8(); // Sword Level
            nm->GetU8(); // Sword Percent
            nm->GetU8(); // Axe Level
            nm->GetU8(); // Axe Percent
            nm->GetU8(); // Distance Level
            nm->GetU8(); // Distance Percent
            nm->GetU8(); // Shield Level
            nm->GetU8(); // Shield Percent
            nm->GetU8(); // Fish Level
            nm->GetU8(); // Fish Percent


            return true;
        case 0xA2: // Status Icons
            nm->GetU8(); //
            return true;
        case 0xA3: // Cancel Attack
            return true;
        case 0xAA: // Creature Speak
            nm->GetU32(); // OT says always 0, perhaps it is NOT!
            nm->GetString(); // creature name
            if (protocolversion >= 780) {
                nm->GetU16(); // player level
            }
            {
                unsigned char msgtype = nm->GetU8();
                switch (msgtype) {
                    case 0x01: // say
                    case 0x02: // whisper
                    case 0x03: // yell

                    case 0x10: // monster 1
                    case 0x11: // monster 2
                        nm->GetU16(); // position
                        nm->GetU16();
                        nm->GetChar();
                        break;
                    case 0x05: // yellow
                    case 0x0A: // red -- r1 #c gamemaster command
                    case 0x0E: // red -- r2 #d counsellor command ??

                        nm->GetU16(); // channel id
                        break;
                    case 0x09: // broadcast
                    case 0x04: // private
                    case 0x0B: // private red     @name@text
                    case 0x0C: // orange
                        break;

                }
            }
            nm->GetString(); // message

            return true;
        case 0xAB: // Channels Dialog
            {
                unsigned char chancount = nm->GetU8();
                for (int i = 0 ; i < chancount ; i++) {
                    nm->GetU16(); // channel id
                    nm->GetString(); // channel name
                }
            }
            return true;
        case 0xAC: // Channel Open
            nm->GetU16(); // channel id
            nm->GetString(); // channel name
            return true;
        case 0xAD: // Private Open
            nm->GetString(); // Player name
            return true;
        case 0xB2: // Create Private Channel
            nm->GetU16(); // channel id
            nm->GetString(); // channel name
            return true;
        case 0xB4: // Text Message
            nm->GetU8(); // msg class
            console.insert( nm->GetString() ); // message itself

            return true;
        case 0xB5: // Cancel Walk
            nm->GetU8(); // direction
            return true;
        //case 0xBE: // Floor Up
            //return false;
        //case 0xBF: // Floor Down
            // return false;
        case 0xC8: // Outfit List
            // FIXME abstract this through "getcreaturelook" or somethinglikehtat
            nm->GetU32(); // creature id
            nm->GetU8(); // look type
            nm->GetU8(); // look head
            nm->GetU8(); // look body
            nm->GetU8(); // look legs
            nm->GetU8(); // look feet

            nm->GetU8(); // first outfit
            nm->GetU8(); // last outfit

            return true;
        case 0xD2: // VIP Add
            nm->GetU32(); // GUID
            nm->GetString(); // creature name
            nm->GetU8(); // online
            return true;
        case 0xD3: // VIP Login
            nm->GetU32(); // GUID
            return true;
        case 0xD4: // VIP Logout
            nm->GetU32(); // GUID
            return true;
        default: {
            char tmp[256];
            printf("Protocol %d: unfamiliar gameworld packet %02x\n", protocolversion, packetid);
            sprintf(tmp, "Protocol %d: Unfamiliar gameworld packet %02x\n\nIf this is a fully supported protocol, please report this bug!", protocolversion, packetid);
            this->errormsg = tmp;

            logonsuccessful = false;
            return false;
        }

    }
}

void Protocol76::GetPosition(NetworkMessage *nm, position_t *pos) {
    pos->x = nm->GetU16();
    pos->y = nm->GetU16();
    pos->z = nm->GetU8();
}
char Protocol76::GetStackpos(NetworkMessage *nm) {
    return nm->GetU8(); // stackpos
}
