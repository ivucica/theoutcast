#ifdef USEENCRYPTION

#include "protocol.h"
#include "protocol79.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
#include "items.h"
#include "assert.h"
#include "console.h"
#include "player.h"
#include "map.h"
#include "sound.h"
Protocol79::Protocol79 () {
    protocolversion = 790;
    fingerprints[FINGERPRINT_TIBIADAT] = 0x439D5A33;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x439852BE;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x4450C8D8;

    maxx = 18; maxy = 14; maxz = 14;
}

Protocol79::~Protocol79() {
}

bool Protocol79::CharlistLogin(const char *username, const char *password) {
    NetworkMessage nm;

    ONThreadSafe(threadsafe);

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


    if (!nm.Dump(s)) {
        this->errormsg = "Could not write to socket.\nPossibly premature disconnect.";
        ONThreadUnsafe(threadsafe);
        return false;
    }

    nm.Clean();
    if (!nm.FillFromSocket(s)) {
        this->errormsg = "Could not read from socket.\nPossibly premature disconnect.";
        ONThreadUnsafe(threadsafe);
        return false;
    }

    Close();

    nm.XTEADecrypt(key);

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    ONThreadUnsafe(threadsafe);
    return logonsuccessful;
}

bool Protocol79::GameworldLogin () {
    // this is valid for 7.9!
    // 7.7 has a bit different order of stuff! check out old outcast's sources
    NetworkMessage nm;

    SetProtocolStatus("Preparing to transfer logon data...");
    connectiontype = GAMEWORLD;

    nm.AddU8(0x0A); // protocol id

    // in 7.72 onwards move this BEFORE the keys and BEFORE the encryption
    nm.AddU16(0x02); // client OS
    nm.AddU16(protocolversion);

    SetProtocolStatus("RSA encryption...");
    nm.RSABegin();

    //key[3] = 92;
    // encryption keys
    for (int i = 0 ; i < 4 ; i++) {
        nm.AddU32(key[i]);
        //printf("KEY %d - %d\n", i, key[i]);
    }


    // are we a gamemaster
    nm.AddChar(0);

    // account number and password
    nm.AddU32(atol(this->username.c_str())); // this does NOT exist before 7.4
    nm.AddString(this->charlist[this->charlistselected].charactername);
    nm.AddString(this->password);



    nm.RSAEncrypt();

    SetProtocolStatus("Transmitting logon data...");

    if (!nm.Dump(s)) {
        this->errormsg = "Could not write to socket.\nPossibly premature disconnect.";
        return false;
    }


//    SetStance(DEFENSIVE, STAND);

    SetProtocolStatus("Waiting for response...");
    //nm.Clean();
    NetworkMessage nm2;
    //nm.FillFromSocket(s);
    if (!nm2.FillFromSocket(s )) {
        this->errormsg = "Could not read from socket.\nPossibly premature disconnect.";
        return false;
    }
    nm2.XTEADecrypt(key);
    logonsuccessful = true;
    while ((signed int)(nm2.GetSize())>0 && ParsePacket(&nm2));
    if ((signed int)(nm2.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    if (logonsuccessful) active = true;

    return logonsuccessful;
}

#endif
