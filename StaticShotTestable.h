#ifndef STATICShotTestable_H
#define STATICShotTestable_H



#include <d3d9.h>
#include <d3dx9.h>
#include "ShotTestable.h"

#define MAX_STATICShotTestable 128

class StaticShotTestable{
public:
  static int StaticShotTestablenum;
  static StaticShotTestable* allStaticShotTestables[MAX_STATICShotTestable];

  void addStaticShotTestableList();

  virtual int shotTest(D3DXVECTOR3 pos, D3DXVECTOR3 dir, int *index, float *pU, float* pV, float *dist) = 0;

  virtual void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize) = 0;

};

#endif