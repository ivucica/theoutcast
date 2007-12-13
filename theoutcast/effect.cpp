#include <GLICT/fonts.h>
#include <string>
#include <sstream>
#include "effect.h"
#include "tile.h"
#include "sound.h"
#include "database.h"
#include "protocol.h"
extern bool dontloadspr;
extern  float fps;
Effect::Effect(Tile *parent) {
    this->parent = parent;
}

Effect::~Effect() {
}

bool Effect::AnimationAdvance(float advance, bool overlayed) {
    float oldanimationpercent = this->animationpercent;
//    printf("Advancing effect animation - %g, type %s, overlayed %s\n", animationpercent, effecttype==MAGICEFFECT ? "magiceffect" : "text", overlayed ? "true" : "false");


    if (this->animationpercent != 100.) {
        if ((effecttype==MAGICEFFECT)) {
//            printf("It's an effect\n");
            if (!overlayed) {
                Thing::AnimationAdvance(advance);
            }

        } else { // text and distance shot ...
            if (overlayed) {
//                printf("Ani advanced\n");
				if (effecttype == DISTANCESHOT)
					animationpercent += 500./fps;
				else
					animationpercent += 50./fps;
                if (animationpercent > 100.) animationpercent = 100.; // FIXME (Khaos#4#) isnan?
            } else {
//                printf("Not advancing ani\n");
            }
        }
    }
    if (this->animationpercent < oldanimationpercent || this->animationpercent == 100)  { // this means that the animation is restarting ...
    	// TODO (Khaos#1#) here it is a potentially dangerous act... we must see if it's valid and non-breaking to remove effect here

        //printf("Magic effect dies\n");
        parent->DelayedRemove(this);

        return false;
    }
    return true;
}
#include "console.h"
static int EffectPlaySound(void *NotUsed, int argc, char **argv, char **azColName) {
	if (argc) {
		if (argv[0]) {
			std::stringstream s;
			s << "sounds/" << argv[0];
			//console.insert(argv[0]);
			SoundPlay(s.str().c_str());
		}
	}
	return 0;
}
void Effect::SetType(unsigned short type, void* extra) {
	//printf("Creating new sprite for effect type %d\n", type);
    if (!dontloadspr) {
    	if (!extra) {
    		dbExecPrintf(dbData, EffectPlaySound, NULL, NULL, "select `soundfile` from effects%d where effectid='%d';", protocol->GetProtocolVersion(), type);

			sprgfx = new ObjSpr(type, 2);
    	}
		else {
			dbExecPrintf(dbData, EffectPlaySound, NULL, NULL, "select `soundfile` from distances%d where distanceid='%d';", protocol->GetProtocolVersion(),  type);
			sprgfx = new ObjSpr(type, 3);
		}
    }
    //printf("Created\n");
	this->effecttype = MAGICEFFECT;
}
void Effect::SetText(std::string &text, unsigned char color, bool animated) {
    textcolor.b = (color % 6) / 5.;
    textcolor.g = ((color / 6) % 6) / 5.;
    textcolor.r = (color / 36) / 5.;

    printf("%d - %02x %02x %02x - %g %g %g\n", color, (char)(textcolor.r * 256), (char)(textcolor.g * 256), (char)(textcolor.b * 256), textcolor.r, textcolor.g, textcolor.b);

    textcolor.a = 1.;

    this->text = text;

    this->effecttype = (animated ? ANIMATEDTEXT : TEXT);

}
void Effect::SetDistanceDeltaTarget(short x, short y) {
	this->effecttype = DISTANCESHOT;
	this->distdelta.x = x;
	this->distdelta.y = y;
}
#include "console.h"
void Effect::Render(const position_t *pos, bool overlayed) {
    switch (effecttype) {
        case MAGICEFFECT:
            if (!overlayed) Thing::Render();
            break;
        case TEXT:
            glColor4f(textcolor.r, textcolor.g, textcolor.b, textcolor.a);
            glRotatef(180,1,0,0);
            if (overlayed) glictFontRender(text.c_str(), "system", 16 - glictFontSize(text.c_str(), "system")/2., -64);
            glRotatef(180,1,0,0);
            glColor4f(1., 1., 1., 1.);
            break;
        case ANIMATEDTEXT:
            glColor4f(textcolor.r, textcolor.g, textcolor.b, textcolor.a);
            glRotatef(180,1,0,0);
            if (overlayed) glictFontRender(text.c_str(), "system", 0, -64-animationpercent/3.);
            glRotatef(180,1,0,0);
            glColor4f(1., 1., 1., 1.);
            break;
		case DISTANCESHOT: {

			glTranslatef((distdelta.x*animationpercent/100.  * 32.), -(distdelta.y*animationpercent/100. * 32.), 0);


			position_t p;


			if (distdelta.x < 0) p.x = 0;
			if (distdelta.x == 0) p.x = 1;
			if (distdelta.x > 0) p.x = 2;

			if (distdelta.y < 0) p.y = 0;
			if (distdelta.y == 0) p.y = 1;
			if (distdelta.y > 0) p.y = 2;

			if (overlayed) {
				sprgfx->Render(&p);
				//std::stringstream s;
				//s << (distdelta.x*animationpercent/100.*32.) << " " << (distdelta.y*animationpercent/100.*32.) << " - " << distdelta.x << "x" << distdelta.y << " => " << animationpercent/100.;
				//console.insert(s.str());
				//printf("%s\n", s.str().c_str());

			}


			break;
		}
        default:
            printf("Still don't know how to render this magic effect\n");
    }
}

bool Effect::IsGround() {
    return false;
}
