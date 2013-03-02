#ifndef SHADOWTEST_H
#define SHADOWTEST_H


#include <d3d9.h>
#include <d3dx9.h>
#include "BasicX.h"
#include "Tools.h"
#include "DirectInput.h"
#include "Drawable.h"

#define MAX_SHADOW 32

class ShadowTest:public BasicX{


public:
  ShadowTest(char *);

  void calculateShadow(D3DXVECTOR3 light);
  void drawShadowVolume();

  int shadowVNum;
  D3DXVECTOR3 m_pVertices[32000];
  int m_dwNumVertices;


  static int shadowSize;
  static ShadowTest* allShadows[MAX_SHADOW];
  static ID3DXEffect *shadowEffect;

  int shadowNum;

  void addShadowList();
  WORD* pEdges;


  
    ID3DXMesh* shadowMesh;
};

#endif