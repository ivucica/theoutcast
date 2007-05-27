#include <GLICT/globals.h>
#include <GLICT/fonts.h>
#include "assert.h"
#include "console.h"// FIXME REMOVE ME

#include "protocol.h"
#include "glutwin.h"
#include "container.h"

void GM_Gameworld_ClickExec(position_t *pos, glictEvents evt);

Container::Container(std::string title, unsigned char containerid, unsigned short icon, unsigned char capacity) {
    this->containerid = containerid;
    this->icon = icon;
    this->capacity = capacity;

    window.SetCaption(title);
    window.SetWidth(166);
    window.SetHeight(40 * 2 + 10);
    window.AddObject(&panel);

    panel.SetWidth(166);
    panel.SetHeight(40*2);
    panel.SetBGActiveness(false);
    panel.SetPos(0, 10);
    panel.SetVirtualSize(166, ((capacity / 4) + 1) * 37);

    objects = (glictPanel**)malloc(sizeof(glictPanel*) * capacity);
    printf("Allocating %d\n", sizeof(glictPanel*) * capacity);
    ASSERTFRIENDLY(objects, "Failed to allocate some memory for storing items in container.");
    for (int i = 0 ; i < capacity ; i++ ) {
        glictPanel *p = new glictPanel;
        objects[i] = p;

        containeritemdata_t *c = (containeritemdata_t*)malloc(sizeof(containeritemdata_t));


        c->container = this;
        c->elementid = i;


        panel.AddObject(objects[i]);

        objects[i]->SetWidth(32);
        objects[i]->SetHeight(32);
        objects[i]->SetPos(8+ (i % 4) * 37, 5 + (i / 4) * 37 );
        objects[i]->SetBGColor(.2, .2, .2, 1.);
        objects[i]->SetCustomData(c);
        objects[i]->SetOnPaint(Container_SlotsOnPaint);
        objects[i]->SetOnClick(Container_SlotsOnClick);
        objects[i]->SetOnMouseDown(Container_SlotsOnMouseDown);
        objects[i]->SetOnMouseUp(Container_SlotsOnMouseUp);

    }
    unsigned char *cid = (unsigned char *)malloc(4);
    *cid = containerid;
    close.SetWidth(10);
    close.SetHeight(10);
    close.SetPos(150, 0);
    close.SetCaption("X");
    close.SetOnClick(Container_CloseOnClick);
    close.SetCustomData(cid);
    window.AddObject(&close);
}
Container::~Container() {
    for (int i = 0 ; i < capacity; i++) {
        free(objects[i]->GetCustomData());
        delete objects[i];
    }
    free(objects);

    // cid delete
    // delete only once, although it might be used in many places!
    // (meaning, add UseCount variable)
    free(close.GetCustomData());
}

glictWindow* Container::GetWindow() {
    return &window;
}

Thing* Container::GetItem(unsigned char pos) {
    if (pos >= contents.size()) return NULL;
    return contents[pos];
}
void Container::Insert(Thing *t) {
    Insert(t, false);
}
void Container::Insert(Thing *t, bool begin) {
    contents.insert((begin ? contents.begin() : contents.end()), t);

    RebuildCounts();
}

void Container::Remove(unsigned int pos) {
    std::vector<Thing*>::iterator it = contents.begin();
    it+=pos;
    delete *it;
    contents.erase(it);

    RebuildCounts();
}
void Container::Replace(unsigned int pos, Thing *t) {
    delete contents[pos];
    contents[pos] = t;

    RebuildCounts();
}

void Container::RebuildCounts() {

    Thing *t;
    for (int i = 0 ; i < capacity ; i++ ) {
        containeritemdata_t *c = (containeritemdata_t*)objects[i]->GetCustomData();
        if (c->container->GetItem(c->elementid) && (t = c->container->GetItem(c->elementid))->IsStackable()) {
            char tmp[256];
            sprintf(tmp, "%dx", t->GetCount());
            objects[i]->SetCaption(tmp);
        } else objects[i]->SetCaption("");
    }
}

unsigned short Container::GetContainerID() {
    return containerid;
}

void Container_SlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller) {

    containeritemdata_t *c = (containeritemdata_t*)caller->GetCustomData();

    //char tmp[256];
    //sprintf(tmp, "%d", c->elementid );
    //caller->SetCaption(tmp);

    //printf("%s\n", tmp);
/*
    ((glictPanel*)caller)->SetBGColor(
        0,
        0,
        ((glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots)  / 20. + 0.25,
        1. );
*/


    if (clipped->bottom <= clipped->top) return;
    if (clipped->right <= clipped->left) return;


    glViewport(clipped->left, glictGlobals.h - clipped->bottom, clipped->right - clipped->left, clipped->bottom - clipped->top);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 32, 0, 32, -100, 100);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
		glLoadIdentity();

		//player->RenderInventory((glictPanel*)caller - ((GM_Gameworld*)game)->panInvSlots);
        if (c->container->GetItem(c->elementid)) {
            c->container->GetItem(c->elementid)->Render();
            c->container->GetItem(c->elementid)->AnimationAdvance(100./fps);
        }

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,winw,0,winh, -100, 100);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		glTranslatef(0,-winh,0.0);

		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glViewport(0,0,glictGlobals.w,glictGlobals.h);
}
void Container_SlotsOnClick(glictPos* pos, glictContainer* caller) {
    containeritemdata_t *c = (containeritemdata_t*)caller->GetCustomData();




    int slot = c->elementid;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = c->container->GetContainerID() | 0x40 ;
    pos2.z = slot;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSECLICK);

}
void Container_SlotsOnMouseDown(glictPos* pos, glictContainer* caller) {
    containeritemdata_t *c = (containeritemdata_t*)caller->GetCustomData();


    int slot = c->elementid;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = c->container->GetContainerID() | 0x40 ;
    pos2.z = slot;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEDOWN);

}
void Container_SlotsOnMouseUp(glictPos* pos, glictContainer* caller) {
    containeritemdata_t *c = (containeritemdata_t*)caller->GetCustomData();

    int slot = c->elementid;
    position_t pos2;

    pos2.x = 0xFFFF;
    pos2.y = c->container->GetContainerID() | 0x40 ;
    pos2.z = slot;

    GM_Gameworld_ClickExec(&pos2, GLICT_MOUSEUP);

}

void Container_CloseOnClick(glictPos* pos, glictContainer* caller) {
    unsigned char *c = (unsigned char *)caller->GetCustomData();


    char tmp[256];
    sprintf(tmp, "Demand close %d", *c);
    console.insert(tmp, CONYELLOW);

    protocol->CloseContainer(*c);

}
