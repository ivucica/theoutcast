#include <malloc.h>
#include <string.h>
#ifndef WIN32
	#include <sys/socket.h>
#endif
#ifdef USEENCRYPTION
    #ifdef GMP
        #include <gmp.h>
    #endif
    #ifdef TFM
//        #define FP_SIZE 1024
        extern "C" {
            #include <tfm.h> // tomfastmath ftw
        }
    #endif
    #ifdef LTM
        #include <tommath.h> // on msvc we can't build tomsfastmath :/
    #endif
#endif
#include "assert.h"
#include "protocol.h"
#include "networkmessage.h"
#include "debugprint.h"
#include "types.h"
#include "socketstrings.h"
#include "bsdsockets.h"

#define MTU 1456

#ifndef MAX
	#define MAX(a,b) (a > b ? a : b)
#endif
#ifndef MIN
	#define MIN(a,b) (a < b ? a : b)
#endif


NetworkMessage::NetworkMessage() {}

NetworkMessage::~NetworkMessage() {}

bool NetworkMessage::Dump(SOCKET s)
{
    // FIXME it crashes on a memcpy sometimes
    // reverify!!
    unsigned int sizetosend = GetSize();
    char *tmp;

    if (!(tmp = (char*)malloc(sizetosend+2)))
	{
        DEBUGPRINT(0, 1, "malloc() failed while sending message");
        return false;
    }

    DEBUGPRINT(3, 0, "Dumping %d bytes to connection (%02x)\n", sizetosend, sizetosend);

	*(unsigned short*)tmp = (unsigned short)sizetosend;
    memcpy(tmp+2, currentposition, sizetosend);

    if (send(s, tmp, sizetosend + 2, 0) != sizetosend + 2)
	{
	    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Failed to dump to socket %x -- %s\n", (int)s, SocketErrorDescription());
	    return false;
	}

    free(tmp);

    return true;
}

void NetworkMessage::AddString(const char *str) {
	unsigned short len = (unsigned short)strlen(str);
	this->Add((char*)&len, 2);
	this->Add(str, len);
}
void NetworkMessage::AddString(std::string str) {
	this->AddString(str.c_str());
}

void NetworkMessage::AddChar(char u) {
	this->Add(&u, 1);
}

void NetworkMessage::AddU8(unsigned char u) {
	this->Add((char*)&u, 1);
}

void NetworkMessage::AddU16(unsigned short u) {
	this->Add((char*)&u, 2);
}

void NetworkMessage::AddU32(unsigned long u) {
	this->Add((char*)&u, 4);
}

void NetworkMessage::AddItemID(itemid_t id) {
	/*this->AddU16(id);*/
	this->Add((char*)&id, 2);
}

int NetworkMessage::FillFromBuffer (Buffer *buf) {
	unsigned int sz;
	if (buf->GetSize() >= 2) {
		buf->Peek((char*)&sz, 2);
		if (buf->GetSize() >= sz + 2) {
			this->Add ((buf->buffer) + 2, sz);
			buf->Trim (sz + 2);
			return 1;
		}
	}
	return 0;
}

bool NetworkMessage::FillFromSocket (SOCKET s)
{
	signed int sizereadresult = 0;
	signed int readsofar = 0;
	unsigned short sz;
	char *toadd;

	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Expecting size read on netmsg %x.\n", (int)this);
	sizereadresult = recv(s, (char*)&sz, 2, 0);
	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Read %x bytes.\n", sizereadresult);

	if (sizereadresult != 2)
	{
		if(sizereadresult == SOCKET_ERROR)
		{
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Connection error of some sort on netmsg %x;\n", (int)this);
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "%s\n", SocketErrorDescription());
		}
		else if(sizereadresult)
		{
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Netmsg %x has read %d bytes for size (should be 2)\n", (int)this, sizereadresult);
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "%s\n", SocketErrorDescription());
		}
		else
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Socket %x gracefully closed, nothing we can do about it.\n", (int)s);
	    return false;
	}

	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Filling %d bytes from socket; this msg(%x) has already %d bytes\n", sz, (int)this, GetSize());

	toadd = (char*)malloc(sz);

	while (readsofar < sz)
	{
		signed int readthisturn;
        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Trying to read %d\n", (char)(MIN(sz - readsofar, 0xff)));
        readthisturn = recv(s, toadd + readsofar, MIN(sz-readsofar, MTU), 0);


        if (readthisturn > 0)
			readsofar += readthisturn;
		else if(!readthisturn)
		{
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Socket %x gracefully closed, nothing we can do about it.\n", (int)s);
            free(toadd);
			return false;
        }
		else
		{
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Error reading on netmsg %x! Readthisturn contains %d\n", (int)this, readthisturn);
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "%s\n", SocketErrorDescription());
            free(toadd);
			return false;
		}
	}
	DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Netmsg %x filled.\n", (int)this);

	this->Add(toadd, sz);
	free(toadd);

	return true;
}

unsigned char NetworkMessage::GetU8 () {
	unsigned char ret;
	Read ((char*)&ret, 1);
	return ret;
}

unsigned short NetworkMessage::GetU16 () {
	unsigned short ret;
	Read ((char*)&ret, 2);
	return ret;
}

unsigned long NetworkMessage::GetU32 () {
	unsigned long ret;
	Read ((char*)&ret, 4);
	return ret;
}

unsigned char NetworkMessage::PeekU8 () {
	unsigned char ret;
	Peek ((char*)&ret, 1);
	return ret;
}

unsigned short NetworkMessage::PeekU16 () {
	unsigned short ret;
	Peek ((char*)&ret, 2);
	return ret;
}

unsigned long NetworkMessage::PeekU32 () {
	unsigned long ret;
	Peek ((char*)&ret, 4);
	return ret;
}


char NetworkMessage::GetChar () {
	char ret;
	Read ((char*)&ret, 1);
	return ret;
}

char* NetworkMessage::GetString (char* target, unsigned int maxsize) {
	unsigned int strsize = GetU16();
	unsigned int usdsize;
	if (target == NULL) {
		char *toreturn;

		usdsize = MIN(strsize, this->GetSize());
		toreturn = (char*)malloc(usdsize+1);

		this->Peek(toreturn, usdsize);
		this->Trim(strsize);
		toreturn[usdsize] = 0;

		return toreturn;
	} else {
		usdsize = MIN(MIN(maxsize-1, strsize), this->GetSize());
		this->Peek(target, usdsize);
		this->Trim(strsize);
		target[usdsize] = 0;
		return target;
	}
	return NULL;
}
std::string NetworkMessage::GetString () {
    unsigned short strsize;
	unsigned short usdsize;

    strsize = GetU16();
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "STR SIZE %hd\n", strsize);

    char *toreturn;
    usdsize = MIN(strsize, this->GetSize());
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "USD SIZE %d\n", usdsize);
    toreturn = (char*)malloc(usdsize+1);

    this->Peek(toreturn, usdsize);
    this->Trim(strsize);
    toreturn[usdsize] = 0;
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Getting string: %s, size: %d, usdsize: %d\n", toreturn, strsize, usdsize);

    return toreturn;
}

void NetworkMessage::RSABegin() {
    rsaoffset = size;
}

void NetworkMessage::RSAEncrypt() {
#ifdef USEENCRYPTION
    int rsablocksize = size - rsaoffset;
    unsigned char msg[128] = {0}; // every rsa data block is 128 bytes in size. if we have less data then rest if simply unused, but rsa algorythm always generates 128 byte blocks and it still needs 128 bytes of space
    // first byte in block must be 0, encryption demands it
    //memcpy(msg+1, &rsablocksize, 2); // rsa blocks are NOT prepended with the real size of the message
    memcpy(msg+1, buffer+rsaoffset, rsablocksize); // then we copy our data into the rsa block's data area


    //char* modulus = "";//paste here the modulus
    char *modulus;
    if (protocol->CipSoft()) { // if were logging into one of cip's servers
        //strcpy(modulus, "142996239624163995200701773828988955507954033454661532174705160829347375827760388829672133862046006741453928458538592179906264509724520840657286865659265687630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212883967"); // 7.7
        modulus = ("124710459426827943004376449897985582167801707960697037164044904862948569380850421396904597686953877022394604239428185498284169068581802277612081027966724336319448537811441719076484340922854929273517308661370727105382899118999403808045846444647284499123164879035103627004668521005328367415259939915284902061793"); // 7.72, 7.81, 7.9*
        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "ENCODING WITH CIPSOFT\n");
    } else { // it's an ot
        modulus = ("109120132967399429278860960508995541528237502902798129123468757937266291492576446330739696001110603907230888610072655818825358503429057592827629436413108566029093628212635953836686562675849720620786279431090218017681061521755056710823876476444260558147179707119674283982419152118103759076030616683978566631413");
    }

    // after that, run the rsa encoding algorythm over data area of rsa block
    #ifdef GMP

        mpz_t m_p, m_q, m_u, m_d, m_dp, m_dq;
        mpz_t m_mod, m_e;
        mpz_t m,c;

        //////////////RSA INIT////////////////////////////
        mpz_init2(m_mod, 1024);
        mpz_init2(m_e, 32);




        mpz_set_ui(m_e, 65537); //public exponent
        mpz_set_str(m_mod, modulus, 10);
        /////////////END RSA INIT//////////////////////////

        mpz_init2(m, 1024);
        mpz_init2(c, 1024);

        //size is ignored always copy 128 bytes
        mpz_import(m, 128, 1, 1, 0, 0, msg);

        mpz_powm(c, m, m_e, m_mod);

        size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
        memset(msg, 0, 128 - count);
        mpz_export(&msg[128 - count], NULL, 1, 1, 0, 0, c);

        mpz_clear(m);
        mpz_clear(c);


        //////////////////RSA DEINIT//////////////////////
        mpz_clear(m_mod);
        mpz_clear(m_e);
        ////////////////END RSA DEINIT////////////////////

    #endif


    #ifdef TFM
        fp_int m_mod, m_e, m, c;


        fp_init(&c);
        fp_init(&m);
        fp_init(&m_mod);
        fp_init(&m_e);

        fp_set(&m_e, 65537);
        fp_read_radix(&m_mod, modulus, 10); // read in radix == procitaj u bazi

        fp_read_unsigned_bin(&m, msg, 128);

        fp_exptmod(&m, &m_e, &m_mod, &c); //  c = (m ^ m_e) % m_mod
        fp_to_unsigned_bin(&c, msg);


    #endif

    #ifdef LTM
        mp_int m_mod, m_e, m, c;


        mp_init(&c);
        mp_init(&m);
        mp_init(&m_mod);
        mp_init(&m_e);

        mp_set(&m_e, 65537);
        mp_read_radix(&m_mod, modulus, 10); // read in radix == procitaj u bazi

        mp_read_unsigned_bin(&m, msg, 128);

        mp_exptmod(&m, &m_e, &m_mod, &c); //  c = (m ^ m_e) % m_mod
        mp_to_unsigned_bin(&c, msg);
    #endif

    // buffer of the message should now contain enough space for both the
    // unencoded data from the buffer, as well as the encoded data
    buffer = (char*)realloc(buffer, rsaoffset + 128);
    // then we should copy the encoded data over the old, unencoded version
    // of the now-encoded data
    memcpy(buffer + rsaoffset, msg, 128);
    // finally, updated size of the buffer is always 128, plus the unencoded
    // data from the beginning of the buffer
    size = rsaoffset + 128;
    currentposition = buffer;

    //ShowContents();
#endif
}

void NetworkMessage::XTEADecrypt(unsigned long* m_key) {
#ifdef USEENCRYPTION


  unsigned char *key = (unsigned char*)m_key;
  unsigned long length = GetSize();
  unsigned long delta = 0x9e3779b9;                   /* a key schedule constant */
  unsigned long sum;

  //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "%d %d %d %d\n", m_key[0], m_key[1], m_key[2], m_key[3]);

  int n = 0;

//  DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "WILL DECRYPT THIS STUFF: \n");
//  for (int i=0; i < length; i++) {
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "%02x ", (char)(currentposition[i]));
//  }
//  DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "\n");

  while (n < length)
  {

    sum = 0xC6EF3720;
    unsigned long v0 = *((unsigned long*)(currentposition+n));
    unsigned long v1 = *((unsigned long*)(currentposition+n+4));

    for(int i=0; i<32; i++)
    {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + ((unsigned long*)key)[sum>>11 & 3]);
        sum -= delta;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + ((unsigned long*)key)[sum & 3]);
    }

    *((unsigned long*)(currentposition+n))   = v0;
    *((unsigned long*)(currentposition+n+4)) = v1;

    n += 8;
  }


    ASSERT((*((unsigned short*)currentposition)+2 <= GetSize()))
    ASSERT(currentposition)
	if (*((unsigned short*)currentposition)+2 <= GetSize()) {
//	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Message claims it's %d bytes big\n", *((unsigned short*)currentposition)+2);
        size = size - GetSize() + *((unsigned short*)currentposition)+2;
//        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "New size: %d\n", GetSize());
        //ShowContents();
	}
    else {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "There was a decryption error, for certain! We decrypted more data than received!\nDecrypted message claims we have %d bytes?\n", *((unsigned short*)currentposition)+2);
        ShowContents();
    }
//	_assert(size < 5000);
	Trim(2);
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "now a total of %d bytes\n", size);

#endif




#if 0
	unsigned long k[4];
	k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];

    printf("%u %u %u %u\n", k[0], k[1], k[2], k[3]);
    unsigned long* buffer_l = (unsigned long*)(buffer);
	unsigned long readposition = 0;
	while(readposition < size/4){

		unsigned long v0 = buffer_l[readposition], v1 = buffer_l[readposition + 1];
		/*unsigned long delta = 0x61C88647;//0x9e3779b9;
		unsigned long sum = 0xC6EF3720;


		for(unsigned long i = 0; i<32; i++) {
			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
			sum += delta;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
		}
		*/



		// http://en.wikipedia.org/wiki/XTEA
		unsigned char num_rounds = 32;
		unsigned long delta = 0x9e3779b9;
        unsigned long sum = delta * num_rounds;
        for(int i=0; i<num_rounds; i++) {
            v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
            sum -= delta;
            v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
        }

		buffer_l[readposition] = v0; buffer_l[readposition + 1] = v1;

		readposition += 2;

	}
    ASSERT((*((unsigned short*)buffer)+2 <= size))
	if (*((unsigned short*)buffer)+2 <= size)
        size = *((unsigned short*)buffer)+2;
    else
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "There was a decryption error, for certain! We decrypted more data than received!\nDecrypted message claims we have %d bytes?\n", *((unsigned short*)buffer)+2);

//	_assert(size < 5000);
	Trim(2);
//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "now a total of %d bytes\n", size);
#endif
}



#include "console.h"


void NetworkMessage::XTEAEncrypt(unsigned long* m_key) {
#ifdef USEENCRYPTION
	unsigned long k[4];
	k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];

	int m_ReadPos = 0;

    int diff = currentposition - buffer;
    buffer = (char*)realloc(buffer, size+2 + (8 - ((size+2) % 8)));
    currentposition = buffer + diff;
    memmove(buffer+2,buffer,size);
    buffer[0] = (unsigned char)(size);
	buffer[1] = (unsigned char)(size >> 8);
	size+=2 + (8 - ((size+2) % 8));

	unsigned long read_pos = 0;
	unsigned long* buffer2 = (unsigned long*)buffer;

	while(read_pos < size/4){
		unsigned long v0 = buffer2[read_pos], v1 = buffer2[read_pos + 1];
		unsigned long delta = 0x61C88647;
		unsigned long sum = 0;

		for(unsigned long i = 0; i<32; i++) {
			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum -= delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
		}
		buffer2[read_pos] = v0; buffer2[read_pos + 1] = v1;
		read_pos = read_pos + 2;
	}


#endif
}
void NetworkMessage::ShowContents() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "NetworkMessage::ShowContents() // %d bytes\n", size);
    for (int i = 0; i < GetSize() ; i++) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "0x%02x,", (unsigned char)currentposition[i]);
    }
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "\n");
    return;
}
