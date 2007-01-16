#include <vector>
#include <stdio.h>
#include <windows.h>
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

	if (extension=="bmp") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
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

	if (extension=="bmp") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
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
    ASSERT(this->imgid < SPRCount);
    ASSERT(SPRPointers);

    FILE *f = fopen(fname.c_str(), "rb");
    if (!f) {
        printf("Error opening sprite file %s.", fname.c_str());
        return NULL;
    }
    printf("Seeking to %d (spr count %d, sprites should begin at %d) \n", SPRPointers[imgid], SPRCount, 4 * SPRCount + 6);
    if (!(SPRPointers[imgid] >= 4 * SPRCount + 6 && SPRPointers[imgid])) {
        fclose(f);
        return NULL;
    }


    fseek(f, SPRPointers[imgid], SEEK_SET);


    // FIXME study the following code and rewrite it // this has been copypasted :/ // it also crashes!


    fgetc(f);fgetc(f);fgetc(f); // what do these do?
    unsigned short size;
    fread(&size, 2, 1, f);



    if (!size) {

        fclose(f);
        return NULL;
    }





    RGBA *rgba = (RGBA*)malloc(32*32*4);
    bool transparent = true;
    int destination = 0;
    for (int initialftell = ftell(f); ftell(f) < initialftell + size-1; ) {
        unsigned short pixelchunksize;
        fread(&pixelchunksize, 2, 1, f);
        if (pixelchunksize>1024) {
            fclose(f);
            free(rgba);
            return NULL;

        }
        printf("%s pixel chunk size: %d\n", transparent ? "Transparent" :  "Solid", pixelchunksize);
        printf("At position %d, reading until %d\n", ftell(f), initialftell + size - 1 );
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
                printf("Reading 3 bytes to pixel %d - ftell is %d\n", i+destination, ftell(f));
                rgba[i+destination].a = 255;
            }
        }
        destination += pixelchunksize;
        transparent = !transparent;
        printf("after read At position %d, reading until %d\n", ftell(f), initialftell + size - 1 );
    }
    w=32; h=32;
    fclose(f);
    /*

    unsigned char *dump = (unsigned char*)malloc(size);
    fread(dump, size, 1, f);


    unsigned long i, state, pos;
	unsigned short npix;

	unsigned char *rgb = (unsigned char*)malloc(32*32*3);
	memset(rgb,0x11,32*32*3);



	state=0;
	pos=0;
	for(i=0; i < size;)
	{
		memcpy(&npix, dump+i, 2); // number of pixels (transparent or defined)
		printf("%d %s pixels\n", npix, state ? "transparent" : "solid");
		//system("pause");
		i += 2;

		switch(state)
		{
		default: // state 0, drawing transparent pixels
			state=1;
			break;
		case 1: // state 1, drawing defined pixels
			//ASSERT(pos*3 < 32*32*3);
			memcpy(rgb+(pos*3), dump+i, npix*3);
			i += npix*3;
			state=0;
			break;
		}
		pos += npix;
	}


    unsigned char* rgba = (unsigned char*)malloc(32*32*4);


    for(int i=0; i<32;i++) {
        for(int j=0; j < 32;j++) {
            rgba[i*32*4+j*4] = rgb[(32-i-1)*32*3+j*3];//red
            rgba[i*32*4+j*4+1] = rgb[(32-i-1)*32*3+j*3+1];//green
            rgba[i*32*4+j*4+2] = rgb[(32-i-1)*32*3+j*3+2]; //blue
            rgba[i*32*4+j*4+3] = (rgba[i*32*4+j*4]==0x11 && rgba[i*32*4+j*4+1]==0x11 && rgba[i*32*4+j*4+2]==0x11) ? 0 : 255;
        }
    }


    w = 32; h = 32;

    fclose(f);

    free(dump);
    free(rgb);

    */

    return (RGBA*)rgba;

}
Texture::~Texture() {
    glDeleteTextures(1, &textureid);
}

void Texture::StorePixels() {
    glGenTextures(1, &textureid);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, textureid);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w,
                    h, GL_RGBA, GL_UNSIGNED_BYTE, pikseli /*pImage->data*/);
    bool simpletextures = false; // to fool him until simpletextures is truly implemented
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR);
    free(pikseli);
}

void Texture::Bind() {
	//printf("Binding texture %s\n", fname.c_str());
	glBindTexture(GL_TEXTURE_2D, textureid);
}

