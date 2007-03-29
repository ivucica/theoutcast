
#include <malloc.h>
#include <stdio.h>
#ifndef WIN32

const char* SocketErrorDescription() {
	return "Can't identify errors under BSD sockets.";
}

#else

#include <windows.h>
const char* SocketErrorDescription() {
    int wsalasterr;
    switch (wsalasterr=WSAGetLastError()) {
        case WSAEINTR:
            return "Interrupted function call. "; //return("WSAEINTR");
            break;
        case WSAEACCES:
            return "Permission denied. "; //return("WSAEACCES");
            break;
        case WSAEFAULT:
            return "Bad address. "; //return("WSAEFAULT");
            break;
        case WSAEINVAL:
            return "Invalid argument. "; //return("WSAEINVAL");
            break;
        case WSAEMFILE:
            return "Too many open files. ";
            break;
        case WSAEWOULDBLOCK:
            return "Resource temporarily unavailable. "; //return("WSAEWOULDBLOCK");
            break;
        case WSAEINPROGRESS:
            return "Operation now in progress. "; //return("WSAEINPROGRESS");
            break;
        case WSAEALREADY:
            return "Operation already in progress. "; //return("WSAEALREADY");
            break;
        case WSAENOTSOCK:
            return "Socket operation on nonsocket. ";//return("WSAENOTSOCK");
            break;
        case WSAEDESTADDRREQ:
            return "Destination address required. ";
            break;
        case WSAEMSGSIZE:
            return "Message too long. ";
            break;
        case WSAEPROTOTYPE:
            return "Protocol wrong type for socket. ";
            break;
        case WSAENOPROTOOPT:
            return "Bad protocol option. ";
            break;
        case WSAEPROTONOSUPPORT:
            return "Protocol not supported. ";
            break;
        case WSAESOCKTNOSUPPORT:
            return "Socket type not supported. ";
            break;
        case WSAEOPNOTSUPP:
            return "Operation not supported. ";
            break;
        case WSAEPFNOSUPPORT:
            return "Protocol family not supported. ";
            break;
        case WSAEAFNOSUPPORT:
            return "Address family not supported by protocol family. ";
            break;
        case WSAEADDRINUSE:
            return "Address already in use. "; //return("WSAEADDRINUSE");
            break;
        case WSAEADDRNOTAVAIL:
            return "Cannot assign requested address. "; //return("WSAEADDRNOTAVAIL");
            break;
        case WSAENETDOWN:
            return "Network is down."; //return("WSAENETDOWN");
            break;
        case WSAENETUNREACH:
            return "Network is unreachable. "; //return("WSAENETUNREACH");
            break;
        case WSAENETRESET:
            return "Network dropped connection on reset. ";
            break;
        case WSAECONNABORTED:
            return "Software caused connection abort. ";
            break;
        case WSAECONNRESET:
            return "Connection reset by peer. ";
            break;
        case WSAENOBUFS:
            return "No buffer space available. "; //return("WSAENOBUFS");
            break;
        case WSAEISCONN:
            return "Socket is already connected. "; //return("WSAEISCONN");
            break;
        case WSAENOTCONN:
            return "Socket is not connected. ";
            break;
        case WSAESHUTDOWN:
            return "Cannot send after socket shutdown. ";
            break;
        case WSAETIMEDOUT:
            return "Connection timed out. "; //return("WSAETIMEDOUT");
            break;
        case WSAECONNREFUSED:
            return "Connection refused. "; //return("WSAECONNREFUSED");
            break;
        case WSAEHOSTDOWN:
            return "Host is down. ";
            break;
        case WSAEHOSTUNREACH:
            return "No route to host. ";
            break;
        case WSAEPROCLIM:
            return "Too many processes. ";
            break;
        case WSASYSNOTREADY:
            return "Network subsystem is unavailable. ";
            break;
        case WSAVERNOTSUPPORTED:
            return "Winsock.dll version out of range. ";
            break;
        case WSANOTINITIALISED:
            return "Successful WSAStartup not yet performed. "; //return("WSANOTINITIALISED");
            break;
        case WSAEDISCON:
            return "Graceful shutdown in progress. ";
            break;
        case WSATYPE_NOT_FOUND:
            return "Class type not found. ";
            break;
        case WSAHOST_NOT_FOUND:
            return "Host not found. ";
            break;
        case WSATRY_AGAIN:
            return "Nonauthoritative host not found. ";
            break;
        case WSANO_RECOVERY:
            return "This is a nonrecoverable error. ";
            break;
        case WSANO_DATA:
            return "Valid name, no data record of requested type. ";
            break;
        #ifndef _MSC_VER
        case WSA_INVALID_HANDLE:
            return "Specified event object handle is invalid. ";
            break;
        case WSA_INVALID_PARAMETER:
            return "One or more parameters are invalid. ";
            break;
        case WSA_IO_INCOMPLETE:
            return "Overlapped I/O event object not in signaled state. ";
            break;
        case WSA_IO_PENDING:
            return "Overlapped operations will complete later. ";
            break;
        case WSA_NOT_ENOUGH_MEMORY:
            return "Insufficient memory available. ";
            break;
        case WSA_OPERATION_ABORTED:
            return "Overlapped operation aborted. ";
            break;
        #endif
        case WSASYSCALLFAILURE:
            return "System call failure. ";
            break;



        default: {
            char *t = (char*)malloc(50);
            sprintf(t, "Unknown: %d", wsalasterr);
            return(t);
            break;
        }
    }
}

#endif
