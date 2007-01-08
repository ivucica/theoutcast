#include <math.h>
#include <time.h>
#include "protocol.h"
#include "items.h"
Protocol* protocol;

Protocol::Protocol() {
    s = 0;
    protocolversion = 0;
    connectiontype = NONE;
    motd = "";
    errormsg = "";
    username = "";
    password = "";
    charlistselected = 0;
    charlistcount = 0;

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
    this->errormsg = "Protocol does not support this functionality.";
	return false;
}
bool Protocol::GameworldLogin() {
    this->errormsg = "Protocol does not support this functionality.";

    return false;
}

bool Protocol::ParsePacket(NetworkMessage *nm) {
    unsigned char packetid = nm->GetU8();
    printf("parsing packet\n");
    printf("Protocol %d: packet %02x\n", protocolversion, packetid);
    switch (connectiontype) {
        case CHARLIST:
            return ParseCharlist(nm, packetid);
        case GAMEWORLD:
            return ParseGameworld(nm, packetid);
        default:
            printf("Protocol %d: unknown connection type, cannot parse any packets\n", protocolversion);
            errormsg = "Unknown connection type, cannot parse any packets.\n\nIf this is a fully supported protocol, please report this bug!";
            logonsuccessful = false;
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
                logonsuccessful = false;
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
            default: {
                char tmp[256];
                printf("Protocol %d: unfamiliar charlist packet %02x\n", protocolversion, packetid);
                sprintf(tmp, "Protocol %d: Unfamiliar charlist packet %02x\n\nIf this is a fully supported protocol, please report this bug!", protocolversion, packetid);
                this->errormsg = tmp;

                logonsuccessful = false;
                return false;
            }
        }

}

bool Protocol::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    printf("Protocol %d: ParseGameworld() not implemented\n", protocolversion);
    errormsg = "ParseGameworld() not implemented.\n\nIf this is a fully supported protocol, please report this bug!";
    logonsuccessful = false;

    return false;
}



void Protocol::ParseMapDescription (NetworkMessage *nm, int w, int h, int destx, int desty, int destz) {
    int startz, endz, stepz;
    unsigned int skip=0;
    if (destz > 7) { // if we're underground
        startz = destz - 2; // then we see two floors above
        endz = min (this->maxz, destz + 2); // and two floors below
        stepz = 1; // and we move from top to bottom
        printf("Underground\n");
        //system("pause");
    } else { // if we're on the surface
        startz = 7; // we see from the surface leve
        endz = 0; // to the top
        stepz = -1;
        printf("Surface\n");
        //system("pause");
    }
    skip = 0;
    for (int z = startz; z != endz; z+=stepz) {
        ParseFloorDescription(nm, w, h, destx, desty, z, &skip);
    }
}
void Protocol::ParseFloorDescription(NetworkMessage *nm, int w, int h, int destx, int desty, int destz, unsigned int *skip) {

    //static unsigned int skip; // statics are kept between function calls ... neato! cooooool! yipiiiyeah! :)
    // however im not sure what happens if they're declaration-time initialized ... are they reinitialized with every function call?


    for (int y = desty; y < desty + h; y++) {
        for (int x = destx; x < destx + w; x++) {
            if (!*skip) {
                if (nm->PeekU16() >= 0xFF00) {
                    *skip = (nm->GetU16() & 0xFF);
                    printf("Skipping %d tiles\n", *skip);
                } else {
                    ParseTileDescription(nm, x, y, destz);
                    *skip = (nm->GetU16() & 0xFF);
                    printf("Skipping %d tiles\n", *skip);
                }
            } else {
                (*skip)--;
            }
        }
    }
}

void Protocol::ParseTileDescription(NetworkMessage *nm, int x, int y, int z) {
    for (;;) {
        if (nm->PeekU16() >= 0xFF00) {
            printf("Reached end of tile\n");
            return;
        } else {
            Object *obj;
            ParseObjectDescription(nm, obj);
        }
    }
}

void Protocol::ParseObjectDescription(NetworkMessage *nm, Object *obj) {
    // MUST ACCEPT NULL as second param
    int type = nm->GetU16();

    // temporary vars that will be stored inside object's description once Object class is defined
    int looktype;
    printf("Object type %d\n", type);
    switch (type) {
        case 0x0061: // new creature
        case 0x0062: // known creature
            if (type == 0x0061) { // new creature
                nm->GetU32(); // remove creature with this id
                nm->GetU32(); // new creature's id
                nm->GetString(); // name string
            }
            if (type == 0x0062) {
                nm->GetU32(); // known creature's id
            }
            nm->GetU8(); // health percent
            nm->GetU8(); // direction
            if (protocolversion < 770) {
                looktype = nm->GetU8();
            } else {
                looktype = nm->GetU16();
            }

            if (looktype) { // regular creature look
                nm->GetU8(); // head
                nm->GetU8(); // body
                nm->GetU8(); // legs
                nm->GetU8(); // feet

                // 7.8+: GetByte(); // addons

            } else { // extended creature look
                nm->GetU16(); // itemid that this creature looks like
            }

            nm->GetU8(); // lightlevel
            nm->GetU8(); // lightcolor

            nm->GetU16(); // speedindex


            // only in 7.5 +
            nm->GetU8(); // skull
            nm->GetU8(); // shield
            break;
        case 0x0063: // creature that has only direction altered
            nm->GetU32(); // creature id
            nm->GetU8(); // look direction

            break;
        default: // regular item
        // FIXME perhaps the order is not right ... maybe splash/fluidcontainer color is coming first, not stackable amount!
        //       check with a realworld example
            //ASSERT(type >= 100 && type <= items_n);
            printf("Item %d\n", type);
            if (items[type].stackable) {
                printf("Count %d\n", nm->GetU8());
            }
            if (items[type].splash || items[type].fluidcontainer) {
                printf("Color %d\n", nm->GetU8());
            }

    }
}

bool Protocol::CipSoft() {
    return false;
}

unsigned short Protocol::GetProtocolVersion () {
    printf("PROTOCOL VERSION : %d\n", protocolversion);
    return protocolversion;
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
