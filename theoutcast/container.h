#ifndef __CONTAINER_H
#define __CONTAINER_H

#include <GLICT/window.h>
#include <GLICT/panel.h>
#include "thing.h"


class Container {
    public:
        Container(std::string title, unsigned char containerid, unsigned short icon, unsigned char capacity);
        ~Container();

        void Insert(Thing *t);
        Thing* GetItem(unsigned char pos);

        glictWindow* GetWindow();
        unsigned short GetContainerID();
    private:
        glictWindow window;
        glictPanel panel;
        glictPanel **objects;
        glictButton close;

        unsigned int capacity;
        unsigned char containerid;
        unsigned short icon;

        std::vector<Thing*> contents;
};

typedef struct {
    Container *container;
    unsigned int elementid;
} containeritemdata_t;

void Container_SlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void Container_SlotsOnClick(glictPos* pos, glictContainer* caller);
void Container_CloseOnClick(glictPos* pos, glictContainer* caller);
#endif
