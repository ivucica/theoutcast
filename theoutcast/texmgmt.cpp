#ifdef WIN32
	#include <windows.h>
	#include <io.h> // filelength
#endif
#include <vector>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "debugprint.h"
#include "texmgmt.h"
#include "imgfmts.h"
#include "assert.h"
#include "sprfmts.h"
#include "types.h"

extern version_t glversion;


static std::vector<Texture*> textures;
static int texcount=0;
#ifndef WIN32
static int filesize (FILE* f) {
	int loc = ftell(f);
	int size = 0;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, loc, SEEK_SET);
	return size;
}
#else
static int filesize (FILE* f) {
#ifdef _MSC_VER
	return _filelength(_fileno(f));
#else
	return filelength(fileno(f)) ;
#endif
}
#endif

void TextureFreeSlot();
Texture::Texture(std::string fname) {
//	if (texcount > 50) TextureFreeSlot();

	this->fname = fname;
	this->imgid = 0;

	textureid = NULL;
	Texture* t = this->Find();
	if (t) {
		this->textureid = t->textureid;
		this->pikseli = t->pikseli;
		this->w = t->w;
		this->h = t->h;
		this->loaded = t->loaded;
		this->usecount = t->usecount;
		*(this->usecount)++;
		return;
	}

	this->loaded = (bool*)malloc(sizeof(bool));
	*(this->loaded) = false;
	this->usecount = (int*)malloc(sizeof(int));
	*(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(GLuint));
	*(this->textureid) = 0;

	std::string extension = fname.substr(fname.length() - 3, 3);

    	pikseli = NULL;


	//printf("Determining extension: %s\n", extension.c_str());
	if (extension=="bmp" || extension=="BMP") {
	        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr" || extension=="SPR") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        	pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
	        this->StorePixels();
	}

	textures.insert(textures.end(), this);
}
Texture::Texture(std::string fname, unsigned short id) {
//	if (texcount > 50) TextureFreeSlot();

	int w, h;
	this->fname = fname;
	this->imgid = id;

	textureid = NULL;
    Texture* t = this->Find();
	if (t) {
		this->textureid = t->textureid;
 		this->pikseli = t->pikseli;
		this->w = t->w;
		this->h = t->h;
		this->loaded = t->loaded;
		this->usecount = t->usecount;
		*(this->usecount)++;
		return;
	}
	this->loaded = (bool*)malloc(sizeof(bool));
	*(this->loaded) = false;
    this->usecount = (int*)malloc(sizeof(int));
    *(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(GLuint));
	*(this->textureid) = 0;


	pikseli=NULL;
	std::string extension = fname.substr(fname.length() - 3, 3);


	if (extension=="bmp" || extension=="BMP") {
	        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr" || extension=="SPR") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        	pikseli = this->FetchSPRPixels();
	}
	if (pikseli) {
	        this->StorePixels();
	} else printf("Er, idk about pixels.\n");

	textures.insert(textures.end(), this);
}

RGBA *Texture::FetchBMPPixels() {
    RGBA *pikseli;
	FILE *f = fopen(fname.c_str(), "rb");
printf("Loading bitmap %s\n", fname.c_str());
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

printf("Loaded bitmap %s\n", fname.c_str());
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
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Error opening sprite file %s.", fname.c_str());
        return NULL;
    }

    ASSERT(imgid)

    //printf("Seeking to %d (spr count %d, sprites should begin at %d) \n", SPRPointers[imgid], SPRCount, 4 * SPRCount + 6);
    if (!(SPRPointers[imgid] >= 4 * SPRCount + 6 && SPRPointers[imgid])) {
        fclose(f);
        return NULL;
    }
    if (SPRPointers[imgid]>filesize(f)) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SIZE DOUBLEPLUSUNGOOD - pic %d, ptr %d\n", imgid, SPRPointers[imgid]);
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
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "PIXELCHUNKSIZE invalid for sprite beginning at %d, imgid %d\n", initialftell, imgid);
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
printf("Destroying a texture\n");
    if (*(usecount)==1) {
		printf("Unloading texture %d\n", *textureid);

		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
			if ((*it)->usecount == this->usecount) { // if that's the same texture
				textures.erase(it);
				break;
			}
		}

    	if (pikseli) free(pikseli);
		if (loaded) {
			glDeleteTextures(1, textureid);
			texcount --;
			*loaded = false;
			*textureid = 0;
		}
		free(loaded);
		free(usecount);

    }

}
void Texture::StorePixels() {
   // glEnable(GL_TEXTURE_2D);
   texcount ++;
   if (texcount > 50) TextureFreeSlot();
   //printf("Allocating texture number %d\n", texcount);
   ASSERT(textureid);
    glGenTextures(1, textureid);
    if (!(*textureid)) {
        printf("ERROR GENERATING TEXTURE SPACE (perhaps wrong thread?)\n");
        //system("pause");
        //return;
        exit(1);
    }
//    printf("Great success\n");
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, *textureid);

    bool simpletextures = false; // to fool him until simpletextures is truly implemented

    if (fname.substr(fname.length() - 3, 3)=="spr" || fname.substr(0, 6)=="skins/") {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (glversion.major == 1 && glversion.minor >= 2 || glversion.major >= 2) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR);
    }



    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w,
                    h, GL_RGBA, GL_UNSIGNED_BYTE, pikseli /*pImage->data*/);


//    free(pikseli);
//    pikseli = NULL;

    *loaded = true;


}

bool Texture::UnloadGL() {
	//printf("Texture::UnloadGL(): %s\n", *loaded ? "texture loaded" : "texture not loaded\n");
        if (*loaded) {
            glDeleteTextures(1, textureid);
            *textureid = 0;
            texcount --;
      	//	printf("Unloaded texture\n");
		*loaded = false;
		return true;
	} else {
		//printf("Texture already unloaded, skipping\n");
		return false;
	}

}

void Texture::AssureLoadedness() {
    printf("Checking up on %s\n", fname.c_str());
    ASSERTFRIENDLY(textureid, "Texture::AssureLoadedness(): I thought we had a texture id malloc()'ed. But it appears not so.");

	if (!(*textureid)) {
	    if (!pikseli) {
            //printf("WOAH! Dude, texture %s (%d) not boundable!\n", fname.c_str(), imgid);
            //system("pause");
	    } else {
	    	printf("Re-storing pixels for %s\n", fname.c_str());
	        StorePixels();
	        /*if (pikseli) {
	            printf("Serious texturing problem, dude!\n");
	            system("pause");
	            free(pikseli);
	            pikseli = NULL;
	        }*/
		printf("Stored to %d\n", *textureid);
	        if (!(*textureid)) {
	            printf("WTF!! TExture sjhit\n");
	            system("pause");
	        }
	    }
	}
}
void Texture::Bind() {
	AssureLoadedness();
/*
	for (std::vector<Texture*>::reverse_iterator it = textures.rbegin(); it != textures.rend() ; it++) {
		if (*it == this) {
			//printf("Moving texture %s...\n", (*it)->fname.c_str());
			textures.erase(it.base());
			textures.insert(textures.end(), this);
			break;
		}
	}
*/
//	glEnable(GL_TEXTURE_2D);
	//printf("Binding %s\n", fname.c_str());
	if (*textureid) glBindTexture(GL_TEXTURE_2D, *textureid); else printf("We had a major situation here, soldier.\n");
}

Texture* Texture::Find() {
	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
		if ((*it)->fname == this->fname && (*it)->imgid == this->imgid) {
			return *it;
		}
	}
	return NULL;
}

void TextureFreeSlot() {
	// FIXME should track when was the last time texture was used, find the oldest one, etc...
	if (!textures.size()) {
		printf("No texture to unload\n");
		return;
	}
	std::vector<Texture*>::iterator it = textures.begin();
retry:
	Texture *t = *it;
//	printf("Proposing to delete %s\n", t->fname.c_str());
	if (t) {
//		printf("Unloading a texture... %s\n", t->fname.c_str() );
		if (t->UnloadGL()) return; else {
//			printf("Already unloaded, retrying\n");
			it++;
			goto retry;
		}
	}
}
