#ifdef USEENCRYPTION

#include "protocol.h"
#include "protocol77.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
Protocol77::Protocol77 () {
    protocolversion = 770;
    fingerprints[FINGERPRINT_TIBIADAT] = 0x439D5A33;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x439852BE;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x4450C8D8;
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
    nm.AddU32(atol(username));
    nm.AddString(password);

    nm.RSAEncrypt();

    // FIXME inside dump, we should check whether or not socket is still open
    // or after dump, at least
    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s);

    nm.XTEADecrypt(key);

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");
    return logonsuccessful;
}

#endif
