#include <stdio.h>
#include "sprfmts.h"

unsigned long *SPRPointers=NULL;
unsigned short SPRCount;
std::string SPRFile;
bool SPRLoader(std::string sprfile) { // loads only spr pointers
    unsigned long signature;
    FILE *fp;

    if (SPRPointers) {
        printf("WARNING: Unclean reloading of sprites. Free them regular way!\n");
        SPRUnloader();
    }
    SPRPointers = NULL;
    SPRCount = 0;
    SPRFile = "";

    fp = fopen(sprfile.c_str(), "rb");
    if (!fp) {
        printf("Error opening sprite file\n", sprfile.c_str());
        return false;
    }
    printf("OPENING SPRITE FILE %s\n", sprfile.c_str());
    //system("pause");
    SPRFile = sprfile;

    fread(&signature, 4, 1, fp);
    fread(&SPRCount, 2, 1, fp);
    SPRPointers = (unsigned long*)malloc(sizeof(unsigned long) * (SPRCount+1));
    for (int i = 1 ; i < SPRCount+1 ; i++) {
        fread(&SPRPointers[i], 4, 1, fp);
        //printf("%d\n", SPRPointers[i]);
        //if (i==2) {printf("%d\n", SPRPointers[i]); system("pause"); }
    }
    //system("pause");

    fclose(fp);
    return true;
}
bool SPRUnloader() {

    if (SPRPointers) free(SPRPointers);
    SPRPointers = NULL;
    SPRCount = 0;

}
