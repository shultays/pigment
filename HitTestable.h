#ifndef HITTESTABLE_H
#define HITTESTABLE_H

#include <d3d9.h>
#include <d3dx9.h>

#define MAX_HITTESTABLE 128
#include "Drawable.h"

class HitTestable {
public:

  virtual int HitTest(D3DXVECTOR3 oldn, D3DXVECTOR3 *newn, float *speed, float DeltaTime) = 0;
  
  int HitTestplace;
 
  static int HitTestnum;
  static HitTestable* allHitTestables[MAX_HITTESTABLE];

  
  void addHitTest();
  void removeHitTest();

};

#endif