#ifndef BULLET_H
#define BULLET_H


#include <d3d9.h>
#include <d3dx9.h>
#include "Drawable.h"

#include "NetworkStructs.h"


struct recievedBuff{
  int hitType;
  int owner;
  int id;
  int paintableNum;
  int index;
  float u;
  float v;
  D3DXVECTOR4 color;
} ;


class Bullet: public Drawable{
public:
  static ID3DXMesh *ball;

  D3DXVECTOR3 dir;

  DWORD initTime;
  float dieTime;
  int owner;

  float cpU, cpV;
  int cindex;
  int n;

  void setup();
  int tick(float deltaTime);

  int id;

  int side;
  static int nextId;

  void draw(int m);

  static ID3DXEffect *effectS;
  static Bullet* bullets[1024];
  static int bulletNum;
  static Bullet* add(D3DXVECTOR3 pos, D3DXVECTOR3 dir, int owner, DWORD currentTime, D3DXVECTOR4 c);

  static Bullet* add(New_Bullet_struct *nbs);
  static void tickAll(float deltaTime);

  static void gotHit( int hitType, int owner, int id, int paintableNum, int index, float u, float v, D3DXVECTOR4 c);

  static int recievedEarlyNum;
  static struct recievedBuff recBuff[128];

  D3DXVECTOR4 color;

  DWORD bornTime;

};

#endif