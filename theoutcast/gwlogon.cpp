
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

inline void GWLogon_Status(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
}

ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;




	sockaddr_in sin;

	menuclass->charlist.SetCaption("Entering game world");

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s==INVALID_SOCKET) {
		GWLogon_ReportError(&menuclass->charlist, "Failed to create socket. (1)");
		return 1;
	}

	GWLogon_Status(&menuclass->charlist, "Resolving service...");
	hostent *he = gethostbyname(menuclass->txtLoginServer.GetCaption().c_str() );
	char convertedaddr[256];
	ULONG addr;
	char** addrs;
	if (he) {
		addrs = (char**)he->h_addr_list;
	} else {
		GWLogon_ReportError(&menuclass->charlist, "Cannot resolve server name. (2)");
		return 2;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = *(ULONG*)(addrs[0]);
	sin.sin_port = htons(LOGINPORT);

	GWLogon_Status(&menuclass->charlist, "Connecting...");

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

    closesocket(s);

	return 0;
}

