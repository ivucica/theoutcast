#ifndef __SPRFMTS_H
#define __SPRFMTS_H

#include <string>

bool SPRLoader(std::string sprfile);
bool SPRUnloader();

extern unsigned long *SPRPointers;
extern unsigned short SPRCount;
extern std::string SPRFile;

#endif
