#ifndef DYNAMICX_H
#define DYNAMICX_H

#include "Camera.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"
#include "HitTestable.h"
#include "BasicX.h"
#include "PaintableX.h"
#include "ShotTestable.h"
#include "DynamicShotTestable.h"


#define BOTTOM 80
#define TOP 20
#define RADIUS 25


class OrbitX : public PaintableX, public DynamicShotTestable{
public:

  OrbitX(char *name);
  float speeds, initPos, yaw;

  void draw(int m);

  

  int shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist);

  int rotateSelf;
  
  void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);
  void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);
};

#endif