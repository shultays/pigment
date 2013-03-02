#ifndef DynamicShotTestable_H
#define DynamicShotTestable_H

#include <d3d9.h>
#include <d3dx9.h>


#include "ShotTestable.h"

#define MAX_DynamicShotTestable 128

class DynamicShotTestable{
public:
  static int DynamicShotTestablenum;
  static DynamicShotTestable* allDynamicShotTestables[MAX_DynamicShotTestable];

  void addDynamicShotTestableList();

  virtual int shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist) = 0;

  virtual void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize) = 0;
};

#endif