#include "protocolsp.h"
#include "database.h"
#include "threads.h"
#include "sound.h"
#include "defines.h"
#include "assert.h"
#include "console.h"
#include "player.h"
#include "map.h"
#include "debugprint.h"
#include "items.h"
#include "creatures.h"


#include "spcreature.h"

extern bool dontloadspr;
std::vector<SPCreature> spcreatures;

void CharList_ReportError(glictMessageBox* mb, const char* txt);
void CharList_ReportSuccess(glictMessageBox* mb, const char* txt);
void CharList_Status(glictMessageBox* mb, const char* txt);

void GWLogon_ReportError(glictMessageBox* mb, const char* txt);
void GWLogon_ReportSuccess(glictMessageBox* mb, const char* txt);
void GWLogon_Status(glictMessageBox* mb, const char* txt);



ProtocolSP::ProtocolSP() {
    protocolversion = 792; // singleplayer will always load latest supported SPR version
    fingerprints[FINGERPRINT_TIBIADAT] = 0x459E7B73;
    fingerprints[FINGERPRINT_TIBIASPR] = 0x45880FE8;
    fingerprints[FINGERPRINT_TIBIAPIC] = 0x45670923;

    maxx = 18; maxy = 14; maxz = 14;


    spcreatures.insert(spcreatures.end(),SPCreature("Tim", 100, 100, 14, 31, 7, 35, 0, 0, 0, 0, 0, 0));
    spcreatures.insert(spcreatures.end(), SPCreature("Ann", 50, 100, position_t(17, 32, 7), creaturelook_t(30, 0, 0, 0, 0, 0, 0)));
    spcreatures.insert(spcreatures.end(), SPCreature("Pyetro", 50, 100, position_t(30, 32, 7), creaturelook_t(99, 0, 0, 0, 0, 0, 0)));
    spcreatures.insert(spcreatures.end(), SPCreature("Markus", 100, 100, position_t(31, 32, 7), creaturelook_t(132, 0, 0, 0, 0, 0, 0)));
    spcreatures.insert(spcreatures.end(), SPCreature("Aurora", 100, 100, position_t(37, 34, 7), creaturelook_t(58, 0, 0, 0, 0, 0, 0)));
    spcreatures.insert(spcreatures.end(), SPCreature("Gruumsh", 100, 100, position_t(22, 28, 7), creaturelook_t(55, 0, 0, 0, 0, 0, 0)));
    spcreatures.insert(spcreatures.end(), SPCreature("Johnny", 25, 100, position_t(25, 27, 7), creaturelook_t(35, 0, 0, 0, 0, 0, 0)));
}

ProtocolSP::~ProtocolSP() {
	spcreatures.empty();
}
ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList_SP(ONThreadFuncArgumentType menuclass_void) {

	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;
//char tmp[250];
//printf("$$$$$$$$$$$$$$$\n");
//fgets(tmp, 50, stdin);
//printf("((((((((((((\n");
    dbExec(dbUser, "begin transaction;", NULL, 0, NULL);
    dbSaveSetting("protocol", "SP");
    dbSaveSetting("server", menuclass->txtLoginServer.GetCaption().c_str());
    dbSaveSetting("username", menuclass->txtLoginUsername.GetCaption().c_str());
    dbSaveSetting("password", menuclass->txtLoginPassword.GetCaption().c_str());
    dbExec(dbUser, "end transaction;", NULL, 0, NULL);



    if (protocol->CharlistLogin(menuclass->txtLoginUsername.GetCaption().c_str(), menuclass->txtLoginPassword.GetCaption().c_str()) ) {
        CharList_ReportSuccess(&menuclass->charlist, protocol->GetMotd().c_str() );
    } else {
        CharList_ReportError(&menuclass->charlist, protocol->GetError().c_str() );
    }
  //  fgets(tmp, 50, stdin);
    return 0;

}



ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon_SP(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;

    SoundPlay("sounds/drums.wav");
	menuclass->charlist.SetCaption("Entering game");



    if (protocol->GameworldLogin() ) {
        GWLogon_Status(&menuclass->charlist, "Entering game...");
        //GWLogon_ReportSuccess(&menuclass->charlist, protocol->GetMotd().c_str() );
        if (protocol->newgamemode==GM_CHARMGR)
            menuclass->GoToCharMgr();
        else
            menuclass->GoToGameworld();
    } else {
        GWLogon_ReportError(&menuclass->charlist, protocol->GetError().c_str() );
    }

	return 0;
}


bool ProtocolSP::GameworldWork() {
/*	NetworkMessage nm;
	printf("working\n");

	printf("running %d creatures\n", spcreatures.size());
	for (std::vector<SPCreature>::iterator it = spcreatures.begin(); it!=spcreatures.end(); it++) {
		it->Run(nm);
	}


    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

*/
#ifndef WIN32
	sleep(1);
#endif
	return true;

}

void ProtocolSP::CharlistConnect() {
    char tmp[50];
//    fgets(tmp, 50, stdin);
    thrCharList = ONNewThread(Thread_CharList_SP, game); //CreateThread(NULL, 0, Thread_CharList, ((GM_MainMenu*)game), 0, &((GM_MainMenu*)game)->thrCharListId);
//    printf("--------\n");
//    fgets(tmp, 50, stdin);
//    printf("###########\n");
}
bool ProtocolSP::CharlistLogin(const char *username, const char *password) {
    NetworkMessage nm;

    ONThreadSafe(threadsafe);
    connectiontype = CHARLIST;

    this->username = username;
    this->password = password;

    FILE *f = fopen((std::string("save/") + username + ".ous").c_str(),"r");
    if (!f) {
        FILE *fo = fopen((std::string("save/") + username + ".ous").c_str(),"w");
        if (!fo) {
            nm.AddU8(0x0A);
            nm.AddString("You need write permissions on save/ subfolder of \nThe Outcast to start a local game.");
            goto packetparsing;
        } else {
            fprintf(fo, "%s\n", password);
            fclose(fo);
            f = fopen((std::string("save/") + username + ".ous").c_str(),"r");

        }
    }

    {
        char filepwd[255];
        fscanf(f, "%s", filepwd);

        if (strcmp(filepwd, password)) {
            nm.AddU8(0x0A);
            nm.AddString("You entered incorrect password.");
        } else {
            char charname[255];

            nm.AddU8(0x14);
            nm.AddString("7435\nWelcome to Clavicula, a singleplayer mode for The Outcast!\n\nClavicula is an attempt to create a singleplayer game \nsimilar to Tibia. To create a character, choose Character\nManager option from the character list.");
            nm.AddU8(0x64);

            int pos = ftell(f);
            int spcount = 0;
            while (fscanf(f, "%s", charname)==1) spcount ++;
            fseek(f, pos, SEEK_SET);
            nm.AddU8(1 + spcount); // one character is CREATE CHARACTER, others are temp count to make dynamic list
            nm.AddString("Character Manager");
            nm.AddString("Clavicula");
            nm.AddU32(0); // ip address
            nm.AddU16(0); // port


            while (fscanf(f, "%s", charname)==1)  {
                nm.AddString(charname);
                nm.AddString("Clavicula");
                nm.AddU32(0); // ip address
                nm.AddU16(0); // port
            }

            nm.AddU16(0); // free account
        }
    }

    // by default logon is a success
    logonsuccessful = true;

    packetparsing:
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    ONThreadUnsafe(threadsafe);
    return logonsuccessful;
}
void ProtocolSP::GameworldConnect() {
    thrGWLogon = ONNewThread(Thread_GWLogon_SP, game); //CreateThread(NULL, 0, Thread_CharList, ((GM_MainMenu*)game), 0, &((GM_MainMenu*)game)->thrCharListId);
}
bool ProtocolSP::GameworldLogin () {
    NetworkMessage nm;

    ONThreadSafe(threadsafe);
    connectiontype = GAMEWORLD;

    //nm.AddU8(0x14);
    //nm.AddString(this->charlist[this->charlistselected]->charactername);
    //spcount ++;
    if (!strcmp(this->charlist[this->charlistselected]->charactername, "Character Manager")) {
        nm.AddU8(0x0C); // charmgr...
        nm.AddU8(0x01); // ...enter

        FILE *f = fopen((std::string("save/") + this->username + ".ous").c_str(), "r");
        ASSERTFRIENDLY(f, "It appears that savefile has mysteriously disappeared. Exiting");
        fclose(f);


    }
    else {

        nm.AddU8(0x0A); // player's creature id shall be 1
        nm.AddU32(1);

        nm.AddU8(0x32); // report bugs?
        nm.AddU8(0);
        nm.AddU8(0);

        nm.AddU8(0x64); // player teleport
        nm.AddU16(30);
        nm.AddU16(30);
        nm.AddU8(7);
        int tilesremaining = 18*16*7;
        player = new Player(1);
        player->SetPos(30,30,7);
        /*for (int i = 0; i < maxx; i ++)
            for (int j = 0; j < maxy; j++) {*/
		dontloadspr = true;
		ItemsLoad();
		CreaturesLoad();
		for (int k = 7; k >= 0; k--) {
			for (int i = player->GetPosX() - maxx/2+1; i < player->GetPosX() + maxx/2+1; i++) {
				for (int j = player->GetPosY() - maxy/2+1; j < player->GetPosY() + maxy/2+1; j++) {
					//printf("%d\n", tilesremaining);
					std::vector<Thing*> remover;
					Tile t;
					this->SPFillTile(t, remover,i,j,k);
					//printf("so far so good\n");

					if (i == player->GetPosX() && j == player->GetPosY() && k == player->GetPosZ()) {
						Creature *c = new Creature;
						creaturelook_t cl;
						cl.head = 50;
						cl.body = 59;
						cl.legs = 70;
						cl.feet = 80;
						cl.addons = 0;
						c->SetCreatureID(1);
						c->SetType(128, &cl);
						printf("manyak\n");
						c->SetName("Noob");
						printf("sretche\n");
						c->SetDirection(NORTH);
						printf("umom\n");
						c->SetHP(70);
						printf("gruumsh\n");
						t.Insert(c, false);
						printf("petaq\n");
						remover.insert(remover.begin(), c);
					}


					this->SPAddTile(&t, &nm);
					for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
						delete *it;
					}
					/*nm.AddU16(102);
					if (i == maxx/2-1 && j == maxy/2-1) {
						nm.AddU16(0x0061);
						nm.AddU32(0); // remove with this id
						nm.AddU32(1); // creatureid -- player is 1
						nm.AddString("Newbie");
						nm.AddU8(25); // health
						nm.AddU8(0); //dir
						nm.AddU16(128); // lookid
						nm.AddU8(50);
						nm.AddU8(60);
						nm.AddU8(70);
						nm.AddU8(80);
						nm.AddU8(0); // addons

						nm.AddU8(0); // lightlevel
						nm.AddU8(0); // lightcolor
						nm.AddU16(500); // speed
						nm.AddU8(0); // skull
						nm.AddU8(0); // shield

					}*/
					tilesremaining--;
					//nm.AddU16(0xFF00);
				}
			}
		}
		CreaturesUnload();
		ItemsUnload();
		dontloadspr = false;
		delete player; player = NULL;
        while(tilesremaining) {

            nm.AddU8(tilesremaining > 255 ? 255 : tilesremaining);
            tilesremaining -= tilesremaining > 255 ? 255 : tilesremaining;

            nm.AddU8(0xFF);
        }
    }


    ((GM_MainMenu*)game)->DestroyCharlist();





    // by default logon is a success
    logonsuccessful = true;

    packetparsing:
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    ONThreadUnsafe(threadsafe);
    return logonsuccessful;

}

void ProtocolSP::Close() {
	console.insert("Singleplayer disconnection?!", CONRED);
    //printf("Internal error\n");
    //system("pause");
    //exit(1);
}

void ProtocolSP::Move(direction_t dir) {
	if (!player->GetCreature()->IsMoving()) {

		switch (dir) {
			case NORTH:
				if (player->GetPosY() == 1) {
					player->GetCreature()->CancelMoving();
					return;
				}
				break;
			case WEST:
				if (player->GetPosX() == 1) {
					player->GetCreature()->CancelMoving();
					return;
				}
				break;

		}

        gamemap.Lock();
        if (dir != STOP) {
            player->GetCreature()->StartMoving();
            player->GetCreature()->SetDirection(dir);
        }
        else
            player->GetCreature()->CancelMoving();

		gamemap.Unlock();
		DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Locally moved\n");


		// now we imagine we're the serverside ...
		NetworkMessage nm;
		/*nm.AddU8(0x15); // for your information
		nm.AddString("Moving not supported yet. We're cancelling you.");
		nm.AddU8(0xB5); // cancel move
		nm.AddU8(dir);*/


		nm.AddU8(0x65+dir);
		switch (dir) {
			case NORTH:
				for (int k = 7; k >= 0; k--) {
					for (int i = player->GetPosX() - maxx/2+1; i < player->GetPosX() + maxx/2+1; i++) {
						//for (int j = player->GetPosY() - maxy/2+1; j < player->GetPosY() + maxy/2+1; j++) {
						int j = player->GetPosY() - maxy/2;
							Tile t;
							std::vector<Thing*> remover;
							this->SPFillTile(t, remover, i, j, k);
							this->SPAddTile(&t, &nm);
							for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
								delete *it;
							}
						//}
					}
				}
				break;
			case SOUTH:
				for (int k = 7; k >= 0; k--) {
					for (int i = player->GetPosX() - maxx/2+1; i < player->GetPosX() + maxx/2+1; i++) {
						//for (int j = player->GetPosY() - maxy/2+1; j < player->GetPosY() + maxy/2+1; j++) {
						int j = player->GetPosY() + maxy/2+1;
							Tile t;
							std::vector<Thing*> remover;
							this->SPFillTile(t, remover, i, j, k);
							this->SPAddTile(&t, &nm);
							for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
								delete *it;
							}
						//}
					}
				}
				break;
			case WEST:
				for (int k = 7; k >= 0; k--) {
					//for (int i = player->GetPosX() - maxx/2+1; i < player->GetPosX() + maxx/2+1; i++) {
					int i = player->GetPosX() - maxx/2;
						for (int j = player->GetPosY() - maxy/2+1; j < player->GetPosY() + maxy/2+1; j++) {

							Tile t;
							std::vector<Thing*> remover;
							this->SPFillTile(t, remover, i, j, k);
							this->SPAddTile(&t, &nm);
							for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
								delete *it;
							}
						}
					//}
				}
				break;
			case EAST:
				for (int k = 7; k >= 0; k--) {
					//for (int i = player->GetPosX() - maxx/2+1; i < player->GetPosX() + maxx/2+1; i++) {
					int i = player->GetPosX() + maxx/2+1;
						for (int j = player->GetPosY() - maxy/2+1; j < player->GetPosY() + maxy/2+1; j++) {

							Tile t;
							std::vector<Thing*> remover;
							this->SPFillTile(t, remover, i, j, k);
							this->SPAddTile(&t, &nm);
							for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
								delete *it;
							}
						}
					//}
				}
				break;

		}
		/*
		switch (dir) {
			case NORTH:
			case SOUTH: {
				int tilesremaining = maxx*6;
				if (player->GetPosY() < maxy / 2 + 1){
					nm.AddU8(maxx);
					nm.AddU8(0xFF);
				}
				else
					for (int i = 0; i < maxx; i ++) {

						nm.AddU16(102);
						nm.AddU16(0xFF00);
					}

				while(tilesremaining) {
					nm.AddU8(tilesremaining > 255 ? 255 : tilesremaining);
					tilesremaining -= tilesremaining > 255 ? 255 : tilesremaining;
					nm.AddU8(0xFF);
				}
			}
			case EAST:
			case WEST: {

				int tilesremaining = maxy*6 + maxy;

				if (player->GetPosX() < maxx / 2 + 1){
					nm.AddU8(maxy);
					nm.AddU8(0xFF);
				}

				else for (int i = 0; i < maxy + 1; i ++) {
					nm.AddU16(102);
					nm.AddU16(0xFF00);
				}

				while(tilesremaining) {
					nm.AddU8(tilesremaining > 255 ? 255 : tilesremaining);
					tilesremaining -= tilesremaining > 255 ? 255 : tilesremaining;
					nm.AddU8(0xFF);
				}

			}

		}*/

		nm.AddU8(0x6D);
		nm.AddU16(player->GetPosX()),
		nm.AddU16(player->GetPosY());
		nm.AddU8(player->GetPosZ());
		position_t pos;
		player->GetPos(&pos);
		Tile *t = gamemap.GetTile(&pos);
		nm.AddU8( t->GetTopCreatureStackpos() );

		switch (dir) {
			case NORTH:
				nm.AddU16(player->GetPosX()),
				nm.AddU16(player->GetPosY()-1);
				nm.AddU8(player->GetPosZ());
				break;
			case SOUTH:
				nm.AddU16(player->GetPosX()),
				nm.AddU16(player->GetPosY()+1);
				nm.AddU8(player->GetPosZ());
				break;
			case EAST:
				nm.AddU16(player->GetPosX()+1),
				nm.AddU16(player->GetPosY());
				nm.AddU8(player->GetPosZ());
				break;
			case WEST:
				nm.AddU16(player->GetPosX()-1),
				nm.AddU16(player->GetPosY());
				nm.AddU8(player->GetPosZ());
				break;
		}

		ParsePacket(&nm);
		ParsePacket(&nm);

		while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
		if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");
    }

}
void ProtocolSP::OCMCreateCharacter() {
    ((GM_CharMgr*)game)->ShowCreateCharacter();
}
void ProtocolSP::OCMCharlist() {
    ((GM_CharMgr*)game)->ShowCharList();
}

void ProtocolSP::SPAddTile(Tile*tl, NetworkMessage *nm) {
/*	for (std::vector<Item*>::iterator it = t->itemlayers[0].begin(); it != t->itemlayers[0].end(); it++) {
	}*/
	if (tl->ground) {
		nm->AddU16(tl->ground->GetType());
	}

	for (std::vector<Creature*>::iterator it = tl->creatures.begin(); it != tl->creatures.end(); it++) {

		SPAddCreature(nm, *it);

	}
	nm->AddU16(0xFF00);
}


void ProtocolSP::SPAddCreature(NetworkMessage *nm, Creature* cr) {

	nm->AddU16(0x0061);
	nm->AddU32(0); // remove with this id
	nm->AddU32(cr->GetCreatureID() ); // creatureid -- player is 1
	nm->AddString(cr->GetName());
	nm->AddU8(cr->GetHP()); // health
	nm->AddU8(cr->GetDirection()); //dir
	nm->AddU16(cr->GetType() ); // lookid
	nm->AddU8(cr->GetCreatureLook().head);
	nm->AddU8(cr->GetCreatureLook().body);
	nm->AddU8(cr->GetCreatureLook().legs);
	nm->AddU8(cr->GetCreatureLook().feet);
	nm->AddU8(0); // addons

	nm->AddU8(0); // lightlevel
	nm->AddU8(0); // lightcolor
	nm->AddU16(500); // speed
	nm->AddU8(0); // skull
	nm->AddU8(0); // shield



}
void ProtocolSP::SPFillTile(Tile &t, std::vector<Thing*>&remover, int i, int j, int k) {
	if (k == 7 && i == 29 && j == 29) {
		Item *i = new Item;
		i->SetType(101, NULL);
		t.Insert(i, false);
		remover.insert(remover.begin(), i);
	} else
	if (k == 7 && i == 31) {
		Item *i = new Item;
		i->SetType(103, NULL);
		t.Insert(i, false);
		remover.insert(remover.begin(), i);
	} else
	if (k == 7 && j == 31) {
		Item *i = new Item;
		i->SetType(104, NULL);
		t.Insert(i, false);
		remover.insert(remover.begin(), i);
	} else
	if (k == 7 && i > 0 && j > 0) {
		Item *i = new Item;
		i->SetType(102, NULL);
		t.Insert(i, false);
		remover.insert(remover.begin(), i);
	}



	for (std::vector<SPCreature>::iterator it = spcreatures.begin(); it!=spcreatures.end(); it++) {
		if (i == it->pos.x && j == it->pos.y && k == it->pos.z) {
			Creature *c = it->MakeCreature();
			t.Insert(c, false);
			remover.insert(remover.begin(), c);
		}
	}
}

void ProtocolSP::LookAt(position_t *pos) {
	NetworkMessage nm;
	printf("working\n");

	printf("running %d creatures\n", spcreatures.size());
	for (std::vector<SPCreature>::iterator it = spcreatures.begin(); it!=spcreatures.end(); it++) {
		it->Run(nm);
	}


    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");


//	sleep(1);

}
