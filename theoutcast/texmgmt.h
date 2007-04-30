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
		RGBA *FetchSPRPixels(unsigned int imgid); // FIXME should be unsigned short imgid
		RGBA* ColorizeCreature(RGBA *pixels, RGBA *templatepixels, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet);
		void StorePixels();

		Texture* Find(); // finds another texture with its own properties (fname and imgid)
		bool UnloadGL(); // unloads the texture from opengl context.

		void AssureLoadedness();

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

		friend class Skin;
		friend void TextureFreeSlot();
};

void TextureInit();
void TextureDeinit();
#endif
