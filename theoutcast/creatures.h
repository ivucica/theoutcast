#ifndef __CREATURES_H
#define __CREATURES_H

#include "types.h"
void CreaturesLoad();
void CreaturesLoad_NoUI(unsigned int protocolversion);
void CreaturesUnload();
extern creature_t **creatures;
extern int creatures_n;


#endif
