#ifndef __ONBUFFER_H
#define __ONBUFFER_H

#include "threads.h"

class Buffer {
	public:
		Buffer();
		~Buffer();

		unsigned int GetSize();
		unsigned int Read(char* dest, unsigned int buflen);
		unsigned int Peek(char* dest, unsigned int buflen);
		void Flush();
		void Clean();

		void Trim(unsigned int buflen);
		void Add(const char* src, unsigned int buflen);

	private:
		ONCriticalSection cs;
		char *currentposition;
		unsigned int _Peek(char* dest, unsigned int buflen);
		void _Flush();
	protected:
		char *buffer;
		unsigned int size;

		friend class NetworkMessage;
};

#endif
