#include <vector>
#include <stdio.h>
#include <windows.h>
#include <io.h> // filelength
#include "texmgmt.h"
#include "imgfmts.h"
#include "assert.h"
#include "sprfmts.h"
Texture::Texture(std::string fname) {
	this->fname = fname;
	this->imgid = 0;
	std::string extension = fname.substr(fname.length() - 3, 3);

    pikseli = NULL;

	textureid = 0;

	if (extension=="bmp" || extension=="BMP") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr" || extension=="SPR") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
        this->StorePixels();
	}
}
Texture::Texture(std::string fname, unsigned short id) {
	int w, h;
	this->fname = fname;
	this->imgid = id;
	pikseli=NULL;
	std::string extension = fname.substr(fname.length() - 3, 3);

	textureid = 0;

	if (extension=="bmp" || extension=="BMP") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr" || extension=="SPR") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
        this->StorePixels();
	}
}

RGBA *Texture::FetchBMPPixels() {
    RGBA *pikseli;
	FILE *f = fopen(fname.c_str(), "rb");
    if (!f) {
        printf("Error opening bitmap %s.", fname.c_str());
        return NULL;
    }
    if (!LoadBitmapFromFile2RGBA(f, (int*)&w, (int*)&h, &pikseli)) {
        printf("Error loading bitmap %s.\n", fname.c_str());
        fclose(f);
        return NULL;
    }
    fclose(f);
    return pikseli;
}

RGBA *Texture::FetchSPRPixels() {

    //ASSERT(this->imgid != 0);
    //printf("Reading %d from %s (%d total sprites)\n", this->imgid, fname.c_str(), SPRCount);
    ASSERT(this->imgid < SPRCount);
    ASSERT(SPRPointers);


    if (imgid == 0) {
        RGBA *rgba = (RGBA*)malloc(32*32*4);
        memset(rgba, 0, 32*32*4);
        w=32; h=32;

        return rgba;

    }

    FILE *f = fopen(fname.c_str(), "rb");
    if (!f) {
        printf("Error opening sprite file %s.", fname.c_str());
        return NULL;
    }

    ASSERT(imgid)

    //printf("Seeking to %d (spr count %d, sprites should begin at %d) \n", SPRPointers[imgid], SPRCount, 4 * SPRCount + 6);
    if (!(SPRPointers[imgid] >= 4 * SPRCount + 6 && SPRPointers[imgid])) {
        fclose(f);
        return NULL;
    }
    if (SPRPointers[imgid]>filelength(fileno(f))) {
            printf("SIZE DOUBLEPLUSUNGOOD\n");
            //system("pause");
        fclose(f);
        return NULL;
    }


    fseek(f, SPRPointers[imgid], SEEK_SET);


    fgetc(f);fgetc(f);fgetc(f); // what do these do?
    unsigned short size;
    fread(&size, 2, 1, f);



    if (!size) {

        fclose(f);
        return NULL;
    }





    RGBA *rgba = (RGBA*)malloc(32*32*4);
    memset(rgba, 0, 32*32*4);
    bool transparent = true;
    int destination = 0;
/*
    unsigned char dontbeginwithtransparence = fgetc(f);
    if (dontbeginwithtransparence == 0xFF) {
        transparent = false;
    } else {
        fseek(f, -1, SEEK_CUR);
    }
*/
    for (int initialftell = ftell(f); ftell(f) < initialftell + size-1; ) {
        unsigned short pixelchunksize;
        fread(&pixelchunksize, 2, 1, f);
        if ( pixelchunksize>1024) {
            printf("PIXELCHUNKSIZE invalid for sprite beginning at %d, imgid %d\n", initialftell, imgid);
            //system("pause");

            fclose(f);
            free(rgba);
            return NULL;

        }
        //printf("%s pixel chunk size: %d\n", transparent ? "Transparent" :  "Solid", pixelchunksize);
        //printf("At position %d, reading until %d\n", ftell(f), initialftell + size - 1 );
        if (transparent) {
            for (int i = 0; i < pixelchunksize; ++i) {
                rgba[i+destination].r = 0;
                rgba[i+destination].g = 0;
                rgba[i+destination].b = 0;
                rgba[i+destination].a = 0;
            }
        } else {
            for (int i = 0; i < pixelchunksize; ++i) {
                ASSERT( i + destination < 32 * 32 )
                fread(&rgba[i+destination], 3, 1, f);
//                printf("Reading 3 bytes to pixel %d - ftell is %d\n", i+destination, ftell(f));
                rgba[i+destination].a = 255;
            }
        }
        destination += pixelchunksize;
        transparent = !transparent;
        //printf("after read At position %d, reading until %d\n", ftell(f), initialftell + size - 1 );
    }
    w=32; h=32;
    fclose(f);

    return (RGBA*)rgba;

}
Texture::~Texture() {
    glDeleteTextures(1, &textureid);
}

void Texture::StorePixels() {
   // glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureid);
    if (!textureid) {
        //printf("ERROR GENERATING TEXTURE SPACE (perhaps wrong thread?)\n");
        //system("pause");
        return;
    }
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, textureid);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w,
                    h, GL_RGBA, GL_UNSIGNED_BYTE, pikseli /*pImage->data*/);
    bool simpletextures = false; // to fool him until simpletextures is truly implemented
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR);
    free(pikseli);
    pikseli = NULL;
}

void Texture::Bind() {
	//printf("Binding texture %s\n", fname.c_str());

	if (!textureid) {
	    if (!pikseli) {
            //printf("WOAH! Dude, texture %s (%d) not boundable!\n", fname.c_str(), imgid);
            //system("pause");
	    } else {
	        StorePixels();
	        if (pikseli) {
	            printf("Serious texturing problem, dude!\n");
	            system("pause");
	            free(pikseli);
	            pikseli = NULL;
	        }
	        if (!textureid) {
	            printf("WTF!! TExture sjhit\n");
	            system("pause");
	        }
	    }
	}
	glBindTexture(GL_TEXTURE_2D, textureid);
}

