#include <vector>
#include <stdio.h>
#include <windows.h>
#include "texmgmt.h"
#include "imgfmts.h"
Texture::Texture(std::string fname) {
	this->fname = fname;
	std::string extension = fname.substr(fname.length() - 3, 3);
	RGBA *pikseli=NULL;
	int w, h;
	textureid = 0;
	
	if (extension=="bmp") {
		FILE *f = fopen(fname.c_str(), "r");
		if (!f) {
			printf("Error opening bitmap %s.", fname.c_str());
			return;
		}
		if (!LoadBitmapFromFile2RGBA(f, &w, &h, &pikseli)) {
			printf("Error loading bitmap %s.\n", fname.c_str());
			fclose(f);
			return;
		}
		fclose(f);
	}

	if (pikseli) {
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

}

Texture::~Texture() {
}

void Texture::Bind() {
	//printf("Binding texture %s\n", fname.c_str());
	glBindTexture(GL_TEXTURE_2D, textureid);
}
