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
		~Texture();
		void Bind ();

		RGBA *FetchBMPPixels();
		RGBA *FetchSPRPixels();
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

#endif
