#include "options.h"

Options options;
Options::Options() {
    maptrack = 0;
    fullscreen = 0;
	intro = 1;
	os_cursor = 0;
	skin = "default";
}

Options::~Options() {
}

void Options::Save() {
    dbExecPrintf(dbUser, 0, 0, 0, "begin transaction;");

    // bools
	dbSaveSetting("maptrack", maptrack ? "1" : "0");
    dbSaveSetting("fullscreen", fullscreen ? "1" : "0");
	dbSaveSetting("intro", intro ? "1" : "0");
	dbSaveSetting("os_cursor", os_cursor ? "1" : "0");

	// strings
	dbSaveSetting("skin", skin.c_str());

    dbExecPrintf(dbUser, 0, 0, 0, "end transaction;");
}
void Options::Load() {
    char tmp[256];

	// bools
    dbLoadSetting("maptrack", tmp, 256, "0");
    if (tmp[0]=='1') maptrack = true; else maptrack = false;

    dbLoadSetting("fullscreen", tmp, 256, "0");
    if (tmp[0]=='1') fullscreen = true; else fullscreen = false;

	dbLoadSetting("intro", tmp, 256, "1");
    if (tmp[0]=='1') intro = true; else intro = false;

	dbLoadSetting("os_cursor", tmp, 256, "0");
    if (tmp[0]=='1') os_cursor = true; else os_cursor = false;

	// strings
	dbLoadSetting("skin", tmp, 256, "default");
    skin = tmp;

}
