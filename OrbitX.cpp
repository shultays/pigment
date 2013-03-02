#include "OrbitX.h"

#include "Game.h"


typedef struct{
  D3DXVECTOR3 pos;
  D3DXVECTOR3 norm;
  D3DXVECTOR2 UV;
} D3DVERTEX;
  

OrbitX::OrbitX(char *xFile) :PaintableX(xFile){

  size = 1;
  coors = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  rotation = 0;
  recalculateWorld();

  rotateSelf = 0;
  addDynamicShotTestableList();
}


void OrbitX::draw(int m){

  D3DXMATRIX  Rotate, Rotate2;

  D3DXMatrixRotationX(&Rotate, yaw);
  D3DXMatrixRotationY(&Rotate2, initPos+(Tools::g->CurrentTime - Networks::timediff)*(speeds));

  World = Rotate*Rotate2;


  if(rotateSelf) World = Rotate2*World;
  Tools::m4_inverse(WorldInverse, World);

  PaintableX::draw(m);
}

int OrbitX::shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist) {
  D3DXMATRIX Translate;
  
  D3DXMatrixTranslation(&Translate, posO.x, posO.y, posO.z);
  Translate = Translate*WorldInverse;

  posO = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);
  
  D3DXMatrixTranslation(&Translate, posN.x, posN.y, posN.z);
  Translate = Translate*WorldInverse;

  posN = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);

  BOOL bHit = FALSE;
  DWORD dwFace;
  FLOAT fBary1, fBary2, fDist;

  D3DXIntersect(mesh, &posO, &(posN-posO), &bHit, &dwFace, &fBary1, &fBary2, &fDist,
                               NULL, NULL);

 

  if(bHit){
    if(fDist < 1.0){
      Tools::g->con.buff << fDist << " " <<Tools::Distance(posO, posN);
      Tools::g->con.Print();
      *pU = fBary1;
      *pV = fBary2;
      *dist = fDist;
      *index = dwFace;
      return 1;
    }    
  }
  return 0;
}


void OrbitX::paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  PaintableX::paint(index, pU, pV, c, circleSize);
}


void OrbitX::bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  paint(index, pU, pV, c, circleSize);
}