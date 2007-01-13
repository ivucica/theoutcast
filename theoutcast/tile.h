#ifndef __TILE_H
#define __TILE_H

#include <vector>
class Tile {
    public:
        Tile();
        ~Tile();

        void insert(Object *obj);
        void remove(Object *obj);
        void remove(unsigned char stackpos);

        void replace(Object *original, Object *newobject);
        void replace(unsigned char stackpos, Object *newobject);

        Object *getstackpos(unsigned char stackpos);
    private:
        std::vector<Object*> itemlayers[3], creatures;
};

#endif
