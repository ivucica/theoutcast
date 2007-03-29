
#include "protocol.h"
#include "protocol75.h"
#include "networkdirect.h"
#include "networkmessage.h"
#include "defines.h"
#include "console.h"
#include "player.h"
#include "map.h"
#include "tile.h"
#include "sound.h"
Protocol75::Protocol75 () {
    protocolversion = 750;
    // FIXME Put CORRECT fingerprints
    fingerprints[FINGERPRINT_TIBIADAT] = 0x439D5A33;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x439852BE;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x4450C8D8;

    maxx = 18; maxy = 14; maxz = 14;
}

Protocol75::~Protocol75() {
}

bool Protocol75::CharlistLogin(const char *username, const char *password) {

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

    if (logonsuccessful) active = true;
    return logonsuccessful;
}

bool Protocol75::GameworldLogin () {
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

    if (logonsuccessful) active = true;

    return logonsuccessful;
}



void Protocol75::GetPlayerStats(NetworkMessage *nm) {
// 7.5 version
// for more check out old outcast :/


    player->SetHP(nm->GetU16()); // hp
    player->SetMaxHP(nm->GetU16()); // max hp
    player->SetCap(nm->GetU16()); // cap
    player->SetExp(nm->GetU32()); // exp
    player->SetLevel(nm->GetU8()); // lvl
    player->SetLevelPercent(nm->GetU8()); // level percent
    player->SetMP(nm->GetU16()); // mp
    player->SetMaxMP(nm->GetU16()); // mmp
    player->SetMLevel(nm->GetU8()); // mag lvl
    player->SetMLevelPercent(nm->GetU8()); // maglvl percent
    player->SetSoulPoints(nm->GetU8()); // soul


}
