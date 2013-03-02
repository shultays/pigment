#ifndef ShadowVolume_H
#define ShadowVolume_H


#include <d3d9.h>
#include <d3dx9.h>
#include "BasicX.h"
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"

#define MAX_SHADOW 1024

class ShadowVolume{
public:
  
  static int shadowSize;
  static ShadowVolume* allShadows[MAX_SHADOW];

  int shadowNum;

  void addShadowList();
  
  ShadowVolume::ShadowVolume();
  
  virtual void drawShadowVolume(D3DXVECTOR3 light) = 0;
};



#endif