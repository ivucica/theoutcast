#include <GLICT/globals.h>
#include <GLICT/skinner.h>
#include "skin.h"

Skin skin;
Skin::Skin() {

}
Skin::~Skin() {
    Unload();
}
void Skin::Load(const char* what) {
    glictSize s;
	std::string dirname = "skins/";
    dirname += what;
    dirname += "/";

    Unload();

    wintl = new Texture(dirname + "window/tl.bmp");
    wint  = new Texture(dirname + "window/t.bmp");
    wintr = new Texture(dirname + "window/tr.bmp");
    winl  = new Texture(dirname + "window/l.bmp");
    winc  = new Texture(dirname + "window/c.bmp");
    winr  = new Texture(dirname + "window/r.bmp");
    winbl = new Texture(dirname + "window/bl.bmp");
    winb  = new Texture(dirname + "window/b.bmp");
    winbr = new Texture(dirname + "window/br.bmp");

    s.w = wintl->w; s.h = wintl->h; win.SetTL(wintl->textureid, &s);
    s.w = wint->w; s.h = wint->h; win.SetTop(wint->textureid, &s);
    s.w = wintr->w; s.h = wintr->h; win.SetTR(wintr->textureid, &s);
    s.w = winl->w; s.h = winl->h; win.SetLeft(winl->textureid, &s);
    s.w = winc->w; s.h = winc->h; win.SetCenter(winc->textureid, &s);
    s.w = winr->w; s.h = winr->h; win.SetRight(winr->textureid, &s);
    s.w = winbl->w; s.h = winbl->h; win.SetBL(winbl->textureid, &s);
    s.w = winb->w; s.h = winb->h; win.SetBottom(winb->textureid, &s);
    s.w = winbr->w; s.h = winbr->h; win.SetBR(winbr->textureid, &s);

    glictGlobals.windowBodySkin = &win;




    btntl = new Texture(dirname + "button/tl.bmp");
    btnt  = new Texture(dirname + "button/t.bmp");
    btntr = new Texture(dirname + "button/tr.bmp");
    btnl  = new Texture(dirname + "button/l.bmp");
    btnc  = new Texture(dirname + "button/c.bmp");
    btnr  = new Texture(dirname + "button/r.bmp");
    btnbl = new Texture(dirname + "button/bl.bmp");
    btnb  = new Texture(dirname + "button/b.bmp");
    btnbr = new Texture(dirname + "button/br.bmp");

    s.w = btntl->w; s.h = btntl->h; btn.SetTL(btntl->textureid, &s);
    s.w = btnt->w; s.h = btnt->h; btn.SetTop(btnt->textureid, &s);
    s.w = btntr->w; s.h = btntr->h; btn.SetTR(btntr->textureid, &s);
    s.w = btnl->w; s.h = btnl->h; btn.SetLeft(btnl->textureid, &s);
    s.w = btnc->w; s.h = btnc->h; btn.SetCenter(btnc->textureid, &s);
    s.w = btnr->w; s.h = btnr->h; btn.SetRight(btnr->textureid, &s);
    s.w = btnbl->w; s.h = btnbl->h; btn.SetBL(btnbl->textureid, &s);
    s.w = btnb->w; s.h = btnb->h; btn.SetBottom(btnb->textureid, &s);
    s.w = btnbr->w; s.h = btnbr->h; btn.SetBR(btnbr->textureid, &s);

    glictGlobals.buttonSkin = &btn;
    glictGlobals.buttonHighlightSkin = &btn;


}
void Skin::Unload() {

    delete wintl;
    delete wint;
    delete wintr;
    delete winl;
    delete winc;
    delete winr;
    delete winbl;
    delete winb;
    delete winbr;

    win.SetTL(0, NULL);
    win.SetTop(0, NULL);
    win.SetTR(0, NULL);
    win.SetLeft(0, NULL);
    win.SetCenter(0, NULL);
    win.SetRight(0, NULL);
    win.SetBL(0, NULL);
    win.SetBottom(0, NULL);
    win.SetBR(0, NULL);

	delete btntl;
    delete btnt;
    delete btntr;
    delete btnl;
    delete btnc;
    delete btnr;
    delete btnbl;
    delete btnb;
    delete btnbr;

    btn.SetTL(0, NULL);
    btn.SetTop(0, NULL);
    btn.SetTR(0, NULL);
    btn.SetLeft(0, NULL);
    btn.SetCenter(0, NULL);
    btn.SetRight(0, NULL);
    btn.SetBL(0, NULL);
    btn.SetBottom(0, NULL);
    btn.SetBR(0, NULL);

    glictGlobals.buttonSkin = NULL;
    glictGlobals.buttonHighlightSkin = NULL;

}
