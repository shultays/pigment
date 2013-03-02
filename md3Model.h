#ifndef MD3MODEL_H
#define MD3MODEL_H

#include "Tools.h"
#include "Drawable.h"
  
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct customvertex_struct{
  float coors[3];
  float normal[3];
  float UV[2];
}typedef customvertex;

struct vec3s{
  float x,y,z;
}typedef vec3;

struct tag_struct{
  char name[64];
  vec3 org;
  vec3 ori[3];
}typedef tag;

struct shader_struct{
  char name[64];
  int index;
}typedef shader;

struct triangle_struct{
  int t0,t1,t2;
}typedef triangle;

struct texcoord_struct{
  float U,V;
}typedef texcoord;

struct vertex_struct{
  short x,y,z;
  short normal;
}typedef vertex;


struct surface_struct{
  int ident;
  char name[64];
  
  int flag;
  int numframes;
  int numshaders;
  int numverts;
  int numtris;
  
  int triangleoff;
  int shaderoff;
  int texcoordoff;
  int vertexoff;
  int end;
}typedef surface;

struct surfaceelements{
  shader *shaders;
  triangle *triangles;
  texcoord *texcoords;
  vertex *vertices;
}typedef surfaceelements;

struct frame_struct{
  
  vec3 minb, maxb, orgb;
  float radb;
  char nameb[16];
}typedef frame;
  

struct header_struct{
   int ident;
   int version; 
   char name[64];
   
   int flag;
   int framenum;
   int tagnum;
   int surfacenum;
   int skinnum;
 
   int frameoff;
   int tagoff;
   int surfaceoff;
   int eofoff;

}typedef header;

class md3Model: public Drawable{
public:
  header h;
  frame *frames;
  tag *tags;
  
  int *trianglenum;
  int *vertnum;

  surface *surfaces;
  surfaceelements *sel;
  
  short **indices;
  customvertex **allver;

  void load(char *file, char *skin, int s);

  void draw(int f1, int f2, float inter);

  void draw(int m);
  void clear(void);
  void release(void);

  LPDIRECT3DVERTEXBUFFER9 *v_buffer, *height_buffer;
  LPDIRECT3DINDEXBUFFER9 *i_buffer;
  LPDIRECT3DTEXTURE9 texture;  

  LPDIRECT3DVERTEXDECLARATION9 vertexDecl ;

  float min;
  float max;
};

#endif