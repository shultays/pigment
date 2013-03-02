

#include "SolidX.h"

#include "Game.h"

SolidX::~SolidX(){
  delete[] normals;
  delete[] normalizeds;
}

#include "Math.h"


LPD3DXMESH box;
SolidX::SolidX(char *xFile):PaintableX(xFile){

	D3DXCreateBox(Tools::D3DDevice, 1,1,1,&box, NULL);
  size = 1;
  coors = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  rotation = 0;
  recalculateWorld();

  minBounds = new D3DXVECTOR3[subsetNum];
  maxBounds = new D3DXVECTOR3[subsetNum];

  //
  trianglenum = mesh->GetNumFaces();
  int vertexnum = mesh->GetNumVertices();

	PBYTE lock = NULL;


  while(FAILED(mesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID *)&lock)));
  WORD *indexbuffercopy = new WORD[trianglenum*3];
  ZeroMemory(indexbuffercopy, sizeof(WORD)*trianglenum*3);
  memcpy(indexbuffercopy, lock, sizeof(WORD)*trianglenum*3);
  mesh->UnlockIndexBuffer();



	PBYTE pVertices = NULL;
	Vertex *m_pVertices = new Vertex[vertexnum];
	while(FAILED(mesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID *)&pVertices)));

  ZeroMemory(m_pVertices, sizeof(Vertex)*vertexnum);


	memcpy(m_pVertices, pVertices, sizeof(Vertex)*vertexnum);
  mesh->UnlockVertexBuffer();

  
  triangles = new D3DXVECTOR3[trianglenum*3];
  normals = new D3DXVECTOR3[trianglenum];
  normalizeds = new D3DXVECTOR3[trianglenum];

  for(int i=0; i<trianglenum*3; i++){
    int z = indexbuffercopy[i];
    triangles[i] = D3DXVECTOR3( m_pVertices[z].X,  m_pVertices[z].Y,  m_pVertices[z].Z);
  }

  for(int i=0; i<trianglenum; i++){
    normals[i] = Tools::Cross(triangles[i*3+1] - triangles[i*3], triangles[i*3+2] - triangles[i*3]);
    normalizeds[i] = Tools::Normalize(normals[i]);
  }

  for(int i=0; i<subsetNum; i++){
    minBounds[i] = D3DXVECTOR3(1000000, 1000000, 1000000);
    maxBounds[i] = D3DXVECTOR3(-1000000, -1000000, -1000000);

    for(int j=attTable[i].VertexStart; j<attTable[i].VertexStart+attTable[i].VertexCount; j++){
      
      int z = j;

      if(m_pVertices[z].X < minBounds[i].x) minBounds[i].x  = m_pVertices[z].X;
      if(m_pVertices[z].Y < minBounds[i].y) minBounds[i].y  = m_pVertices[z].Y;
      if(m_pVertices[z].Z < minBounds[i].z) minBounds[i].z  = m_pVertices[z].Z;

      if(m_pVertices[z].X > maxBounds[i].x) maxBounds[i].x  = m_pVertices[z].X;
      if(m_pVertices[z].Y > maxBounds[i].y) maxBounds[i].y  = m_pVertices[z].Y;
      if(m_pVertices[z].Z > maxBounds[i].z) maxBounds[i].z  = m_pVertices[z].Z;
      
    }
  }

  delete[] m_pVertices;
  delete[] indexbuffercopy;


  addHitTest();
  addStaticShotTestableList();

}

void SolidX::paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  PaintableX::paint(index, pU, pV, c, circleSize);
}
void SolidX::draw(int m){
	/*
  for(int z = 0; z<subsetNum; z++){
  D3DXMATRIX w, w1, w2;

  D3DXVECTOR3 l = maxBounds[z] - minBounds[z];
  D3DXVECTOR3 p = minBounds[z] + l/2;

  D3DXMatrixTranslation(&w1, p.x, p.y, p.z);
  D3DXMatrixScaling(&w2, l.x+5, l.y+5, l.z+5);

  w = w2*w1*World;

  effect->SetMatrix( "World", &(w));
  effect->SetMatrix( "WorldViewProjection", &(w * (*View) * (*Proj)) );
  
  effect->SetFloatArray("ConstantColor", D3DXVECTOR4(1.0,1.0,1.0,0.4),4);

  effect->Begin(0, 0);
  effect->BeginPass(m);
  box->DrawSubset(0);
  effect->EndPass();
  effect->End();
  }*/

  PaintableX::draw(m);
}

inline int SolidX::TriInRange(float h1, float h2, float r, D3DXVECTOR3 *pos, D3DXVECTOR3 opos,  
                             int m, D3DXVECTOR3 *res, float *speed, float DeltaTime){

  int inr, i;
  int lower, higher;
   
  int numverts;
  D3DXVECTOR3 verts[6];
  D3DXVECTOR3 verts2[6];
       
  D3DXVECTOR3 closest,org,cpl,normal,oorg;
  float cdis, ndis;

  D3DXVECTOR3 *vPoly = triangles+m*3;

  org=*pos;
  org.y=0;

  oorg=opos;
  oorg.y=0;

  lower=0;
  higher=0;
  inr=0;
  numverts=0;

  if (vPoly[0].y<h2) lower=1;
  else  if (vPoly[1].y<h2) lower=1;
  else  if (vPoly[2].y<h2) lower=1;

  if (vPoly[0].y>h1) higher=1;
  else if (vPoly[1].y>h1) higher=1;
  else if (vPoly[2].y>h1) higher=1;

  if( !higher && ! lower) return 0;

  lower = 0;
  higher = 0;
  if(vPoly[0].x > org.x - RADIUS) lower = 1;
  else if(vPoly[1].x > org.x - RADIUS) lower = 1;
  else if(vPoly[2].x > org.x - RADIUS) lower = 1;
   
  if(vPoly[0].x < org.x + RADIUS) higher = 1;
  else if(vPoly[1].x < org.x + RADIUS) higher = 1;
  else if(vPoly[2].x < org.x + RADIUS) higher = 1;

  if( !higher && ! lower) return 0;
    
  lower = 0;
  higher = 0;
  if(vPoly[0].z > org.z - RADIUS) lower = 1;
  else if(vPoly[1].z > org.z - RADIUS) lower = 1;
  else if(vPoly[2].z > org.z - RADIUS) lower = 1;
    
  if(vPoly[0].z < org.z + RADIUS) higher = 1;
  else if(vPoly[1].z < org.z + RADIUS) higher = 1;
  else if(vPoly[2].z < org.z + RADIUS) higher = 1;

  if( !higher && ! lower) return 0;

  if (1){
    for(i=0; i<3; i++){
       verts[i] = vPoly[i];
    }
    verts[3] = vPoly[0];
    float a, b;

    int numverts2 = 0;


    for(i=0; i<3; i++){
      if(verts[i].y > h1 && verts[i+1].y > h1){ //icerden iceri, ikinciyi al
        verts2[numverts2] = verts[i+1];
        numverts2++;
      }else if(verts[i].y > h1 && verts[i+1].y <= h1){ //icerden disari, ikinci clip ve onu al
          
        a = (verts[i+1].x - verts[i].x)/(verts[i+1].y - verts[i].y);
        b = verts[i].x - a*verts[i].y;
        verts2[numverts2].x = a*h1 + b;

        a = (verts[i+1].z - verts[i].z)/(verts[i+1].y - verts[i].y);
        b = verts[i].z - a*verts[i].y;
        verts2[numverts2].z = a*h1 + b;

        verts2[numverts2].y = h1;

        numverts2++;

      }else if(verts[i].y <= h1 && verts[i+1].y > h1){ //dýþardan içeri, ilk clip, ikinci direk al
        a = (verts[i+1].x - verts[i].x)/(verts[i+1].y - verts[i].y);
        b = verts[i].x - a*verts[i].y;
        verts2[numverts2].x = a*h1 + b;

        a = (verts[i+1].z - verts[i].z)/(verts[i+1].y - verts[i].y);
        b = verts[i].z - a*verts[i].y;
        verts2[numverts2].z = a*h1 + b;

        verts2[numverts2].y = h1;
        numverts2++;

        verts2[numverts2] = verts[i+1];
        numverts2++;
      }
    }


    verts2[numverts2] = verts2[0];

    numverts = 0;
    for(i=0; i<numverts2; i++){
      if(verts2[i].y <= h2 && verts2[i+1].y <= h2){ //icerden iceri, ikinciyi al
        verts[numverts] = verts2[i+1];
        verts[numverts].y = 0;
        numverts++;
      }else if(verts2[i].y <= h2 && verts2[i+1].y > h2){ //icerden disari, ikinci clip ve onu al
        
        a = (verts2[i+1].x - verts2[i].x)/(verts2[i+1].y - verts2[i].y);
        b = verts2[i].x - a*verts2[i].y;
        verts[numverts].x = a*h2 + b;
        a = (verts2[i+1].z - verts2[i].z)/(verts2[i+1].y - verts2[i].y);
        b = verts2[i].z - a*verts2[i].y;
        verts[numverts].z = a*h2 + b;

        verts[numverts].y = 0;
        numverts++;

      }else if(verts2[i].y > h2 && verts2[i+1].y <= h2){ //dýþardan içeri, ilk clip, ikinci direk al
        a = (verts2[i+1].x - verts2[i].x)/(verts2[i+1].y - verts2[i].y);
        b = verts2[i].x - a*verts2[i].y;
        verts[numverts].x = a*h2 + b;

        a = (verts2[i+1].z - verts2[i].z)/(verts2[i+1].y - verts2[i].y);
        b = verts2[i].z - a*verts2[i].y;
        verts[numverts].z = a*h2 + b;
        verts[numverts].y = 0;
        numverts++;

        verts[numverts] = verts2[i+1];
        verts[numverts].y = 0;
        numverts++;
      }
    }

    if(numverts == 0) return 0;

    verts[numverts]=verts[0];
  
    closest=verts[0];
    cdis=100000;
    D3DXVECTOR3 resnc;
    for (i=0;i<numverts;i++){               
      cpl=Tools::ClosestPointOnLine(verts[i],verts[i+1],org);
      ndis=Tools::Distance(cpl,org);
      if (ndis<cdis){
        cdis=ndis;
        closest=cpl;
      }
    }

   
    if (cdis<r){
    
      if(1){
          
        if(cdis != 0){
          res->x = org.x + (closest.x - org.x)*r/cdis;
          res->z = org.z + (closest.z - org.z)*r/cdis;          
        }else{
          res->x = org.x;
          res->z = org.z;
        }   
        res->y = BOTTOM/size+(normals[m].x*(res->x - vPoly[0].x) + normals[m].z*(res->z - vPoly[0].z))/(-normals[m].y) + vPoly[0].y;
            
        if(res->y > h1+7500*DeltaTime  || (normalizeds[m].y < 0.55 && normalizeds[m].y > -0.55)){

            *res = closest;
            return 3;
        }
        if(normals[m].y > 0){         
          res->y = BOTTOM/size+(normals[m].x*(res->x - vPoly[0].x) + normals[m].z*(res->z - vPoly[0].z))/(-normals[m].y) + vPoly[0].y;
            
        
          speed[3] = 100;
          return 1;
        }else if(normals[m].y < 0){
          res->y = -TOP/size+(normals[m].x*(res->x - vPoly[0].x) + normals[m].z*(res->z - vPoly[0].z))/(-normals[m].y) + vPoly[0].y;
           
       
          return 1;
        }
      }
    }

    int t1 = 1, t2 = 1;
  
    for(i=0; i<numverts; i++){

      D3DXVECTOR3 v1 = verts[i+1] - verts[i];
      D3DXVECTOR3 v2 = org - verts[i];
      if(v1.x*v2.z - v1.z*v2.x < -0.0001) t1 = 0;
      else if(v1.x*v2.z - v1.z*v2.x > +0.0001) t2 = 0;
      //if(v1.x*v1.x + v1.z*v1.z < 1) t1 = t2 = 0;
    }

    if(t1 || t2){
      speed[0] = 0;
      speed[1] = 0;
      speed[2] = 0;
      if(normals[m].y > 0){
        
        res->y = BOTTOM/size+(normals[m].x*(pos->x - vPoly[0].x) + normals[m].z*(pos->z - vPoly[0].z))/(-normals[m].y) + vPoly[0].y;
        speed[3] = 100.0;

        return 1;
      }else if(normals[m].y < 0){
          
        res->y = -TOP/size+(normals[m].x*(pos->x - vPoly[0].x) + normals[m].z*(pos->z - vPoly[0].z))/(-normals[m].y) + vPoly[0].y;
          
        return 1;
      }
    }
  }


  return 0;
}

int SolidX::HitTest(D3DXVECTOR3 oldn, D3DXVECTOR3 *newn, float *speed, float DeltaTime){

  D3DXMATRIX Translate;

  
  D3DXMatrixTranslation(&Translate, oldn.x, oldn.y, oldn.z);
  Translate = Translate*WorldInverse;

  oldn = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);

  D3DXMatrixTranslation(&Translate, newn->x, newn->y, newn->z);
  Translate = Translate*WorldInverse;
  *newn = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);

  int ret = 0;

  for(int j=0; j<subsetNum; j++){
    D3DXVECTOR3 testmin = minBounds[j];
    D3DXVECTOR3 testmax = maxBounds[j];

    testmin.x -= RADIUS/size + 100;
    testmin.z -= RADIUS/size + 100;
    
    testmax.x += RADIUS/size + 100;
    testmax.z += RADIUS/size + 100;

    testmin.y -= TOP/size + 100;
    testmax.y += BOTTOM/size + 100;

    if(newn->x < testmin.x || newn->y < testmin.y || newn->z < testmin.z
    ||newn->x > testmax.x || newn->y > testmax.y || newn->z > testmax.z) continue;

    for(int i=attTable[j].FaceStart; i<attTable[j].FaceStart+(attTable[j].FaceCount); i++){
      D3DXVECTOR3 res;
      int r;
      if(r = TriInRange(newn->y-BOTTOM/size, newn->y+TOP/size, RADIUS/size, newn, oldn, i, &res, speed, DeltaTime)){
        ret = 1;
        D3DXVECTOR3 bnorm=*newn-res;

        bnorm.y=0;

        float bignum=sqrt((bnorm.x*bnorm.x)+(bnorm.z*bnorm.z));
        bnorm.x/=bignum;
        bnorm.z/=bignum;

        if(r == 1){
          newn->y = res.y;
          
          speed[0] = 0;
          speed[1] = 0;
          speed[2] = 0;

          //newn->x = res.x+16*bnorm.x;
          //newn->z = res.z+16*bnorm.z;
        }else{
          newn->x = res.x+RADIUS*bnorm.x;
          newn->z = res.z+RADIUS*bnorm.z;
        }
      }
    }
  }
  
  D3DXMatrixTranslation(&Translate, newn->x, newn->y, newn->z);
  Translate = Translate*World;
  *newn = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);


  return ret;
}

int SolidX::shotTest(D3DXVECTOR3 pos, D3DXVECTOR3 dir,
                          
  int *index, float *pU, float* pV, float *dist){

  D3DXMATRIX Translate;

  
  D3DXMatrixTranslation(&Translate, pos.x, pos.y, pos.z);
  Translate = Translate*WorldInverse;

  pos = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);


  
  D3DXMatrixTranslation(&Translate, dir.x, dir.y, dir.z);
  Translate._44 = 0;
  Translate = Translate*WorldInverse;

  dir = D3DXVECTOR3((Translate)._41, (Translate)._42, (Translate)._43);


  BOOL bHit = FALSE;
  DWORD dwFace;
  FLOAT fBary1, fBary2, fDist;

  D3DXIntersect(mesh, &pos, &dir, &bHit, &dwFace, &fBary1, &fBary2, &fDist,
                               NULL, NULL);

  if(bHit){
    *pU = fBary1;
    *pV = fBary2;
    *dist = fDist;
    *index = dwFace;
    return 1;
  }
  return 0;
}


  
void SolidX::bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  paint(index, pU, pV, c, circleSize);
}

int SolidX::getSide(int index, float pU, float pV, D3DXVECTOR3* coor){

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

  D3DXVECTOR3 pos = tri[0].pos + (tri[1].pos-tri[0].pos)*pU + (tri[2].pos-tri[0].pos)*pV;
  D3DXVECTOR3 norm = Tools::Normalize(Tools::Cross(tri[1].pos-tri[0].pos,tri[2].pos-tri[0].pos));

  *coor = pos;

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

  u*=paintSize;
  v*=paintSize;


  D3DLOCKED_RECT temp; 
  if(paints[subset]->LockRect(0, &temp, 0, 0) != D3D_OK) ALERT;
  unsigned int *a = (unsigned int*)temp.pBits;

  unsigned int c = a[(int)u+(int)v*paintSize];

  if(paints[subset]->UnlockRect(0) != D3D_OK) ALERT;

  if(c>>24 < 10) return -1;
 
  if((c>>16)&255 > 10) return 0;
  if((c)&255 > 10) return 1;
  return -1;
}