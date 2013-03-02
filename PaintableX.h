#ifndef PAINTABLE_X
#define PAINTABLE_X

#include "BasicX.h"
#include "ShotTestable.h"


class PaintableX: public BasicX , public Paintable{
public:

typedef struct{
  D3DXVECTOR3 pos;
  D3DXVECTOR3 norm;
  D3DXVECTOR2 UV;
} D3DVERTEX;
  
  IDirect3DTexture9** paints;

  D3DXVECTOR2 *paintMaxUV;
  D3DXVECTOR2 *paintMinUV;


  PaintableX(char *c);
  
  void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);


  int *paintSizes;
  int pixelInSphere(int i, int j, D3DXVECTOR3 &pos, D3DVERTEX *tri,
                  D3DXVECTOR2 &uvMax, D3DXVECTOR2 &uvMin, float AreaABC, int paintSize , float circleSize);


  D3DXATTRIBUTERANGE *attTable;

  
  virtual int shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist) = 0;


  void draw(int m);

};

#endif

