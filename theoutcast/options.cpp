#include "options.h"

Options options;
Options::Options() {
    maptrack = 0;
    fullscreen = 0;
}

Options::~Options() {
}

void Options::Save() {
    dbExecPrintf(dbUser, 0, 0, 0, "begin transaction;");
    dbSaveSetting("maptrack", maptrack ? "1" : "0");
    dbSaveSetting("fullscreen", fullscreen ? "1" : "0");
    dbExecPrintf(dbUser, 0, 0, 0, "end transaction;");
}
void Options::Load() {
    char tmp[256];

    dbLoadSetting("maptrack", tmp, 256, "0");
    if (tmp[0]=='1') maptrack = true; else maptrack = false;

    dbLoadSetting("fullscreen", tmp, 256, "0");
    if (tmp[0]=='1') fullscreen = true; else fullscreen = false;
}
