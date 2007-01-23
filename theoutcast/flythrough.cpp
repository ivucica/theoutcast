//#include "compiler.h"


#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include "flythrough.h"
#include "math.h"
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void flythrough_c::set_active_keyframe(int n){
    kf_i = n;

    posx1 = kf[kf_i]->posx1; posy1 = kf[kf_i]->posy1; posz1 = kf[kf_i]->posz1;
    posx2 = kf[kf_i]->posx2; posy2 = kf[kf_i]->posy2; posz2 = kf[kf_i]->posz2;

    rotx1 = kf[kf_i]->rotx1; roty1 = kf[kf_i]->roty1; rotz1 = kf[kf_i]->rotz1;
    rotx2 = kf[kf_i]->rotx2; roty2 = kf[kf_i]->roty2; rotz2 = kf[kf_i]->rotz2;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool flythrough_c::load(std::string filename)
{
    if (loaded) return false;
    FILE* f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    unsigned long n;
    fread(&n, sizeof(n), 1, f);
    for (int i = 0 ; i < n ; ++i) {
        new_keyframe();
        fread(kf[i], sizeof(kf_t), 1, f);
    }

    fclose(f);
    loaded = true;

    set_active_keyframe(0);
    return true;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
flythrough_c::~flythrough_c()
{

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
flythrough_c::flythrough_c()
{
    loaded = false;
    on_finish = NULL;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void flythrough_c::new_keyframe()
{
    kf_t* kframe = new kf_t;
    //ZeroMemory(kframe, sizeof(kf_t));
    kf.insert(kf.end(), kframe);
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void flythrough_c::set_cam_pos(float fps)
{

    float percent, percent2;
    percent2 = player_current_kf_time / kf[kf_i]->length;
    percent = 1. - percent2;

    glRotatef(rotx1*percent + rotx2*percent2, 1., 0., 0.);
    glRotatef(roty1*percent + roty2*percent2, 0., 1., 0.);
    glRotatef(rotz1*percent + rotz2*percent2, 0., 0., 1.);
    glTranslatef(posx1*percent + posx2*percent2, posy1*percent + posy2*percent2, posz1*percent + posz2*percent2);



    if (fps) {
        player_current_time += 1. / fps;
        player_current_kf_time += 1. / fps;
        if (isnan(player_current_time) || isnan(player_current_kf_time)) {
            player_current_time = 0;
            player_current_kf_time = 0;
        }
        if (player_current_kf_time > kf[kf_i]->length || player_current_kf_time < 0.) {
            //printf("Next keyframe\n");
            if (kf_i + 1 != kf.size()) set_active_keyframe(kf_i+1); else ++kf_i; // 'else' is here because we want the comparison in kf_i == kf_size() to work properly
            player_current_kf_time = 0.;
        }
        if (kf_i >= kf.size() || kf_i < 0) {
            //printf("Reached end\n");
            set_active_keyframe(0);
            player_current_time = 0.;

            if (on_finish) on_finish(on_finish_arg);
        }
    }


}

