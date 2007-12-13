#ifndef __PROTOCOL81_H
#define __PROTOCOL81_H

#include "protocol81.h"
#include "defines.h"
class Protocol81 : public Protocol79 {
    public:
        Protocol81() {
            this->protocolversion = 810;
            fingerprints[FINGERPRINT_TIBIADAT] = 0x475D3747;
            fingerprints[FINGERPRINT_TIBIASPR] = 0x475D0B01;
            fingerprints[FINGERPRINT_TIBIAPIC] = 0x4742FCD7;
            }
        ~Protocol81() {}


};

#endif

