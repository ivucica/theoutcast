#ifdef USESDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


#include <GL/gl.h> // FIXME (Khaos#1#) Remove since it's only here coz of rendermousecursor
#include <GL/glu.h> // FIXME (Khaos#1#) Remove since it's only here coz of rendermousecursor
#include <map>
#include "sdlwin.h"
#include "texmgmt.h"
#include "simple_effects.h"
#include "options.h"
#include "gamemode.h"
#include "util.h"
#include "defines.h"
#include "debugprint.h"
extern int texcount;
int winw=0, winh=0;
int ptrx=0, ptry=0;

int frames=0;
clock_t lasttime;
float fps=0;
bool mayanimate=false;
int sdl_WindowHandle;
int sdlw_keymods=0;

struct sdltimer_s {
	SDL_TimerID timerid;
	void(*func)(int);
	int arg;
};
std::map<int,sdltimer_s*> timers;

void sdlw_Init(int *argc, char**argv){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr,"Couldn't initialize SDL: %s\n",SDL_GetError());
        exit( 1 );
    }


}

void sdlw_Display() {
    game->Render();
    SDL_GL_SwapBuffers();
}
void sdlw_Reshape (int w, int h){
    winw = w; winh = h;
    game->ResizeWindow();
}
void sdlw_Mouse (int button, int shift, int mousex, int mousey) {
    if (shift==WIN_PRESS) DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Mouse press\n");
    if (shift==WIN_RELEASE) DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"Mouse release\n");
    game->MouseClick(button, shift, mousex, mousey);
}
void sdlw_Idle (){}
void sdlw_PassiveMouse (int mousex, int mousey) {
    ptrx = mousex;
	ptry = mousey;
}
void sdlw_SetMousePointer(std::string texturefile) {
    if (mousepointer_object) {
        delete mousepointer_object;
        mousepointer_object = NULL;
    }
	if (mousepointer) delete mousepointer;

	if (texturefile=="DEFAULT") {
		if (options.os_cursor) texturefile = "WINDOWS"; else texturefile = "mousepointer.bmp";
	}

	if (texturefile == "WINDOWS") {
//		glutSetCursor(GLUT_CURSOR_INHERIT );
		mousepointer = NULL;
	}
	else {
//		glutSetCursor(GLUT_CURSOR_NONE);
		mousepointer = new Texture(texturefile);
	}
}
void sdlw_SetMousePointer(Object *obj) {
	if (mousepointer_object) {
	    delete mousepointer_object;
	}
	mousepointer_object = obj;
	//glutSetCursor(GLUT_CURSOR_NONE);
}
int fpszerocounter=0;
void sdlw_FPS (int param){
	DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"FPS: %d\n",frames);
	if (frames==0) {
		fpszerocounter++;
		printf("FPS zero counter: %d\n", fpszerocounter);
		if (fpszerocounter>3) {
			NativeGUIError("It would appear that a timer error has occured. Game will exit now.", "The Outcast - Fatal Error");
			exit(1);
		}

	} else {
		fpszerocounter = 0;
	}
	fps = frames;
	frames = 0;
	sdlw_Timer(1000, sdlw_FPS, 0);


	char tmp[256];
	sprintf(tmp, "%s / FPS: %c%c%.02f, TexCount: %d", APPTITLE, fps <= 10.009 ? '<' : ' ', fps <= 10.009 ? '=' : ' ', fps, texcount);
	SDL_WM_SetCaption(tmp, NULL);

}
void sdlw_MayAnimateToTrue(int param){
	mayanimate = true;
}
void sdlw_Key(unsigned char key, int x, int y){
    game->KeyPress(key,x,y);
}
void sdlw_SpecKey(int key, int x, int y){
    game->SpecKeyPress(key,x,y);

}

void sdlw_CreateDisplay(){

    int bpp;
        int i;
        int rgb_size[3];
        int w = 640;
        int h = 480;
        int done = 0;
        int frames;
        Uint32 start_time, this_time;
    unsigned long videoflags=SDL_OPENGL;
    int gamma = 1;
        float color[8][3]= {{ 1.0,  1.0,  0.0},
                            { 1.0,  0.0,  0.0},
                            { 0.0,  0.0,  0.0},
                            { 0.0,  1.0,  0.0},
                            { 0.0,  1.0,  1.0},
                            { 1.0,  1.0,  1.0},
                            { 1.0,  0.0,  1.0},
                            { 0.0,  0.0,  1.0}};
        float cube[8][3]= {{ 0.5,  0.5, -0.5},
                           { 0.5, -0.5, -0.5},
                           {-0.5, -0.5, -0.5},
                           {-0.5,  0.5, -0.5},
                           {-0.5,  0.5,  0.5},
                           { 0.5,  0.5,  0.5},
                           { 0.5, -0.5,  0.5},
                           {-0.5, -0.5,  0.5}};
                           int value;

    if ( SDL_GetVideoInfo()->vfmt->BitsPerPixel <= 8 ) {
            bpp = 8;
    } else {

            bpp = 16;  /* More doesn't seem to work, says SDL example */
    }

    if (options.fullscreen) videoflags |= SDL_FULLSCREEN;
    //videoflags |= SDL_NOFRAME;
    videoflags |= SDL_RESIZABLE;



// some options:
    int fsaa = 0;
    int accel = 1;
    int sync = 1;


            /* Initialize the display */
        switch (bpp) {
            case 8:
                rgb_size[0] = 3;
                rgb_size[1] = 3;
                rgb_size[2] = 2;
                break;
            case 15:
            case 16:
                rgb_size[0] = 5;
                rgb_size[1] = 5;
                rgb_size[2] = 5;
                break;
            default:
                rgb_size[0] = 8;
                rgb_size[1] = 8;
                rgb_size[2] = 8;
                break;
        }
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, rgb_size[0] );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, rgb_size[1] );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, rgb_size[2] );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        if ( fsaa ) {
                SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
                SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, fsaa );
        }
        if ( accel ) {
                SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
        }
        if ( sync ) {
                SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
        } else {
                SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
        }
        if ( SDL_SetVideoMode( w, h, bpp, videoflags ) == NULL ) {
                fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
                SDL_Quit();
                exit(1);
        }

        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Screen BPP: %d\n", SDL_GetVideoSurface()->format->BitsPerPixel);
        /*DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "\n");
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Vendor     : %s\n", glGetString( GL_VENDOR ) );
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Renderer   : %s\n", glGetString( GL_RENDERER ) );
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Version    : %s\n", glGetString( GL_VERSION ) );
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "\n");*/


        SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
        //printf( "SDL_GL_RED_SIZE: requested %d, got %d\n", rgb_size[0],value);
        SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
        //printf( "SDL_GL_GREEN_SIZE: requested %d, got %d\n", rgb_size[1],value);
        SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
        //printf( "SDL_GL_BLUE_SIZE: requested %d, got %d\n", rgb_size[2],value);
        SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
        //printf( "SDL_GL_DEPTH_SIZE: requested %d, got %d\n", bpp, value );
        SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
        //printf( "SDL_GL_DOUBLEBUFFER: requested 1, got %d\n", value );
        if ( fsaa ) {
                SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
                printf("SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
                SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
                printf("SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
        }
        if ( accel ) {
                SDL_GL_GetAttribute( SDL_GL_ACCELERATED_VISUAL, &value );
                printf( "SDL_GL_ACCELERATED_VISUAL: requested 1, got %d\n", value );
        }
        if ( sync ) {
                SDL_GL_GetAttribute( SDL_GL_SWAP_CONTROL, &value );
                printf( "SDL_GL_SWAP_CONTROL: requested 1, got %d\n", value );
        }

        /* Set the window manager title bar */
        SDL_WM_SetCaption( "The Outcast", NULL);


        /* Set the gamma for the window */
        if ( gamma != 0.0 ) {
                SDL_SetGamma(gamma, gamma, gamma);
        }



}
void sdlw_MainLoop(){
    SDL_Event event;
    bool done = false;

    fps = 0;//FIXME (Khaos#1#) Need real fps calc
    mayanimate = false;
    sdlw_Reshape(640,480);



	sdlw_Timer(1000, sdlw_FPS, 0);
    while (!done) {
        sdlw_Display();
        frames++;
        while( SDL_PollEvent( &event ) ) {
            // event handlin
            switch (event.type) {
            case SDL_ACTIVEEVENT:
                /* See what happened */
                /*printf( "app %s ", event.active.gain ? "gained" : "lost" );
                if ( event.active.state & SDL_APPACTIVE ) {
                        printf( "active " );
                } else if ( event.active.state & SDL_APPMOUSEFOCUS ) {
                        printf( "mouse " );
                } else if ( event.active.state & SDL_APPINPUTFOCUS ) {
                        printf( "input " );
                }
                printf( "focus\n" );*/
                break;
			case SDL_KEYUP:

                if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT && sdlw_keymods & KMOD_SHIFT)
					sdlw_keymods ^= KMOD_SHIFT;
				break;
            case SDL_KEYDOWN:
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                        done = 1;
                }/*
                if ( (event.key.keysym.sym == SDLK_g) &&
                     (event.key.keysym.mod & KMOD_CTRL) ) {
//                        HotKey_ToggleGrab();
                }
                if ( (event.key.keysym.sym == SDLK_z) &&
                     (event.key.keysym.mod & KMOD_CTRL) ) {
//                        HotKey_Iconify();
                }
                if ( (event.key.keysym.sym == SDLK_RETURN) &&
                     (event.key.keysym.mod & KMOD_ALT) ) {
//                        HotKey_ToggleFullScreen();
                }*/

                if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT && !(sdlw_keymods & KMOD_SHIFT))
					sdlw_keymods ^= KMOD_SHIFT;


                if ( event.key.keysym.sym == SDLK_LEFT ||
                     event.key.keysym.sym == SDLK_RIGHT ||
                     event.key.keysym.sym == SDLK_UP ||
                     event.key.keysym.sym == SDLK_DOWN // TODO (Khaos#1#) Add pageup pagedown, home end
                     )
                {
                    sdlw_SpecKey(event.key.keysym.sym,0,0);//FIXME (Khaos#4#) 0,0 should be "currentmousexy"
                } else {
                	// outcast expects what glut usually serves: completely prepared keys, with shift and all that
                	int key = event.key.keysym.sym;
                	if (key==SDLK_LSHIFT || key==SDLK_RSHIFT) break;

                	DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Keypress: %d\n", key);
                	if (event.key.keysym.mod & KMOD_SHIFT) {
                		printf("SHIFT (%d)\n", event.key.keysym.mod);
                		if (key >= 'a' && key <='z')
							key-=32;
						else if (key >= '0' && key <='9')
							key-=(15+1);
                	}
                	if (key < 32 && key != 8 && key != 27 && key != 13 && key != 10) break;
                	DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Passed: %d\n", key);
                    sdlw_Key(key,1,0);
                }

                DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL,"key '%s' pressed\n",
                        SDL_GetKeyName(event.key.keysym.sym));

                break;
            case SDL_MOUSEMOTION:
                sdlw_PassiveMouse(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
				//TODO (Khaos#3#) passing left/right button
                sdlw_Mouse( 0, event.button.state == SDL_PRESSED ? WIN_PRESS : WIN_RELEASE, ptrx, ptry);
                break;
            case SDL_QUIT:
                done = 1;
                break;


            }

        }
    }

}

Uint32 sdlw_InternalOneTimeTimer(Uint32 interval, void*param) {

	int tmapid = (int)param;
	std::map<int,sdltimer_s*>::iterator it;
	sdltimer_s *sts = (it=timers.find(tmapid))->second;
	SDL_RemoveTimer(sts->timerid);
	sts->func(sts->arg);
	delete sts;
}
void sdlw_Timer(int ms, void(*func)(int), int arg) {

	sdltimer_s *st = new sdltimer_s;
	int tmapid=rand();
	timers[tmapid]=st;
	st->func = func;
	st->arg = arg;
	st->timerid = SDL_AddTimer(ms, sdlw_InternalOneTimeTimer, (void*)(tmapid));
}

int sdlw_GetModifiers() {
	printf("Modifiers: ");
	if (sdlw_keymods & KMOD_SHIFT) {
		printf("SHIFT ");
	}
	if (sdlw_keymods & KMOD_CTRL) {
		printf("CTRL ");
	}
	if (sdlw_keymods & KMOD_ALT) {
		printf("ALT ");
	}
	if (sdlw_keymods & KMOD_META) {
		printf("META ");
	}
	printf(" (%d)\n", sdlw_keymods);
	return sdlw_keymods;
}
#endif
