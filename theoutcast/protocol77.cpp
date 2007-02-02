#ifdef USEENCRYPTION

#include "protocol.h"
#include "protocol77.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
#include "items.h"
#include "assert.h"
#include "console.h"
Protocol77::Protocol77 () {
    protocolversion = 770;
    fingerprints[FINGERPRINT_TIBIADAT] = 0x439D5A33;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x439852BE;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x4450C8D8;

    maxx = 18; maxy = 14; maxz = 14;
}

Protocol77::~Protocol77() {
}

bool Protocol77::CharlistLogin(const char *username, const char *password) {

    NetworkMessage nm;

    connectiontype = CHARLIST;

    nm.AddU8(0x01); // protocol id
    nm.AddU16(0x02); // client OS
    nm.AddU16(protocolversion);

    nm.AddU32(fingerprints[FINGERPRINT_TIBIADAT]); // tibia.dat
    nm.AddU32(fingerprints[FINGERPRINT_TIBIASPR]); // tibia.spr
    nm.AddU32(fingerprints[FINGERPRINT_TIBIAPIC]); // tibia.pic

    nm.RSABegin();

    // encryption keys
    for (int i = 0 ; i < 4 ; i++) {
        nm.AddU32(key[i]);
    }

    // account number and password
    nm.AddU32(atol((this->username = username).c_str()));
    nm.AddString(this->password = password);

    nm.RSAEncrypt();

    // FIXME inside dump, we should check whether or not socket is still open
    // or after dump, at least
    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s);

    nm.XTEADecrypt(key);

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    if (logonsuccessful) active = true;

    return logonsuccessful;
}

bool Protocol77::GameworldLogin () {
    // this is valid for 7.7!
    // 7.72 has a bit different order of stuff! check out old outcast's sources
    NetworkMessage nm;

    connectiontype = GAMEWORLD;

    nm.AddU8(0x0A); // protocol id


    nm.RSABegin();

    // encryption keys
    for (int i = 0 ; i < 4 ; i++) {
        nm.AddU32(key[i]);
    }


    // in 7.72 onwards move this BEFORE the keys and BEFORE the encryption
    nm.AddU16(0x02); // client OS
    nm.AddU16(protocolversion);


    // are we a gamemaster
    nm.AddChar(0);

    // account number and password
    nm.AddU32(atol(this->username.c_str())); // this does NOT exist before 7.4
    nm.AddString(this->charlist[this->charlistselected].charactername);
    nm.AddString(this->password);


    nm.RSAEncrypt();

    // FIXME inside dump, we should check whether or not socket is still open
    // or after dump, at least
    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s );

    nm.XTEADecrypt(key);

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    if (logonsuccessful) active = true;

    return logonsuccessful;
}
bool Protocol77::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    switch (packetid) {
        default: {
            char tmp[256];
            printf("Protocol %d: unfamiliar gameworld packet %02x\n", protocolversion, packetid);
            sprintf(tmp, "Protocol %d: Unfamiliar gameworld packet %02x\nThis protocol is temporarily unsupported while\n7.6 is in development.", protocolversion, packetid);
            this->errormsg = tmp;

            this->Close();
            logonsuccessful = false;
            return false;
        }

    }
}


#endif
