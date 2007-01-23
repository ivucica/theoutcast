#ifndef __SOUND_H
#define __SOUND_H

bool SoundInit(char *errmsg);
bool SoundPlay(const char* filename);
bool SoundSetMusic(const char* filename);
void SoundSetMusicVolume(unsigned char newvalue);
#endif
