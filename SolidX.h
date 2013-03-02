#ifndef SolidX_H
#define SolidX_H

#include "Camera.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"
#include "HitTestable.h"


#include "PaintableX.h"
#include "StaticShotTestable.h"

#define BOTTOM 80
#define TOP 20
#define RADIUS 25


class SolidX : public HitTestable, public StaticShotTestable, public PaintableX{
public:

  int HitTest(D3DXVECTOR3 oldn, D3DXVECTOR3 *newn, float *speed, float DeltaTime);

  int trianglenum;
  D3DXVECTOR3 *triangles;
  D3DXVECTOR3 *normals;
  D3DXVECTOR3 *normalizeds;


  D3DXVECTOR3 *minBounds;
  D3DXVECTOR3 *maxBounds;

  SolidX(char* xFile);

  ~SolidX();


  void draw(int m);

  int TriInRange(float h1, float h2, float r, 
  D3DXVECTOR3 *pos,D3DXVECTOR3 opos, int m, D3DXVECTOR3 *res, float *speed, float DeltaTime);


  int shotTest(D3DXVECTOR3 pos, D3DXVECTOR3 dir, int *index, float *pU, float* pV, float *dist);
  
  void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);
  
  void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);

  int getSide(int index, float pU, float pV, D3DXVECTOR3 *coor);
};

#endif