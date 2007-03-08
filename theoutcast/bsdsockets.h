#ifndef __BSDSOCKETS_H
#define __BSDSOCKETS_H

#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <netdb.h>
	#define SOCKADDR sockaddr
	#define INVALID_SOCKET -1
	#define closesocket close
	#define SOCKET_ERROR -1
#endif

#endif
