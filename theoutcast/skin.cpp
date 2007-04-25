#include <GLICT/globals.h>
#include <GLICT/skinner.h>
#include "skin.h"
#include "colors.h"

Skin skin;
Skin::Skin() {
    wintl = NULL;
    wint  = NULL;
    wintr = NULL;
    winl  = NULL;
    winc  = NULL;
    winr  = NULL;
    winbl = NULL;
    winb  = NULL;
    winbr = NULL;

    btntl = NULL;
    btnt  = NULL;
    btntr = NULL;
    btnl  = NULL;
    btnc  = NULL;
    btnr  = NULL;
    btnbl = NULL;
    btnb  = NULL;
    btnbr = NULL;

    bthtl = NULL;
    btht  = NULL;
    bthtr = NULL;
    bthl  = NULL;
    bthc  = NULL;
    bthr  = NULL;
    bthbl = NULL;
    bthb  = NULL;
    bthbr = NULL;

    txttl = NULL;
    txtt  = NULL;
    txttr = NULL;
    txtl  = NULL;
    txtc  = NULL;
    txtr  = NULL;
    txtbl = NULL;
    txtb  = NULL;
    txtbr = NULL;


}
Skin::~Skin() {
    Unload();
}
void Skin::Load(const char* what) {

    std::string dirname = "skins/";
    dirname += what;
    dirname += "/";

    Unload();


    {
        FILE *f = fopen((dirname + "colors.txt").c_str(), "r");
        if (f) {
            fscanf(f, "%g %g %g %g", &glictGlobals.buttonTextColor.r, &glictGlobals.buttonTextColor.g, &glictGlobals.buttonTextColor.b, &glictGlobals.buttonTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.buttonHighlightTextColor.r, &glictGlobals.buttonHighlightTextColor.g, &glictGlobals.buttonHighlightTextColor.b, &glictGlobals.buttonHighlightTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.panelTextColor.r,  &glictGlobals.panelTextColor.g,  &glictGlobals.panelTextColor.b,  &glictGlobals.panelTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.textboxTextColor.r,  &glictGlobals.textboxTextColor.g,  &glictGlobals.textboxTextColor.b,  &glictGlobals.textboxTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.windowTitleColor[0], &glictGlobals.windowTitleColor[1], &glictGlobals.windowTitleColor[2], &glictGlobals.windowTitleColor[3]);

            for (int i = 0; i < 7; i++) {
                fscanf(f, "%g %g %g %g", &consolecolors[i*4], &consolecolors[i*4+1], &consolecolors[i*4+2], &consolecolors[i*4+3]);
            }



            fclose(f);
        }
    }

    wintl = new Texture(dirname + "window/tl.bmp");
    wint  = new Texture(dirname + "window/t.bmp");
    wintr = new Texture(dirname + "window/tr.bmp");
    winl  = new Texture(dirname + "window/l.bmp");
    winc  = new Texture(dirname + "window/c.bmp");
    winr  = new Texture(dirname + "window/r.bmp");
    winbl = new Texture(dirname + "window/bl.bmp");
    winb  = new Texture(dirname + "window/b.bmp");
    winbr = new Texture(dirname + "window/br.bmp");





    btntl = new Texture(dirname + "button/tl.bmp");
    btnt  = new Texture(dirname + "button/t.bmp");
    btntr = new Texture(dirname + "button/tr.bmp");
    btnl  = new Texture(dirname + "button/l.bmp");
    btnc  = new Texture(dirname + "button/c.bmp");
    btnr  = new Texture(dirname + "button/r.bmp");
    btnbl = new Texture(dirname + "button/bl.bmp");
    btnb  = new Texture(dirname + "button/b.bmp");
    btnbr = new Texture(dirname + "button/br.bmp");




    bthtl = new Texture(dirname + "buttonhighlight/tl.bmp");
    btht  = new Texture(dirname + "buttonhighlight/t.bmp");
    bthtr = new Texture(dirname + "buttonhighlight/tr.bmp");
    bthl  = new Texture(dirname + "buttonhighlight/l.bmp");
    bthc  = new Texture(dirname + "buttonhighlight/c.bmp");
    bthr  = new Texture(dirname + "buttonhighlight/r.bmp");
    bthbl = new Texture(dirname + "buttonhighlight/bl.bmp");
    bthb  = new Texture(dirname + "buttonhighlight/b.bmp");
    bthbr = new Texture(dirname + "buttonhighlight/br.bmp");


    txttl = new Texture(dirname + "textbox/tl.bmp");
    txtt  = new Texture(dirname + "textbox/t.bmp");
    txttr = new Texture(dirname + "textbox/tr.bmp");
    txtl  = new Texture(dirname + "textbox/l.bmp");
    txtc  = new Texture(dirname + "textbox/c.bmp");
    txtr  = new Texture(dirname + "textbox/r.bmp");
    txtbl = new Texture(dirname + "textbox/bl.bmp");
    txtb  = new Texture(dirname + "textbox/b.bmp");
    txtbr = new Texture(dirname + "textbox/br.bmp");

    AssureLoadedness();
}
void Skin::AssureLoadedness() {

    glictSize s;


    wintl->Bind(); 	s.w = wintl->w; s.h = wintl->h; win.SetTL(*wintl->textureid, &s);
    wint->Bind();	s.w = wint->w;  s.h = wint->h; 	win.SetTop(*wint->textureid, &s);
    wintr->Bind();	s.w = wintr->w; s.h = wintr->h; win.SetTR(*wintr->textureid, &s);
    winl->Bind();	s.w = winl->w;  s.h = winl->h; 	win.SetLeft(*winl->textureid, &s);
    winc->Bind();	s.w = winc->w;  s.h = winc->h; 	win.SetCenter(*winc->textureid, &s);
    winr->Bind();	s.w = winr->w;  s.h = winr->h; 	win.SetRight(*winr->textureid, &s);
    winbl->Bind();	s.w = winbl->w; s.h = winbl->h; win.SetBL(*winbl->textureid, &s);
    winb->Bind();	s.w = winb->w;  s.h = winb->h; 	win.SetBottom(*winb->textureid, &s);
    winbr->Bind();	s.w = winbr->w; s.h = winbr->h; win.SetBR(*winbr->textureid, &s);


    glictGlobals.windowBodySkin = &win;



    btntl->Bind(); 	s.w = btntl->w; s.h = btntl->h; btn.SetTL(*btntl->textureid, &s);
    btnt->Bind();	s.w = btnt->w;  s.h = btnt->h; 	btn.SetTop(*btnt->textureid, &s);
    btntr->Bind();	s.w = btntr->w; s.h = btntr->h; btn.SetTR(*btntr->textureid, &s);
    btnl->Bind();	s.w = btnl->w;  s.h = btnl->h; 	btn.SetLeft(*btnl->textureid, &s);
    btnc->Bind();	s.w = btnc->w;  s.h = btnc->h; 	btn.SetCenter(*btnc->textureid, &s);
    btnr->Bind();	s.w = btnr->w;  s.h = btnr->h; 	btn.SetRight(*btnr->textureid, &s);
    btnbl->Bind();	s.w = btnbl->w; s.h = btnbl->h; btn.SetBL(*btnbl->textureid, &s);
    btnb->Bind();	s.w = btnb->w;  s.h = btnb->h; 	btn.SetBottom(*btnb->textureid, &s);
    btnbr->Bind();	s.w = btnbr->w; s.h = btnbr->h; btn.SetBR(*btnbr->textureid, &s);


    glictGlobals.buttonSkin = &btn;



    bthtl->Bind(); 	s.w = bthtl->w; s.h = bthtl->h; bth.SetTL(*bthtl->textureid, &s);
    btht->Bind();	s.w = btht->w;  s.h = btht->h; 	bth.SetTop(*btht->textureid, &s);
    bthtr->Bind();	s.w = bthtr->w; s.h = bthtr->h; bth.SetTR(*bthtr->textureid, &s);
    bthl->Bind();	s.w = bthl->w;  s.h = bthl->h; 	bth.SetLeft(*bthl->textureid, &s);
    bthc->Bind();	s.w = bthc->w;  s.h = bthc->h; 	bth.SetCenter(*bthc->textureid, &s);
    bthr->Bind();	s.w = bthr->w;  s.h = bthr->h; 	bth.SetRight(*bthr->textureid, &s);
    bthbl->Bind();	s.w = bthbl->w; s.h = bthbl->h; bth.SetBL(*bthbl->textureid, &s);
    bthb->Bind();	s.w = bthb->w;  s.h = bthb->h; 	bth.SetBottom(*bthb->textureid, &s);
    bthbr->Bind();	s.w = bthbr->w; s.h = bthbr->h; bth.SetBR(*bthbr->textureid, &s);


    glictGlobals.buttonHighlightSkin = &bth;



    txttl->Bind(); 	s.w = txttl->w; s.h = txttl->h; txt.SetTL(*txttl->textureid, &s);
    txtt->Bind();	s.w = txtt->w;  s.h = txtt->h; 	txt.SetTop(*txtt->textureid, &s);
    txttr->Bind();	s.w = txttr->w; s.h = txttr->h; txt.SetTR(*txttr->textureid, &s);
    txtl->Bind();	s.w = txtl->w;  s.h = txtl->h; 	txt.SetLeft(*txtl->textureid, &s);
    txtc->Bind();	s.w = txtc->w;  s.h = txtc->h; 	txt.SetCenter(*txtc->textureid, &s);
    txtr->Bind();	s.w = txtr->w;  s.h = txtr->h; 	txt.SetRight(*txtr->textureid, &s);
    txtbl->Bind();	s.w = txtbl->w; s.h = txtbl->h; txt.SetBL(*txtbl->textureid, &s);
    txtb->Bind();	s.w = txtb->w;  s.h = txtb->h; 	txt.SetBottom(*txtb->textureid, &s);
    txtbr->Bind();	s.w = txtbr->w; s.h = txtbr->h; txt.SetBR(*txtbr->textureid, &s);


    glictGlobals.textboxSkin = &txt;

	
}
void Skin::Unload() {

    if (wintl) delete wintl; wintl = NULL;
    if (wint)  delete wint;  wint  = NULL;
    if (wintr) delete wintr; wintr = NULL;
    if (winl)  delete winl;  winl  = NULL;
    if (winc)  delete winc;  winc  = NULL;
    if (winr)  delete winr;  winr  = NULL;
    if (winbl) delete winbl; winbl = NULL;
    if (winb)  delete winb;  winb  = NULL;
    if (winbr) delete winbr; winbr = NULL;

    win.SetTL(0, NULL);
    win.SetTop(0, NULL);
    win.SetTR(0, NULL);
    win.SetLeft(0, NULL);
    win.SetCenter(0, NULL);
    win.SetRight(0, NULL);
    win.SetBL(0, NULL);
    win.SetBottom(0, NULL);
    win.SetBR(0, NULL);

    if (btntl) delete btntl; btntl = NULL;
    if (btnt)  delete btnt;  btnt  = NULL;
    if (btntr) delete btntr; btntr = NULL;
    if (btnl)  delete btnl;  btnl  = NULL;
    if (btnc)  delete btnc;  btnc  = NULL;
    if (btnr)  delete btnr;  btnr  = NULL;
    if (btnbl) delete btnbl; btnbl = NULL;
    if (btnb)  delete btnb;  btnb  = NULL;
    if (btnbr) delete btnbr; btnbr = NULL;

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


    if (bthtl) delete bthtl; bthtl = NULL;
    if (btht)  delete btht;  btht  = NULL;
    if (bthtr) delete bthtr; bthtr = NULL;
    if (bthl)  delete bthl;  bthl  = NULL;
    if (bthc)  delete bthc;  bthc  = NULL;
    if (bthr)  delete bthr;  bthr  = NULL;
    if (bthbl) delete bthbl; bthbl = NULL;
    if (bthb)  delete bthb;  bthb  = NULL;
    if (bthbr) delete bthbr; bthbr = NULL;

    bth.SetTL(0, NULL);
    bth.SetTop(0, NULL);
    bth.SetTR(0, NULL);
    bth.SetLeft(0, NULL);
    bth.SetCenter(0, NULL);
    bth.SetRight(0, NULL);
    bth.SetBL(0, NULL);
    bth.SetBottom(0, NULL);
    bth.SetBR(0, NULL);

    glictGlobals.buttonHighlightSkin = NULL;

    if (txttl) delete txttl; txttl = NULL;
    if (txtt)  delete txtt;  txtt  = NULL;
    if (txttr) delete txttr; txttr = NULL;
    if (txtl)  delete txtl;  txtl  = NULL;
    if (txtc)  delete txtc;  txtc  = NULL;
    if (txtr)  delete txtr;  txtr  = NULL;
    if (txtbl) delete txtbl; txtbl = NULL;
    if (txtb)  delete txtb;  txtb  = NULL;
    if (txtbr) delete txtbr; txtbr = NULL;

    txt.SetTL(0, NULL);
    txt.SetTop(0, NULL);
    txt.SetTR(0, NULL);
    txt.SetLeft(0, NULL);
    txt.SetCenter(0, NULL);
    txt.SetRight(0, NULL);
    txt.SetBL(0, NULL);
    txt.SetBottom(0, NULL);
    txt.SetBR(0, NULL);

    glictGlobals.textboxSkin = NULL;

}
