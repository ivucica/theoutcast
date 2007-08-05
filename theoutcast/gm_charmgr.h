#ifndef __GM_CHARMGR
#define __GM_CHARMGR

#include <vector>
#include <string>

#include <GLICT/container.h>
#include <GLICT/window.h>
#include <GLICT/button.h>
#include <GLICT/panel.h>
#include <GLICT/textbox.h>

#include "gamemode.h"
#include "texmgmt.h"
#include "threads.h"


class GM_CharMgr : public GameMode {
    public:
        GM_CharMgr();
        ~GM_CharMgr();

        void Render();
        void SpecKeyPress(int key, int x, int y );
        void ResizeWindow();
        void MouseClick (int button, int shift, int mousex, int mousey);
        void KeyPress (unsigned char key, int x, int y);

        void MsgBox (const char* mbox, const char* title);
        void CreateCharlist();
        void DestroyCharlist();

        void ShowCharList();
        void ShowCreateCharacter();

    private:
        Texture *bg;
        glictContainer desktop;
        glictWindow winCharacterList;
        glictButton btnCharlistCreate, btnCharlistCancel;

        glictWindow winCreateCharacter;
        glictButton btnCreateCharacterOk, btnCreateCharacterCancel;
        glictPanel pnlCreateCharacterName;
        glictTextbox txtCreateCharacterName;
        glictPanel pnlCreateCharacterVocation;
        glictScrollbar scbCreateCharacterVocation;

        std::vector<std::string> vocationlist;

        ONCriticalSection threadsafe;

    friend void GM_CharMgr_MBOnDismiss(glictPos* pos, glictContainer* caller);
    friend void GM_CharMgr_CharList_Character(glictPos* pos, glictContainer *caller);
    friend void GM_CharMgr_CreateCharacter_Ok(glictPos* pos, glictContainer *caller);
    friend void GM_CharMgr_CreateCharacter_Cancel(glictPos* pos, glictContainer *caller);
    friend void GM_CharMgr_CreateCharacter_Vocation(glictPos* pos, glictContainer *caller);
};


#endif // __GM_CHARMGR

