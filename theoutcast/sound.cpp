
#include <stdio.h>
#include <string.h>

#include <map>
#include <string>
#ifdef USESOUNDS
	#include <fmod/fmod.h>
#endif
#include "sound.h"

#define MUSICCHANNEL 0
#define INF 90000
#define MAXSOUNDS 15
bool soundworks=false;

#ifdef USESOUNDS
FSOUND_STREAM* soundmusic = NULL;

std::map<std::string, FSOUND_SAMPLE*> soundcache;
#endif

bool SoundInit(char *errorbuf) {
#ifndef USESOUNDS
	return false;
#else
    if (soundworks) {
        printf("Sound system already started\n");
        if (errorbuf) strcpy(errorbuf, "Sound system already started");
        return false;
    }
    FSOUND_SetBufferSize(75); // 75 ms buffer
    FSOUND_SetDriver(FSOUND_OUTPUT_WINMM);
    if (!FSOUND_Init(32000, 64, 0)) {
        printf("Sound system init error\n");
        if (errorbuf) strcpy(errorbuf, "Sound system init error\n");
        return false;
    }

    soundworks = true;
    return true;
#endif
}

bool SoundPlay(const char* filename) {
#ifndef USESOUNDS
	return false;
#else
    FSOUND_SAMPLE *s=NULL;
    std::map<std::string, FSOUND_SAMPLE*>::iterator it;

    if ((it = soundcache.find(filename)) == soundcache.end()) {
        // FIXME cache purging method should be made more straightforward
        if (soundcache.size() > MAXSOUNDS) {
            FSOUND_Sample_Free(soundcache.begin()->second);
            soundcache.erase(soundcache.begin());
        }
        s = FSOUND_Sample_Load(FSOUND_FREE, filename, 0, 0, 0);
        if (s)
            soundcache[filename] = s;
        else
            printf("Error loading sound\n");
    } else {
        s = it->second;
    }
    if (s) {
        FSOUND_SetVolumeAbsolute(FSOUND_FREE, 192);
        FSOUND_PlaySound(FSOUND_FREE, s);
    }
#endif
}

bool SoundSetMusic(const char* filename) {

    if (!soundworks) {
        printf("Sound doesnt work, cant set bg music\n");
        return false;
    }

#ifndef USESOUNDS
	return false;
#else

    if (soundmusic) FSOUND_Stream_Close(soundmusic);
    if (!filename) return true;
    soundmusic=FSOUND_Stream_Open(filename,FSOUND_LOOP_NORMAL, 0, 0);
    if (!soundmusic) return false;

    FSOUND_SetVolumeAbsolute(MUSICCHANNEL, 128);
    FSOUND_Stream_SetLoopCount(soundmusic,INF);
    FSOUND_Stream_Play (MUSICCHANNEL,soundmusic);
    return true;
#endif
}

void SoundSetMusicVolume(unsigned char newvalue) {
#ifdef USESOUNDS
    FSOUND_SetVolumeAbsolute(MUSICCHANNEL, newvalue);
    //printf("Sound set to %d\n", newvalue);
#endif
}
