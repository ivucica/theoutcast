#include <math.h>
#include <time.h>
#include "protocol.h"
Protocol* protocol;

Protocol::Protocol() {
    s = 0;
    protocolversion = 0;
    connectiontype = NONE;
    motd = "";
}
Protocol::~Protocol() {
}

void Protocol::SetSocket(SOCKET socket) {
    s = socket;
    srand(time(NULL));
    key[0] = rand();
    key[1] = rand();
    key[2] = rand();
    key[3] = rand();
}

bool Protocol::CharlistLogin(const char *username, const char *password) {
	return false;
}

bool Protocol::ParsePacket(NetworkMessage *nm) {
    unsigned char packetid = nm->GetU8();
    printf("Protocol %d: packet %02x\n", protocolversion, packetid);
    switch (connectiontype) {
        case CHARLIST:
            return ParseCharlist(nm, packetid);
            break;
        default:
            printf("Protocol %d: unknown connection type, cannot parse any packets\n", protocolversion);
            return false;
    }
    printf("Protocol %d: %d bytes remaining in message\n", protocolversion, nm->GetSize());
}



// so far, fortunately, same for all protocols
// if not, we can simply override ;)
bool Protocol::ParseCharlist (NetworkMessage *nm, unsigned char packetid) {
    switch (packetid) {

            case 0x0A:
                this->errormsg = nm->GetString();
                logonsuccessful = false;
                return true;

            case 0x14:
                motd = nm->GetString();
                return true;
            case 0x1E:
                this->errormsg = "Your client needs to use a newer protocol.\n"
                                 "If appropriate protocol is not available in\n"
                                 "this client or its newer version, you will\n"
                                 "need to use Tibia to connect to this server.\n"
                                 "We, however, do not condone this activity!";
                logonsuccessful = true;
                return true;

            case 0x64: {
                charlistcount = nm->GetU8();
                charlist = (character_t*)malloc(charlistcount * sizeof(character_t));
                for (int i = 0 ; i < charlistcount ; i++) {
                    nm->GetString(charlist[i].charactername, 128);
                    nm->GetString(charlist[i].worldname, 128);
                    charlist[i].ipaddress = nm->GetU32();
                    charlist[i].port = nm->GetU16();
                    //printf("Protocol %d: %s %s %d.%d.%d.%d port %d\n", protocolversion, charlist[i].charactername, charlist[i].worldname, ((unsigned char*)&charlist[i].ipaddress)[0], ((unsigned char*)&charlist[i].ipaddress)[1], ((unsigned char*)&charlist[i].ipaddress)[2], ((unsigned char*)&charlist[i].ipaddress)[3], charlist[i].port);
                }
                premiumdays = nm->GetU16();

                return true;
            }
            default:
                printf("Protocol %d: unfamiliar packet %02x\n", protocolversion, packetid);
                return false;
        }

}

bool Protocol::CipSoft() {
    return false;
}

bool ProtocolSetVersion (unsigned short protocolversion) {
    if (protocol) delete protocol;

    switch (protocolversion) {
        case 760:
            protocol = new Protocol76;
            return true;
#ifdef USEENCRYPTION
        case 770:
            protocol = new Protocol77;
            return true;
#endif
        default:
            return false;
    }

}
