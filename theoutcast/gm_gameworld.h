
#include <GLICT/container.h>
#include <GLICT/window.h>
#include <GLICT/textbox.h>
#include <GLICT/button.h>
#include "gamemode.h"
#include "objspr.h"
#include "types.h"
#include "container.h"
#include "threads.h"

class GM_Gameworld : public GameMode {
    public:
        GM_Gameworld();
        ~GM_Gameworld();

        void Render();
        void KeyPress (unsigned char key, int x, int y) ;
        void SpecKeyPress(int key, int x, int y);
        void MouseClick (int button, int shift, int mousex, int mousey);
        void ResizeWindow();

        void UpdateStats();

        void AddContainer(Container *c, unsigned int x, unsigned int y);
        void RemoveContainer(Container *c);
        unsigned int GetContainersX();
        unsigned int GetContainersY();
        void MsgBox (const char* mbox, const char* title);
    private:
        ObjSpr *g;
        glictContainer desktop, containerdesktop;
        glictWindow winWorld, winConsole, winInventory, winStats;

        // winconsole
        glictTextbox txtConMessage;
        glictButton btnConSend;

        // wininventory
        glictPanel panInvSlots[10];

        // winstats
        glictPanel panStaStats;
        glictButton btnStaStance1, btnStaStance2, btnStaStance3;
        glictButton chkStaChase;
        glictButton btnInviteParty, btnLeaveParty, btnJoinParty, btnRevokeParty, btnPassParty;

        position_t useex_item1_pos; unsigned char useex_item1_stackpos;

        stancechase_t chase;
        stanceaggression_t stance;

        bool invitingparty, joiningparty, revokingparty, passingparty;

        ONCriticalSection desktopthreadsafe;

    friend void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
    friend void GM_Gameworld_InvSlotsOnClick(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_InvSlotsOnMouseDown(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_InvSlotsOnMouseUp(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_ClickExec(position_t *pos, glictEvents evttype );
    friend void GM_Gameworld_StaStanceOnClick(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_StaChaseOnClick(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_MBOnDismiss(glictPos* pos, glictContainer* caller);
    friend void GM_Gameworld_StaInviteParty(glictPos *pos, glictContainer* caller);
    friend void GM_Gameworld_StaLeaveParty(glictPos *pos, glictContainer* caller);
    friend void GM_Gameworld_StaRevokeParty(glictPos *pos, glictContainer* caller);
    friend void GM_Gameworld_StaJoinParty(glictPos *pos, glictContainer* caller);
    friend void GM_Gameworld_StaPassParty(glictPos *pos, glictContainer* caller);
};
void GM_Gameworld_ConSendOnClick (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnClick (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnMouseDown (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnMouseUp (glictPos* pos, glictContainer* caller);
void GM_Gameworld_WorldOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void GM_Gameworld_ConsoleOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void GM_Gameworld_InvSlotsOnPaint(glictRect *real, glictRect *clipped, glictContainer *caller);
void GM_Gameworld_InvSlotsOnClick(glictPos* pos, glictContainer* caller);
void GM_Gameworld_InvSlotsOnMouseDown(glictPos* pos, glictContainer* caller);
void GM_Gameworld_InvSlotsOnMouseUp(glictPos* pos, glictContainer* caller);
void GM_Gameworld_ClickExec(position_t *pos, glictEvents evttype );
void GM_Gameworld_StaStanceOnClick(glictPos* pos, glictContainer* caller);
void GM_Gameworld_StaChaseOnClick(glictPos* pos, glictContainer* caller);
void GM_Gameworld_MBOnDismiss(glictPos* pos, glictContainer* caller);
void GM_Gameworld_StaInviteParty(glictPos *pos, glictContainer* caller);
void GM_Gameworld_StaLeaveParty(glictPos *pos, glictContainer* caller);
void GM_Gameworld_StaRevokeParty(glictPos *pos, glictContainer* caller);
void GM_Gameworld_StaJoinParty(glictPos *pos, glictContainer* caller);
void GM_Gameworld_StaPassParty(glictPos *pos, glictContainer* caller);
