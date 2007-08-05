
#define LOGINPORT 7171
#ifdef WIN32
	#include <windows.h>
#endif

#include <GLICT/messagebox.h>
#include <GLICT/fonts.h>
#include "gm_mainmenu.h"
#include "socketstrings.h"
#include "networkmessage.h"
#include "networkdirect.h"
#include "threads.h"
#include "debugprint.h"
#include "protocol.h"
#include "database.h"
#include "items.h"
#include "sound.h"
#include "bsdsockets.h"
#include "glutfont.h"
void GWLogon_ReportError(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Error accessing character");
	mb->SetHeight(64 - (11*3) + glictFontNumberOfLines(txt)*11 );
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonError);
}
void GWLogon_ReportSuccess(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Logged on!");
	mb->SetHeight(64 - (11*3) + glictFontNumberOfLines(txt)*11 );
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonOK);
}

void GWLogon_Status(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
}

ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;
	sockaddr_in sin;

    SoundPlay("sounds/drums.wav");
	menuclass->charlist.SetCaption("Entering game");
	menuclass->charlist.SetMessage("Creating socket...");

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s==INVALID_SOCKET) {
		GWLogon_ReportError(&menuclass->charlist, "Failed to create socket. (1)");
		((GM_MainMenu*)game)->DestroyCharlist();
		return (ONThreadFuncReturnType)1;
	}



    if (!strcmp(menuclass->txtLoginServer.GetCaption().c_str(), "server.tibia.com") ||
        !strcmp(menuclass->txtLoginServer.GetCaption().c_str(), "login01.tibia.com") ) {
        protocol->CipSoft(true);
    } else {
        protocol->CipSoft(false);
    }

	#ifdef WIN32
    // 0 = blocking, 1 = nonblocking
	unsigned long mode = 0;
	ioctlsocket(s, FIONBIO, &mode);
	#endif


	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = protocol->charlist[protocol->charlistselected]->ipaddress; //*(ULONG*)(addrs[0]);
    sin.sin_port = htons(protocol->charlist[protocol->charlistselected]->port );

	{
	    char ip[256], tmp[256];

	    sprintf(ip, "%d.%d.%d.%d", (int)((unsigned char*)&protocol->charlist[protocol->charlistselected]->ipaddress)[0],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected]->ipaddress)[1],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected]->ipaddress)[2],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected]->ipaddress)[3]);
        protocol->gameworldserver = ip;
        protocol->gameworldport = protocol->charlist[protocol->charlistselected]->port;

	    sprintf(tmp, "Connecting to %s:%d...", ip,
	    (int)protocol->charlist[protocol->charlistselected]->port
	    );

	    GWLogon_Status(&menuclass->charlist, tmp);
	    printf("%s\n", tmp);
	}



	if (connect(s, (SOCKADDR*)&sin, sizeof(sin))) {
		char tmp[512];
		sprintf(tmp, "Socket error:\n%s (3)", SocketErrorDescription());
		GWLogon_ReportError(&menuclass->charlist, tmp);

        ((GM_MainMenu*)game)->DestroyCharlist();
		return (ONThreadFuncReturnType)3;
	}



	GWLogon_Status(&menuclass->charlist, "Logging in...");

    protocol->SetSocket(s);

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

