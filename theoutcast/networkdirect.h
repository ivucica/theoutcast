#ifndef __NETWORKDIRECT_H
#define __NETWORKDIRECT_H

#ifdef WIN32
	#include <windows.h>
#else
    #ifndef SOCKET
        typedef int SOCKET;
	#endif
#endif

void SendU8(SOCKET s, unsigned char value);
void SendU16(SOCKET s, unsigned short value);
void SendU32(SOCKET s, unsigned long value);

#endif
