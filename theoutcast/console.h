#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <string>
#include <vector>
#include "colors.h"
#include "types.h"
#include "threads.h"
struct consoleentry {
    std::string text;
    consolecolors_t color;
} ;




class Console {
    private:
        std::vector <consoleentry> con;
        ONCriticalSection threadsafe;
    public:
        Console();
        ~Console();

        const std::string operator [](int id);
        void insert(std::string txt); // inserts default yellow text
        void insert(std::string txt, consolecolors_t col); // inserts anycolor text

        void insert(std::string txt, bool debug);
        void insert(std::string txt, consolecolors_t col, bool debug);

        void draw(char count);
        void clear();
};
extern Console console;

#endif
