#ifndef __PROTOCOL77_H
#define __PROTOCOL77_H

#include "protocol.h"

class Protocol77 : public Protocol {
    public:
        Protocol77();
        ~Protocol77();


        bool CharlistLogin(const char *username, const char *password);

};

#endif
