#include <stdio.h>
#include "sprfmts.h"

unsigned long *SPRPointers;
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
    SPRPointers = (unsigned long*)malloc(sizeof(SPRPointers[0]) * SPRCount);
    for (int i = 0 ; i < SPRCount ; i++) {
        fread(&SPRPointers[i], 4, 1, fp);
        printf("%d\n", SPRPointers[i]);
        //if (i==2) {printf("%d\n", SPRPointers[i]); system("pause"); }
    }
    //system("pause");

    fclose(fp);
    return true;
}
bool SPRUnloader() {
    free(SPRPointers);
    SPRCount = 0;

}
