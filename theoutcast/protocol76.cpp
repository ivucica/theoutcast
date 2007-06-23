
#include "protocol.h"
#include "protocol76.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
#include "console.h"
#include "player.h"
#include "map.h"
#include "tile.h"
#include "sound.h"
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
    if ((signed int)(nm.GetSize())!=0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++ %d remain\n", nm.GetSize());


    Close();
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
    nm.AddString(this->charlist[this->charlistselected]->charactername);
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

    if (logonsuccessful) active = true;

    return logonsuccessful;
}
