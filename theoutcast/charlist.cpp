
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
inline void CharList_ReportError(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Error accessing account");
	mb->SetHeight(64);
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonError);
}
inline void CharList_ReportSuccess(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
	mb->SetEnabled(true);
	mb->SetCaption("Message of the Day");
	mb->SetHeight(64);
	mb->SetOnDismiss(GM_MainMenu_CharList_LogonOK);
}

inline void CharList_Status(glictMessageBox* mb, const char* txt) {
	mb->SetMessage(txt);
}

ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList(ONThreadFuncArgumentType menuclass_void) {
	GM_MainMenu* menuclass = (GM_MainMenu*)menuclass_void;



	((GM_MainMenu*)game)->charlist.SetMessage("Storing to database...");

    dbExec(dbUser, "begin transaction;", NULL, 0, NULL);
    dbSaveSetting("protocol", menuclass->txtLoginProtocol.GetCaption().c_str());
    dbSaveSetting("server", menuclass->txtLoginServer.GetCaption().c_str());
    dbSaveSetting("username", menuclass->txtLoginUsername.GetCaption().c_str());
    dbSaveSetting("password", menuclass->txtLoginPassword.GetCaption().c_str());
    dbExec(dbUser, "end transaction;", NULL, 0, NULL);

	sockaddr_in sin;

	menuclass->charlist.SetCaption("Logging in...");

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s==INVALID_SOCKET) {
		CharList_ReportError(&menuclass->charlist, "Failed to create socket. (1)");
		return 1;
	}

	CharList_Status(&menuclass->charlist, "Resolving service...");
	hostent *he = gethostbyname(menuclass->txtLoginServer.GetCaption().c_str() );
	char convertedaddr[256];
	ULONG addr;
	char** addrs;
	if (he) {
		addrs = (char**)he->h_addr_list;
	} else {
		CharList_ReportError(&menuclass->charlist, "Cannot resolve server name. (2)");
		return 2;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = *(ULONG*)(addrs[0]);
	sin.sin_port = htons(LOGINPORT);

	CharList_Status(&menuclass->charlist, "Connecting...");

	if (connect(s, (SOCKADDR*)&sin, sizeof(sin))) {
		char tmp[256];
		sprintf(tmp, "Socket error:\n%s (3)", SocketErrorDescription());
		CharList_ReportError(&menuclass->charlist, tmp);

		return 3;
	}

	CharList_Status(&menuclass->charlist, "Retrieving character list...");

    protocol->SetSocket(s);
    if (protocol->CharlistLogin(menuclass->txtLoginUsername.GetCaption().c_str(), menuclass->txtLoginPassword.GetCaption().c_str()) ) {
        CharList_ReportSuccess(&menuclass->charlist, protocol->GetMotd().c_str() );
    } else {
        CharList_ReportError(&menuclass->charlist, protocol->GetError().c_str() );
    }

    closesocket(s);

	return 0;
}

