#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "skinner.h"

/// \todo   Check if structs can be assigned directly (not the specific elements
///         one by one...)

#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b))
#endif
glictSkinner::glictSkinner() {
    topleft = 0;
    topright = 0;
    bottomleft = 0;
    bottomright = 0;

    top = 0;
    left = 0;
    right = 0;
    bottom = 0;

    center = 0;


    topleftf = NULL;
    toprightf = NULL;
    bottomleftf = NULL;
    bottomrightf = NULL;

    topf = NULL;
    left = NULL;
    right = NULL;
    bottom = NULL;

    center = NULL;
}

glictSkinner::~glictSkinner() {
}

void glictSkinner::Paint(glictSize *size) {
    int x1, y1, x2, y2;

    glEnable(GL_TEXTURE_2D);
    glColor4f(1., 1., 1., 1.);
    // first the corners...

    if (topleft || topleftf) {
        x1 = y1 = 0;
        x2 = toplefts.w+1; y2 = toplefts.h+1;
        if (topleftf)
            topleftf(this, topleftp);
        else
            glBindTexture(GL_TEXTURE_2D, topleft);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);
        glVertex2f(x1, y2);
        glTexCoord2f(1, 1);
        glVertex2f(x2, y2);
        glTexCoord2f(1, 0);
        glVertex2f(x2, y1);
        glEnd();
    }

    if (topright || toprightf) {
        x1 = size->w - toprights.w -1; y1 = 0;
        x2 = size->w; y2 = toprights.h + 1;
        if (toprightf)
            toprightf(this, toprightp);
        else
            glBindTexture(GL_TEXTURE_2D, topright);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);
        glVertex2f(x1, y2);
        glTexCoord2f(1, 1);
        glVertex2f(x2, y2);
        glTexCoord2f(1, 0);
        glVertex2f(x2, y1);
        glEnd();
    }

    if (bottomleft || bottomleftf) {
        x1 = 0; y1 = size->h - bottomlefts.h - 1;
        x2 = bottomlefts.w + 1; y2 = size->h;
        if (bottomleftf)
            bottomleftf(this, bottomleftp);
        else
            glBindTexture(GL_TEXTURE_2D, bottomleft);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);
        glVertex2f(x1, y2);
        glTexCoord2f(1, 1);
        glVertex2f(x2, y2);
        glTexCoord2f(1, 0);
        glVertex2f(x2, y1);
        glEnd();
    }

    if (bottomright || bottomrightf) {
        x1 = size->w - bottomrights.w - 1; y1 = size->h - bottomrights.h - 1;
        x2 = size->w; y2 = size->h;
        if (bottomrightf)
            bottomrightf(this, bottomrightp);
        else
            glBindTexture(GL_TEXTURE_2D, bottomright);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 1);
        glVertex2f(x1, y2);
        glTexCoord2f(1, 1);
        glVertex2f(x2, y2);
        glTexCoord2f(1, 0);
        glVertex2f(x2, y1);
        glEnd();
    }



    if (top || topf) {
        if (topf)
            topf(this, topp);
        else
            glBindTexture(GL_TEXTURE_2D, top);

        for (int pos = (topleft || topleftf) ? toplefts.w : 0; pos < size->w - ((topright || toprightf) ? toprights.w : 0); pos+=tops.w) {
            x1 = pos; y1 = 0;
            x2 = min(size->w - ((topright || toprightf) ? toprights.w : 0) , pos + tops.w) + 1; y2 = tops.h + 1;
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(x1, y1);
            glTexCoord2f(0, 1);
            glVertex2f(x1, y2);
            glTexCoord2f(1, 1);
            glVertex2f(x2, y2);
            glTexCoord2f(1, 0);
            glVertex2f(x2, y1);
            glEnd();
        }
    }

    if (bottom || bottomf) {
        if (bottomf)
            bottomf(this, bottomp);
        else
            glBindTexture(GL_TEXTURE_2D, bottom);

        for (int pos = (bottomleft || bottomleftf) ? bottomlefts.w : 0; pos < size->w - ((bottomright || bottomrightf) ? bottomrights.w : 0); pos+=bottoms.w) {
            x1 = pos; y1 = size->h - bottoms.h - 1;
            x2 = min(size->w - ((bottomleft || bottomleftf) ? bottomlefts.w : 0), pos + bottoms.w) + 1; y2 = size->h;
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(x1, y1);
            glTexCoord2f(0, 1);
            glVertex2f(x1, y2);
            glTexCoord2f(1, 1);
            glVertex2f(x2, y2);
            glTexCoord2f(1, 0);
            glVertex2f(x2, y1);
            glEnd();
        }
    }



    if (left || leftf) {
        if (leftf)
            leftf(this, leftp);
        else
            glBindTexture(GL_TEXTURE_2D, left);
        for (int pos = (topleft || topleftf) ? toplefts.h : 0; pos < size->h - ((bottomleft || bottomleftf) ? bottomlefts.w : 0); pos+=lefts.h) {
            x1 = 0; y1 = pos;
            x2 = lefts.w; y2 = min(size->h - ((bottomleft || bottomleftf) ? bottomlefts.h : 0), pos + lefts.h);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(x1, y1);
            glTexCoord2f(0, 1);
            glVertex2f(x1, y2);
            glTexCoord2f(1, 1);
            glVertex2f(x2, y2);
            glTexCoord2f(1, 0);
            glVertex2f(x2, y1);
            glEnd();
        }
    }


    if (right || rightf) {
        if (rightf)
            rightf(this, rightp);
        else
            glBindTexture(GL_TEXTURE_2D, right);
        for (int pos = (topright || toprightf) ? toprights.h : 0; pos < size->h - ((bottomright || bottomrightf) ? bottomrights.w : 0); pos+=rights.h) {
            x1 = size->w - rights.w; y1 = pos;
            x2 = size->w; y2 = min(size->h - ((bottomright || bottomrightf) ? bottomrights.w : 0), pos + rights.h);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(x1, y1);
            glTexCoord2f(0, 1);
            glVertex2f(x1, y2);
            glTexCoord2f(1, 1);
            glVertex2f(x2, y2);
            glTexCoord2f(1, 0);
            glVertex2f(x2, y1);
            glEnd();
        }
    }


    if (center || centerf) {
        if (centerf)
            centerf(this, centerp);
        else
            glBindTexture(GL_TEXTURE_2D, center);
        for (int pos = (left || leftf) ? lefts.w : 0; pos < size->w - ((right || rightf) ? rights.w : 0); pos+=centers.w) {
            for (int pos2 = (top || topf) ? tops.h : 0; pos2 < size->h - ((bottom || bottomf) ? bottoms.h : 0); pos2+=centers.h) {


                y1 = pos2;
                y2 = min(size->h - ((bottom || bottomf) ? bottoms.h : 0), pos2 + centers.h);

                x1 = pos;
                x2 = min(size->w - ((right || rightf) ? rights.w : 0), pos + centers.w);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex2f(x1, y1);
                glTexCoord2f(0, 1);
                glVertex2f(x1, y2);
                glTexCoord2f(1, 1);
                glVertex2f(x2, y2);
                glTexCoord2f(1, 0);
                glVertex2f(x2, y1);
                glEnd();
            }
        }
    }



    glDisable(GL_TEXTURE_2D);

}

void glictSkinner::SetTL(GLuint textureid, glictSize *size ) {
    this->topleftf = NULL;
    this->topleft = textureid;
    if (size) this->toplefts = *size;
}
void glictSkinner::SetTL(glictBindCallback bindf, void* param, glictSize *size) {
    this->topleftf = bindf;
    this->topleftp = param;
    if (size) this->toplefts = *size;
}
void glictSkinner::SetTR(GLuint textureid, glictSize *size ) {
    this->toprightf = NULL;
    this->topright = textureid;
    if (size) this->toprights = *size;
}
void glictSkinner::SetTR(glictBindCallback bindf, void* param, glictSize *size) {
    this->toprightf = bindf;
    this->toprightp = param;
    if (size) this->toprights = *size;
}
void glictSkinner::SetBL(GLuint textureid, glictSize *size ) {
    this->bottomleftf = NULL;
    this->bottomleft = textureid;
    if (size) this->bottomlefts = *size;
}
void glictSkinner::SetBL(glictBindCallback bindf, void* param, glictSize *size) {
    this->bottomleftf = bindf;
    this->bottomleftp = param;
    if (size) this->bottomlefts = *size;
}
void glictSkinner::SetBR(GLuint textureid, glictSize *size ) {
    this->bottomrightf = NULL;
    this->bottomright = textureid;
    if (size) this->bottomrights = *size;
}
void glictSkinner::SetBR(glictBindCallback bindf, void* param, glictSize *size) {
    this->bottomrightf = bindf;
    this->bottomrightp = param;
    if (size) this->bottomrights = *size;
}
void glictSkinner::SetTop(GLuint textureid, glictSize *size ) {
    this->topf = NULL;
    this->top = textureid;
    if (size) this->tops = *size;
}
void glictSkinner::SetTop(glictBindCallback bindf, void* param, glictSize *size) {
    this->topf = bindf;
    this->topp = param;
    if (size) this->tops = *size;
}
void glictSkinner::SetBottom(GLuint textureid, glictSize *size ) {
    this->bottomf = NULL;
    this->bottom = textureid;
    if (size) this->bottoms = *size;
}
void glictSkinner::SetBottom(glictBindCallback bindf, void* param, glictSize *size) {
    this->bottomf = bindf;
    this->bottomp = param;
    if (size) this->bottoms = *size;
}
void glictSkinner::SetLeft(GLuint textureid, glictSize *size ) {
    this->leftf = NULL;
    this->left = textureid;
    if (size) this->lefts = *size;
}
void glictSkinner::SetLeft(glictBindCallback bindf, void* param, glictSize *size) {
    this->leftf = bindf;
    this->leftp = param;
    if (size) this->lefts = *size;
}
void glictSkinner::SetRight(GLuint textureid, glictSize *size ) {
    this->rightf = NULL;
    this->right = textureid;
    if (size) this->rights = *size;
}
void glictSkinner::SetRight(glictBindCallback bindf, void* param, glictSize *size) {
    this->rightf = bindf;
    this->rightp = param;
    if (size) this->rights = *size;
}

void glictSkinner::SetCenter(GLuint textureid, glictSize *size ) {
    this->centerf = NULL;
    this->center = textureid;
    if (size) this->centers = *size;
}
void glictSkinner::SetCenter(glictBindCallback bindf, void* param, glictSize *size) {
    this->centerf = bindf;
    this->centerp = param;
    if (size) this->centers = *size;
}

glictSize *glictSkinner::GetLeftSize() {
    return &(this->lefts);
}
glictSize *glictSkinner::GetTopSize() {
    return &(this->tops);
}
glictSize *glictSkinner::GetRightSize() {
    return &(this->rights);
}
glictSize *glictSkinner::GetBottomSize() {
    return &(this->bottoms);
}
