#ifndef BASICX_H
#define BASICX_H


#include <d3d9.h>
#include <d3dx9.h>
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"

class BasicX:public Drawable{

public:
  BasicX(char *);
  ID3DXMesh *mesh;
  IDirect3DTexture9** textures;

  void draw(int m);

  DWORD subsetNum;
  
  LPD3DXBUFFER materialBuffer;
  DWORD numMaterials;            

  int *hasTexture;
  D3DXVECTOR4 *color;
};

#endif