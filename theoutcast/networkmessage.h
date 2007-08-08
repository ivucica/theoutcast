#ifndef __NETWORKMESSAGE_H
#define __NETWORKMESSAGE_H

#ifdef WIN32
	#include <windows.h>
	#ifdef __GNU_C
		#include <winsock2.h>
	#endif
#else
    #ifndef SOCKET
        typedef int SOCKET;
	#endif
#endif


#include "buffer.h"
#include "types.h"
class NetworkMessage : public Buffer {
	public:
		NetworkMessage();
		~NetworkMessage();

		bool Dump(SOCKET s);
		void AddString(const char* str);
		void AddString(std::string str);
		void AddChar(char chr);
		void AddU8(unsigned char chr);
		void AddU16(unsigned short num);
		void AddU32(unsigned long num);
		void AddItemID(itemid_t id);
		int  FillFromBuffer (Buffer *buf);
		bool FillFromSocket (SOCKET s);

        void RSABegin();
		void RSAEncrypt();
		void XTEADecrypt(unsigned long* k);
        void XTEAEncrypt(unsigned long* k);

		unsigned char GetU8();
		unsigned short GetU16 ();
		unsigned long GetU32 ();
		unsigned char PeekU8();
		unsigned short PeekU16 ();
		unsigned long PeekU32 ();
		char GetChar ();
		char* GetString (char* target, unsigned int maxsize);
		std::string GetString ();

		void ShowContents();

    private:
        unsigned int rsaoffset;
};

#endif
