#ifndef __NETWORKMESSAGE_H
#define __NETWORKMESSAGE_H

#ifdef WIN32
	#include <windows.h>
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

		void Dump(SOCKET s);
		void AddString(const char* str);
		void AddChar(char chr);
		void AddU8(unsigned char chr);
		void AddU16(unsigned short num);
		void AddU32(unsigned long num);
		void AddItemID(itemid_t id);
		int  FillFromBuffer (Buffer *buf);
		void FillFromSocket (SOCKET s);

        void RSABegin();
		void RSAEncrypt();
		void XTEADecrypt(unsigned long* k);

		unsigned char GetU8();
		unsigned short GetU16 ();
		unsigned long GetU32 ();
		char GetChar ();
		char* GetString (char* target, unsigned int maxsize);
		std::string NetworkMessage::GetString ();

    private:
        unsigned int rsaoffset;
};

#endif
