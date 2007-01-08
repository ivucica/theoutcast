
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
bool Protocol76::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    switch (packetid) {
        case 0x0A: // Creature ID
            printf("Own creature ID: %d\n", nm->GetU32());

            return true;
        case 0x14: // Login error message
            errormsg = nm->GetString();
            logonsuccessful = false;
            return false;
        case 0x32: // Something Else, Bug Report
            printf("Unknown value: %d\n", nm->GetU8());
            printf("Can report bugs: %d\n", nm->GetU8());
            return true;
        case 0x64: // Player Location Initialization
            {
                int x, y, z;
                x = nm->GetU16();
                y = nm->GetU16();
                z = nm->GetU8();

                printf("Player location: %d %d %d\n", x, y, z);
                ItemsLoad();
                //errormsg = "Logon was a success.\n\nHowever, we don't support map fetching yet :/";
                //logonsuccessful = false;

                //return false;
                ParseMapDescription(nm, 18, 14, x - 8, y - 6, z);
                return true;
            }
        case 0x78: // Inventory Item
        case 0x79: // Inventory Empty
            nm->GetU8(); // item slot
            if (packetid == 0x78) {
                ParseObjectDescription(nm, NULL);
            }
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
        case 0x8D: // Creature Light
            nm->GetU32();//creature id
            nm->GetU8(); //lightradius
            nm->GetU8(); //lightcolor
            return true;
        case 0xA0: // Player Stats
        // 7.6 version
        // for more check out old outcast :/

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
            // Make a new function "getskill" which will fetch both level and percent

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
        case 0xB4: // Text Message
            nm->GetU8(); // msg class
            console.insert( nm->GetString() ); // message itself

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

