#ifndef __OBJ3DS_H
#define __OBJ3DS_H

#ifdef HAVE_LIB3DS
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/node.h>
#include <lib3ds/vector.h>
#endif

#include "object.h"

class Obj3ds : public Object {
    public:
        Obj3ds();
        Obj3ds(const char* filename);
        ~Obj3ds();

        bool Render();

        // obj3ds specifics
        bool LoadFile(const char* filename);
#ifdef HAVE_LIB3DS
        bool Loaded() const {if (!data3ds) return false; else return true;}
#else
        bool Loaded() const { return false; }
#endif
    private:
#ifdef HAVE_LIB3DS
        // obj3ds specifics
        void RenderNode (Lib3dsNode *node);
        Lib3dsFile *data3ds;
#endif


};

#endif


