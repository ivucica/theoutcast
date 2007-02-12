#include <malloc.h>
#include <string.h> // memcpy
#include "buffer.h"

Buffer::Buffer() {
    ONInitThreadSafe(cs);
    buffer = (char*)malloc(0);
    currentposition = buffer;
    size = 0;
}
Buffer::~Buffer() {
    ONDeinitThreadSafe(cs);
    free(buffer);
}

unsigned int Buffer::GetSize() {
    return size - (currentposition - buffer);
}
unsigned int Buffer::Read(char* dest, unsigned int buflen) {
    ONThreadSafe(cs);
    buflen = _Peek(dest, buflen);
    currentposition += buflen;
    ONThreadUnsafe(cs);
    return buflen;
}
unsigned int Buffer::Peek(char* dest, unsigned int buflen) {
    ONThreadSafe(cs);
    buflen = _Peek(dest, buflen);
    ONThreadUnsafe(cs);
    return buflen;
}
unsigned int Buffer::_Peek(char* dest, unsigned int buflen) {
    // _Peek assumes that it is called within criticalsection cs
	if (buflen>size - (currentposition - buffer)) buflen = size - (unsigned int)(currentposition - buffer);
	if (size <= (unsigned int)(currentposition - buffer)) return 0;
    if (dest) memcpy(dest, currentposition, buflen);
    return buflen;
}
#include <stdio.h>
void Buffer::Trim(unsigned int buflen) {
    ONThreadSafe(cs);
    printf("Before: %02x\n", *currentposition);
    currentposition += buflen;
    printf("After: %02x\n", *currentposition);
    ONThreadUnsafe(cs);
}
void Buffer::Flush() {
    ONThreadSafe(cs);
    _Flush();
    buffer = (char*)realloc(buffer, size);
    currentposition = buffer;
    ONThreadUnsafe(cs);
}
void Buffer::_Flush() {
    // _Flush assumes that it is called within criticalsection cs
    unsigned int a;
    memmove(buffer, currentposition, size-(a = (unsigned int)(currentposition - buffer)));
	currentposition = buffer;
    size -= a;
}
void Buffer::Add(const char* src, unsigned int buflen) {

	if (buflen==-1) return;
	ONThreadSafe(cs);
    _Flush();
    unsigned int offset = (unsigned int)(currentposition - buffer);
    buffer = (char*)realloc(buffer, size + buflen);
    memcpy(buffer+size, src, buflen);
    currentposition = buffer + offset;
    size += buflen;
    ONThreadUnsafe(cs);
}
void Buffer::Clean() {
    ONThreadSafe(cs);
    size = 0;
    free(buffer);
    buffer = (char*)malloc(0);
    currentposition = buffer;
    ONThreadUnsafe(cs);
}
