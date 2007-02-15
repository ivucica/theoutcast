#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include "simple_effects.h"
#include "defines.h"

void FlagEffect(float beginx, float beginy, float endx, float endy, int divx, int divy, float anglebegin, float anglediff, float strength) {
    FlagEffect(beginx, beginy, endx, endy, divx, divy, anglebegin, anglediff, strength, false, false);
}

void FlagEffect(float beginx, // 200
				float beginy, // 0
				float endx, // 425
				float endy, // 100
				int divx, // 10
				int divy, // 10
				float anglebegin, // sine_flag_angle
				float anglediff, // 720
				float strength, // 6
                bool flipx,
				bool flipy
				) {
	glPushMatrix();
	//float beginx = 200., beginy = 0;
	//float endx = 425., endy = 100.;


	float deltax = endx - beginx, deltay = endy - beginy;

	//int divx = 10, divy = 10;

	float increasex = deltax / (float)divx;
	float increasey = deltay / (float)divy;

	float angincrey = anglediff / (float)divy;

	//glRotatef(180., 1.0, 0.0, 0.0);
    glTranslatef(0,deltay,0.0);

	glBegin(GL_QUADS);
		float angy = anglebegin;
		float angyshift = 0.;
		float angyshift2 = 0.;
        for (float i=0;i<deltax;i+=increasex) {
			angy += angincrey;
			angyshift = sin(angy * PI / 180.) * strength;
			angyshift2 = sin((angy + angincrey) * PI / 180.) * strength;
			for (float j=0;j<deltay;j+=increasey) {
                if (!flipy) {
                    glTexCoord2f((i+increasex)/deltax,1.-j/deltay); glVertex2f(beginx+i+increasex,(beginy-j)+angyshift2);
                    glTexCoord2f((i+increasex)/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey)+angyshift2);
                    glTexCoord2f(i/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey)+angyshift);
                    glTexCoord2f(i/deltax,1.-j/deltay); glVertex2f(beginx+i,(beginy-j)+angyshift);
                } else {
                    glTexCoord2f((i+increasex)/deltax,j/deltay); glVertex2f(beginx+i+increasex,(beginy-j)+angyshift2);
                    glTexCoord2f((i+increasex)/deltax,(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey)+angyshift2);
                    glTexCoord2f(i/deltax,(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey)+angyshift);
                    glTexCoord2f(i/deltax,j/deltay); glVertex2f(beginx+i,(beginy-j)+angyshift);
                }




			}
		}
	glEnd();
	glPopMatrix();

}
void StillEffect(float beginx,	float beginy, float endx, float endy, int divx, int divy) {
    StillEffect(beginx, beginy, endx, endy, divx, divy, false, false);
}

void StillEffect(float beginx, float beginy, float endx, float endy, int divx, int divy, bool flipx, bool flipy) {
    StillEffect(beginx, beginy, endx, endy, divx, divy, flipx, flipy, false);
}

void StillEffect(float beginx, // 200
				float beginy, // 0
				float endx, // 425
				float endy, // 100
				int divx, // 10
				int divy, // 10
				bool flipx,
				bool flipy,
				bool culltype
        ) {
    glPushMatrix();
	//float beginx = 200., beginy = 0;
	//float endx = 425., endy = 100.;


	float deltax = endx - beginx, deltay = endy - beginy;



	//int divx = 10, divy = 10;

	float increasex = deltax / (float)divx;
	float increasey = deltay / (float)divy;



	//glRotatef(180., 1.0, 0.0, 0.0);
    glTranslatef(0,deltay,0.0);

	glBegin(GL_QUADS);
		for (float i=0;i<deltax;i+=increasex) {
			for (float j=0;j<deltay;j+=increasey) {
			    if (!culltype) {
                    if (!flipy) {
                        glTexCoord2f((i+increasex)/deltax,j/deltay); glVertex2f(beginx+i+increasex,(beginy-j));
                        glTexCoord2f((i+increasex)/deltax,(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey));
                        glTexCoord2f(i/deltax,(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey));
                        glTexCoord2f(i/deltax,j/deltay); glVertex2f(beginx+i,(beginy-j));
                    } else {
                        glTexCoord2f((i+increasex)/deltax,1.-j/deltay); glVertex2f(beginx+i+increasex,(beginy-j));
                        glTexCoord2f((i+increasex)/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey));
                        glTexCoord2f(i/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey));
                        glTexCoord2f(i/deltax,1.-j/deltay); glVertex2f(beginx+i,(beginy-j));
                    }
			    } else {
			        if (!flipy) {
                        glTexCoord2f((i+increasex)/deltax,j/deltay); glVertex2f(beginx+i+increasex,(beginy-j));
                        glTexCoord2f(i/deltax,j/deltay); glVertex2f(beginx+i,(beginy-j));
                        glTexCoord2f(i/deltax,(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey));
                        glTexCoord2f((i+increasex)/deltax,(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey));
                    } else {
                        glTexCoord2f((i+increasex)/deltax,1.-j/deltay); glVertex2f(beginx+i+increasex,(beginy-j));
                        glTexCoord2f(i/deltax,1.-j/deltay); glVertex2f(beginx+i,(beginy-j));
                        glTexCoord2f(i/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i,(beginy-j-increasey));
                        glTexCoord2f((i+increasex)/deltax,1.-(j+increasey)/deltay); glVertex2f(beginx+i+increasex,(beginy-j-increasey));
                    }
			    }
            }
		}
	glEnd();
	glPopMatrix();

}
