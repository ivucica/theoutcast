#ifndef __DISTANCES_H
#define __DISTANCES_H

#include "types.h"
void DistancesLoad();
void DistancesLoad_NoUI(unsigned int protocolversion);
void DistancesUnload();
extern distance_t **distances;
extern int distances_n;


#endif
