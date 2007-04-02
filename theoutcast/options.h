#ifndef __OPTIONS_H
#define __OPTIONS_H

#include "database.h"

class Options {
    public:
        Options();
        ~Options();

        void Save();
        void Load();

        bool maptrack;
        bool fullscreen;
};


extern Options options;
#endif
