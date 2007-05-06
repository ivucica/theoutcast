#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <string>
#include <vector>
#include "colors.h"
#include "types.h"
#include "threads.h"
typedef struct {
    char* text;
    consolecolors_t color;
} consoleentry;



typedef std::vector <consoleentry*> consolecontainer;

class Console {
    private:
        consolecontainer con;
        ONCriticalSection threadsafe;
    public:
        Console();
        ~Console();

        const std::string operator [](int id);
        void insert(std::string txt); // inserts default yellow text
        void insert(std::string txt, consolecolors_t col); // inserts anycolor text
        void draw(char count);
        void clear();
};
extern Console console;

#endif
