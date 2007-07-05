#ifdef USESDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


#include <GL/gl.h> // FIXME (Khaos#1#) Remove since it's only here coz of rendermousecursor
#include <GL/glu.h> // FIXME (Khaos#1#) Remove since it's only here coz of rendermousecursor
#include "sdlwin.h"
#include "texmgmt.h"
#include "simple_effects.h"
#include "options.h"
#include "gamemode.h"

int winw=0, winh=0;
int ptrx=0, ptry=0;
Texture* mousepointer=NULL;
int frames=0;
clock_t lasttime;
float fps=0;
bool mayanimate=false;
float cursoraniangle=0.;
Object *mousepointer_object;
int sdl_WindowHandle;


void sdlw_Init(int *argc, char**argv){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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
    if (shift==WIN_PRESS) printf("press\n");
    if (shift==WIN_RELEASE) printf("release\n");
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
void sdlw_FPS (int param){}
void sdlw_MayAnimateToTrue(int param){}
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

    // TODO (Khaos#4#) what does noframe do? videoflags |= SDL_NOFRAME

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

        printf("Screen BPP: %d\n", SDL_GetVideoSurface()->format->BitsPerPixel);
        printf("\n");
        printf( "Vendor     : %s\n", glGetString( GL_VENDOR ) );
        printf( "Renderer   : %s\n", glGetString( GL_RENDERER ) );
        printf( "Version    : %s\n", glGetString( GL_VERSION ) );
        printf( "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
        printf("\n");


        SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
        printf( "SDL_GL_RED_SIZE: requested %d, got %d\n", rgb_size[0],value);
        SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
        printf( "SDL_GL_GREEN_SIZE: requested %d, got %d\n", rgb_size[1],value);
        SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
        printf( "SDL_GL_BLUE_SIZE: requested %d, got %d\n", rgb_size[2],value);
        SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
        printf( "SDL_GL_DEPTH_SIZE: requested %d, got %d\n", bpp, value );
        SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
        printf( "SDL_GL_DOUBLEBUFFER: requested 1, got %d\n", value );
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
        SDL_WM_SetCaption( "The Outcast", "sdlgloutcast" );


        /* Set the gamma for the window */
        if ( gamma != 0.0 ) {
                SDL_SetGamma(gamma, gamma, gamma);
        }



}
void sdlw_MainLoop(){
    SDL_Event event;
    bool done = false;

    fps = 20;//FIXME (Khaos#1#) Need real fps calc
    mayanimate = true;
    sdlw_Reshape(640,480);

    while (!done) {
        sdlw_Display();
        while( SDL_PollEvent( &event ) ) {
            // event handlin
            switch (event.type) {
            case SDL_ACTIVEEVENT:
                /* See what happened */
                printf( "app %s ", event.active.gain ? "gained" : "lost" );
                if ( event.active.state & SDL_APPACTIVE ) {
                        printf( "active " );
                } else if ( event.active.state & SDL_APPMOUSEFOCUS ) {
                        printf( "mouse " );
                } else if ( event.active.state & SDL_APPINPUTFOCUS ) {
                        printf( "input " );
                }
                printf( "focus\n" );
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
                if ( event.key.keysym.sym == SDLK_LEFT ||
                     event.key.keysym.sym == SDLK_RIGHT ||
                     event.key.keysym.sym == SDLK_UP ||
                     event.key.keysym.sym == SDLK_DOWN // TODO (Khaos#1#) Add pageup pagedown, home end
                     )
                {
                    sdlw_SpecKey(event.key.keysym.sym,0,0);//FIXME (Khaos#4#) 0,0 should be "currentmousexy"
                } else {
                    sdlw_Key(event.key.keysym.sym,1,0);
                }

                printf("key '%s' pressed\n",
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
void sdlw_Timer(int ms, void(*func)(int), int arg) {}

// FIXME (Khaos#1#) Move into more appropriate file (that'll apply for both sdl and glut)
void RenderMouseCursor() {
	////////////////////////////CURSOR RENDERING/////////////////////
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.,winw,0.,winh);
	//glRotatef(180.0, 1.0, 0.0, 0.0);
	//glTranslatef(0,-winh,0.0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (!mousepointer_object && mousepointer) {
        mousepointer->Bind();
        glEnable(GL_TEXTURE_2D);

        //FlagEffect(ptrx-32., ptry-32., ptrx+32., ptry+32., 10, 10, cursoraniangle, 360., 2.	);
        StillEffect(ptrx-32., winh-ptry-32., ptrx+32., winh-ptry+32., 10, 10, false, false, true);

    } else if (mousepointer_object) {

        glPushMatrix();
			glTranslatef(ptrx - 16., winh-ptry - 16., 0);
			mousepointer_object->Render();
			mousepointer_object->AnimationAdvance(100./fps);
        glPopMatrix();

    }
	glDisable(GL_TEXTURE_2D);
	if (fps && mayanimate) cursoraniangle += 180. / fps;
	if (cursoraniangle > 360.) cursoraniangle -= 360.;

	//////////////////////END CURSOR RENDERING//////////////////////


}

#endif