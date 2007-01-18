#ifndef __SIMPLE_EFFECTS_H
#define __SIMPLE_EFFECTS_H

void FlagEffect(float beginx, float beginy, float endx, float endy, int divx, int divy, float anglebegin, float anglediff, float strength );
void StillEffect(float beginx, float beginy, float endx, float endy, int divx, int divy);


void FlagEffect(float beginx, float beginy, float endx, float endy, int divx, int divy, float anglebegin, float anglediff, float strength, bool flipx, bool flipy );
void StillEffect(float beginx, float beginy, float endx, float endy, int divx, int divy, bool flipx, bool flipy);
#endif
