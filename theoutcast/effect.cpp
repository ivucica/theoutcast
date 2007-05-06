#include "effect.h"
#include "tile.h"
Effect::Effect(Tile *parent) {
    this->parent = parent;
}

Effect::~Effect() {
}

bool Effect::AnimationAdvance(float advance) {
    float oldanimationpercent = this->animationpercent;
    printf("Advancing effect animation - %g\n", animationpercent);

    if (this->animationpercent != 100.) Thing::AnimationAdvance(advance);
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
        parent->Remove(this);
        return false;
    }
    return true;
}

void Effect::SetType(unsigned short outfit, void* extra) {
    sprgfx = new ObjSpr(outfit, 2);
}

void Effect::Render(position_t *pos) {
    printf("Painting effect\n");
    /*glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINE_LOOP);
        glColor4f(1.,0.,0.,1.);
        glVertex2f(0,0);
        glVertex2f(32 * animationpercent / 100.,0);
        glVertex2f(32 * animationpercent / 100.,32 * animationpercent / 100.);
        glVertex2f(0,32 * animationpercent / 100.);
        glColor4f(1.,1.,1.,1.);
        glEnd();
    glEnable(GL_TEXTURE_2D);*/
    Thing::Render();
}
