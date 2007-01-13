#include <stdio.h>
#include "sprfmts.h"

unsigned short *SPRPointers;
unsigned short SPRCount;
std::string SPRFile;
bool SPRLoader(std::string sprfile) { // loads only spr pointers
    unsigned long signature;
    FILE *fp;

    SPRPointers = NULL;
    SPRCount = 0;
    SPRFile = "";

    fp = fopen(sprfile.c_str(), "rb");
    if (!fp) {
        printf("Error opening sprite file\n", sprfile.c_str());
        return false;
    }
    SPRFile = sprfile;


    fread(&signature, 4, 1, fp);
    fread(&SPRCount, 2, 1, fp);
    SPRPointers = (unsigned short*)malloc(sizeof(SPRPointers[0]) * SPRCount);
    for (int i = 0 ; i < SPRCount ; i++) {
        fread(&SPRPointers[i], 2, 1, fp);
    }

    fclose(fp);
    return true;
}
bool SPRUnloader() {
    free(SPRPointers);
    SPRCount = 0;

}
