
#define LOGINPORT 7171
#include <windows.h>

#include <GLICT/messagebox.h>
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
inline void GWLogon_ReportError(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Error accessing account");
	mb->SetHeight(64);
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonError);
}
inline void GWLogon_ReportSuccess(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Message of the Day");
	mb->SetHeight(64);
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonOK);
}

void GWLogon_Status(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
}

ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;
	sockaddr_in sin;

    SoundPlay("sounds/drums.wav");
	menuclass->charlist.SetCaption("Entering game world");

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s==INVALID_SOCKET) {
		GWLogon_ReportError(&menuclass->charlist, "Failed to create socket. (1)");
		return 1;
	}


    if (!strcmp(menuclass->txtLoginServer.GetCaption().c_str(), "server.tibia.com")) {
        protocol->CipSoft(true);
    } else {
        protocol->CipSoft(false);
    }

    // 0 = blocking, 1 = nonblocking
	unsigned long mode = 0;
	ioctlsocket(s, FIONBIO, &mode);


	/*GWLogon_Status(&menuclass->charlist, "Resolving service...");
	hostent *he = gethostbyname( protocol->charlist[protocol->charlistselected].ipaddress   );
	char convertedaddr[256];
	ULONG addr;
	char** addrs;
	if (he) {
		addrs = (char**)he->h_addr_list;
	} else {
		GWLogon_ReportError(&menuclass->charlist, "Cannot resolve server name. (2)");
		return 2;
	}*/

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = protocol->charlist[protocol->charlistselected].ipaddress; //*(ULONG*)(addrs[0]);
	sin.sin_port = htons(protocol->charlist[protocol->charlistselected].port );

	{
	    char tmp[256];
	    sprintf(tmp, "Connecting to %d.%d.%d.%d:%d...", (int)((unsigned char*)&protocol->charlist[protocol->charlistselected].ipaddress)[0],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected].ipaddress)[1],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected].ipaddress)[2],
	    (int)((unsigned char*)&protocol->charlist[protocol->charlistselected].ipaddress)[3],
	    (int)protocol->charlist[protocol->charlistselected].port
	    );

	    GWLogon_Status(&menuclass->charlist, tmp);
	    printf("%s\n", tmp);
	}

	//GWLogon_Status(&menuclass->charlist, "Connecting...");

	if (connect(s, (SOCKADDR*)&sin, sizeof(sin))) {
		char tmp[256];
		sprintf(tmp, "Socket error:\n%s (3)", SocketErrorDescription());
		GWLogon_ReportError(&menuclass->charlist, tmp);

		return 3;
	}



	GWLogon_Status(&menuclass->charlist, "Entering game...");

    protocol->SetSocket(s);

    if (protocol->GameworldLogin() ) {
        //GWLogon_ReportSuccess(&menuclass->charlist, protocol->GetMotd().c_str() );
        menuclass->GoToGameworld();
    } else {
        GWLogon_ReportError(&menuclass->charlist, protocol->GetError().c_str() );
    }


	return 0;
}

