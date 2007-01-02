#include <stdio.h>

#ifdef WIN32
    #include <windows.h>
#else
	#include <sys/socket.h>
#endif
#include "networkdirect.h"

#define DO_SEND(s, value) send(s, (char*)&value, sizeof(value), 0);

void SendU8(SOCKET s, unsigned char value) {
    DO_SEND(s, value)
    printf("%02x\n", value);
}

void SendU16(SOCKET s, unsigned short value) {
    DO_SEND(s, value)
    printf("%04x\n", value);
}

void SendU32(SOCKET s, unsigned long value) {
    DO_SEND(s, value)
    printf("%08x\n", value);
}
