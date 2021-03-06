#ifndef __GM_MAINMENU_H
#define __GM_MAINMENU_H

#include <GLICT/container.h>
#include <GLICT/button.h>
#include <GLICT/window.h>
#include <GLICT/panel.h>
#include <GLICT/messagebox.h>
#include <GLICT/textbox.h>
#include <GLICT/scrollbar.h>

#include <GLICT/types.h>

#include "gamemode.h"
#include "texmgmt.h"
#include "threads.h"

#include "obj3ds.h"
#include "flythrough.h"


class GM_MainMenu : public GameMode {
	public:
		GM_MainMenu();
		~GM_MainMenu();

		void Render();
		void ResizeWindow();
		void MouseClick (int button, int shift, int mousex, int mousey);
		void KeyPress (unsigned char key, int x, int y);
		void MsgBox (const char* mbox, const char* title);

        void RebuildMainMenu();

        void CreateCharlist();
        void DestroyCharlist();

        void GoToGameworld();
        void GoToCharMgr();

        void SetLoginStatus(const char *loginstatus);


	private:
		glictContainer desktop;
		glictPanel tibia;
		glictWindow mainmenu;
		glictButton btnNextSprite, btnLogIn, btnTutorial, btnOptions, btnToS, btnAbout, btnExit;

		glictWindow login;
		glictPanel pnlLogin;
		glictPanel pnlLoginProtocol, pnlLoginServer, pnlLoginUsername, pnlLoginPassword;
		glictTextbox txtLoginProtocol, txtLoginServer, txtLoginUsername, txtLoginPassword;
		glictButton btnLoginLogin, btnLoginCancel;
        glictMessageBox charlist;

		glictMessageBox about, tos;

		glictWindow characterlist;
		glictButton btnCharlistCharMgr, btnCharlistCancel;

		glictWindow options;
		glictPanel pnlOptionsMaptrack, pnlOptionsFullscreen, pnlOptionsIntro, pnlOptionsOSCursor, pnlOptionsSound, pnlOptionsMinimap; // checkboxes here only, please
		glictButton btnOptionsMaptrack, btnOptionsFullscreen, btnOptionsIntro, btnOptionsOSCursor, btnOptionsSound, btnOptionsMinimap; // checkboxes here only, please
		glictPanel pnlOptionsSkin; // textboxes here only, please
		glictTextbox txtOptionsSkin; // textboxes here only, please

		glictPanel pnlOptionsRestartWarning;
		glictButton btnOptionsOk, btnOptionsCancel;

		Texture *logo, *bg;
        Obj3ds *city;
        flythrough_c flythrough;

		double sine_flag_angle, bg_move_angle, fadein, fadeout;
		bool aboutIsOpen, tosIsOpen;


        ONCriticalSection threadsafe;

        // callbacks
        void (*OnFadeout)();

	friend void GM_MainMenu_LogIn(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_ToS(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_About(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_Exit(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_CharList_LogonOK(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_CharList_LogonError(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_CharList_Character(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_CharList_Cancel(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_CreatingAccount(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_LoginLogin(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_LoginCancel(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_AboutOnDismiss(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_ToSOnDismiss(glictPos* pos, glictContainer* caller);
	friend void GM_MainMenu_MBOnDismiss(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_NextSprite(glictPos* pos, glictContainer* caller);
    friend void GM_MainMenu_Options(glictPos* pos, glictContainer *caller);
    friend void GM_MainMenu_OptionsCheckbox(glictPos* pos, glictContainer *caller);
    friend void GM_MainMenu_OptionsOk(glictPos* pos, glictContainer *caller);
    friend void GM_MainMenu_OptionsCancel(glictPos* pos, glictContainer *caller);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList(ONThreadFuncArgumentType menuclass_void);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon(ONThreadFuncArgumentType menuclass_void);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList_SP(ONThreadFuncArgumentType menuclass_void);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon_SP(ONThreadFuncArgumentType menuclass_void);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_CharList_ME0(ONThreadFuncArgumentType menuclass_void);
	friend ONThreadFuncReturnType ONThreadFuncPrefix Thread_GWLogon_ME0(ONThreadFuncArgumentType menuclass_void);
	friend void ItemsLoad();
	friend void CreaturesLoad();
	friend void EffectsLoad();
	friend void DistancesLoad();


};
void GM_MainMenu_LogIn(glictPos* pos, glictContainer* caller);
void GM_MainMenu_ToS(glictPos* pos, glictContainer* caller);
void GM_MainMenu_About(glictPos* pos, glictContainer* caller);
void GM_MainMenu_Exit(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CreatingAccount(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CharList_LogonOK(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CharList_LogonError(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CharList_CharMgr(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CharList_Character(glictPos* pos, glictContainer* caller);
void GM_MainMenu_CharList_Cancel(glictPos* pos, glictContainer* caller);
void GM_MainMenu_LoginLogin(glictPos* pos, glictContainer* caller);
void GM_MainMenu_LoginCancel(glictPos* pos, glictContainer* caller);
void GM_MainMenu_AboutOnDismiss(glictPos* pos, glictContainer* caller);
void GM_MainMenu_ToSOnDismiss(glictPos* pos, glictContainer* caller);
void GM_MainMenu_MBOnDismiss(glictPos* pos, glictContainer* caller);
void GM_MainMenu_NextSprite(glictPos* pos, glictContainer* caller);
void GM_MainMenu_Options(glictPos* pos, glictContainer *caller);
void GM_MainMenu_OptionsCheckbox(glictPos* pos, glictContainer *caller);
void GM_MainMenu_OptionsOk(glictPos* pos, glictContainer *caller);
void GM_MainMenu_OptionsCancel(glictPos* pos, glictContainer *caller);
void GM_MainMenu_ExitDo();
void GM_MainMenu_GoToGameworldDo();
void GM_MainMenu_GoToCharMgrDo();


#endif
