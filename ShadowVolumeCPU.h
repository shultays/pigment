#ifndef ShadowVolumeCPU_H
#define ShadowVolumeCPU_H


#include <d3d9.h>
#include <d3dx9.h>
#include "BasicX.h"
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"
#include "ShadowVolume.h"

class ShadowVolumeCPU : public ShadowVolume{
public:
  
  static ID3DXEffect *shadowEffect;

  BasicX *base;
  ShadowVolumeCPU(BasicX *b);

  void calculateShadow(D3DXVECTOR3 light);
  void drawShadowVolume(D3DXVECTOR3 light);


};

#endif