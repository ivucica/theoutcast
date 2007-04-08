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

        bool loaded;
	private:
		GLuint textureid;

		std::string fname;
		unsigned int imgid; // id inside the picture file itself ; for example, in tibia's spr file format, the id of sprite, or in still unsupported gif format's animated variant, the frame
		unsigned long w,h;
		RGBA *pikseli;
		friend class Skin;
};

#endif
