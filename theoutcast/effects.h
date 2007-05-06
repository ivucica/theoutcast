#ifndef __EFFECTS_H
#define __EFFECTS_H

#include "types.h"
void EffectsLoad();
void EffectsLoad_NoUI(unsigned int protocolversion);
void EffectsUnload();
extern effect_t **effects;
extern int effects_n;


#endif
