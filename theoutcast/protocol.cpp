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
#include "gm_mainmenu.h"
#include "gm_gameworld.h"
#include "creatures.h"
#include "effects.h"
#include "distances.h"
#include "charlist.h"
#include "gwlogon.h"


#include "protocol75.h"
#include "protocol76.h"
#ifdef USEENCRYPTION
#include "protocol77.h"
#include "protocol79.h"
  #include "protocol792.h"
  #include "protocol80.h"
#endif

#ifdef INCLUDE_SP
  #include "protocolsp.h"
#endif
#ifdef INCLUDE_ME
  #include "protocolme0.h"
#endif

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

    lastsuccessfulitem = -1;

    if (player) {
        delete player;
        player = NULL;
    }
    newgamemode = GM_GAMEWORLD;

}
Protocol::~Protocol() {
/*    if (player) {
        delete player;
        player = NULL;

    }*/
    ONDeinitThreadSafe(threadsafe);
}
void Protocol::CharlistConnect() {
    thrCharList = ONNewThread(Thread_CharList, game); //CreateThread(NULL, 0, Thread_CharList, ((GM_MainMenu*)game), 0, &((GM_MainMenu*)game)->thrCharListId);
}
void Protocol::GameworldConnect() {
    thrGWLogon = ONNewThread(Thread_GWLogon, game);
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
int lastpreviouspacket = 0; // FIXME remove me
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
    gamemap.Lock();
    while ((signed int)(nm.GetSize())>0 ) {
    	int currentpacket = nm.PeekU8();
    	if (ParsePacket(&nm)) lastpreviouspacket = currentpacket; else {
    		ASSERT(false);
    		break;
    	}

    }
    gamemap.Unlock();

    if ((signed int)(nm.GetSize())!=0) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_WARNING, "++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++ %d remain\n", nm.GetSize());
        nm.ShowContents();
    }
    ONThreadUnsafe(threadsafe);
    return true;
}

void Protocol::Close() {
    //ONThreadSafe(threadsafe);
    printf("Trying to close socket\n");
    #if defined(WIN32) && !defined(_MSC_VER)
	shutdown(s, SD_BOTH);
	#endif
    closesocket(s);
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "CLOSED SOCKET!!!\n");
//    system("pause");
    active = false;
    s = 0;
    //ONThreadUnsafe(threadsafe);
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
    printf("Charlist packet %02x\n", packetid);
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
                charlist = (character_t**)malloc(charlistcount * sizeof(character_t*));
                for (int i = 0 ; i < charlistcount ; i++) {
                    charlist[i] = new character_t;
                    nm->GetString(charlist[i]->charactername, 127);
                    nm->GetString(charlist[i]->worldname, 127);
                    charlist[i]->ipaddress = nm->GetU32();
                    charlist[i]->port = nm->GetU16();
                    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Protocol %d: %s %s %d.%d.%d.%d port %d\n", protocolversion, charlist[i]->charactername, charlist[i]->worldname, ((unsigned char*)&charlist[i]->ipaddress)[0], ((unsigned char*)&charlist[i]->ipaddress)[1], ((unsigned char*)&charlist[i]->ipaddress)[2], ((unsigned char*)&charlist[i]->ipaddress)[3], charlist[i]->port);
                }
                premiumdays = nm->GetU16();

                return true;
            }
            default: {
                char tmp[512];
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

	ASSERTFRIENDLY(player, "Player is null, yet he shouldn't be null.\n");
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
    //ASSERTFRIENDLY(cr, "Protocol::GetCreatureByID() failed");
    if (!cr) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Protocol::GetCreatureByID() failed");
    }
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
	#if 0
	if (effects_n && player) {

		position_t pos;
		player->GetPos(&pos);
		pos.x += 3; pos.y -= 3;

		char tmp[50];
		sprintf(tmp, "Packet, showing on %d %d %d", pos.x, pos.y, pos.z);
		console.insert(tmp);

		Tile *t = gamemap.GetTile(&pos);
		gamemap.Lock();

		Effect* e = new Effect(t);
		e->SetType(2, NULL);
		t->Insert(e, false);
		gamemap.Unlock();
	}
	printf("Added 'ping' effect\n");
	#endif
    switch (packetid) {
        case 0x0A: // Creature ID

            player = new Player(nm->GetU32());
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Own creature ID: %d\n", player->GetCreatureID());
            return true;
        case 0x0B: // GM Actions
            nm->Trim(32); // unknown
            return true;
        case 0x0C: {// The Outcast Character Manager Packets
            unsigned char tocm = nm->GetU8();
            switch (tocm) {
                case 0x01: // enter
                    newgamemode = GM_CHARMGR;
                    break;
                default:
                    Close();
            }
            return true;
        }
        case 0x14: // Generic login error message
            errormsg = nm->GetString();
            SoundPlay("sounds/error.wav");
            logonsuccessful = false;
            return false;
        case 0x15: {// Messagebox Popup ("for your information")
            std::string msg = nm->GetString(); // message
            if (gamemode == GM_GAMEWORLD) ((GM_Gameworld*)game)->MsgBox(msg.c_str(), "For your information");
            return true;
        }
        case 0x16: // Too Many Players login error message
            errormsg = nm->GetString();
            {
                char tmp[255];
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
            if (gamemode == GM_GAMEWORLD) ((GM_Gameworld*)game)->MsgBox("You have went through the unfortunate process of dying.\nHopefully, you will be able to restore your experience\npoints and resume playing. If you have a high-level\nfriend, don't forget to contact him or her so that\nyou get avenged.\n\nAnd train so that you're not a n00b.", "You're dead");
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
                    EffectsLoad();
                    DistancesLoad();
                }

                if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
                ParseMapDescription(nm, maxx, maxy, pos.x - (maxx-1)/2, pos.y - (maxy-1)/2, pos.z);
                if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
                player->FindMinZ();

                newgamemode = GM_GAMEWORLD;
                return true;
            }
        case 0x65: // Move Player North
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Move north\n");


            player->SetPos(player->GetPosX(), player->GetPosY()-1, player->GetPosZ());

            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
            ParseMapDescription(nm, maxx, 1, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy - 1)/2, player->GetPosZ());
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
            player->FindMinZ();


            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "End move north\n");

            return true;
        case 0x66: // Move Player East
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Move east\n");


            player->SetPos(player->GetPosX()+1, player->GetPosY(), player->GetPosZ());

            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
            ParseMapDescription(nm, 1, maxy, player->GetPosX() + (maxx+1)/2, player->GetPosY() - (maxy - 1)/2, player->GetPosZ());
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
            player->FindMinZ();

            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move east\n");

            return true;
        case 0x67: // Move Player South
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move south\n");

            player->SetPos(player->GetPosX(), player->GetPosY()+1, player->GetPosZ());

            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
            ParseMapDescription(nm, maxx, 1, player->GetPosX() - (maxx-1)/2, player->GetPosY() + (maxy+1 )/2, player->GetPosZ());
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
            player->FindMinZ();

            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move south\n");

            return true;
        case 0x68: // Move Player West
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move west\n");


            player->SetPos(player->GetPosX()-1, player->GetPosY(), player->GetPosZ());

            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
            ParseMapDescription(nm, 1, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy - 1)/2, player->GetPosZ());
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");
            player->FindMinZ();


            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move west\n");
            return true;
        case 0x69: {// Tile Update
            position_t pos;

            GetPosition(nm, &pos);
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "begin transaction;");
            if (nm->PeekU16()>0xFF00) {
            	// skip this tile == clear this tile
            	Tile *t = gamemap.GetTile(&pos);
            	t->Empty();
            	nm->GetU16();
            	DEBUGPRINT(DEBUGPRINT_WARNING, DEBUGPRINT_LEVEL_DEBUGGING, "CLEARTILE.\n");
            } else {
				ParseTileDescription(nm, pos.x,pos.y,pos.z);
				nm->GetU16();
            }
            if (options.maptrack) dbExecPrintf(dbUser, NULL, NULL, NULL, "end transaction;");

            return true;
        }
        case 0x6A: {// Add Item
            position_t pos;


            GetPosition(nm, &pos);
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Adding item to %d %d %d\n", pos.x, pos.y, pos.z);
            Tile *tile = gamemap.GetTile(&pos);
            Thing *t;
            t = ParseThingDescription(nm);
            tile->Insert(t, false);


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
            //if (tile->GetStackPos(stackpos) == player->GetCreature()) player->Die();
            tile->Remove(stackpos);



            return true;
        }
        case 0x6D: {// Move Item
            unsigned char stackpos;
            Tile *tile;
            Thing *thing;
            position_t src;
            position_t dst;


			DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Locked map\n");
            GetPosition(nm, &src);
            stackpos = GetStackpos(nm);

            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Received stackpos\n");


            printf("Getting tile %d %d %d\n", src.x, src.y, src.z);
            tile = gamemap.GetTile(&src);
            thing = tile->GetStackPos(stackpos);

            tile->Remove(stackpos, true);

            GetPosition(nm, &dst);

            tile = gamemap.GetTile(&dst);
            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Got tile\n");
            tile->Insert(thing, false);



            // if we managed to get here although the thing is null
            // that means that the player has chosen to continue although we asked him
            // with an assertion that failed inside tile->insert()
            // player should also be able to continue moving albeit the thing->ApproveMove()
            // is not called so we call approvemove on player, justincase
            ASSERT(player)

            if (player->GetCreature()) {
				if (!thing)
					return player->GetCreature()->ApproveMove(), true;
            }
			else {
				console.insert("Couldn't approve player's move, because player has no creature.", CONRED );
				return true;
			}

            if (dst.y < src.y) thing->SetDirection(NORTH);
            if (dst.y > src.y) thing->SetDirection(SOUTH);
            if (dst.x < src.x) thing->SetDirection(WEST);
            if (dst.x > src.x) thing->SetDirection(EAST);

            if (thing != player->GetCreature() || !thing->IsMoving())
                thing->StartMoving();
            thing->ApproveMove();

            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Updated all others\n");

            DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"Unlocked map\n");


            return true;
        }
        case 0x6E: {// Container Open
            // FIXME (Khaos#1#) Sometimes, when this packet happens the thing hangs -- probably a thread issue
            std::string title;
            unsigned char containerid;
            unsigned short icon;
            unsigned char capacity;
            Container *c;

            printf("Getting container stuff\n");

            containerid = nm->GetU8(); // container id
            icon = nm->GetU16(); // container icon
            title = nm->GetString(); // container title
            capacity = nm->GetU8(); // capacity
            nm->GetU8(); // hasparent

            printf("Getting gameworld stuff...\n");

            int x = 10; int y = 10;

            if (gamemode==GM_GAMEWORLD) {
                GM_Gameworld *gw  = dynamic_cast<GM_Gameworld*>(game);
                printf("Getting x...\n");
                x = gw->GetContainersX();
                printf("Getting y...\n");
                y = gw->GetContainersY() + containerid * 10;
            }

            printf("Getting container\n");
            if (Container *tmpc = player->GetContainer(containerid)) {
                glictPos p;
                printf("Getting window pos\n");
                tmpc->GetWindow()->GetPos(&p);
                x = p.x; y = p.y;
                printf("Getting container\n");
                if (gamemode==GM_GAMEWORLD)
                    ((GM_Gameworld*)game)->RemoveContainer(player->GetContainer(containerid));
                printf("Removing container\n");
                player->RemoveContainer(containerid);
            }

            printf("Setting container\n");
            player->SetContainer(containerid, c = new Container(title, containerid, icon, capacity));
            printf("Adding to game\n");
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
        case 0x70: {// Add Container Item
            Container *c = player->GetContainer(nm->GetU8()); // container id
            if (c) c->Insert(ParseThingDescription(nm), true);
            return true;
        }
        case 0x71: {// Replace Container Item
            Container *c = player->GetContainer(nm->GetU8()); // container id
            if (c) {
                unsigned char slot = nm->GetU8();
                c->Replace(slot, ParseThingDescription(nm)); // slot
            }

            return true;
        }
        case 0x72: {// Remove Container Item
            Container *c = player->GetContainer(nm->GetU8()); // container id
            if (c) c->Remove( nm->GetU8() ); // slot
            return true;
        }
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
        case 0x7E: {// Trade Ack
            nm->GetString(); // Other player
            {
                int itemcount = nm->GetU8();
                for (int i = 0; i < itemcount; i++) {
                    delete ParseThingDescription(nm);
                }
            }
            NetworkMessage nm2;
            nm2.AddU8(0x80);
            nm2.Dump(s);
            console.insert("Trade not supported and autorefused", CONRED);
            return true;
        }
        case 0x7F: // Trade Close
            return true;
        case 0x82: // World Light
            nm->GetU8(); // Light Level
            nm->GetU8(); // Light Color
            return true;
        case 0x83: {// Magic Effect
            position_t pos; // position
            Tile *t;
            Effect *e;
            unsigned char type;
            #ifndef USEEFFECTS
            console.insert("Effects disabled\n", CONRED);
            GetPosition(nm, &pos);
            nm->GetU8();

            return true;
            #endif

			printf("Getting position\n");
            GetPosition(nm, &pos);
            printf("Getting tile\n");
            t = gamemap.GetTile(&pos);
            type = nm->GetU8(); // mageffect type
            printf("Effect type: %d\n", type);



            printf("Creating effect\n" );
            e = new Effect(t);
            printf("Setting type\n");
            e->SetType(type, NULL);

            printf("Inserting effect\n");
            t->Insert(e, false);


            printf("Set it up, unlocking and finishing processing of mageff\n");
			printf("Unlocked\n");

            return true;
        }
        case 0x84: {// Animated Text
        	#ifndef USEEFFECTS
        	position_t pos; // position
            GetPosition(nm, &pos);

        	nm->GetU8(); // color

        	std::string msg = "Animated text: ";
        	msg += nm->GetString(); // message
        	console.insert(msg, CONBLUE);
        	return true;
        	#else
            position_t pos; // position
            GetPosition(nm, &pos);
            Tile *t = gamemap.GetTile(&pos);

            unsigned char color = nm->GetU8(); // color
            std::string msg = nm->GetString(); // message
            Effect *e = new Effect(t);
            e->SetText(msg, color, true);
            t->Insert(e, true);
            return true;
            #endif
        }
        case 0x85: {// Distance Shot


            #ifndef USEEFFECTS
            console.insert("Effects disabled (distance shot)\n", CONRED);
			position_t src; // position
            GetPosition(nm, &src);

            position_t dst; // position
            GetPosition(nm, &dst);

            nm->GetU8(); // type of shot

            return true;

            #else
            Tile *t;
            Effect *e;
            unsigned char type;



            position_t src; // position
            GetPosition(nm, &src);

            position_t dst; // position
            GetPosition(nm, &dst);

			DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT1\n", type);

            t = gamemap.GetTile(&src);
            type = nm->GetU8(); // mageffect type

			DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT2\n", type);

            e = new Effect(t);
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT3\n", type);
            e->SetType(type, (void*)1); // anything but NULL for second argument causes it to be a distance shot
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT4\n", type);
            e->SetDistanceDeltaTarget(dst.x - src.x, dst.y - src.y);
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT5\n", type);

            t->Insert(e, false);
			DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, ">=>=>=>=>=> Distance shot effect %d PT .. doh\n", type);


            return true;
            #endif
        }
        case 0x86: // Creature Square
            nm->GetU32(); // around which creature
            nm->GetU8(); // square color
            return true;
        case 0x8C: {// Creature HP
            Creature *c = GetCreatureByID(nm);
            if (c)
                c->SetHP(nm->GetU8()); // health percent
            else {
                //console.insert("There was a problem adjusting a creature's health.\n", CONRED);
                nm->GetU8();
            }
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
        case 0x90: {// Creature Skull
            Creature *cr = GetCreatureByID(nm);
            cr->SetSkull((skull_t)nm->GetU8());//creature id, skull type
            return true;
        }
        case 0x91: // Creature Shield
            GetCreatureByID(nm);//creature id
            nm->GetU8(); // shield type
            return true;
        case 0x96: {// Text Window
            nm->GetU32(); // window id
            nm->GetU16(); // itemid -- for icon?
            nm->GetU16(); // max length
            nm->GetString();
            if (protocolversion >= 792) { //FIXME (Khaos#2#) Check where exactly this was added
                nm->GetString(); // unknown
                nm->GetString();
            }
            return true;
        }
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
        case 0xA2: {// Status Icons

            unsigned short icons;
            unsigned int internalicons=0;
            if (protocolversion < 780) {// chech where it turned into 16bit, probably where the DROWNING appeared
                icons = nm->GetU8();
            } else {
                icons = nm->GetU16();
            }

            internalicons = icons; // if icon ids change somewhere, we need to define a function in Protocol7X class that will return internalicons...

            player->SetIcons(internalicons);

            return true;
        }
        case 0xA3: // Cancel Attack
            return true;
        case 0xAA: {// Creature Speak
            position_t pos(0,0,0);
            consolecolors_t concol;
            if (protocolversion > 760) {// i presume?
                nm->GetU32(); // OT says always 0, perhaps it is NOT!
            }
            std::string creaturename = nm->GetString(); // creature name
            unsigned short creaturelevel=0;
            if (protocolversion >= 780) {
                creaturelevel = nm->GetU16(); // player level
            }

            unsigned char msgtype = nm->GetU8();
            printf("MSGTYPE: %02x\n", msgtype);
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

            switch (msgtype) {
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x05:
                default:
                    concol = CONYELLOW;
                    break;
                case 0x10:
                case 0x11:
                case 0x0C:
                    concol = CONORANGE;
                    break;
                case 0x09:
                case 0x0A:
                case 0x0E:
                    concol = CONRED;
                    break;
                case 0x04:
                    concol = CONLTBLUE;
            }

            std::string message = nm->GetString(); // message

            console.insert(creaturename + ": " + message, concol);

			#ifdef USEEFFECTS
            if (pos.x && pos.y && pos.z) {
                Tile *t = gamemap.GetTile(&pos);
                Effect *e = new Effect(t);
                std::string extratext = "";
                if (msgtype == 0x01) extratext=" says";
                if (msgtype == 0x02) extratext=" whispers";
                if (msgtype == 0x03) extratext=" yells";
                std::string s = creaturename + extratext + ": " + message;
                e->SetText(s, 210, false);
                t->Insert(e, true);
            }
            #endif

            return true;
        }

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
			if (player->GetCreature()) {
				player->GetCreature()->SetDirection((direction_t)nm->GetU8()); // direction
				player->GetCreature()->CancelMoving();
				SoundPlay("sounds/bleep.wav");
			} else {
				nm->GetU8();
			}
            return true;
        case 0xBE: {// Floor Up
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Move up\n");

            player->SetPos(player->GetPosX()+1, player->GetPosY()+1, player->GetPosZ()-1);


            //ParseMapDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2, player->GetPosZ());

            unsigned int skip=0;
            if (player->GetPosZ()==7)
                for (int i = 5 ; i >=0; i--)
                    ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2 , i, &skip);
             else if (player->GetPosZ()>7)
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2 , player->GetPosZ()-2, &skip);

            player->FindMinZ();



			player->GetCreature()->ApproveMove();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"End move up\n");

            return true;
        }
        case 0xBF: {// Floor Down
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Floor down\n");



            player->SetPos(player->GetPosX()-1, player->GetPosY()-1, player->GetPosZ()+1);


            //ParseMapDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2, player->GetPosY() - (maxy-1)/2, player->GetPosZ());
            unsigned int skip=0;
            if (player->GetPosZ()==8) {
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosY() - (maxy-1)/2 , player->GetPosZ(), &skip);
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosY() - (maxy-1)/2 , player->GetPosZ()+1, &skip);
                printf("RECEIVING TWO FLOORS\n");
            }
            if (player->GetPosZ() >= 8) {
                ParseFloorDescription(nm, maxx, maxy, player->GetPosX() - (maxx-1)/2 , player->GetPosY() - (maxy-1)/2 , player->GetPosZ()+2, &skip);
                printf("RECEIVING ONE FLOOR\n");
            }


            player->FindMinZ();



			player->GetCreature()->ApproveMove();
            DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "End floor down\n");

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
        char tmp[512];
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Protocol %d: unfamiliar gameworld packet %02x\nLast previous packet is %02x", protocolversion, packetid, lastpreviouspacket);
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
        ASSERTFRIENDLY(TextureIntegrityTest(), "Protocol::ParseMapDescription(): Texture integrity test failed");
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
                        ParseTileDescription(nm, x + player->GetPosZ() - destz, y + player->GetPosZ() - destz, destz);
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

    //printf("Tile %d %d %d\n", x, y, z);
    position_t p;
    p.x = x; p.y = y; p.z = z;

    Tile *t = gamemap.GetTile(&p);
    t->Empty();
    for (;;) {
        if (nm->PeekU16() >= 0xFF00) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Reached end of tile\n");
            static bool locked;
            //if (locked = gamemap.locked) gamemap.Unlock();
            //t->StoreToDatabase();
            //if (locked) gamemap.Lock();
            return;
        } else {
            Thing *obj = ParseThingDescription(nm);
            t->Insert(obj, true);
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
//    printf("-------------%d----------\n", type);
    switch (type) {
        case 0x0061: // new creature
        case 0x0062: {// known creature
            if (type == 0x0061) { // new creature
            	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "New creature \n");
                nm->GetU32(); // remove creature with this id
                creatureid = nm->GetU32(); // new creature's id
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Creature id %d\n", creatureid);
                creature = gamemap.GetCreature(creatureid, creature);
                /*thing = creature;*/
                ASSERTFRIENDLY(creature, "gamemap.GetCreature() has DELETED creature\n");
                creature->SetName(nm->GetString()); // name string
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Got new creature %s\n", creature->GetName().c_str());
            }
            if (type == 0x0062) {
            	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Old creature \n");
                creatureid = nm->GetU32(); // known creature's id
                creature = gamemap.GetCreature(creatureid, creature);
                /*thing = creature;*/
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Got old creature %s\n", creature->GetName().c_str());
            }
            ASSERT(thing)
			ASSERT(creature)


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
                (creature)->SetSkull((skull_t)nm->GetU8()); // skull
                nm->GetU8(); // shield
            }

			printf("Now setting creature up\n");
            // stuff can only be set up AFTER the SetType() call ...
            // gotta check why, but we can live with that for now

            creature->SetType(creaturelook.type, &creaturelook);
            creature->SetDirection((direction_t)dir); // direction
            creature->SetSpeed(speedindex);
            creature->SetHP(hp);
            thing= creature;
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
            char tmp[512];
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
	ASSERTFRIENDLY(thing, "the end result that is to be returned is actually null! why? how?");
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
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Moving\n");
    if (!player) {
    	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "There is no player, we're not moving after all\n");
    	ONThreadUnsafe(threadsafe);
    	return;
    }
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Step 1\n");
    if (!player->GetCreature()) {
    	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "There is no player creature, we're not moving after all\n");
    	ONThreadUnsafe(threadsafe);
    	return;
    }
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Step 2\n");
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x65 + dir);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Moved\n");
        gamemap.Lock();
        if (dir != STOP) {
            player->GetCreature()->StartMoving();
            player->GetCreature()->SetDirection(dir);
        }
        else
            player->GetCreature()->CancelMoving();
		gamemap.Unlock();
		DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Locally moved\n");
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::Turn(direction_t dir) {

    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Turning\n");
    if (!player) {
    	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "There is no player, we're not turning after all\n");
    	ONThreadUnsafe(threadsafe);
    	return;
    }

    if (!player->GetCreature()) {
    	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "There is no player creature, we're not turning after all\n");
    	ONThreadUnsafe(threadsafe);
    	return;
    }
    if (!player->GetCreature()->IsMoving()) {
        nm.AddU8(0x6F + dir);
        if (protocolversion >= 770)
            nm.XTEAEncrypt(key);
        nm.Dump(s);
        gamemap.Lock();
        player->GetCreature()->SetDirection(dir);
        gamemap.Unlock();
    }
    ONThreadUnsafe(threadsafe);
}
void Protocol::Speak(speaktype_t sp, const char *message) {
    Speak(sp, message, NULL, 0);
}
void Protocol::Speak(speaktype_t sp, const char *message, unsigned long destination) {
    Speak(sp, message, NULL, destination);
}
void Protocol::Speak(speaktype_t sp, const char *message, const char *destination) {
    Speak(sp, message, destination, 0);
}
void Protocol::Speak(speaktype_t sp, const char *message, const char *deststr, unsigned long destlong) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x96);
    switch (sp) {
        case NORMAL:
            nm.AddU8(sp);
            nm.AddString(message);
            break;
        case PRIVATE:
            nm.AddU8(sp);
            nm.AddString(deststr);
            nm.AddString(message);
            console.insert(std::string(deststr) + "> " + message, CONLTBLUE);
            break;
        default:
            console.insert("A still-unsupported way of speaking :/", CONRED);
            ONThreadUnsafe(threadsafe);
            return;
    }
    nm.ShowContents();
    if (protocolversion >= 770) {
        nm.XTEAEncrypt(key);
        //nm.XTEADecrypt(key);
        //nm.ShowContents();
        //nm.XTEAEncrypt(key);
    }

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
        char tmp [512];
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
void Protocol::InviteParty(Creature *c) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA3);
    nm.AddU32(c->GetCreatureID());
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::JoinParty(Creature *c) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA4);
    nm.AddU32(c->GetCreatureID());
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

void Protocol::RevokeInviteParty(Creature *c) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA5);
    nm.AddU32(c->GetCreatureID());
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

void Protocol::PassLeadershipParty(Creature *c) {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA6);
    nm.AddU32(c->GetCreatureID());
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

void Protocol::LeaveParty() {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0xA7);
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}
void Protocol::Logout() {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x14);
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}


/////////////////////////extensions!////////////////////////////////
bool Protocol::CanCreateCharacter() { // by default, a protocol CAN'T create character. if a protocol will be able to do so, it'll have its own code for determining it...
    return false;
}

void Protocol::OCMCreateCharacter() {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x0C); // extension byte
    nm.AddU8(0x01); // report that we clicked on Create Character
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}


void Protocol::OCMCharlist() {
    NetworkMessage nm;
    ONThreadSafe(threadsafe);
    nm.AddU8(0x0C); // extension byte
    nm.AddU8(0x02); // we want to go back to charlist
    if (protocolversion >= 770)
        nm.XTEAEncrypt(key);
    nm.Dump(s);
    ONThreadUnsafe(threadsafe);
}

void Protocol::AddPosition(NetworkMessage *nm, const position_t *pos) {
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
        #ifdef INCLUDE_SP
        case 0xFFFF: // singleplayer protocol
            protocol = new ProtocolSP;
            return true;
        #endif
        #ifdef INCLUDE_ME
        case 0xFF00: // microedition protocol 0
            protocol = new ProtocolME0;
            return true;
        #endif

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
        case 800:
            protocol = new Protocol80;
            return true;
#endif
        default:
            return false;
    }

}
