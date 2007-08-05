#include <GLICT/fonts.h>
#include <sstream>
#include "creature.h"
#include "types.h"
#include "texmgmt.h"
#include "simple_effects.h"
extern Texture* texSkull;
extern bool dontloadspr;
Creature::Creature() {
    attacked = false;
    hp = 0;
    name = "unnamed";
    skull = SKULL_NONE;
}
Creature::~Creature() {
	delete sprgfx;
	sprgfx = NULL;
}
void Creature::SetType(unsigned short outfit, void* extra) {
    ONThreadSafe(threadsafe);
    printf("Setting creature type\n");
    creaturelook_t *crl = (creaturelook_t *)extra;

    this->type = outfit;
    this->creaturelook = *crl;
    printf("Creating sprite\n");
    if (outfit != 0) {
        if (!dontloadspr) sprgfx = new ObjSpr(outfit, crl->head, crl->body, crl->legs, crl->feet);
    } else {
    	printf("Extended look\n");
        if (!dontloadspr) sprgfx = new ObjSpr(crl->extendedlook , 0);
    }
    printf("created\n");
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
void Creature::SetSkull(skull_t s) {
    this->skull = s;
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
    //printf("Creature animation advance: %g\n", animationpercent);
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
		case STOP:
			// standing
			break;
		default:
			// nothing
			break;
    }
}
#include "player.h" // REMOVE ME
void Creature::Render(const position_t *pos) {
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
    //if (player) if (this->GetCreatureID() == player->GetCreatureID()) glColor4f(1., 0., 0., 1.);
    Thing::Render(pos);
    //if (player) if (this->GetCreatureID() == player->GetCreatureID()) glColor4f(1., 1., 1., 1.);
    glTranslatef(8,-8,0);
    if (wasmoving) {
        glPopMatrix();
    }
}
void Creature::Render() {
    position_t p(0,0,0);
    Render(&p);
}

void Creature::SetAttacked(bool atk) {
    this->attacked = atk;
}
#include "console.h"
void Creature::SetHP(unsigned char hp) {
/*    {
        char tmp[255];
        sprintf(tmp, "Changing health of %s to %d", name.c_str(), hp);
        console.insert(tmp, true);
    }*/
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
    if (sprgfx->sli.unknown>1) glTranslatef(-(sprgfx->sli.width-1)*32, (sprgfx->sli.height-1)*32, 0);
    glColor3f(.3, .3, .3);
    glBegin(GL_QUADS);
    glVertex2f(0 - 8, 32+11 + 8);
    glVertex2f( 32 - 8, 32+11 + 8);
    glVertex2f( 32 - 8 , 32+16 + 8);
    glVertex2f(0 - 8, 32+16 + 8);
    glEnd();

    if (hp >= 50.0) {
        glColor3f(  (50. / hp), hp / 50. , 0.);
    } else {
        glColor3f(  1., hp / 50. , 0.);
    }
    glBegin(GL_QUADS);
    glVertex2f(0 - 8, 32+11 + 8);
    glVertex2f(hp * 32 / 100 - 8, 32+11 + 8);
    glVertex2f(hp * 32 / 100 - 8, 32+16 + 8);
    glVertex2f(0 - 8, 32+16 + 8);
    glEnd();

	glRotatef(180,1,0,0);
    glictFontRender(GetName().c_str(), "system", -8, -32 - 10- 8);
    glRotatef(180,1,0,0);

    {
	glRotatef(180,1,0,0);
	std::stringstream nargh;
	nargh << this->GetCreatureID();
    glictFontRender(nargh.str().c_str(), "system", -8, -32 - 20- 8);
    glRotatef(180,1,0,0);
    }


    if (this->skull) {
        texSkull->Bind();
        switch (this->skull) {
            case SKULL_YELLOW:
                glColor4f(1., 1., 0., 1.);
                break;
            case SKULL_GREEN:
                glColor4f(0., 1., 0., 1.);
                break;
            case SKULL_WHITE:
                glColor4f(1., 1., 1., 1.);
                break;
            case SKULL_RED:
                glColor4f(1., 0., 0., 1.);
                break;
            default:
                glColor4f(.4, .4, .4, 1.);
                break;
        }
        glEnable(GL_TEXTURE_2D);
        StillEffect(32-8-8, 32, 32-8, 32+8, 10, 10, false, false, true);

        glDisable(GL_TEXTURE_2D);
    }
	if (sprgfx->sli.unknown>1) glTranslatef((sprgfx->sli.width-1)*32, -(sprgfx->sli.height-1)*32, 0);
    glColor4f(1.,1.,1.,1.);
    if (wasmoving) glPopMatrix();

}
bool Creature::IsStackable()  {
	return false;
}
creaturelook_t Creature::GetCreatureLook() {
	return creaturelook;
}
