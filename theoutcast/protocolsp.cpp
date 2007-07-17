#include "protocolsp.h"
#include "database.h"
#include "threads.h"
#include "sound.h"
#include "defines.h"
#include "assert.h"
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
}

ProtocolSP::~ProtocolSP() {
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
        int tilesremaining = 18*14*7;
        for (int i = 0; i < 18; i ++)
            for (int j = 0; j < 14; j++) {
                printf("%d\n", tilesremaining);
                nm.AddU16(102);
                if (i == 8 && j == 6) {
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




                }
                tilesremaining--;
                nm.AddU16(0xFF00);
            }
        while(tilesremaining) {

            nm.AddU8(tilesremaining > 255 ? 255 : tilesremaining);
            tilesremaining -= tilesremaining > 255 ? 255 : tilesremaining;
            printf("%d\n", tilesremaining);
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
    //printf("Internal error\n");
    //system("pause");
    //exit(1);
}
void ProtocolSP::OCMCreateCharacter() {
    ((GM_CharMgr*)game)->ShowCreateCharacter();
}
void ProtocolSP::OCMCharlist() {
    ((GM_CharMgr*)game)->ShowCharList();
}
