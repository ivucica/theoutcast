#include <GLICT/fonts.h>
#include "creature.h"
#include "types.h"
Creature::Creature() {
    attacked = false;
    hp = 0;
    name = "unnamed";
}
Creature::~Creature() {
}
void Creature::SetType(unsigned short outfit, void* extra) {
    ONThreadSafe(threadsafe);
    creaturelook_t *crl = (creaturelook_t *)extra;

    this->type = outfit;
    this->creaturelook = *crl;
    if (outfit != 0)
        sprgfx = new ObjSpr(outfit, crl->head, crl->body, crl->legs, crl->feet);
    else
        sprgfx = new ObjSpr(crl->extendedlook , 0);
    ONThreadUnsafe(threadsafe);
}
void Creature::SetCreatureID(unsigned long creatureid) {
    this->id = creatureid;
}
unsigned long Creature::GetCreatureID() {
    return this->id;
}
bool Creature::IsGround() {
    return false;
}
void Creature::SetName(std::string creaturename) {
    this->name = creaturename;
}
std::string Creature::GetName() {
    return this->name;
}
bool Creature::AnimationAdvance(float advance) {
    float oldanimationpercent = this->animationpercent;
    if (moving && this->animationpercent != 100.) Thing::AnimationAdvance(advance);
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
        this->animationpercent = 0;
        if (sprgfx) sprgfx->AnimationSetValue(0);
        if (!preapproved)
            this->moving = false;
        else
            this->animationpercent = 100.;
    }
    return true;
}
void Creature::CauseAnimOffset(bool individual) {   // if we're rendering an individual, default is ok
                                                    // if we're rendering the screen, we need to invert

    switch (direction) {
        case NORTH:
            glTranslatef(0, ((preapproved ? 0 : -32.)  + 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case SOUTH:
            glTranslatef(0, ((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case WEST:
            glTranslatef(((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0, 0);
            break;
        case EAST:
            glTranslatef(((preapproved ? 0 : -32) + 32.*animationpercent/100.) * (individual ? 1. : -1), 0, 0);
            break;

        case NORTHWEST:
            glTranslatef(((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), ((preapproved ? 0 : -32.)  + 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case SOUTHWEST:
            glTranslatef(((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), ((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case NORTHEAST:
            glTranslatef(((preapproved ? 0 : -32) + 32.*animationpercent/100.) * (individual ? 1. : -1), ((preapproved ? 0 : -32.)  + 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
        case SOUTHEAST:
            glTranslatef(((preapproved ? 0 : -32) + 32.*animationpercent/100.) * (individual ? 1. : -1), ((preapproved ? 0 : 32.) - 32.*animationpercent/100.) * (individual ? 1. : -1), 0);
            break;
    }
}
void Creature::Render(position_t *pos) {
	bool wasmoving = false;
    if (moving) {
		wasmoving = true;
        glPushMatrix();
        CauseAnimOffset(true);
    }

    if (attacked) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1.,0.,0.,1.);
        glVertex2f(0,0);
        glVertex2f(32,0);
        glVertex2f(32,32);
        glVertex2f(0,32);
        glColor4f(1.,1.,1.,1.);
        glEnd();
    }
    glTranslatef(-8,8,0);
    Thing::Render(pos);
    glTranslatef(8,-8,0);
    if (wasmoving) {
        glPopMatrix();
    }
}
void Creature::Render() {
    position_t p = {0, 0, 0};
    Render(&p);
}

void Creature::SetAttacked(bool atk) {
    this->attacked = atk;
}

void Creature::SetHP(unsigned char hp) {
    this->hp = hp;
}
unsigned char Creature::GetHP() {
    return hp;
}
bool Creature::IsApproved() {
    return preapproved;
}
void Creature::RenderOverlay() {
	bool wasmoving = false;
    if (moving) {
		wasmoving = true;
        glPushMatrix();
        CauseAnimOffset(true);

    }
    glColor3f(.3, .3, .3);
    glBegin(GL_QUADS);
    glVertex2f(0, 32+11);
    glVertex2f( 32 , 32+11);
    glVertex2f( 32 , 32+16);
    glVertex2f(0, 32+16);
    glEnd();

    if (hp >= 50.0) {
        glColor3f(  (50. / hp), hp / 50. , 0.);
    } else {
        glColor3f(  1., hp / 50. , 0.);
    }
    glBegin(GL_QUADS);
    glVertex2f(0, 32+11);
    glVertex2f(hp * 32 / 100, 32+11);
    glVertex2f(hp * 32 / 100, 32+16);
    glVertex2f(0, 32+16);
    glEnd();

    glictFontRender(GetName().c_str(), "system", 0, 32);

    glColor4f(1.,1.,1.,1.);
    if (wasmoving) glPopMatrix();

}
