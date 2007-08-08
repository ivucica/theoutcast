#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <string>
#include "database.h"

class Options {
    public:
        Options();
        ~Options();

        void Save();
        void Load();

        bool maptrack;
        bool fullscreen;
		bool intro;
		bool os_cursor;
		bool sounds;
		std::string skin;

};


extern Options options;
#endif
