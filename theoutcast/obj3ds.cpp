#ifdef _MSC_VER
    #include <stdlib.h> // doesnt like exit() defined elsewhere.
#endif

#include <string.h>
#include <malloc.h>
#include <math.h>

#include <GL/glut.h>
#include "obj3ds.h"
#include "texmgmt.h"
extern float fps;
bool uselists = true;
/* public functions */


Obj3ds::Obj3ds() {
}

Obj3ds::Obj3ds(const char* filename) {
    printf("OBJ3DS Trying to load %s\n", filename);
    if (!LoadFile(filename)) printf("Obj3ds NOT loaded\n");
}

Obj3ds::~Obj3ds() {
    if (data3ds) lib3ds_file_free(data3ds);
}

bool Obj3ds::LoadFile(const char* filename) {
    printf("Obj3ds:\tLoading %s.\n", filename);
    data3ds = lib3ds_file_load(filename);
    if (!data3ds) {
        printf("Obj3ds:\tFailed to load %s.\n", filename);
        return false;
    }
    else {
        for (Lib3dsMaterial *mat = data3ds->materials; mat; mat = mat->next) {
            if (mat)
                if (strlen(mat->texture1_map.name)) {
                    printf("Loading texture %s\n", mat->texture1_map.name);
                    mat->user.p = (void*)(new Texture(mat->texture1_map.name));
                }
        }
        lib3ds_file_eval(data3ds,0);
        return true;
    }
}

bool Obj3ds::Render() {
    Lib3dsNode *p;
//    printf("Rendering\n");
    if (!data3ds) {
    	printf("Can't render nonexisting object\n");
        return false;
    }
    //lib3ds_file_eval(data3ds, animation_frame += 25. / fps);

    glPushMatrix();
    glRotatef(270., 1., 0., 0.);
    for (p=data3ds->nodes; p!=0; p=p->next) {
        //printf("Rendering one of root nodes\n");
        this->RenderNode(p);
    }
    //printf("Popping\n");
    glPopMatrix();
    return true;
}


// this routine is based on Lib3ds's player.c sample
void Obj3ds::RenderNode(Lib3dsNode *node) {

  {
    Lib3dsNode *p;
    for (p=node->childs; p!=0; p=p->next) {
      this->RenderNode(p);
    }
  }
  static int pushes=0;
  if (node->type==LIB3DS_OBJECT_NODE) {
    if (strcmp(node->name,"$$$DUMMY")==0) {
      return;
    }

    if (!node->user.d) {
      Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(data3ds, node->name);
      ASSERT(mesh);
      if (!mesh) {
        return;
      }

    if (uselists) {
      node->user.d=glGenLists(1);
      glNewList(node->user.d, GL_COMPILE);
    } else {
      Lib3dsObjectData *d;

      glPushMatrix();

      d=&node->data.object;

      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
//      printf("Pivotization %d\n", pushes++);
    }

      {
        unsigned p;
        Lib3dsVector *normalL=(Lib3dsVector*)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

        {
          Lib3dsMatrix M;
          lib3ds_matrix_copy(M, mesh->matrix);
          lib3ds_matrix_inv(M);
//	  printf("Multiplying by matrix\n");
          glMultMatrixf(&M[0][0]);
        }
        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (p=0; p<mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
          Lib3dsMaterial *mat=0;
          if (f->material[0]) {
            mat=lib3ds_file_material_by_name(data3ds, f->material);
          }

          if (mat) {
            static GLfloat black[4]={0,0,0,1};
            float s;
            Texture *t = (Texture*)mat->user.p;
            if (t) {
                glEnable(GL_TEXTURE_2D);
//                printf("Applying texture\n");
		t->Bind();

                static Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
                static Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
                static Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, s);

            } else {
                glDisable(GL_TEXTURE_2D);

//                printf("Material application %s\n", f->material);
                glMaterialfv(GL_FRONT, GL_AMBIENT, black);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
                //glColor4fv(mat->diffuse);

            }
            s = pow(2, 10.0*mat->shininess);
            if (s>128.0) {
              s=128.0;
            }
            glMaterialf(GL_FRONT, GL_SHININESS, s);
          }
          else {
//            printf("Default material application\n");
            glDisable(GL_TEXTURE_2D);
            static Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
            static Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
            static Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
            //glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          }
          {
            int i;
//	    printf("Painting triangles\n");
//	    glDisable(GL_LIGHTING);
	    glColor4f(1,1,1,1);



	    glBegin(GL_TRIANGLES);
              //glNormal3fv(f->normal);
              for (i=0; i<3; ++i) {
                glNormal3fv(normalL[3*p+i]);
                if (mesh->texelL) {
                    glTexCoord2f(mesh->texelL[f->points[i]][0], 1.-mesh->texelL[f->points[i]][1]);

                }
                glVertex3fv(mesh->pointL[f->points[i]].pos);

              }
            glEnd();

//	    printf("Done with triangles\n");
          }
        }

        free(normalL);
	//printf("Freeing\n");
      }

      if (uselists) {
        glEndList();
	//printf("Ended list\n");
      } else {
//        printf("Pop %d\n", pushes--);
        glPopMatrix();
        //printf("Popped\n");
      }
    }

    if (node->user.d && uselists) {
      Lib3dsObjectData *d;

      glPushMatrix();
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(node->user.d);
      //glutSolidSphere(50.0, 20,20);
      glPopMatrix();
    }
  }
}

