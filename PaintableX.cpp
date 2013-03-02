#include "PaintableX.h"

#include "Game.h"

PaintableX::PaintableX(char *c):BasicX(c){

  D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();

  paints = new IDirect3DTexture9*[numMaterials];
  paintSizes = new int[numMaterials];
  paintMaxUV = new D3DXVECTOR2[numMaterials];
  paintMinUV = new D3DXVECTOR2[numMaterials];


  DWORD tempDWORD;
  mesh->GetAttributeTable(NULL,&tempDWORD);

  attTable = new D3DXATTRIBUTERANGE[subsetNum];
  mesh->GetAttributeTable(attTable,&tempDWORD);

  D3DVERTEX *vertices;
  mesh->LockVertexBuffer(0, (LPVOID*)(&vertices));



  for(DWORD i=0; i<numMaterials; i++){
    paintMaxUV[i].x = 0;
    paintMaxUV[i].y = 0;
    paintMinUV[i].x = 10000;
    paintMinUV[i].y = 10000;

    double centerx=0, centery=0, centerz=0;
    for(DWORD j=attTable[i].VertexStart; j<attTable[i].VertexStart+attTable[i].VertexCount;j++){


      if(vertices[j].UV.x> paintMaxUV[i].x)  paintMaxUV[i].x = vertices[j].UV.x;
      if(vertices[j].UV.y > paintMaxUV[i].y)  paintMaxUV[i].y = vertices[j].UV.y;
      
      if(vertices[j].UV.x < paintMinUV[i].x)  paintMinUV[i].x = vertices[j].UV.x;
      if(vertices[j].UV.y < paintMinUV[i].y)  paintMinUV[i].y = vertices[j].UV.y;

      centerx += vertices[j].pos.x/attTable[i].VertexCount;
      centery += vertices[j].pos.y/attTable[i].VertexCount;
      centerz += vertices[j].pos.z/attTable[i].VertexCount;
    }

    float dist;
    
   
    D3DXComputeBoundingSphere(&vertices[attTable[i].VertexStart].pos, attTable[i].VertexCount,
      sizeof(D3DVERTEX),  &D3DXVECTOR3(centerx, centery, centerz), &dist);


    int l2 = dist/128;
    if(l2 == 0) l2 = 2;
    if(l2 > 16) l2 = 16;

    paintSizes[i] = l2*128;

    
    Tools::D3DDevice->CreateTexture(paintSizes[i], paintSizes[i], 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &paints[i], NULL); 

  }
  mesh->UnlockVertexBuffer();
  
}



inline int PaintableX::pixelInSphere(int i, int j, D3DXVECTOR3 &pos, D3DVERTEX *tri,
                  D3DXVECTOR2 &uvMax, D3DXVECTOR2 &uvMin, float AreaABC
                 , int paintSize , float circleSize){

  
      D3DXVECTOR2 p = D3DXVECTOR2((float)abs(i)/paintSize
      ,(float)abs(j)/paintSize);
    
      
  p.x *= (uvMax.x-uvMin.x);
  p.y *= (uvMax.y-uvMin.y);
  p.x += uvMin.x;
  p.y += uvMin.y;

  float AreaPBC = (tri[2].UV-p).x*(tri[0].UV-p).y -  (tri[2].UV-p).y*(tri[0].UV-p).x;
  float pU1 = AreaPBC / AreaABC;
  float AreaPCA = (tri[0].UV-p).x*(tri[1].UV-p).y -  (tri[0].UV-p).y*(tri[1].UV-p).x;
  float pV1 = AreaPCA / AreaABC;
  
  D3DXVECTOR3 posp = tri[0].pos + (tri[1].pos-tri[0].pos)*pU1 + (tri[2].pos-tri[0].pos)*pV1;

  return Tools::Distance2(pos, posp) < (circleSize)/(size*size);

}

void PaintableX::paint(int index, float pU, float pV,                 
                     D3DXVECTOR4 c, float circleSize){


  LPDIRECT3DVERTEXBUFFER9 pVB;
  LPDIRECT3DINDEXBUFFER9 pIB;

  mesh->GetVertexBuffer( &pVB );
  mesh->GetIndexBuffer( &pIB );

  WORD* pIndices;
  D3DVERTEX* pVertices;

  int subset = -1;

  for(int i=0; i<subsetNum; i++){
    if(index >= attTable[i].FaceStart && index < attTable[i].FaceStart + attTable[i].FaceCount){
      subset = i;
      break;
    }
  }

  
  int paintSize = paintSizes[subset];

 
  D3DVERTEX tri[3];
  
  pIB->Lock(0, 0, (void**)&pIndices, 0 );
  pVB->Lock(0, 0, (void**)&pVertices, 0 );

    tri[0] = pVertices[pIndices[index*3+0]];
    tri[1] = pVertices[pIndices[index*3+1]];
    tri[2] = pVertices[pIndices[index*3+2]];

  pIB->Unlock();
  pVB->Unlock();

  
  DWORD alp = c.w*255;
  DWORD r = c.x*255;
  DWORD g = c.y*255;
  DWORD b = c.z*255; 


  DWORD color = (alp<<24)&0xFF000000 | (r<<16)&0x00FF0000
          | (g<<8)&0x0000FF00
          | (b)&0x000000FF; 



  D3DXVECTOR3 pos = tri[0].pos + (tri[1].pos-tri[0].pos)*pU + (tri[2].pos-tri[0].pos)*pV;
  D3DXVECTOR3 norm = Tools::Normalize(Tools::Cross(tri[1].pos-tri[0].pos,tri[2].pos-tri[0].pos));



  D3DXVECTOR2 uvMax = paintMaxUV[subset];
  D3DXVECTOR2 uvMin = paintMinUV[subset];

  
  D3DXVECTOR2 UV1 = tri[1].UV-tri[0].UV;
  D3DXVECTOR2 UV2 = tri[2].UV-tri[0].UV;

  float AreaABC = UV1.x*UV2.y - UV1.y*UV2.x;

 
  float u, v;

  u = tri[0].UV.x+ (tri[1].UV.x-tri[0].UV.x)*pU + (tri[2].UV.x-tri[0].UV.x)*pV;
  v = tri[0].UV.y+ (tri[1].UV.y-tri[0].UV.y)*pU + (tri[2].UV.y-tri[0].UV.y)*pV;

  u -= paintMinUV[subset].x;
  v -= paintMinUV[subset].y;

  u /= (paintMaxUV[subset]-paintMinUV[subset]).x;
  v /= (paintMaxUV[subset]-paintMinUV[subset]).y;
  
  float cx = u*paintSize;
  float cy = v*paintSize;
 
  int xmax = paintSize;
  for(int i=cx; i<paintSize*2; i++){
    if(!pixelInSphere(i, cy, pos, tri, uvMax, uvMin, AreaABC, paintSize, circleSize*circleSize)){
      xmax = i;
      break;
    }
  }

  int ymax = paintSize;
  for(int j=cy; j<paintSize*2; j++){
    if(!pixelInSphere(cx, j, pos, tri, uvMax, uvMin, AreaABC, paintSize, circleSize*circleSize)){
      ymax = j;
      break;
    }
  }

  int xmin = cx - (xmax - cx);
  int ymin = cy - (ymax - cy);

  
  float diff = (float)(xmax-xmin)/(ymax-ymin);
  diff = diff*diff;

  float rad = (xmax-xmin)*(xmax-xmin)/4.0f;


  if(xmin < 0) xmin = 0;
  if(ymin < 0) ymin = 0;
  if(xmax > paintSize) xmax = paintSize;
  if(ymax > paintSize) ymax = paintSize;
  
  D3DLOCKED_RECT temp; 

  RECT rect;

  rect.left = xmin;
  rect.right = xmax;
  rect.top = ymin;
  rect.bottom = ymax;


  //ALERT1(temp.Pitch);

  cx -= xmin;
  cy -= ymin;
  xmax = xmax-xmin;
  xmin = 0;

  ymax = ymax-ymin;
  ymin = 0;


  if(rect.left >= rect.right || rect.bottom <= rect.top) return;

  int t1, t2;

  if(paints[subset]->LockRect(0, &temp, &rect, 0) != D3D_OK) ALERT;
  unsigned int *a = (unsigned int*)temp.pBits;

  for(int i=0; i<xmax; i++){
    for(int j=0; j<ymax; j++){
      
      t1 = i-cx;
      t2 = j-cy;

      if( t1*t1 + t2*t2*diff < rad){
          a[i+j*paintSize] = color;
      }
    }
  }
  

  if(paints[subset]->UnlockRect(0) != D3D_OK) ALERT;
  
}



void PaintableX::draw(int m){
  effect->SetMatrix( "World", &(World));
  effect->SetMatrix( "WorldViewProjection", &(World * (*View) * (*Proj)) );
  for(int i=0; i<subsetNum; i++){
 
    effect->SetTexture( "PaintTexture0", paintTexture);
    if(hasTexture[i]){
      effect->SetTexture( "Texture0", textures[i] );
      effect->SetFloatArray("ConstantColor", D3DXVECTOR4(0.0,0.0,0.0,0.0),4);
    }else{
      effect->SetFloatArray("ConstantColor", color[i],4);
  //    if(Tools::DInput->KeyDown(DIK_O)) effect->SetFloatArray("ConstantColor", D3DXVECTOR4(0.0, 0.0, 0.0, 1.0) ,4);
    }
      effect->SetTexture( "Paint0",  paints[i] );
      effect->SetFloatArray("maxUV", (paintMaxUV[i]-paintMinUV[i]), 2);
      effect->SetFloatArray("minUV", paintMinUV[i], 2);
	  effect->Begin(0, 0);
	  effect->BeginPass(m);
		mesh->DrawSubset(i);
	  effect->EndPass();
	  effect->End();
  }
}

