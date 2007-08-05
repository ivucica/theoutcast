#ifndef __PROTOCOL80_H
#define __PROTOCOL80_H

#include "protocol80.h"
#include "defines.h"
class Protocol80 : public Protocol79 {
    public:
        Protocol80() {
            this->protocolversion = 800;
            fingerprints[FINGERPRINT_TIBIADAT] = 0x467FD7E6;
            fingerprints[FINGERPRINT_TIBIASPR] = 0x467F9E74;
            fingerprints[FINGERPRINT_TIBIAPIC] = 0x45670923;

            }
        ~Protocol80() {}


};

#endif

