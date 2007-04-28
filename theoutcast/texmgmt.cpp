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
#include "threads.h"
#include "colors.h"
extern version_t glversion;


static std::vector<Texture*> textures;
int texcount=0;
static ONCriticalSection texturethreadsafe;



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

void TextureInit() {
    ONInitThreadSafe(texturethreadsafe);
}
void TextureDeinit() {
    ONDeinitThreadSafe(texturethreadsafe);
}


Texture::Texture(std::string fname) {
//	if (texcount > 50) TextureFreeSlot();

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s\n", fname.c_str());

    ONThreadSafe(texturethreadsafe);
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
		ONThreadUnsafe(texturethreadsafe);
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
        	pikseli = this->FetchSPRPixels(imgid);
	}
	if (pikseli) {
	        this->StorePixels();
	}

	textures.insert(textures.end(), this);
	ONThreadUnsafe(texturethreadsafe);
}
Texture::Texture(std::string fname, unsigned short id) {

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s[%d]\n", fname.c_str(), id);
    ONThreadSafe(texturethreadsafe);
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
		ONThreadUnsafe(texturethreadsafe);
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
        	pikseli = this->FetchSPRPixels(imgid);
	}
	if (pikseli) {
	        this->StorePixels();
	}

	textures.insert(textures.end(), this);
	ONThreadUnsafe(texturethreadsafe);
}

Texture::Texture(std::string fname, unsigned short id, unsigned short templateid, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s[%d/%d]\n", fname.c_str(), id, templateid);
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_WARNING, "Not implemented\n");
    //return;

    ONThreadSafe(texturethreadsafe);
	int w, h;
	this->fname = fname;
	this->imgid = id;


	textureid = NULL;

	this->loaded = (bool*)malloc(sizeof(bool));
	*(this->loaded) = false;
    this->usecount = (int*)malloc(sizeof(int));
    *(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(GLuint));
	*(this->textureid) = 0;



    //printf("ID %d TEMPLATE ID %d\n", id, templateid);
    //system("pause");
    pikseli = this->FetchSPRPixels(id);
    if (id) {
        RGBA *templatepikseli = this->FetchSPRPixels(templateid);
        pikseli = this->ColorizeCreature(pikseli, templatepikseli, head, body, legs, feet);
        free(templatepikseli);
    }


    ONThreadUnsafe(texturethreadsafe);
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

    printf("Loaded bitmap %s\n", fname.c_str());
    return pikseli;
}

RGBA *Texture::FetchSPRPixels(unsigned int imgid) {

    //ASSERT(this->imgid != 0);
    //printf("Reading %d from %s (%d total sprites)\n", this->imgid, fname.c_str(), SPRCount);
    ASSERT(imgid < SPRCount);
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
    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destroying %s\n", fname.c_str());
    ONThreadSafe(texturethreadsafe);
    if (*(usecount)==1) {
		printf("Unloading texture %d\n", *textureid);
//        bool success = false;
		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
			if ((*it)->usecount == this->usecount && (*it)->fname == this->fname) { // if that's the same texture
				textures.erase(it);
//				success = true;
				break;
			}
		}
//		ASSERTFRIENDLY(success, "Did not find the texture inside texture list");

    	if (pikseli) free(pikseli);
		if (loaded) {
			if (textureid) {
			    glDeleteTextures(1, textureid);
                texcount --;
			} else {
			    printf("WEIRD! Texture id is 0, altho' I am loaded!?\n");
            }
			*loaded = false;
			*textureid = 0;
		}
		free(loaded);
		free(usecount);

    }
    (*usecount)--;
    ONThreadUnsafe(texturethreadsafe);
}
void Texture::StorePixels() {
    // glEnable(GL_TEXTURE_2D);

    /*if (texcount > 150) {
        for (int i = 0; i < 50; i++)
    	    TextureFreeSlot();
    }*/

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Storing texture %s\n", fname.c_str());

    ASSERT(textureid);

    glGenTextures(1, textureid);



    //ASSERT(*textureid);
    if (!(*textureid)) {
//        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "ERROR GENERATING TEXTURE SPACE (perhaps wrong thread?)\n");

        //system("pause");
        return;
        //exit(1);
    }
//    printf("Great success\n");

    texcount ++;

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Generated texture space for %s -- %d\n", fname.c_str(), *textureid);

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, *textureid);


    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Bound texture %s\n", fname.c_str());

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

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Parameters set up, storing image of size %dx%d\n", w, h);


    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w,
                    h, GL_RGBA, GL_UNSIGNED_BYTE, pikseli /*pImage->data*/);

//    free(pikseli);
//    pikseli = NULL;

    *loaded = true;

    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Storing texture %s COMPLETE\n", fname.c_str());
}

bool Texture::UnloadGL() {

//	printf("Texture::UnloadGL(): %s\n", *loaded ? "texture loaded" : "texture not loaded");
//	ONThreadSafe(texturethreadsafe);
    if (*loaded) {
        glDeleteTextures(1, textureid);
        *textureid = 0;
        texcount --;
        *loaded = false;
//        ONThreadUnsafe(texturethreadsafe);
        return true;
	} else {
		//printf("Texture already unloaded, skipping\n");
	//	ONThreadUnsafe(texturethreadsafe);
		return false;
	}

}

void Texture::AssureLoadedness() {

    ASSERTFRIENDLY(this, "Texture::AssureLoadedness(): Failed to find 'this'");
    ASSERTFRIENDLY(textureid, "Texture::AssureLoadedness(): I thought we had a texture id malloc()'ed. But it appears not so.");

    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Checking up on %s\n", fname.c_str());

	if (!(*textureid)) {
	    if (!pikseli) {
            //printf("WOAH! Dude, texture %s (%d) not boundable!\n", fname.c_str(), imgid);
            //system("pause");
	    } else {
	    	printf("Re-storing pixels for %s\n", fname.c_str());
	    	ONThreadSafe(texturethreadsafe);
	        StorePixels();
	        ONThreadUnsafe(texturethreadsafe);

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
	//ONThreadSafe(texturethreadsafe);
	AssureLoadedness();

	/*for (std::vector<Texture*>::reverse_iterator it = textures.rbegin()+1; it != textures.rend() ; it++) {
		if (*it == this) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Moving texture %s...\n", (*it)->fname.c_str());
			textures.erase(it.base());
			textures.insert(textures.end(), this);
			break;
		}
	}*/

//	glEnable(GL_TEXTURE_2D);
	//printf("Binding %s\n", fname.c_str());
	if (textureid && *textureid && glIsTexture(*textureid)) glBindTexture(GL_TEXTURE_2D, *textureid); else {
	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "We had a major binding problem with %s\n", fname.c_str());
	}
	//ONThreadUnsafe(texturethreadsafe);
}

Texture* Texture::Find() {
	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
		if (*it) if ((*it)->imgid == this->imgid && (*it)->fname == this->fname) {
			return *it;
		}
	}
	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Requested texture twin of mine not found \n");
	return NULL;
}


RGBA* Texture::ColorizeCreature(RGBA *pixels, RGBA *templatepixels, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    // STUB
    //DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_WARNING, "Texture::ColorizeCreature: Stub\n");
    for (int i=0; i < 32; i++) {
        for (int j = 0; j < 32; j++) if (templatepixels[i * 32 + j].a) {
            if  (templatepixels[i * 32 + j].r && // Yellow == head
                 templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    pixels[i * 32 + j].r *= colors[head * 3 + 0] / 255.f;
                    pixels[i * 32 + j].g *= colors[head * 3 + 1] / 255.f;
                    pixels[i * 32 + j].b *= colors[head * 3 + 2] / 255.f;
                }

            if  (templatepixels[i * 32 + j].r && // Red == body
                !templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    pixels[i * 32 + j].r *= colors[body * 3 + 0] / 255.f;
                    pixels[i * 32 + j].g *= colors[body * 3 + 1] / 255.f;
                    pixels[i * 32 + j].b *= colors[body * 3 + 2] / 255.f;
                }


            if (!templatepixels[i * 32 + j].r && // Green == legs
                 templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    pixels[i * 32 + j].r *= colors[legs * 3 + 0] / 255.f;
                    pixels[i * 32 + j].g *= colors[legs * 3 + 1] / 255.f;
                    pixels[i * 32 + j].b *= colors[legs * 3 + 2] / 255.f;
                }

            if (!templatepixels[i * 32 + j].r && // Blue == feet
                !templatepixels[i * 32 + j].g &&
                 templatepixels[i * 32 + j].b) {
                    pixels[i * 32 + j].r *= colors[feet * 3 + 0] / 255.f;
                    pixels[i * 32 + j].g *= colors[feet * 3 + 1] / 255.f;
                    pixels[i * 32 + j].b *= colors[feet * 3 + 2] / 255.f;
                }



            //printf("*");
        } //else printf(" ");
        //printf("\n");
    }
    return pixels;
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
		if (t->UnloadGL()) {

		    return;
        } else {
//			printf("Already unloaded, retrying\n");
			it++;
			if (it != textures.end()) goto retry;
		}
	}
    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "No texture to unload!");
}
