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
#include "database.h"
#include "options.h"
#include "gm_gameworld.h"
#include "creatures.h"
Protocol* protocol;

Protocol::Protocol() {
    s = 0;
    protocolversion = 0;
    connectiontype = NONE;
    motd = "";
    charlistserver = "";
    charlistport = 7171;
    gameworldserver = "";
    gameworldport = 7171;
    errormsg = "";
    username = "";
    password = "";
    charlistselected = 0;
    charlistcount = 0;
    ONInitThreadSafe(threadsafe);

    if (player) {
        delete player;
        player = NULL;

    }

}
Protocol::~Protocol() {
/*    if (player) {
        delete player;
        player = NULL;

    }*/
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

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "ACTIVATED KEYS %u %u %u %u\n", key[0], key[1], key[2], key[3]);
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

//    nm.ShowContents();

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
    printf("Trying to close socket\n");
    #if defined(WIN32) && !defined(_MSC_VER)
	shutdown(s, SD_BOTH);
	#endif
    closesocket(s);
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "CLOSED SOCKET!!!\n");
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
                    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Protocol %d: %s %s %d.%d.%d.%d port %d\n", protocolversion, charlist[i].charactername, charlist[i].worldname, ((unsigned char*)&charlist[i].ipaddress)[0], ((unsigned char*)&charlist[i].ipaddress)[1], ((unsigned char*)&charlist[i].ipaddress)[2], ((unsigned char*)&charlist[i].ipaddress)[3], charlist[i].port);
                }
                premiumdays = nm->GetU16();

                return true;
            }
            default: {
                char tmp[256];
                DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Protocol %d: unfamiliar charlist packet %02x\n", protocolversion, packetid);
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


    player->SetHP(nm->GetU16()); // hp
    player->SetMaxHP(nm->GetU16()); // max hp
    player->SetCap(nm->GetU16()); // cap
    player->SetExp(nm->GetU32()); // exp
    player->SetLevel(nm->GetU16()); // lvl
    player->SetLevelPercent(nm->GetU8()); // level percent
    player->SetMP(nm->GetU16()); // mp
    player->SetMaxMP(nm->GetU16()); // mmp
    player->SetMLevel(nm->GetU8()); // mag lvl
    player->SetMLevelPercent(nm->GetU8()); // maglvl percent
    player->SetSoulPoints(nm->GetU8()); // soul

    /* dunno where this was added but it is there in 792 */
    if (protocolversion>=792) player->SetStamina(nm->GetU16()); // stamina (minutes)

    if (gamemode==GM_GAMEWORLD) ((GM_Gameworld*)game)->UpdateStats();
}

Creature *Protocol::GetCreatureByID(NetworkMessage *nm) {
    Creature * cr = gamemap.GetCreature(nm->GetU32(), NULL);
    ASSERTFRIENDLY(cr, "Protocol::GetCreatureByID() failed");
    return cr;
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
    player->SetSkill(skillid, nm->GetU8(), nm->GetU8()); // level, percent

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
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "PACKET %d\n", packetid);

    switch (packetid) {
        case 0x0A: // Creature ID

            player = new Player(nm->GetU32());
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Own creature ID: %d\n", player->GetCreatureID());
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
        case 0x28: {// You are dead message
            nm->ShowContents();
            console.insert("You've died, mister. Really really badly died.", CONRED);
            return true;
        }
        case 0x32: // Something Else, Bug Report
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Unknown value: %d\n", nm->GetU8());
            printf("Can report bugs: %d\n", nm->GetU8());
            return true;
        case 0x64: // Player Location Setup
            {
                position_t pos;
                GetPosition(nm, &pos);
                player->SetPos(&pos);
                DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Player location: %d %d %d\n", pos.x, pos.y, pos.z);

                // only if we're in main menu then let's do the item loading
                if (gamemode==GM_MAINMENU) {
                    ItemsLoad();
                    CreaturesLoad();
                }

                ParseMapDescription(nm, maxx, maxy, pos.x - (maxx-1)/2, pos.y - (maxy-1)/2, pos.z);
                player->FindMinZ();
                return true;
            }
        case 0x65: // Move Player North
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Move north\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX(), player->GetPosY()-1, player->GetPosZ());

            ParseMapDescription(nm, maxx, 1, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "End move north\n");

            return true;
        case 0x66: // Move Player East
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Move east\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX()+1, player->GetPosY(), player->GetPosZ());

            ParseMapDescription(nm, 1, maxy, player->GetPos()->x + (maxx+1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move east\n");

            return true;
        case 0x67: // Move Player South
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move south\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX(), player->GetPosY()+1, player->GetPosZ());

            ParseMapDescription(nm, maxx, 1, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y + (maxy+1 )/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move south\n");

            return true;
        case 0x68: // Move Player West
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move west\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX()-1, player->GetPosY(), player->GetPosZ());

            ParseMapDescription(nm, 1, maxy, player->GetPos()->x - (maxx-1)/2, player->GetPos()->y - (maxy - 1)/2, player->GetPos()->z);
            player->FindMinZ();
            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move west\n");
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
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Adding item to %d %d %d\n", pos.x, pos.y, pos.z);
            Tile *tile = gamemap.GetTile(&pos);
            Thing *t;
            t = ParseThingDescription(nm);
            tile->Insert(t);
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

            /*t->Remove(stackpos);
            t->Insert(ParseThingDescription(nm));*/

            t->Replace(stackpos, ParseThingDescription(nm));
            return true;
        }
        case 0x6C: {// Remove Item
            position_t pos;
            GetPosition(nm, &pos);
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Removing item from %d %d %d\n", pos.x, pos.y, pos.z);
            unsigned char stackpos = GetStackpos(nm);
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Stackpos %d\n", stackpos);
            Tile *tile = gamemap.GetTile(&pos);
            tile->Remove(stackpos);

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
            thing = tile->GetStackPos(stackpos);
            tile->Remove(stackpos);

            GetPosition(nm, &dst);

            tile = gamemap.GetTile(&dst);
            tile->Insert(thing);


            // if we managed to get here although the thing is null
            // that means that the player has chosen to continue although we asked him
            // with an assertion that failed inside tile->insert()
            // player should also be able to continue moving albeit the thing->ApproveMove()
            // is not called so we call approvemove on player, justincase
            ASSERT(player)
            ASSERT(player->GetCreature())
            if (!thing) return player->GetCreature()->ApproveMove(), true;

            if (dst.y < src.y) thing->SetDirection(NORTH);
            if (dst.y > src.y) thing->SetDirection(SOUTH);
            if (dst.x < src.x) thing->SetDirection(WEST);
            if (dst.x > src.x) thing->SetDirection(EAST);

            if (thing != player->GetCreature() || !thing->IsMoving())
                thing->StartMoving();
            thing->ApproveMove();
            return true;
        }
        case 0x6E: {// Container Open
            std::string title;
            unsigned char containerid;
            unsigned short icon;
            unsigned char capacity;
            Container *c;

            containerid = nm->GetU8(); // container id
            icon = nm->GetU16(); // container icon
            title = nm->GetString(); // container title
            capacity = nm->GetU8(); // capacity
            nm->GetU8(); // hasparent



            int x = 10; int y = 10;

            if (gamemode==GM_GAMEWORLD) {
                GM_Gameworld *gw  = dynamic_cast<GM_Gameworld*>(game);
                x = gw->GetContainersX();
                y = gw->GetContainersY() + containerid * 10;
            }


            if (Container *tmpc = player->GetContainer(containerid)) {
                glictPos p;
                tmpc->GetWindow()->GetPos(&p);
                x = p.x; y = p.y;
                if (gamemode==GM_GAMEWORLD)
                    ((GM_Gameworld*)game)->RemoveContainer(player->GetContainer(containerid));
                player->RemoveContainer(containerid);
            }

            player->SetContainer(containerid, c = new Container(title, containerid, icon, capacity));
            if (gamemode==GM_GAMEWORLD) // FIXME we need to add delayed addcontainer of some sort
                ((GM_Gameworld*)game)->AddContainer(c, x, y); // perhaps gameworld should during startup check if there's a container that it has not added to desktop yet



            {
                int itemcount = nm->GetU8(); // item count
                for (int i = 0 ; i < itemcount ; i++) {
                    //delete ParseThingDescription(nm);
                    c->Insert(ParseThingDescription(nm));
                }
            }


            {
                char tmp[256];
                sprintf(tmp, "Opened container %d - %s (%d,%d)", containerid, title.c_str(), x, y);
                console.insert(tmp);
            }
            return true;
        }
        case 0x6F: {// Container Close

            unsigned char containerid;
            containerid = nm->GetU8();


            if (gamemode==GM_GAMEWORLD)
                ((GM_Gameworld*)game)->RemoveContainer(player->GetContainer(containerid));
            player->RemoveContainer(containerid);

            {
                char tmp[256];
                sprintf(tmp, "Closed container %d ", containerid);
                console.insert(tmp);
            }


            return true;
        }
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
        case 0x79:{// Inventory Empty
            unsigned int slot = nm->GetU8(); // item slot

            if (packetid == 0x78) {
                player->SetInventorySlot(slot, ParseThingDescription(nm));
            } else {
                player->SetInventorySlot(slot, NULL);
            }
            return true;
        }
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
            //if (c)
                c->SetHP(nm->GetU8()); // health percent
            //else
            //    console.insert("There was a problem adjusting a creature's health.\n", CONRED);

            //if (c) printf("Creature %s health adjustment\n", c->GetName().c_str());

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

            cr->SetType(crl.type, &crl);

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
            return true;
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
            unsigned char msgclass;
            consolecolors_t color;
            msgclass = nm->GetU8(); // msg class


            switch (msgclass) {
                case 0x12: // msgredinfo
                    color = CONRED;
                    break;
                case 0x13: // msgadvance
                    color = CONGREEN;
                    break;
                case 0x14: // msgevent
                case 0x15: // msgevent (commented out in otserv)
                    color = CONWHITE;
                    break;
                case 0x16: // msginfo
                    color = CONGREEN;
                    nm->ShowContents();
                    break;
                case 0x17: // msgsmallinfo
                    color = CONWHITE;
                    break;
                case 0x18: // msgbluetext
                    color = CONBLUE;
                    break;
                case 0x19: // msgredtext
                    color = CONRED;
                    break;
                default:
                    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Unknown textmsg class: 0x%02x\n", msgclass);
                    color = CONWHITE;
            }
            console.insert( y = nm->GetString(), color ); // message itself
            if (y == "Sorry, not possible.") SoundPlay("sounds/bleep.wav");
            if (y == "You are not invited.") SoundPlay("sounds/bleep2.wav");
            if (y == "You may not logout during or immediately after a fight!") SoundPlay("sounds/bleep2.wav");
            return true;
        }
        case 0xB5: // Cancel Walk
            player->GetCreature()->SetDirection((direction_t)nm->GetU8()); // direction
            player->GetCreature()->CancelMoving();
            SoundPlay("sounds/bleep.wav");
            return true;
        case 0xBE: {// Floor Up
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move up\n");
            gamemap.Lock();
            player->SetPos(player->GetPosX()+1, player->GetPosY()+1, player->GetPosZ()-1);


            //ParseMapDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2, player->GetPosZ());

            unsigned int skip=0;
            if (player->GetPosZ()==7)
                for (int i = 5 ; i >=0; i--)
                    ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2 , i, &skip);
             else if (player->GetPosZ()>7)
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2 , player->GetPosZ()-2, &skip);

            player->FindMinZ();



            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move up\n");

            return true;
        }
        case 0xBF: {// Floor Down
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move down\n");
            gamemap.Lock();


            player->SetPos(player->GetPosX()-1, player->GetPosY()-1, player->GetPosZ()+1);


            //ParseMapDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2, player->GetPosZ());
            unsigned int skip=0;
            if (player->GetPosZ()==8) {
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosY() - (maxy-1)/2 , player->GetPosZ(), &skip);
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosY() - (maxy-1)/2 , player->GetPosZ()+1, &skip);
            }
            if (player->GetPosZ() >= 8)
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosZ() - (maxy-1)/2 , player->GetPosZ()+2, &skip);


            player->FindMinZ();


            //player->SetPos(player->GetPosX()+1, player->GetPosY()+1, player->GetPosZ());


            gamemap.Unlock();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "End move down\n");

            return true;
        }
        case 0xC8: {// Outfit List
            creaturelook_t crl;

	    if (this->protocolversion < 790) { // FIXME this is probably incorrect version of protocol where this first appeared. check!

	            Creature *c = GetCreatureByID(nm); // creature id

	            ParseCreatureLook(nm, &crl);

	            nm->GetU8(); // first outfit
	            nm->GetU8(); // last outfit

	    } else {
		    unsigned char countoutfits = nm->GetU8();
		    for (int i = 0; i < countoutfits; i++) {
		        nm->GetU16(); // look type
                nm->GetString(); // outift name
                nm->GetU8(); // addons
		    }
	    }
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
	}

    {
        char tmp[256];
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Protocol %d: unfamiliar gameworld packet %02x\n", protocolversion, packetid);
        sprintf(tmp, "Protocol %d: Unfamiliar gameworld packet %02x\nThis protocol is in testing. Report bugs!", protocolversion, packetid);
        this->errormsg = tmp;
        console.insert(tmp, CONRED);

        nm->ShowContents();
        this->Close();
        logonsuccessful = false;
        return false;
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
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Receiving map - floors %d to %d (step %d)\n", startz, endz, stepz);
    for (int z = startz; z != endz + stepz; z+=stepz) {
        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Getting floor %d\n", z);
        ParseFloorDescription(nm, w, h, destx, desty, z, &skip);
    }

}
void Protocol::ParseFloorDescription(NetworkMessage *nm, int w, int h, int destx, int desty, int destz, unsigned int *skip) {

    //static unsigned int skip; // statics are kept between function calls ... neato! cooooool! yipiiiyeah! :)
    // however im not sure what happens if they're declaration-time initialized ... are they reinitialized with every function call?

    if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");


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
    if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
}

void Protocol::ParseTileDescription(NetworkMessage *nm, int x, int y, int z) {

    //printf("Tile %d %d %d\n", x, y, z);
    position_t p;
    p.x = x; p.y = y; p.z = z;

    Tile *t = gamemap.GetTile(&p);
    t->Empty();
    for (;;) {
        if (nm->PeekU16() >= 0xFF00) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Reached end of tile\n");
            static bool locked;
            if (locked = gamemap.locked) gamemap.Unlock();
            t->StoreToDatabase();
            if (locked) gamemap.Lock();
            return;
        } else {
            Thing *obj = ParseThingDescription(nm);
            t->Insert(obj);
        }
    }
}

Thing* Protocol::ParseThingDescription(NetworkMessage *nm) {
    // MUST ACCEPT NULL as second param
    unsigned int type = GetItemTypeID(nm);


    //printf("Object type %d\n", type);


    Thing *thing = ThingCreate(type);
    ASSERTFRIENDLY(thing, "Unknown 'type'. Cant create thing");
    Creature* creature = dynamic_cast<Creature*>(thing);
    Item *item = dynamic_cast<Item*>(thing);
    // temporary vars that will be stored inside object's description once Object class is defined
    int looktype;
    unsigned long creatureid;
    unsigned short extendedlook=0;

    switch (type) {
        case 0x0061: // new creature
        case 0x0062: {// known creature
            if (type == 0x0061) { // new creature
                nm->GetU32(); // remove creature with this id
                creatureid = nm->GetU32(); // new creature's id
                creature = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
                thing = creature;
                creature->SetName(nm->GetString()); // name string
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Got creature %s\n", creature->GetName().c_str());
            }
            if (type == 0x0062) {
                creatureid = nm->GetU32(); // known creature's id
                creature = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
                thing = creature;
            }
            ASSERT(thing)



            unsigned char hp = nm->GetU8(); // health percent
            unsigned char dir = nm->GetU8();
            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Direction: %d\n", dir);


            creaturelook_t creaturelook;
            ParseCreatureLook(nm, &creaturelook);
            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Creature look: %d\n", creaturelook.type);



            nm->GetU8(); // lightlevel
            nm->GetU8(); // lightcolor

            unsigned short speedindex = nm->GetU16(); // speedindex


            if (protocolversion >= 750) {
                nm->GetU8(); // skull
                nm->GetU8(); // shield
            }

            // stuff can only be set up AFTER the SetType() call ...
            // gotta check why, but we can live with that for now
            thing->SetType(creaturelook.type, &creaturelook);
            thing->SetDirection((direction_t)dir); // direction
            thing->SetSpeed(speedindex);
            ((Creature*)thing)->SetHP(hp);
            break;
        }
        case 0x0063: {// creature that has only direction altered
            creatureid = nm->GetU32(); // creature id
            thing = gamemap.GetCreature(creatureid, dynamic_cast<Creature*>(thing));
            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Creature name: %s\n", ((Creature*)thing)->GetName().c_str());
            char dir = nm->GetU8();
            thing->SetDirection((direction_t)dir); // look direction

            break;
        }
        default: {// regular item
            char tmp[256];
            sprintf(tmp, "Invalid item received from server: %d. Loaded items: %d. Last successful item: %d", type, items_n, lastsuccessfulitem);
            if (!(type >= 100 && type <= items_n)) {
                DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, tmp);
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_ERROR, "For orientation purposes, the remaining unprocessed netmessage is below\n");
                nm->ShowContents();
            }
            ASSERTFRIENDLY(type >= 100 && type <= items_n, tmp);
            if (thing)
                thing->SetType(type, 0);

            if (items[type]->stackable || items[type]->rune) {
                unsigned char x = nm->GetU8();
                if (thing) thing->SetCount(x);
            }
            if (items[type]->splash ||  items[type]->fluidcontainer) {
                unsigned char x = nm->GetU8();
                if (thing) thing->SetSubType(x);
            }
            lastsuccessfulitem = type;
        }
    }

    return thing;
}


void Protocol::SetProtocolStatus(const char *protostat) {
    if (gamemode == GM_MAINMENU)
        ((GM_MainMenu*)game)->SetLoginStatus(protostat);

}

bool Protocol::CipSoft() {
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "==========> CONNECTING to CIPSOFT? %s\n", cipsoft ? "YES" : "NO");
    return cipsoft;
}
bool Protocol::CipSoft(bool cipsoft) {
    this->cipsoft = cipsoft;
    return cipsoft;
}

void Protocol::Move(direction_t dir) {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Moving\n");
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x65 + dir);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        if (dir != STOP) {
            player->GetCreature()->StartMoving();
            player->GetCreature()->SetDirection(dir);
        }
        else
            player->GetCreature()->CancelMoving();
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::Turn(direction_t dir) {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Turning\n");
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x6F + dir);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        player->GetCreature()->SetDirection(dir);
    }
    ONThreadUnsafe(threadsafe);
}

void Protocol::Speak(speaktype_t sp, const char *message) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    if (!strcmp(message, "/logout")) {
        nm.AddU8(0x14);
    } else {
        nm.AddU8(0x96);
        switch (sp) {
            case NORMAL:
                nm.AddU8(sp);
                nm.AddString(message);

                break;
            default:
                console.insert("A still-unsupported way of speaking :/", CONRED);
                ONThreadUnsafe(threadsafe);
                return;
        }
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
void Protocol::LookAt(position_t *pos) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x8C);
    this->AddPosition(&nm, pos);
    if (pos->x!=0xFFFF) { // tile
        Tile *t = gamemap.GetTile(pos);
        Thing *th;
        if (!t) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        unsigned char stackpos = t->GetTopLookAt();
        th = t->GetStackPos(stackpos);
        if (!th) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        nm.AddU16(th->GetType());
        nm.AddU8(stackpos);
    } else { // inventory or container
        if (!(pos->y & 0x40)) { // inventory
            if (!player->inventory[pos->y-1]) {
                ONThreadUnsafe(threadsafe);
                return;
            };
            nm.AddU16(player->inventory[pos->y-1]->GetType());
            nm.AddU8(0);
        } else {
            Container *c = player->GetContainer(pos->y & 0x0F);
            if (!c) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            Thing* t = c->GetItem(pos->z);
            if (!t) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            nm.AddU16(t->GetType());
            nm.AddU8(0);
        }
    }
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::Attack(unsigned long creatureid) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);



    nm.AddU8(0xA1);
    nm.AddU32(gamemap.SetAttackedCreature(creatureid)); // FIXME abstract this with AddCreatureID() or sth

    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::Use(position_t *pos, unsigned char stackpos) {
    // this is "single-click" use, meaning only one item is affected

    NetworkMessage nm;
    ONThreadSafe(threadsafe);


    nm.AddU8(0x82);
    AddPosition(&nm, pos);
    if (pos->x != 0xFFFF) {
        Tile *t = gamemap.GetTile(pos);

        if (!t) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        Thing *th = t->GetStackPos(stackpos);

        if (!th) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        nm.AddU16(th->GetType() );

        nm.AddU8(stackpos);  // FIXME abstract with AddStackPos
        nm.AddU8(player->GetFreeContainer()); // specifies which container to replace
    } else {
        if (!(pos->y & 0x40)) { // inventory
            if (!player->inventory[pos->y - 1]) {
                ONThreadUnsafe(threadsafe);
                return;
            }

            nm.AddU16(player->inventory[pos->y - 1]->GetType() );
            nm.AddU8(stackpos);  // FIXME abstract with AddStackPos
            nm.AddU8(player->GetFreeContainer()); // specifies which container to replace
        } else {
            Container *c = player->GetContainer(pos->y & 0x0F);
            if (!c) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            Thing* t = c->GetItem(pos->z);
            if (!t) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            nm.AddU16(t->GetType());
            nm.AddU8(stackpos);
            nm.AddU8(pos->y & 0x0F); // replace which container ...

        }
    }
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

void Protocol::Use(position_t *pos1, unsigned char stackpos1, position_t *pos2, unsigned char stackpos2) {
    // this is "extended-click" use, meaning multiple items are affected

    NetworkMessage nm;
    Thing *th;
    Tile *t;
    ONThreadSafe(threadsafe);

    nm.AddU8(0x83);

    // first item
    AddPosition(&nm, pos1);
    if (pos1->x != 0xFFFF) {
        Tile *t = gamemap.GetTile(pos1);

        if (!t) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        Thing *th = t->GetStackPos(stackpos1);

        if (!th) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        nm.AddU16(th->GetType() );

        nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos

    } else {
        if (!(pos1->y & 0x40)) { // inventory
            if (!player->inventory[pos1->y - 1]) {
                ONThreadUnsafe(threadsafe);
                return;
            }

            nm.AddU16(player->inventory[pos1->y - 1]->GetType() );
            nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos
        } else {
            Container *c = player->GetContainer(pos1->y & 0x0F);
            if (!c) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            Thing* t = c->GetItem(pos1->z);
            if (!t) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            nm.AddU16(t->GetType());
            nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos
        }
    }

    // second item
    AddPosition(&nm, pos2);
    if (pos2->x != 0xFFFF) {
        Tile *t = gamemap.GetTile(pos2);

        if (!t) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        Thing *th = t->GetStackPos(stackpos2);

        if (!th) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        nm.AddU16(th->GetType() );

        nm.AddU8(stackpos2);  // FIXME abstract with AddStackPos



    } else {
        if (!(pos2->y & 0x40)) { // inventory
            if (!player->inventory[pos2->y - 1]) {
                ONThreadUnsafe(threadsafe);
                return;
            }

            nm.AddU16(player->inventory[pos2->y - 1]->GetType() );
            nm.AddU8(stackpos2);  // FIXME abstract with AddStackPos
        } else {
            Container *c = player->GetContainer(pos2->y & 0x0F);
            if (!c) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            Thing* t = c->GetItem(pos2->z);
            if (!t) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            nm.AddU16(t->GetType());
            nm.AddU8(stackpos2);  // FIXME abstract with AddStackPos
        }
    }

    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::Move(position_t *pos1, unsigned char stackpos1, position_t *pos2, unsigned char stackpos2, unsigned char amount) {

    NetworkMessage nm;
    Thing *th;
    Tile *t;
    ONThreadSafe(threadsafe);

    nm.AddU8(0x78);

    {
        char tmp [256];
        sprintf(tmp, "Moving from %d %d %d to %d %d %d\n", pos1->x, pos1->y, pos1->z, pos2->x, pos2->y, pos2->z);
        console.insert(tmp);
    }

    // first item
    AddPosition(&nm, pos1);
    if (pos1->x != 0xFFFF) {
        Tile *t = gamemap.GetTile(pos1);

        if (!t) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        Thing *th = t->GetStackPos(stackpos1);

        if (!th) {
            ONThreadUnsafe(threadsafe);
            return;
        }

        nm.AddU16(th->GetType() );

        nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos

    } else {
        if (!(pos1->y & 0x40)) { // inventory
            if (!player->inventory[pos1->y - 1]) {
                ONThreadUnsafe(threadsafe);
                return;
            }

            nm.AddU16(player->inventory[pos1->y - 1]->GetType() );
            nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos
        } else {
            Container *c = player->GetContainer(pos1->y & 0x0F);
            if (!c) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            Thing* t = c->GetItem(pos1->z);
            if (!t) {
                ONThreadUnsafe(threadsafe);
                return;
            }
            nm.AddU16(t->GetType());
            nm.AddU8(stackpos1);  // FIXME abstract with AddStackPos
        }
    }

    // second item
    AddPosition(&nm, pos2);

    nm.AddU8(amount); // Amount of items to move ... temporarily set to 1


    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}


void Protocol::CloseContainer(unsigned char cid) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x87);
    nm.AddU8(cid);
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}


void Protocol::AddPosition(NetworkMessage *nm, position_t *pos) {
    nm->AddU16(pos->x);
    nm->AddU16(pos->y);
    nm->AddU8(pos->z);
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
