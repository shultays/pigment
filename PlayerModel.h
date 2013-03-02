#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include "Tools.h"
#include "md3Model.h"
#include "Camera.h"
#include "Drawable.h"

struct translator_struct{
  float move[3];
  D3DXMATRIX matr;
}typedef translator;

class PlayerModel : public Drawable{
public:
  int v[6][4], b[12][4], n[7][4];

  int bnum;
  int nnum;
  int vnum;

  translator *bodytolegs, *headtobody, *weapontobody;

  void load(char *playerfile);

  static ID3DXEffect *md3Effect;

  void draw(int m);

  void release(void);

  float framenumlow; 
  float framenumhigh;

  float coors[3];
  
  md3Model low;
  md3Model high;
  md3Model head;

  md3Model *weapon;

  float rotateHor, rotateVer;
  float rotateAll;
  float legAngle;


  float scale;
  float yMove;

  char showCyl;
  LPD3DXMESH cyl;

  int low_reversed;
  
  float life;

  int side;

  int hasFlag;

  float minY;

  int isDead;
};

#endif