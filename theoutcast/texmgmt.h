#ifndef __TEXMGMT_H
#define __TEXMGMT_H

#include <string>
#include <stdlib.h> // so glut.h works
#include <GL/glut.h>
class Texture {
	public:
		Texture (std::string file);
		~Texture();
		void Bind ();
	private:
		GLuint textureid;
		std::string fname;
};

#endif
