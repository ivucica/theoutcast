#include "protocolsp.h"
#include "database.h"
#include "threads.h"
#include "sound.h"
#include "defines.h"
void CharList_ReportError(glictMessageBox* mb, const char* txt);
void CharList_ReportSuccess(glictMessageBox* mb, const char* txt);
void CharList_Status(glictMessageBox* mb, const char* txt);

void GWLogon_ReportError(glictMessageBox* mb, const char* txt);
void GWLogon_ReportSuccess(glictMessageBox* mb, const char* txt);
void GWLogon_Status(glictMessageBox* mb, const char* txt);

static int spcount=0;

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

    return 0;

}



ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon_SP(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;

    SoundPlay("sounds/drums.wav");
	menuclass->charlist.SetCaption("Entering game");



    if (protocol->GameworldLogin() ) {
        GWLogon_Status(&menuclass->charlist, "Entering game...");
        //GWLogon_ReportSuccess(&menuclass->charlist, protocol->GetMotd().c_str() );
        menuclass->GoToGameworld();
    } else {
        GWLogon_ReportError(&menuclass->charlist, protocol->GetError().c_str() );
    }
    ((GM_MainMenu*)game)->DestroyCharlist();

	return 0;
}




void ProtocolSP::CharlistConnect() {
    thrCharList = ONNewThread(Thread_CharList_SP, game); //CreateThread(NULL, 0, Thread_CharList, ((GM_MainMenu*)game), 0, &((GM_MainMenu*)game)->thrCharListId);
}
bool ProtocolSP::CharlistLogin(const char *username, const char *password) {
    NetworkMessage nm;

    ONThreadSafe(threadsafe);
    connectiontype = CHARLIST;


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
            nm.AddU8(0x14);
            nm.AddString("7435\nWelcome to Clavicula, a singleplayer mode for The Outcast!\n\nClavicula is an attempt to create a singleplayer game \nsimilar to Tibia. To create a character, choose Create Character \noption from the character list.");
            nm.AddU8(0x64);
            nm.AddU8(1 + spcount); // one character is CREATE CHARACTER, others are temp count to make dynamic list
            nm.AddString("Create Character");
            nm.AddString("Clavicula");
            nm.AddU32(0); // ip address
            nm.AddU16(0); // port

            for (int i =0; i < spcount; i++) {
                char tmp[255];
                sprintf(tmp, "Chr %d", i);

                nm.AddString(tmp);
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


    ((GM_MainMenu*)game)->DestroyCharlist();


    nm.AddU8(0x14);
    nm.AddString("Asfarg.");
    spcount ++;

    // by default logon is a success
    logonsuccessful = true;

    packetparsing:
    while ((signed int)(nm.GetSize())>0 && ParsePacket(&nm));
    if ((signed int)(nm.GetSize())>0) printf("++++++++++++++++++++DIDNT EMPTY UP THE NETWORKMESSAGE!++++++++++++++++++\n");

    ONThreadUnsafe(threadsafe);
    return logonsuccessful;

}
