
#include "protocol.h"
#include "protocol77.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
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
    nm.AddU32(atol(username));
    nm.AddString(password);

    nm.Dump(s);

    nm.Clean();
    nm.FillFromSocket(s);

    logonsuccessful = true;
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    return logonsuccessful;
}
