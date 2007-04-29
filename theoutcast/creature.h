#ifndef __CREATURE_H
#define __CREATURE_H

#include <string>
#include "thing.h"

class Creature : public Thing {
    public:
        Creature();
        ~Creature();

        void Render(position_t *pos);
        void Render();
        void RenderOverlay();

        void CauseAnimOffset(bool individual);

        // setting info
        void SetType(unsigned short outfit, void* extra);
        void SetCreatureID(unsigned long creatureid);
        void SetName(std::string creaturename);
        void AnimationAdvance(float advance);
        void SetAttacked(bool atk);
        void SetHP(unsigned char hp);

        bool IsApproved();
        creaturelook_t GetLook() {return creaturelook;}

        // obtaining info
        bool IsGround();
        std::string GetName();
        unsigned long GetCreatureID();
        unsigned char GetHP();
    private:
        unsigned long id;
        std::string name;
        bool attacked;
        unsigned char hp;
        creaturelook_t creaturelook;
};

#endif // __CREATURE_H
