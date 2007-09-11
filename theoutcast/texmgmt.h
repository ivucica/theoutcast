#ifndef __TEXMGMT_H
#define __TEXMGMT_H

#include <string>
#include <stdlib.h> // so glut.h works
#include <GL/glut.h>

#include "imgfmts.h"


class Skin;
class Texture {
	public:
		Texture (std::string file);
		Texture(std::string fname, unsigned short id);
		Texture(std::string fname, unsigned short id, unsigned short templateid, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet);
		~Texture();
		void Bind ();

		RGBA *FetchBMPPixels();
		RGBA *FetchPNGPixels();
		RGBA *FetchSPRPixels(unsigned int imgid); // FIXME should be unsigned short imgid
		RGBA* ColorizeCreature(RGBA *pixels, RGBA *templatepixels, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet);
		void StorePixels();

		Texture* Find(); // finds another texture with its own properties (fname and imgid)
		bool UnloadGL(); // unloads the texture from opengl context.

		void ReplaceOriginalInstance();
		void AssureLoadedness();

		int GetChecksum();
		bool *loaded;
	private:

		// input
		std::string fname;
		unsigned int imgid; // id inside the picture file itself ; for example, in tibia's spr file format, the id of sprite, or in still unsupported gif format's animated variant, the frame

		// contents
		unsigned long w,h;
		RGBA *pikseli;
		GLuint *textureid;

		int* usecount;

        bool intexlist;

        int checksum;

		friend class Skin;
		friend void TextureFreeSlot();
		friend void TextureReportRemaining();
		friend void TextureExpungeRemaining();
		friend bool TextureIntegrityTest_internal (std::string);

};

void TextureInit();
void TextureDeinit();
void TextureReportRemaining();
void TextureExpungeRemaining();
bool TextureIntegrityTest_internal (std::string);

#ifdef _MSC_VER
	#define __PRETTY_FUNCTION__ "[[msvc compiler does not provide __PRETTY_FUNCTION__]]"
#endif
#define TextureIntegrityTest() TextureIntegrityTest_internal(__PRETTY_FUNCTION__)
#endif
