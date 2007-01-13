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
	RGBA *pikseli=NULL;
	std::string extension = fname.substr(fname.length() - 3, 3);

	textureid = 0;

	if (extension=="bmp") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
        this->StorePixels(pikseli);
	}
}
Texture::Texture(std::string fname, unsigned short id) {
	int w, h;
	this->fname = fname;
	this->imgid = id;
	RGBA *pikseli=NULL;
	std::string extension = fname.substr(fname.length() - 3, 3);

	textureid = 0;

	if (extension=="bmp") {
        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
        this->StorePixels(pikseli);
	}
}

RGBA *Texture::FetchBMPPixels() {
    RGBA *pikseli;
	FILE *f = fopen(fname.c_str(), "r");
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

    ASSERT(this->imgid != 0);
    ASSERT(this->imgid < SPRCount);
    ASSERT(SPRPointers);

    FILE *f = fopen(fname.c_str(), "r");
    if (!f) {
        printf("Error opening sprite file %s.", fname.c_str());
        return NULL;
    }
    fseek(f, SPRPointers[imgid], SEEK_SET);


    // FIXME study the following code and rewrite it // this has been copypasted :/ // it also crashes!


    fgetc(f);fgetc(f);fgetc(f); // what do these do?
    unsigned short size;
    fread(&size, 2, 1, f);
    unsigned char *dump = (unsigned char*)malloc(size);

    unsigned long i, state, pos;
	unsigned short npix;

	unsigned char *rgb = (unsigned char*)malloc(32*32*4);
	memset(rgb,0x11,32*32*4);

	state=0;
	pos=0;
	for(i=0; i < size;)
	{
		memcpy(&npix, dump+i, 2); // number of pixels (transparent or defined)
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

    w = 32; h = 32;

    fclose(f);

    free(dump);

    return (RGBA*)rgb;

}
Texture::~Texture() {
    glDeleteTextures(1, &textureid);
}

void Texture::StorePixels(RGBA *pikseli) {
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

