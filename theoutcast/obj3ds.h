#ifndef __OBJ3DS_H
#define __OBJ3DS_H

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/node.h>
#include <lib3ds/vector.h>

#include "object.h"

class Obj3ds : public Object {
    public:
        Obj3ds();
        Obj3ds(const char* filename);
        ~Obj3ds();

        bool LoadFile(const char* filename);
        bool Loaded() {if (!data3ds) return false; else return true;}

        bool Render();

    private:
        void RenderNode (Lib3dsNode *node);

        Lib3dsFile *data3ds;
        float animation_frame;

};

#endif
