#include <windows.h>
const char* SocketErrorDescription() {
    switch (WSAGetLastError()) {
        case WSANOTINITIALISED: 
            return "Successful WSAStartup not yet performed. "; //return("WSANOTINITIALISED");
            break;
        case WSAENETDOWN:
            return "Network is down."; //return("WSAENETDOWN");
            break;
        case WSAEADDRINUSE:
            return "Address already in use. "; //return("WSAEADDRINUSE");
            break;
        case WSAEINTR:
            return "Interrupted function call. "; //return("WSAEINTR");
            break;
        case WSAEINPROGRESS:
            return "Operation now in progress. "; //return("WSAEINPROGRESS");
            break;
        case WSAEALREADY:
            return "Operation already in progress. "; //return("WSAEALREADY");
            break;
        case WSAEADDRNOTAVAIL:
            return "Cannot assign requested address. "; //return("WSAEADDRNOTAVAIL");
            break;
        case WSAEAFNOSUPPORT:
            return "Address family not supported by protocol family. "; //return("WSAEAFNOSUPPORT");
            break;
        case WSAECONNREFUSED:
            return "Connection refused. "; //return("WSAECONNREFUSED");
            break;
        case WSAEFAULT:
            return "Bad address. "; //return("WSAEFAULT");
            break;
        case WSAEINVAL:
            return "Invalid argument. "; //return("WSAEINVAL");
            break;
        case WSAEISCONN:
            return "Socket is already connected. "; //return("WSAEISCONN");
            break;
        case WSAENETUNREACH:
            return "Network is unreachable. "; //return("WSAENETUNREACH");
            break;
        case WSAENOBUFS:
            return "No buffer space available. "; //return("WSAENOBUFS");
            break;
        case WSAENOTSOCK:
            return "Socket operation on nonsocket. ";//return("WSAENOTSOCK");
            break;
        case WSAETIMEDOUT:
            return "Connection timed out. "; //return("WSAETIMEDOUT");
            break;
        case WSAEWOULDBLOCK:
            return "Resource temporarily unavailable. "; //return("WSAEWOULDBLOCK");
            break;
        case WSAEACCES:
            return "Permission denied. "; //return("WSAEACCES");
            break;
        default:
            return("Unknown.");
            break;
    }
}