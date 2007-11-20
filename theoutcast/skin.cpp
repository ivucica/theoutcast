#include <GLICT/globals.h>
#include <GLICT/skinner.h>
#include "texmgmt.h"
#include "skin.h"
#include "assert.h"
#include "defines.h" // for fileexists
#ifdef OUTCAST_SKINS
    #include "colors.h"
#endif

static void Skin_OnBind(glictSkinner* caller, void* t);

Skin *skin;

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

    tmmtl = NULL;
    tmmt  = NULL;
    tmmtr = NULL;
    tmml  = NULL;
    tmmc  = NULL;
    tmmr  = NULL;
    tmmbl = NULL;
    tmmb  = NULL;
    tmmbr = NULL;

    tmmloaded = false;
	#ifdef OUTCAST_SKINS
    nologo = false;
	#endif
}
Skin::~Skin() {
    Unload();

}
void Skin::Load(const char* what) {

    std::string dirname = "skins/";
    dirname += what;
    dirname += "/";

    Unload();

	printf("Dirname %s\n", dirname.c_str());
    printf("Loading colors\n");
    {
        FILE *f = fopen((dirname + "colors.txt").c_str(), "r");
        if (f) {
            fscanf(f, "%g %g %g %g", &glictGlobals.buttonTextColor.r, &glictGlobals.buttonTextColor.g, &glictGlobals.buttonTextColor.b, &glictGlobals.buttonTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.buttonHighlightTextColor.r, &glictGlobals.buttonHighlightTextColor.g, &glictGlobals.buttonHighlightTextColor.b, &glictGlobals.buttonHighlightTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.panelTextColor.r,  &glictGlobals.panelTextColor.g,  &glictGlobals.panelTextColor.b,  &glictGlobals.panelTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.textboxTextColor.r,  &glictGlobals.textboxTextColor.g,  &glictGlobals.textboxTextColor.b,  &glictGlobals.textboxTextColor.a);
            fscanf(f, "%g %g %g %g", &glictGlobals.windowTitleColor.r, &glictGlobals.windowTitleColor.g, &glictGlobals.windowTitleColor.b, &glictGlobals.windowTitleColor.a);

            #ifdef OUTCAST_SKINS
            for (int i = 0; i < 7; i++) {
                fscanf(f, "%g %g %g %g", &consolecolors[i*4], &consolecolors[i*4+1], &consolecolors[i*4+2], &consolecolors[i*4+3]);
            }
            nologo = fileexists((dirname + "nologo.txt").c_str());


            #endif



            fclose(f);
        }
    }


    //return;
    printf("Loading window\n");
    wintl = new Texture(dirname + "window/tl.bmp");
    wint  = new Texture(dirname + "window/t.bmp");
    wintr = new Texture(dirname + "window/tr.bmp");
    winl  = new Texture(dirname + "window/l.bmp");
    winc  = new Texture(dirname + "window/c.bmp");
    winr  = new Texture(dirname + "window/r.bmp");
    winbl = new Texture(dirname + "window/bl.bmp");
    winb  = new Texture(dirname + "window/b.bmp");
    winbr = new Texture(dirname + "window/br.bmp");

    printf("Loading button\n");
    btntl = new Texture(dirname + "button/tl.bmp");
    btnt  = new Texture(dirname + "button/t.bmp");
    btntr = new Texture(dirname + "button/tr.bmp");
    btnl  = new Texture(dirname + "button/l.bmp");
    btnc  = new Texture(dirname + "button/c.bmp");
    btnr  = new Texture(dirname + "button/r.bmp");
    btnbl = new Texture(dirname + "button/bl.bmp");
    btnb  = new Texture(dirname + "button/b.bmp");
    btnbr = new Texture(dirname + "button/br.bmp");



    printf("Loading button highlight\n");
    bthtl = new Texture(dirname + "buttonhighlight/tl.bmp");
    btht  = new Texture(dirname + "buttonhighlight/t.bmp");
    bthtr = new Texture(dirname + "buttonhighlight/tr.bmp");
    bthl  = new Texture(dirname + "buttonhighlight/l.bmp");
    bthc  = new Texture(dirname + "buttonhighlight/c.bmp");
    bthr  = new Texture(dirname + "buttonhighlight/r.bmp");
    bthbl = new Texture(dirname + "buttonhighlight/bl.bmp");
    bthb  = new Texture(dirname + "buttonhighlight/b.bmp");
    bthbr = new Texture(dirname + "buttonhighlight/br.bmp");

    printf("Loading textbox\n");
    txttl = new Texture(dirname + "textbox/tl.bmp");
    txtt  = new Texture(dirname + "textbox/t.bmp");
    txttr = new Texture(dirname + "textbox/tr.bmp");
    txtl  = new Texture(dirname + "textbox/l.bmp");
    txtc  = new Texture(dirname + "textbox/c.bmp");
    txtr  = new Texture(dirname + "textbox/r.bmp");
    txtbl = new Texture(dirname + "textbox/bl.bmp");
    txtb  = new Texture(dirname + "textbox/b.bmp");
    txtbr = new Texture(dirname + "textbox/br.bmp");

    printf("Checking whether to load panelmainmenu\n");
    FILE *f = fopen((dirname + "panelmainmenu/tl.bmp").c_str(), "r");
    if (f) {
        fclose(f);

        printf("Loading panelmainmenu\n");
        tmmloaded = true;
        tmmtl = new Texture(dirname + "panelmainmenu/tl.bmp");
        tmmt  = new Texture(dirname + "panelmainmenu/t.bmp");
        tmmtr = new Texture(dirname + "panelmainmenu/tr.bmp");
        tmml  = new Texture(dirname + "panelmainmenu/l.bmp");
        tmmc  = new Texture(dirname + "panelmainmenu/c.bmp");
        tmmr  = new Texture(dirname + "panelmainmenu/r.bmp");
        tmmbl = new Texture(dirname + "panelmainmenu/bl.bmp");
        tmmb  = new Texture(dirname + "panelmainmenu/b.bmp");
        tmmbr = new Texture(dirname + "panelmainmenu/br.bmp");
//        system("pause");
    }

    printf("Assuring loadedness\n");
    AssureLoadedness();
}
void Skin::AssureLoadedness() {

    glictSize s;

    wintl->Bind(); 	s.w = wintl->w; s.h = wintl->h; win.SetTL(Skin_OnBind, wintl, &s);
    wint->Bind();	s.w = wint->w;  s.h = wint->h; 	win.SetTop(Skin_OnBind, wint, &s);
    wintr->Bind();	s.w = wintr->w; s.h = wintr->h; win.SetTR(Skin_OnBind, wintr, &s);
    winl->Bind();	s.w = winl->w;  s.h = winl->h; 	win.SetLeft(Skin_OnBind, winl, &s);
    winc->Bind();	s.w = winc->w;  s.h = winc->h; 	win.SetCenter(Skin_OnBind, winc, &s);
    winr->Bind();	s.w = winr->w;  s.h = winr->h; 	win.SetRight(Skin_OnBind, winr, &s);
    winbl->Bind();	s.w = winbl->w; s.h = winbl->h; win.SetBL(Skin_OnBind, winbl, &s);
    winb->Bind();	s.w = winb->w;  s.h = winb->h; 	win.SetBottom(Skin_OnBind, winb, &s);
    winbr->Bind();	s.w = winbr->w; s.h = winbr->h; win.SetBR(Skin_OnBind, winbr, &s);


    glictGlobals.windowBodySkin = &win;



    btntl->Bind(); 	s.w = btntl->w; s.h = btntl->h; btn.SetTL(Skin_OnBind, btntl, &s);
    btnt->Bind();	s.w = btnt->w;  s.h = btnt->h; 	btn.SetTop(Skin_OnBind, btnt, &s);
    btntr->Bind();	s.w = btntr->w; s.h = btntr->h; btn.SetTR(Skin_OnBind, btntr, &s);
    btnl->Bind();	s.w = btnl->w;  s.h = btnl->h; 	btn.SetLeft(Skin_OnBind, btnl, &s);
    btnc->Bind();	s.w = btnc->w;  s.h = btnc->h; 	btn.SetCenter(Skin_OnBind, btnc, &s);
    btnr->Bind();	s.w = btnr->w;  s.h = btnr->h; 	btn.SetRight(Skin_OnBind, btnr, &s);
    btnbl->Bind();	s.w = btnbl->w; s.h = btnbl->h; btn.SetBL(Skin_OnBind, btnbl, &s);
    btnb->Bind();	s.w = btnb->w;  s.h = btnb->h; 	btn.SetBottom(Skin_OnBind, btnb, &s);
    btnbr->Bind();	s.w = btnbr->w; s.h = btnbr->h; btn.SetBR(Skin_OnBind, btnbr, &s);

    glictGlobals.buttonSkin = &btn;


    bthtl->Bind(); 	s.w = bthtl->w; s.h = bthtl->h; bth.SetTL(Skin_OnBind, bthtl, &s);
    btht->Bind();	s.w = btht->w;  s.h = btht->h; 	bth.SetTop(Skin_OnBind, btht, &s);
    bthtr->Bind();	s.w = bthtr->w; s.h = bthtr->h; bth.SetTR(Skin_OnBind, bthtr, &s);
    bthl->Bind();	s.w = bthl->w;  s.h = bthl->h; 	bth.SetLeft(Skin_OnBind, bthl, &s);
    bthc->Bind();	s.w = bthc->w;  s.h = bthc->h; 	bth.SetCenter(Skin_OnBind, bthc, &s);
    bthr->Bind();	s.w = bthr->w;  s.h = bthr->h; 	bth.SetRight(Skin_OnBind, bthr, &s);
    bthbl->Bind();	s.w = bthbl->w; s.h = bthbl->h; bth.SetBL(Skin_OnBind, bthbl, &s);
    bthb->Bind();	s.w = bthb->w;  s.h = bthb->h; 	bth.SetBottom(Skin_OnBind, bthb, &s);
    bthbr->Bind();	s.w = bthbr->w; s.h = bthbr->h; bth.SetBR(Skin_OnBind, bthbr, &s);

    glictGlobals.buttonHighlightSkin = &bth;



    txttl->Bind(); 	s.w = txttl->w; s.h = txttl->h; txt.SetTL(Skin_OnBind, txttl, &s);
    txtt->Bind();	s.w = txtt->w;  s.h = txtt->h; 	txt.SetTop(Skin_OnBind, txtt, &s);
    txttr->Bind();	s.w = txttr->w; s.h = txttr->h; txt.SetTR(Skin_OnBind, txttr, &s);
    txtl->Bind();	s.w = txtl->w;  s.h = txtl->h; 	txt.SetLeft(Skin_OnBind, txtl, &s);
    txtc->Bind();	s.w = txtc->w;  s.h = txtc->h; 	txt.SetCenter(Skin_OnBind, txtc, &s);
    txtr->Bind();	s.w = txtr->w;  s.h = txtr->h; 	txt.SetRight(Skin_OnBind, txtr, &s);
    txtbl->Bind();	s.w = txtbl->w; s.h = txtbl->h; txt.SetBL(Skin_OnBind, txtbl, &s);
    txtb->Bind();	s.w = txtb->w;  s.h = txtb->h; 	txt.SetBottom(Skin_OnBind, txtb, &s);
    txtbr->Bind();	s.w = txtbr->w; s.h = txtbr->h; txt.SetBR(Skin_OnBind, txtbr, &s);

    glictGlobals.textboxSkin = &txt;

    if (tmmloaded) {
        tmmtl->Bind(); 	s.w = tmmtl->w; s.h = tmmtl->h; tmm.SetTL(Skin_OnBind, tmmtl, &s);
        tmmt->Bind();	s.w = tmmt->w;  s.h = tmmt->h; 	tmm.SetTop(Skin_OnBind, tmmt, &s);
        tmmtr->Bind();	s.w = tmmtr->w; s.h = tmmtr->h; tmm.SetTR(Skin_OnBind, tmmtr, &s);
        tmml->Bind();	s.w = tmml->w;  s.h = tmml->h; 	tmm.SetLeft(Skin_OnBind, tmml, &s);
        tmmc->Bind();	s.w = tmmc->w;  s.h = tmmc->h; 	tmm.SetCenter(Skin_OnBind, tmmc, &s);
        tmmr->Bind();	s.w = tmmr->w;  s.h = tmmr->h; 	tmm.SetRight(Skin_OnBind, tmmr, &s);
        tmmbl->Bind();	s.w = tmmbl->w; s.h = tmmbl->h; tmm.SetBL(Skin_OnBind, tmmbl, &s);
        tmmb->Bind();	s.w = tmmb->w;  s.h = tmmb->h; 	tmm.SetBottom(Skin_OnBind, tmmb, &s);
        tmmbr->Bind();	s.w = tmmbr->w; s.h = tmmbr->h; tmm.SetBR(Skin_OnBind, tmmbr, &s);
    }


}
void Skin::Unload() {

    printf("Unloading window skin...\n");
    if (wintl) delete wintl; wintl = NULL;
    if (wint)  delete wint;  wint  = NULL;
    if (wintr) delete wintr; wintr = NULL;
    if (winl)  delete winl;  winl  = NULL;
    printf("Center\n");
    if (winc)  delete winc;  winc  = NULL;
    printf("Right\n");
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


    printf("Unloading button skin...\n");

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


    printf("Unloading highlighted button skin...\n");

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

    printf("Unloading textbox skin...\n");

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

    printf("Unloading tibiamainmenu skin...\n");

    if (tmmtl) delete tmmtl; tmmtl = NULL;
    if (tmmt)  delete tmmt;  tmmt  = NULL;
    if (tmmtr) delete tmmtr; tmmtr = NULL;
    if (tmml)  delete tmml;  tmml  = NULL;
    if (tmmc)  delete tmmc;  tmmc  = NULL;
    if (tmmr)  delete tmmr;  tmmr  = NULL;
    if (tmmbl) delete tmmbl; tmmbl = NULL;
    if (tmmb)  delete tmmb;  tmmb  = NULL;
    if (tmmbr) delete tmmbr; tmmbr = NULL;

    tmm.SetTL(0, NULL);
    tmm.SetTop(0, NULL);
    tmm.SetTR(0, NULL);
    tmm.SetLeft(0, NULL);
    tmm.SetCenter(0, NULL);
    tmm.SetRight(0, NULL);
    tmm.SetBL(0, NULL);
    tmm.SetBottom(0, NULL);
    tmm.SetBR(0, NULL);
    tmmloaded = false;

    nologo = false;

    printf("UNLOADED ALL!\n");
}

static void Skin_OnBind(glictSkinner* caller, void* t) {
    glEnable(GL_TEXTURE_2D);
    ((Texture*)t)->Bind();
}
