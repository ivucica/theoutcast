
#include <stdio.h>
#include <string.h>

#include <map>
#include <string>
#if defined(USESOUNDS) && defined(WIN32)
	#include <FMOD/fmod.h>
#endif
#include "options.h"
#include "sound.h"

#define MUSICCHANNEL 0
#define INF 90000
#define MAXSOUNDS 15
bool soundworks=false;

#if defined(USESOUNDS) && defined(WIN32)
FSOUND_STREAM* soundmusic = NULL;

std::map<std::string, FSOUND_SAMPLE*> soundcache;
#endif

bool SoundInit(char *errorbuf) {
#ifndef USESOUNDS
	return false;
#elif defined(WIN32)
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
#else
    soundworks = true;
    return true;
#endif
}

bool SoundPlay(const char* filename) {
	if (!options.sound) return false;
#ifndef USESOUNDS
	return false;
#elif defined(WIN32)
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
#else
	if (!options.sound) return false;
    std::string command = std::string("/bin/sh -c \"play ") + filename + " 2> /dev/null\" & ";
    system(command.c_str());
#endif
}

bool SoundSetMusic(const char* filename) {

    if (!soundworks) {
        printf("Sound doesnt work, cant set bg music\n");
        return false;
    }

#ifndef USESOUNDS
	return false;
#elif defined(WIN32) // windows music

    if (soundmusic) FSOUND_Stream_Close(soundmusic);
    if (!filename) return true;
    if (options.sound) {
    	soundmusic=FSOUND_Stream_Open(filename,FSOUND_LOOP_NORMAL, 0, 0);
		if (!soundmusic) return false;

		//FSOUND_SetVolumeAbsolute(MUSICCHANNEL, 128);
		FSOUND_Stream_SetLoopCount(soundmusic,INF);
		FSOUND_Stream_Play (MUSICCHANNEL,soundmusic);
    }
    return true;
#else // linux music
    return true;
#endif
}

void SoundSetMusicVolume(unsigned char newvalue) {
#if defined(USESOUNDS) && defined(WIN32)
    FSOUND_SetVolumeAbsolute(MUSICCHANNEL, newvalue);
    //printf("Sound set to %d\n", newvalue);
#endif
}
