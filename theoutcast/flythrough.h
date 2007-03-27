#ifndef __FLYTHROUGH_H
#define __FLYTHROUGH_H

#include <string>
#include <vector>

typedef struct {// version of file format
	char major, minor;
	int revision; 
} flyversion_t;
typedef struct {
	int n; // number of frames
} flyheader_t;
typedef struct { // individual keyframe data
	float posx1, posy1, posz1;
	float rotx1, roty1, rotz1;
	float length;
	float posx2, posy2, posz2;
	float rotx2, roty2, rotz2;
	char subtitle[256];
} flykf_t;


typedef std::vector<flykf_t*>::iterator Kf_Iterator; // keyframe iterator


class flythrough_c {
    public:
        flythrough_c();
        ~flythrough_c();

        bool load(std::string filename);
        void set_active_keyframe(int n);
        void set_cam_pos(float fps);

        void new_keyframe();

        void set_on_finish( void(*on_finish_v)(void *), void* on_finish_arg_v ) {
            on_finish = on_finish_v;
            on_finish_arg = on_finish_arg_v;
        }

        float posx1, posy1, posz1, posx2, posy2, posz2;float rotx1, roty1, rotz1, rotx2, roty2, rotz2;

        std::vector<flykf_t*> kf; // keyframes
        int kf_i; // current keyframe

        float player_current_time;
        float player_current_kf_time;

        bool loaded;

    private:
        void(*on_finish)(void*);
        void *on_finish_arg;
};

#endif
