#ifndef ShadowVolumeGPU_H
#define ShadowVolumeGPU_H


#include <d3d9.h>
#include <d3dx9.h>
#include "BasicX.h"
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"
#include "ShadowVolume.h"


class ShadowVolumeGPU : public ShadowVolume{

public:

  static ID3DXEffect *shadowEffect;

  BasicX *base;
  ShadowVolumeGPU(BasicX *b);



  ID3DXMesh* shadowMesh;

  void drawShadowVolume(D3DXVECTOR3 light);
};

#endif