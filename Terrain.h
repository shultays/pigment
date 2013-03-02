#ifndef TERRAIN_H
#define TERRAIN_H

#include "Camera.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Tools.h"
#include "HitTestable.h"
#include "DirectInput.h"
#include "Drawable.h"
#include "StaticShotTestable.h"

class Terrain : public Drawable, public HitTestable, public StaticShotTestable, public Paintable{
public:
  float terrain[513][513];

  float getZ(float x, float y);
  void setup(char *model);
  void draw(int m);

  IDirect3DTexture9* TerrainTexture;

  Terrain(char *model){
    setup(model);
  }

  void Release(void){
    TerrainTexture->Release();
  }

  int HitTest(D3DXVECTOR3 oldn, D3DXVECTOR3 *newn, float *speed, float DeltaTime);

  IDirect3DTexture9* paints[8][8];

  LPDIRECT3DVERTEXBUFFER9 v_buffer;
  LPDIRECT3DINDEXBUFFER9 i_buffer[1][8][8];
  
  int shotTest(D3DXVECTOR3 pos, D3DXVECTOR3 dir, int *index, float *pU, float* pV, float *dist);

  inline int checkTriangle(int i, int j, D3DXVECTOR3 &pos, D3DXVECTOR3 &dir,
	   int *index, float *pU, float* pV, float *dist);

  
  void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);

  void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);

  int paintSize;

  
  int getSide(int index, float pU, float pV, D3DXVECTOR3 *coor);
};


#endif