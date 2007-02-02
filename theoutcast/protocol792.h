#ifndef __PROTOCOL792_H
#define __PROTOCOL792_H

#include "protocol79.h"
#include "defines.h"
class Protocol792 : public Protocol79 {
    public:
        Protocol792() {
            this->protocolversion = 792;
            fingerprints[FINGERPRINT_TIBIADAT] = 0x459E7B73;
            fingerprints[FINGERPRINT_TIBIASPR] = 0x45880FE8;
            fingerprints[FINGERPRINT_TIBIAPIC] = 0x45670923;

            }
        ~Protocol792() {}


};

#endif
