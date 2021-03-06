

/******************************************************
 **** THE CAUSES OF THE GREAT CRASHES *****************


 When the texture object is constructed, first thing we do is attempt to find
 a previous instance of the equivalent texture in the texture list. If we do
 not find it, then we insert "this" instance into the texture list. Later, when
 we wish to dispose of the specified texture, we blindly do a delete of the object,
 but a reference to the specified object remains in the texture list! When we try
 to access it through the texture list for various reasons (including checksum)
 we get an invalid reference to the texture.

 What we must do is maintain a separate list of ALL texture objects, not just the
 "original instances", and once we discover that we have delete'd an "original instance",
 we need to replace the reference to it with an "unoriginal instance", which
 will take over that role.

 **/




#ifdef WIN32
	#include <windows.h>
	#include <io.h> // filelength
#endif
#include <vector>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "texmgmt.h"
#include "imgfmts.h"
#include "assert.h"
#include "console.h" // REMOVEME
#ifdef TEXTURE_SPR
    #include "sprfmts.h"
    #include "colors.h"

    #define FIXCOLORS(type){pixels[i * 32 + j].r = (unsigned char)(pixels[i * 32 + j].r * (colors[type * 3 + 0] / 255.f)); \
                            pixels[i * 32 + j].g = (unsigned char)(pixels[i * 32 + j].g * (colors[type * 3 + 1] / 255.f)); \
                            pixels[i * 32 + j].b = (unsigned char)(pixels[i * 32 + j].b * (colors[type * 3 + 2] / 255.f));}

#endif

#ifdef DETECT_GLVERSION
    #include "types.h"
    extern version_t glversion;
#endif

#ifdef DEBUGLEVEL_BUILDTIME
    #include "debugprint.h"
#else
    static void DEBUGPRINT(...) {}
    #define DEBUGPRINT_LEVEL_DEBUGGING 0
    #define DEBUGPRINT_LEVEL_OBLIGATORY 0
    #define DEBUGPRINT_LEVEL_JUNK 0
    #define DEBUGPRINT_LEVEL_USEFUL 0
    #define DEBUGPRINT_NORMAL 0
    #define DEBUGPRINT_ERROR 0
    #define DEBUGPRINT_WARNING 0
#endif

#ifdef TEXTURE_THREADS
    #include "threads.h"
    static ONCriticalSection texturethreadsafe;
#else
    #define ONInitThreadSafe(x)
    #define ONDeinitThreadSafe(x)
    #define ONThreadSafe(x)
    #define ONThreadUnsafe(x)
#endif



static std::vector<Texture*> textures, textures_all;

int texcount=0;



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
//    punobajtova[0] = 1;
}
void TextureDeinit() {
    ONDeinitThreadSafe(texturethreadsafe);
}

int Texture::GetChecksum() {
	int cs=0;
	for (unsigned int i=0; i<fname.size();i++) {
		cs+=fname[i];
	}
	cs += imgid;
	return cs;
}

Texture::Texture(std::string fname) {
//	if (texcount > 50) TextureFreeSlot();

//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s\n", fname.c_str());


    ONThreadSafe(texturethreadsafe);

	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
	this->fname = fname;
	this->imgid = 0;
	checksum = GetChecksum();
	textureid = NULL;
	textures_all.insert(textures_all.end(), this);
	Texture* t = this->Find();

	if (t) {

		this->textureid = t->textureid;
		this->pikseli = t->pikseli;
		this->w = t->w;
		this->h = t->h;
		this->loaded = t->loaded;
		this->usecount = t->usecount;
		(*this->usecount)++;
		intexlist = true;
		ONThreadUnsafe(texturethreadsafe);
		return;
	}


	this->loaded = (bool*)malloc(sizeof(bool));
	*(this->loaded) = false;
	this->usecount = (int*)malloc(sizeof(int));
	*(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(textureid));
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
	if (extension=="png" || extension=="PNG") {
	        pikseli = this->FetchPNGPixels();
	}
	if (pikseli) {
	        //this->StorePixels();
	}

    this->intexlist = true;

    textures.insert(textures.end(), this);
    ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
	ONThreadUnsafe(texturethreadsafe);
}
Texture::Texture(std::string fname, unsigned short id) {

//    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s[%d]\n", fname.c_str(), id);
    ONThreadSafe(texturethreadsafe);
    ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
	int w, h;
	this->fname = fname;
	this->imgid = id;
	checksum = GetChecksum();
	textureid = NULL;
	textures_all.insert(textures_all.end(), this);
	Texture* t = id ? this->Find() : NULL;
	if (t) {
		this->textureid = t->textureid;
 		this->pikseli = t->pikseli;
		this->w = t->w;
		this->h = t->h;
		this->loaded = t->loaded;
		this->usecount = t->usecount;
		if (!this->usecount) {
		    TextureReportRemaining();
		}
		ASSERT(this->usecount);
		(*this->usecount)++;
		intexlist = true;

		ONThreadUnsafe(texturethreadsafe);
		return;
	}

	this->loaded = (bool*)malloc(sizeof(bool));
	ASSERT(loaded);
	*(this->loaded) = false;
    this->usecount = (int*)malloc(sizeof(int));
    ASSERT(usecount);
    *(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(textureid));
	ASSERT(textureid);
	*(this->textureid) = 0;

	pikseli=NULL;
	std::string extension = fname.substr(fname.length() - 3, 3);


	if (extension=="bmp" || extension=="BMP") {
	        pikseli = this->FetchBMPPixels();
	}
	if (extension=="spr" || extension=="SPR") { // quasi-filetype to allow loading of SPR file format which we think of as a kind of "archive" with tons of pictures
        	pikseli = this->FetchSPRPixels(imgid);
	}
	if (extension=="png" || extension=="PNG") {
	        pikseli = this->FetchPNGPixels();
	}
	if (pikseli) {
	        //this->StorePixels();
	}

	this->intexlist = true;

	textures.insert(textures.end(), this);
	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
	ONThreadUnsafe(texturethreadsafe);

}

Texture::Texture(std::string fname, unsigned short id, unsigned short templateid, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Loading %s[%d/%d]\n", fname.c_str(), id, templateid);
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_WARNING, "Not implemented\n");
    //return;

    ONThreadSafe(texturethreadsafe);
	int w, h;
	this->fname = fname;
	this->imgid = id;
	checksum = GetChecksum();
	textureid = NULL;
	textures_all.insert(textures_all.end(), this);

	this->loaded = (bool*)malloc(sizeof(bool));
	*(this->loaded) = false;
    this->usecount = (int*)malloc(sizeof(int));
    *(this->usecount) = 1;
	this->textureid = (GLuint*)malloc(sizeof(textureid));
	*(this->textureid) = 0;


    pikseli = this->FetchSPRPixels(id);
    if (id) {
        RGBA *templatepikseli = this->FetchSPRPixels(templateid);
        pikseli = this->ColorizeCreature(pikseli, templatepikseli, head, body, legs, feet);
        free(templatepikseli);
    }

    this->intexlist = false;
	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
    ONThreadUnsafe(texturethreadsafe);

}

void Texture::ReplaceOriginalInstance() {
	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
		if ((*it) == this) {
			for (std::vector<Texture*>::iterator it2 = textures_all.begin(); it2 != textures_all.end() ; it2++ ) {
				if ((*it2)->fname == this->fname &&  (*it2)->imgid == this->imgid && (*it2) != this) {
					printf("REPLACED.\n");
					(*it) = (*it2);
					return;
				}
			}
		}
	}
}
Texture::~Texture() {
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Destroying %s\n", fname.c_str());
//    #if DEBUGLEVEL_BUILDTIME == 0
    //return;
//    #endif
	//return;

	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
    ONThreadSafe(texturethreadsafe);
	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
	ASSERTFRIENDLY(*(this->usecount) < 500, "Usecount corruption");
    ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");


	for (std::vector<Texture*>::iterator it = textures_all.begin(); it != textures_all.end() ; it++ ) {
		if ((*it) == this) {
			textures_all.erase(it);
			break;
		}
	}

    ReplaceOriginalInstance();



    if ((*usecount)==1) {
		DEBUGPRINT( DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Unloading texture %d -- %s[%d] ; total textures remaining %d\n", *textureid, this->fname.c_str(), this->imgid, textures.size());
        bool success = false;
		if (intexlist) {
            for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
                //printf("%s[%d] ", (*it)->fname.c_str(), (*it)->imgid);
                if ((*it)->imgid == this->imgid && (*it)->fname == fname  ) { // if that's the same texture
                    DEBUGPRINT( DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Removed %s[%d] from texture list\n", (*it)->fname.c_str(), (*it)->imgid);
                    textures.erase(it);
                    success = true;
                    //system("pause");
                    //TextureReportRemaining();

                    break;
                } /*else {
                    printf("%c%c...", (*it)->imgid == this->imgid ? '!' : 'x', (*it)->fname == this->fname ? '!' : 'x');
                }*/
            }
		}
        else // intexlist
            success = true; // it was never meant to be in texlist, so force success=true

		{
		char tmp[800];
		sprintf(tmp, "Did not find the texture %s[%d] inside texture list\n", this->fname.c_str(), this->imgid);

		if (!success) {
		    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, tmp);
		    TextureReportRemaining();
		}
		ASSERTFRIENDLY(success, tmp);


		}

    	if (pikseli) {
    	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "It was allocated, thus, freeing.\n");
    	    free(pikseli);
    	} else {
    	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_ERROR, "It was not allocated, not freeing.");
    	}


		if (loaded && (*loaded)) {
			if (textureid && *textureid) {
			    if (glIsTexture(*(this->textureid))) {
			    	texcount --;


                    glDeleteTextures(1, this->textureid);


			    } else {
			        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Cannot unload - %d is not a texture\n", *textureid);
			        //system("sleep 2");
			    }
			} else {
			    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "WEIRD! Texture id is NULL or *(texture id) is 0, altho' I am loaded!?\n");
			    ASSERT(this->textureid && (*this->textureid));
            }

			(*loaded) = false;
			(*this->textureid) = 0;
		} else {
			// this is perfectly acceptable if object was created and then removed before fade was completed
		    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Texture was never loaded, not unloading in any way");
		    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "%s %s", loaded ? "not null": "NULL!", usecount ? "not null" : "NULL");
		    //system("sleep 2");
		}


		free(loaded);
		free(usecount);
        free(this->textureid);

    } else {

        (*usecount)--;
    }


    ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity check failed");
    ONThreadUnsafe(texturethreadsafe);
	ASSERTFRIENDLY(TextureIntegrityTest(), "Texture integrity test failed");
}


RGBA *Texture::FetchBMPPixels() {
    RGBA *pikseli;
	FILE *f = fopen(fname.c_str(), "rb");

    if (!f) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Error opening bitmap %s.", fname.c_str());
        return NULL;
    }
    if (!LoadBitmapFromFile2RGBA(f, (int*)&w, (int*)&h, &pikseli)) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Error loading bitmap %s.\n", fname.c_str());
        fclose(f);
        return NULL;
    }
    fclose(f);

    //printf("Loaded bitmap %s\n", fname.c_str());
    return pikseli;
}


RGBA *Texture::FetchPNGPixels() {
    #ifndef PNGSUPPORT
    return NULL;
    #else
    RGBA *pikseli;
	FILE *f = fopen(fname.c_str(), "rb");

    if (!f) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Error opening png %s.", fname.c_str());
        return NULL;
    }
    if (!LoadPNGFromFile2RGBA(f, (unsigned int*)&w, (unsigned int*)&h, &pikseli)) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Error loading png %s.\n", fname.c_str());
        fclose(f);
        return NULL;
    }
    fclose(f);

    //printf("Loaded bitmap %s\n", fname.c_str());
    return pikseli;
    #endif
}


RGBA *Texture::FetchSPRPixels(unsigned int imgid) {
    #ifndef TEXTURE_SPR
    return NULL;
    #else
    //ASSERT(this->imgid != 0);
    //printf("Reading %d from %s (%d total sprites)\n", this->imgid, fname.c_str(), SPRCount);
//    printf("Image %d out of %d\n", imgid, SPRCount);
    ASSERT(imgid < SPRCount);
    ASSERT(SPRPointers);


#if 0
{
        RGBA *rgba = (RGBA*)malloc(32*32*4);
        memset(rgba, imgid, 32*32*4);
        w=32; h=32;

        return rgba;
}
#endif

    if (imgid == 0)
	{
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

    /*printf("%02x ", fgetc(f));
    printf("%02x ", fgetc(f));
    printf("%02x\n", fgetc(f));// what do these do?*/
    fgetc(f); fgetc(f); fgetc(f);

    unsigned short size;
    fread(&size, 2, 1, f);
    //printf("%d\n", size);

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
        /*    for (int i = 0; i < pixelchunksize; ++i) {
                rgba[i+destination].r = 0;
                rgba[i+destination].g = 0;
                rgba[i+destination].b = 0;
                rgba[i+destination].a = 0;
            }
            */ // already 0,0,0,0 ...
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
    #endif
}


void Texture::StorePixels() {
    // glEnable(GL_TEXTURE_2D);

    /*if (texcount > 400) {
        for (int i = 0; i < 25; i++)
    	    TextureFreeSlot();
    }*/

    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Storing texture %s\n", fname.c_str());

    if (!pikseli) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_WARNING, "No pixels to store\n");
        return;
    }

    ASSERT(textureid);

    glGenTextures(1, textureid);


    if (!(*textureid)) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "ERROR GENERATING TEXTURE SPACE (perhaps wrong thread?)\n");

        return;
    }
    texcount ++;

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, *textureid);


    bool simpletextures = false; // to fool him until simpletextures is truly implemented

    if ((/*fname.substr(fname.length() - 3, 3)=="spr" || */ (fname.substr(0, 6)=="skins/"  &&  fname.substr(fname.length() - 6, 6)!="bg.bmp" ) )) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        #ifdef DETECT_GLVERSION
        if (glversion.major == 1 && glversion.minor >= 2 || glversion.major >= 2) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
        #endif
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        #ifdef DETECT_GLVERSION
        }
        #endif

    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, simpletextures ? GL_NEAREST : GL_LINEAR);
    }

    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Parameters set up, storing image of size %dx%d\n", w, h);


    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, // gl_rgba in this line used to be "4"
                    h, GL_RGBA, GL_UNSIGNED_BYTE, pikseli /*pImage->data*/);

//    free(pikseli);
//    pikseli = NULL;

    *loaded = true;

    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Storing texture %s COMPLETE\n", fname.c_str());
}

bool Texture::UnloadGL() {

//	printf("Texture::UnloadGL(): %s\n", *loaded ? "texture loaded" : "texture not loaded");
//	ONThreadSafe(texturethreadsafe);
    if (textureid && *loaded) {
        if (glIsTexture(*textureid)) texcount --; else {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR,"Cannot unload\n");
        }
        glDeleteTextures(1, textureid);
        *textureid = 0;
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
    {
    ASSERTFRIENDLY(this, "Texture::AssureLoadedness(): Failed to find 'this'");
    char tmp[512];
    sprintf(tmp, "Texture::AssureLoadedness(): I thought we had a texture id malloc()'ed. But it appears not so.\n%s %d", fname.c_str(), imgid);
    ASSERTFRIENDLY(textureid, tmp);
	sprintf(tmp, "Texture::AssureLoadedness(): Texture checksum failed\n%s %d", fname.c_str(), imgid);
	ASSERTFRIENDLY(checksum == GetChecksum(), tmp);
    }

    if (!textureid) return;
	if (!(*textureid) || !glIsTexture(*textureid)) {
	    if (!pikseli) {
            //printf("WOAH! Dude, texture %s (%d) not boundable!\n", fname.c_str(), imgid);
            //system("pause");
	    } else {
	    	//printf("Re-storing pixels for %s\n", fname.c_str());
	    	ONThreadSafe(texturethreadsafe);
	        StorePixels();
	        ONThreadUnsafe(texturethreadsafe);

	        /*if (pikseli) {
	            printf("Serious texturing problem, dude!\n");
	            system("pause");
	            free(pikseli);
	            pikseli = NULL;
	        }*/
            //printf("Stored to %d\n", *textureid);
	        if (!(*textureid)) {
	            printf("WTF!! TExture shit\n");
	            system("pause");
	        }
	    }
	}
}
void Texture::Bind() {
	ONThreadSafe(texturethreadsafe);
	AssureLoadedness();

/*
	for (std::vector<Texture*>::reverse_iterator it = textures.rbegin()+1; it != textures.rend() ; it++) {
		if (*it == this) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Moving texture %s...\n", (*it)->fname.c_str());
			textures.erase(it.base());
			textures.insert(textures.begin(), this);
			break;
		}
	}
*/
//	glEnable(GL_TEXTURE_2D);
	//printf("Binding %s\n", fname.c_str());
	if (textureid && *textureid && glIsTexture(*textureid))
        glBindTexture(GL_TEXTURE_2D, *textureid);
    else {
	    DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "We had a binding problem with %s - %s %s %s\n", fname.c_str(),

	    textureid ? "yes" : "no",
	    *textureid ? "yes" : "no",
	    glIsTexture(*textureid) ? "yes" : "no"
	    );
	}
	ONThreadUnsafe(texturethreadsafe);
}

Texture* Texture::Find() {
    //#if DEBUGLEVEL_BUILDTIME == 0
    //return NULL;
    //#endif

	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
//	    printf("%s %d finding %s %d\n", (*it)->fname.c_str(),  (*it)->imgid, this->fname.c_str(),  this->imgid);
		if (*it) if ((*it)->imgid == this->imgid && (*it)->fname == this->fname) {
//			printf("*******************************Found duplicate (%s[%d])**********************\n", fname.c_str(), imgid);
			return *it;
		}
	}
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Requested texture twin of mine not found \n");
	return NULL;
}


RGBA* Texture::ColorizeCreature(RGBA *pixels, RGBA *templatepixels, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {

    #ifndef TEXTURE_SPR
    return pixels;
    #else
    for (int i=0; i < 32; i++) {
        for (int j = 0; j < 32; j++) if (templatepixels[i * 32 + j].a) {
            if  (templatepixels[i * 32 + j].r && // Yellow == head
                 templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    FIXCOLORS(head)
                }

            if  (templatepixels[i * 32 + j].r && // Red == body
                !templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    FIXCOLORS(body)
                }


            if (!templatepixels[i * 32 + j].r && // Green == legs
                 templatepixels[i * 32 + j].g &&
                !templatepixels[i * 32 + j].b) {
                    FIXCOLORS(legs)
                }

            if (!templatepixels[i * 32 + j].r && // Blue == feet
                !templatepixels[i * 32 + j].g &&
                 templatepixels[i * 32 + j].b) {
                    FIXCOLORS(feet)
                }



            //printf("*");
        } //else printf(" ");
        //printf("\n");
    }
    return pixels;
    #endif
}

void TextureFreeSlot() {
	// FIXME  (Khaos#4#) should track when was the last time texture was used, find the oldest one, etc...

	if (!textures.size()) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "No texture to unload\n");
		return;
	}
	std::vector<Texture*>::iterator it = textures.begin();
retry:
	Texture *t = *it;
	//printf("Proposing to delete %s\n", t->fname.c_str());
	if (t) {
		//printf("Unloading a texture... %s\n", t->fname.c_str() );
		if (t->UnloadGL()) {

		    return;
        }
	}
	//printf("Already unloaded, retrying\n");
    it++;
    if (it != textures.end()) goto retry;
    DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "No texture to unload!");
}

void TextureReportRemaining() {
    static Texture *t;

//    #if DEBUGLEVEL_BUILDTIME == 0
//    return;
//    #endif
	ONThreadSafe(texturethreadsafe);
    for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; it++ ) {
        t = (*it);
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_WARNING,"Remaining %s[%d]=%d x%d", t->fname.c_str(), t->imgid, *(t->textureid), *(t->usecount) );
    }
    ONThreadUnsafe(texturethreadsafe);
}
void TextureExpungeRemaining() {
    static Texture *t;

	ONThreadSafe(texturethreadsafe);
    for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end() ; ) {
        t = (*it);
        delete t;
    }
    ONThreadUnsafe(texturethreadsafe);
}


// FIXME (Khaod#1#) item 459+10 causes integrity failure upon unload
bool TextureIntegrityTest_internal (std::string s) {
//	printf("TextureIntegrityTest(%s) -- checking %d textures\n", s.c_str(), textures_all.size());

	Texture*t;
	ONThreadSafe(texturethreadsafe);
	for (std::vector<Texture*>::iterator it = textures_all.begin(); it != textures_all.end() ; it++ ) {
		t=(*it);
		//DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"%s is testing ", s.c_str());
		//DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL,"%s[%d]=%d x%d\n", t->fname.c_str(), t->imgid, *(t->textureid), *(t->usecount) );
		if (t->checksum != t->GetChecksum()) {
			DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_ERROR ,"TEXTURE INTEGRITY CHECK FAILED! Calculated checksum is %d and stored checksum is %d\n" , t->GetChecksum(), t->checksum);
			ONThreadUnsafe(texturethreadsafe);
			return false;
		}
		if (*(t->usecount) > 500) {
			DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_ERROR ,"TEXTURE INTEGRITY CHECK FAILED! Usecount is way too large - %d\n" , *(t->usecount));
			ONThreadUnsafe(texturethreadsafe);
			return false;
		}
	}
	ONThreadUnsafe(texturethreadsafe);
	return true;
}
