#ifndef __ITEM_H
#define __ITEM_H

#include "thing.h"
#include "types.h"
class Item : public Thing {
    public:
        Item();
        ~Item();

        void Render();
        void Render(const position_t *pos);
    private:

};

#endif // __ITEM_H
