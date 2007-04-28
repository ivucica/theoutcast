#ifndef __ITEMS_H
#define __ITEMS_H

#include "types.h"
void ItemsLoad();
void ItemsLoad_NoUI(unsigned int protocolversion);
void ItemsUnload();
extern item_t **items;
extern int items_n;

#endif
