#include <math.h>
#include <time.h>
#include "protocol.h"
#include "items.h"
#include "debugprint.h"
#include "thing.h"
#include "tile.h"
#include "map.h"
#include "player.h"
#include "console.h"
#include "sound.h"
#include "assert.h"
#include "bsdsockets.h"
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
    ONInitThreadSafe(threadsafe);

}
Protocol::~Protocol() {
    ONDeinitThreadSafe(threadsafe);
}

void Protocol::SetSocket(SOCKET socket) {
    ONThreadSafe(threadsafe);
    s = socket;
    srand(time(NULL));
    key[0] = rand() % 0xFFFFFFFF;
    key[1] = rand() % 0xFFFFFFFF;
    key[2] = rand() % 0xFFFFFFFF;
    key[3] = rand() % 0xFFFFFFFF;

    printf("ACTIVATED KEYS %u %u %u %u\n", key[0], key[1], key[2], key[3]);
    ONThreadUnsafe(threadsafe);
}

bool Protocol::CharlistLogin(const char *username, const char *password) {
    this->errormsg = "Protocol does not support this functionality.";
	return false;
}
bool Protocol::GameworldLogin() {
    this->errormsg = "Protocol does not support this functionality.";

    return false;
}

bool Protocol::GameworldWork() {
    NetworkMessage nm;


    if (!active) return false;

    nm.Clean();
    nm.Flush();

    nm.FillFromSocket(s);

    if (!nm.GetSize()) return false;
    ONThreadSafe(threadsafe);
    if (protocolversion >= 770) nm.XTEADecrypt(key);


    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));

    if ((signed int)(nm.GetSize())!=0) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_WARNING, "++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++ %d remain\n", nm.GetSize());
        nm.ShowContents();
    }
    ONThreadUnsafe(threadsafe);
    return true;
}

void Protocol::Close() {
    ONThreadSafe(threadsafe);
    #ifdef WIN32
	shutdown(s, SD_BOTH);
	#endif
    closesocket(s);
    printf("CLOSED SOCKET!!!\n");
//    system("pause");
    active = false;
    s = 0;
    ONThreadUnsafe(threadsafe);
}
bool Protocol::ParsePacket(NetworkMessage *nm) {
    unsigned char packetid = nm->GetU8();

    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Protocol %d: packet %02x\n", protocolversion, packetid);
    switch (connectiontype) {
        case CHARLIST:
            return ParseCharlist(nm, packetid);
        case GAMEWORLD:
            return ParseGameworld(nm, packetid);
        default:
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Protocol %d: unknown connection type, cannot parse any packets\n", protocolversion);
            errormsg = "Unknown connection type, cannot parse any packets.\n\nIf this is a fully supported protocol, please report this bug!";
            logonsuccessful = false;
            return false;
    }
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Protocol %d: %d bytes remaining in message\n", protocolversion, nm->GetSize());
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
                    printf("Protocol %d: %s %s %d.%d.%d.%d port %d\n", protocolversion, charlist[i].charactername, charlist[i].worldname, ((unsigned char*)&charlist[i].ipaddress)[0], ((unsigned char*)&charlist[i].ipaddress)[1], ((unsigned char*)&charlist[i].ipaddress)[2], ((unsigned char*)&charlist[i].ipaddress)[3], charlist[i].port);
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
/*
bool Protocol::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    printf("Protocol %d: ParseGameworld() not implemented\n", protocolversion);
    errormsg = "ParseGameworld() not implemented.\n\nIf this is a fully supported protocol, please report this bug!";
    logonsuccessful = false;

    return false;
}
*/

void Protocol::GetPlayerStats(NetworkMessage *nm) {
// 7.6 version
// for more check out old outcast :/

// FIXME store into player

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

    /* dunno where this was added but it is there in 792 */
    if (protocolversion>=792) nm->GetU16(); // stamina (minutes)

}

Creature *Protocol::GetCreatureByID(NetworkMessage *nm) {
    return gamemap.GetCreature(nm->GetU32(), NULL);
}
unsigned short Protocol::GetItemTypeID(NetworkMessage *nm) {
    return nm->GetU16();
}

void Protocol::GetPosition(NetworkMessage *nm, position_t *pos) {
    pos->x = nm->GetU16();
    pos->y = nm->GetU16();
    pos->z = nm->GetU8();
}
char Protocol::GetStackpos(NetworkMessage *nm) {
    return nm->GetU8(); // stackpos
}

void Protocol::ParseCreatureLook(NetworkMessage *nm, creaturelook_t *crl) {
    if (protocolversion < 770) {
        crl->type = nm->GetU8();
    } else {
        crl->type = nm->GetU16();
    }

    if (crl->type) { // regular creature look
        crl->head = nm->GetU8(); // head
        crl->body = nm->GetU8(); // body
        crl->legs = nm->GetU8(); // legs
        crl->feet = nm->GetU8(); // feet
        if (protocolversion < 780)
            crl->addons = 0;
        else
            crl->addons = nm->GetU8(); // addons

    } else { // extended creature look
        crl->extendedlook = nm->GetU16(); // itemid that this creature looks like
    }
}

void Protocol::GetPlayerSkill(NetworkMessage *nm, skill_t skillid) {
    nm->GetU8(); // level
    nm->GetU8(); // percent

    // store into player
}
void Protocol::GetPlayerSkills(NetworkMessage *nm) {
    GetPlayerSkill(nm, FIST);
    GetPlayerSkill(nm, CLUB);
    GetPlayerSkill(nm, SWORD);
    GetPlayerSkill(nm, AXE);
    GetPlayerSkill(nm, DISTANCE);
    GetPlayerSkill(nm, SHIELDING);
    GetPlayerSkill(nm, FISHING);
}

bool Protocol::ParseGameworld(NetworkMessage *nm, unsigned char packetid) {
    printf("PACKET %d\n", packetid);
    switch (packetid) {
        case 0x0A: // Creature ID

            player = new Player(nm->GetU32());
            printf("Own creature ID: %d\n", player->GetCreatureID());
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
        case 0x1E: {// Ping Message
            NetworkMessage nm2;
            nm2.AddU8(0x1E);
            if (this->protocolversion >= 770) nm2.XTEAEncrypt(key);
            nm2.Dump(s);
            }
            return true;
        case 0x32: // Something Else, Bug Report
            printf("Unknown value: %d\n", nm->GetU8());
            printf("Can report bugs: %d\n", nm->GetU8());
            return true;
        case 0x64: // Player Location Setup
            {
                position_t pos;
                GetPosition(nm, &pos);
                player->SetPos(&pos);
                printf("Player location: %d %d %d\n", pos.x, pos.y, pos.z);

                // only if we're in main menu then let's do the item loading
                if (dynamic_cast<GM_MainMenu*>(game)) {
                    ItemsLoad();
                    CreaturesLoad();
                }

                ParseMapDescription(nm, maxx, maxy, pos.x - (maxx-1)/2, pos.y - (maxy-1)/2, pos.z);
                player->FindMinZ();
                return true;
            }
        case 0x65: // Move Player North
            printf("Move north\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX(), player->GetPosY()-1, player->GetPosZ());

            ParseMapDescription(nm, maxx, 1, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            printf("End move north\n");

            return true;
        case 0x66: // Move Player East
            printf("Move east\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX()+1, player->GetPosY(), player->GetPosZ());

            ParseMapDescription(nm, 1, maxy, player->GetPos()->x + (maxx+1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            printf("End move east\n");

            return true;
        case 0x67: // Move Player South
            printf("Move south\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX(), player->GetPosY()+1, player->GetPosZ());

            ParseMapDescription(nm, maxx, 1, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y + (maxy+1 )/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            printf("End move south\n");

            return true;
        case 0x68: // Move Player West
            printf("Move west\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX()-1, player->GetPosY(), player->GetPosZ());

            ParseMapDescription(nm, 1, maxy, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            printf("End move west\n");
            return true;
        case 0x69: {// Tile Update
            position_t pos;
            gamemap.Lock();
            GetPosition(nm, &pos);
            ParseTileDescription(nm, pos.x,pos.y,pos.z);
            gamemap.Unlock();
            return true;
        }
        case 0x6A: {// Add Item
            position_t pos;
            gamemap.Lock();
            GetPosition(nm, &pos);
            printf("%d %d %d\n", pos.x, pos.y, pos.z);
            Tile *tile = gamemap.GetTile(&pos);
            Thing *t;
            t = ParseThingDescription(nm);
            tile->insert(t);
            gamemap.Unlock();
            return true;
        }
        case 0x6B: {// Replace Item
            position_t pos;

            Tile *t;
            unsigned char stackpos;

            GetPosition(nm, &pos);

            t = gamemap.GetTile(&pos);

            stackpos = GetStackpos(nm);

            t->remove(stackpos);
            t->insert(ParseThingDescription(nm));
            return true;
        }
        case 0x6C: {// Remove Item
            position_t pos;
            GetPosition(nm, &pos);
            printf("Removing from %d %d %d\n", pos.x, pos.y, pos.z);
            unsigned char stackpos = GetStackpos(nm);
            printf("Stackpos %d\n", stackpos);
            Tile *tile = gamemap.GetTile(&pos);
            tile->remove(stackpos);

            return true;
        }
        case 0x6D: {// Move Item
            unsigned char stackpos;
            Tile *tile;
            Thing *thing;
            position_t src;
            position_t dst;

            GetPosition(nm, &src);
            stackpos = GetStackpos(nm);

            tile = gamemap.GetTile(&src);
            thing = tile->getstackpos(stackpos);
            tile->remove(stackpos);

            GetPosition(nm, &dst);

            tile = gamemap.GetTile(&dst);
            tile->insert(thing);

            if (dst.y < src.y) thing->SetDirection(NORTH);
            if (dst.y > src.y) thing->SetDirection(SOUTH);
            if (dst.x < src.x) thing->SetDirection(WEST);
            if (dst.x > src.x) thing->SetDirection(EAST);

            if (thing != player->GetCreature() || !thing->IsMoving())
                thing->StartMoving();
            thing->ApproveMove();
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
                    delete ParseThingDescription(nm);
                }
            }
            return true;
        case 0x6F: // Container Close
            nm->GetU8();
            return true;
        case 0x70: // Add Container Item
            nm->GetU8(); // container id
            ParseThingDescription(nm);
            return true;
        case 0x71: // Replace Container Item
            nm->GetU8(); // container id
            nm->GetU8(); // slot
            delete ParseThingDescription(nm);

            return true;
        case 0x72: // Remove Container Item
            nm->GetU8(); // container id
            nm->GetU8(); // slot
            return true;
        case 0x78: // Inventory Item
        case 0x79: // Inventory Empty
            nm->GetU8(); // item slot
            if (packetid == 0x78) {
                delete ParseThingDescription(nm);
            }
            return true;
        case 0x7D: // Trade Request
        case 0x7E: // Trade Ack
            nm->GetString(); // Other player
            {
                int itemcount = nm->GetU8();
                for (int i = 0; i < itemcount; i++) {
                    delete ParseThingDescription(nm);
                }
            }
            return true;
        case 0x7F: // Trade Close
            return true;
        case 0x82: // World Light
            nm->GetU8(); // Light Level
            nm->GetU8(); // Light Color
            return true;
        case 0x83: {// Magic Effect
            position_t pos; // position
            GetPosition(nm, &pos);

            nm->GetU8(); // mageffect type
            return true;
        }
        case 0x84: // Animated Text
            position_t pos; // position
            GetPosition(nm, &pos);

            nm->GetU8(); // color
            nm->GetString(); // message
            return true;
        case 0x85: // Distance Shot
            position_t src; // position
            GetPosition(nm, &src);

            position_t dst; // position
            GetPosition(nm, &dst);

            nm->GetU8(); // type of shot
            return true;
        case 0x86: // Creature Square
            nm->GetU32(); // around which creature
            nm->GetU8(); // square color
            return true;
        case 0x8C: {// Creature HP
            Creature *c = GetCreatureByID(nm);
            nm->GetU8(); // health percent
            if (c) printf("Creature %s health adjustment\n", c->GetName().c_str());

            return true;
        }
        case 0x8D: // Creature Light
            GetCreatureByID(nm);//creature id
            nm->GetU8(); //lightradius
            nm->GetU8(); //lightcolor
            return true;
        case 0x8E: {// Set Creature Outfit
        // FIXME abstract this through "getcreaturelook" or somethinglikehtat
            Creature *cr = GetCreatureByID(nm);//creature id

            creaturelook_t crl;
            ParseCreatureLook(nm, &crl);

            cr->SetType(crl.type, crl.extendedlook);

            return true;
        }
        case 0x8F: // Creature Speed
            GetCreatureByID(nm);//creature id
            nm->GetU16(); // speed index
            return true;
        case 0x90: // Creature Skull
            GetCreatureByID(nm);//creature id
            nm->GetU8(); // skull type
            return true;
        case 0x91: // Creature Shield
            GetCreatureByID(nm);//creature id
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

            GetPlayerStats(nm);
            break;
        case 0xA1: // Player Skills
            GetPlayerSkills(nm);
            return true;
        case 0xA2: // Status Icons
        //FIXME abstract status icons
            if (protocolversion < 780) // chech where it turned into 16bit
                nm->GetU8();
            else
                nm->GetU16();
            return true;
        case 0xA3: // Cancel Attack
            return true;
        case 0xAA: {// Creature Speak
            position_t pos;
            if (protocolversion > 760) {// i presume?
                nm->GetU32(); // OT says always 0, perhaps it is NOT!
            }
            std::string creaturename = nm->GetString(); // creature name
            unsigned short creaturelevel=0;
            if (protocolversion >= 780) {
                creaturelevel = nm->GetU16(); // player level
            }

            unsigned char msgtype = nm->GetU8();
            switch (msgtype) {
                case 0x01: // say
                case 0x02: // whisper
                case 0x03: // yell

                case 0x10: // monster 1
                case 0x11: // monster 2
                    this->GetPosition(nm, &pos);
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

            std::string message = nm->GetString(); // message

            console.insert(creaturename + ": " + message, CONYELLOW);
            }
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
        case 0xB4: {// Text Message
            std::string y;
            nm->GetU8(); // msg class
            console.insert( y = nm->GetString() ); // message itself
            if (y == "Sorry, not possible.") SoundPlay("sounds/bleep.wav");
            if (y == "You are not invited.") SoundPlay("sounds/bleep2.wav");
            return true;
        }
        case 0xB5: // Cancel Walk
            nm->GetU8(); // direction
            return true;
        //case 0xBE: // Floor Up
            //return false;
        //case 0xBF: // Floor Down
            // return false;
        case 0xC8: {// Outfit List
            creaturelook_t crl;
            GetCreatureByID(nm); // creature id

            ParseCreatureLook(nm, &crl);

            nm->GetU8(); // first outfit
            nm->GetU8(); // last outfit

            return true;
        }
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
            sprintf(tmp, "Protocol %d: Unfamiliar gameworld packet %02x\nThis protocol is in testing. Report bugs!", protocolversion, packetid);
            this->errormsg = tmp;
            console.insert(tmp, CONRED);

            nm->ShowContents();
            this->Close();
            logonsuccessful = false;
            return false;
        }

    }
}

void Protocol::ParseMapDescription (NetworkMessage *nm, int w, int h, int destx, int desty, int destz) {
    int startz, endz, stepz;
    unsigned int skip=0;
    ASSERT(destz <= maxz);

    if (destz > maxz/2) { // if we're underground
        startz = destz - 2; // then we see two floors above
        endz = min (this->maxz, destz + 2); // and two floors below
        stepz = 1; // and we move from top to bottom

    } else { // if we're on the surface
        startz = maxz/2; // we see from the surface level
        endz = 0; // to the top
        stepz = -1;

    }
    skip = 0;
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Receiving map - floors %d to %d (step %d)\n", startz, endz, stepz);
    for (int z = startz; z != endz + stepz; z+=stepz) {
        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Getting floor %d\n", z);
        ParseFloorDescription(nm, w, h, destx, desty, z, &skip);
    }

}
void Protocol::ParseFloorDescription(NetworkMessage *nm, int w, int h, int destx, int desty, int destz, unsigned int *skip) {

    //static unsigned int skip; // statics are kept between function calls ... neato! cooooool! yipiiiyeah! :)
    // however im not sure what happens if they're declaration-time initialized ... are they reinitialized with every function call?

    for (int x = destx; x < destx + w; x++) {
        for (int y = desty; y < desty + h; y++) {

                //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Working on tile %d %d %d\n", x, y, destz);
                if (!*skip) {
                    if (nm->PeekU16() >= 0xFF00) {
                        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Skip chunk\n");
                        *skip = (nm->GetU16() & 0xFF);
                        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Skipping %d tiles\n", *skip);
                    } else {
                        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Taking in\n");
                        ParseTileDescription(nm, x + player->GetPos()->z - destz, y + player->GetPos()->z - destz, destz);
                        *skip = (nm->GetU16() & 0xFF);
                        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Skipping %d tiles\n", *skip);
                    }
                } else {
                    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Skipped (%d remaining)\n", (*skip)-1);
                    (*skip)--;
                }
            }
        }
}

void Protocol::ParseTileDescription(NetworkMessage *nm, int x, int y, int z) {

//    printf("Tile %d %d %d\n", x, y, z);
    position_t p;
    p.x = x; p.y = y; p.z = z;

    Tile *t = gamemap.GetTile(&p);
    t->empty();
    for (;;) {
        if (nm->PeekU16() >= 0xFF00) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Reached end of tile\n");
            return;
        } else {
            Thing *obj = ParseThingDescription(nm);
            t->insert(obj);
        }
    }

}

Thing* Protocol::ParseThingDescription(NetworkMessage *nm) {
    // MUST ACCEPT NULL as second param
    unsigned int type = GetItemTypeID(nm);

    Thing *thing = ThingCreate(type);
    ASSERTFRIENDLY(thing, "Unknown 'type'. Cant create thing");
    Creature* creature = dynamic_cast<Creature*>(thing);
    Item *item = dynamic_cast<Item*>(thing);;
    // temporary vars that will be stored inside object's description once Object class is defined
    int looktype;
    unsigned long creatureid;
    unsigned short extendedlook=0;
    //printf("Object type %d\n", type);
    switch (type) {
        case 0x0061: // new creature
        case 0x0062: {// known creature
            if (type == 0x0061) { // new creature
                nm->GetU32(); // remove creature with this id
                creatureid = nm->GetU32(); // new creature's id
                creature = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
                thing = creature;
                creature->SetName(nm->GetString()); // name string
            }
            if (type == 0x0062) {
                creatureid = nm->GetU32(); // known creature's id
                creature = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
                thing = creature;
            }
            ASSERT(thing)



            nm->GetU8(); // health percent
            char dir = nm->GetU8();
            printf("Direction: %d\n", dir);


            creaturelook_t creaturelook;
            ParseCreatureLook(nm, &creaturelook);
            printf("Creature look: %d\n", creaturelook.type);



            nm->GetU8(); // lightlevel
            nm->GetU8(); // lightcolor

            unsigned short speedindex = nm->GetU16(); // speedindex


            if (protocolversion >= 750) {
                nm->GetU8(); // skull
                nm->GetU8(); // shield
            }

            // stuff can only be set up AFTER the SetType() call ...
            // gotta check why, but we can live with that for now
            thing->SetType(creaturelook.type, creaturelook.extendedlook);
            thing->SetDirection((direction_t)dir); // direction
            thing->SetSpeed(speedindex);
            break;
        }
        case 0x0063: {// creature that has only direction altered
            creatureid = nm->GetU32(); // creature id
            thing = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
            printf("Creature name: %s\n", ((Creature*)thing)->GetName().c_str());
            char dir = nm->GetU8();
            thing->SetDirection((direction_t)dir); // look direction

            break;
        }
        default: // regular item
            ASSERT(type >= 100 && type <= items_n);
            if (thing)
                thing->SetType(type, 0);

            if (items[type].stackable) {
                unsigned char x = nm->GetU8();
                if (thing) thing->SetCount(x);
            }
            if (items[type].splash || items[type].fluidcontainer) {
                unsigned char x = nm->GetU8();
            }
    }
    return thing;
}


void Protocol::SetProtocolStatus(const char *protostat) {
    if (gamemode == GM_MAINMENU)
        ((GM_MainMenu*)game)->SetLoginStatus(protostat);

}

bool Protocol::CipSoft() {
    printf("==========> CONNECTING to CIPSOFT? %s\n", cipsoft ? "YES" : "NO");
    return cipsoft;
}
bool Protocol::CipSoft(bool cipsoft) {
    this->cipsoft = cipsoft;
    return cipsoft;
}

void Protocol::MoveNorth() {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x65);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        player->GetCreature()->StartMoving();
        player->GetCreature()->SetDirection(NORTH);
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::MoveSouth() {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x67);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        player->GetCreature()->StartMoving();
        player->GetCreature()->SetDirection(SOUTH);
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::MoveWest() {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x68);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        player->GetCreature()->StartMoving();
        player->GetCreature()->SetDirection(WEST);
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::MoveEast() {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x66);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        player->GetCreature()->StartMoving();
        player->GetCreature()->SetDirection(EAST);
    }
    ONThreadUnsafe(threadsafe);
}

void Protocol::Speak(speaktype_t sp, const char *message) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x96);
    switch (sp) {
        case NORMAL:
            nm.AddU8(sp);
            nm.AddString(message);
            break;
        default:
            console.insert("Still unsupported way of speaking :/", CONRED);
            ONThreadUnsafe(threadsafe);
            return;
    }

    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);

    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::SetStance(stanceaggression_t aggression, stancechase_t chase) {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA0);
    nm.AddU8(aggression);
    nm.AddU8(chase);
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

unsigned short Protocol::GetProtocolVersion () {
    return protocolversion;
}


bool ProtocolSetVersion (unsigned short protocolversion) {
    if (protocol) {
        delete protocol;
        protocol = NULL;
    }

    switch (protocolversion) {
        case 750:
            protocol = new Protocol75;
            return true;
        case 760:
            protocol = new Protocol76;
            return true;
#ifdef USEENCRYPTION
        case 770:
            protocol = new Protocol77;
            return true;
        case 790:
            protocol = new Protocol79;
            return true;
        case 792:
            protocol = new Protocol792;
            return true;
#endif
        default:
            return false;
    }

}
