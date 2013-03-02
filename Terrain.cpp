#include "Terrain.h"


#include "Game.h"


#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct MESHVERTEX { D3DXVECTOR3 p, n; FLOAT tu, tv; };

void Terrain::setup(char *model){

  paintSize = 512;

  if(paints == NULL) ALERT;
  
  for(int i=0; i<128; i++){
    for(int j=0; j<128; j++){
      terrain[i][j] = 0;
    }
  }

	
  Tools::LoadTexture("./terrain2.jpg", TerrainTexture);

  MESHVERTEX v[129*129];


  D3DLOCKED_RECT rect;
  if(TerrainTexture->LockRect(0, &rect, NULL, D3DLOCK_READONLY) != D3D_OK) ALERT;

  unsigned char *arr = (unsigned char*)rect.pBits;
  for(int i=0; i<128; i++){
    for(int j=0; j<128; j++){
      terrain[i][j] = ((arr[(i*128+j)*4])/256.0f)*750;
      v[i*128 + j].p = D3DXVECTOR3((i-64)*10000/64.0f,  terrain[i][j], (j-64)*10000/64.0f);
      if(i>0 &&j>0)v[i*128 + j].n = Tools::Normalize(v[i*128 + j - 129].n + Tools::Cross(v[i*128 + j].p-v[i*128 + j-1].p, v[i*128 + j].p-v[i*128 + j-128].p));
      v[i*128 + j].tu = i/6.4f;
      v[i*128 + j].tv = j/6.4f;
      
    }
  }
  TerrainTexture->UnlockRect(0);
  
  int a = Tools::D3DDevice->CreateVertexBuffer(128*128*sizeof(MESHVERTEX),
                               0,
                               CUSTOMFVF,
                               D3DPOOL_MANAGED,
                               &v_buffer,
                               NULL);
  

  void *pVoid;
    
  while(v_buffer->Lock(0, 0, (void**)&pVoid, 0));
  memcpy(pVoid, v, 128*128*sizeof(MESHVERTEX));
  v_buffer->Unlock();

  short ibuff[16*16*6];

  for(int a=0; a<8; a++){
	  for(int b=0; b<8; b++){
      Tools::D3DDevice->CreateTexture(paintSize, paintSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &paints[a][b], NULL); 

      for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
          
		      int x = a*16+i;
		      int y = b*16+j;

          if(a==7 && i== 15) x--;
          if(b==7 && j== 15) y--;

          ibuff[i*16*6+j*6+0] = x*128 + y;
          ibuff[i*16*6+j*6+1] = x*128 + y + 1;
          ibuff[i*16*6+j*6+2] = x*128 + y + 128;

          ibuff[i*16*6+j*6+3] = x*128 + y + 128;
          ibuff[i*16*6+j*6+4] = x*128 + y + 1;
          ibuff[i*16*6+j*6+5] = x*128 + y + 129;
        }
      }

      Tools::D3DDevice->CreateIndexBuffer(16*16*6*sizeof(short),
                                  0,
                                  D3DFMT_INDEX16,
                                  D3DPOOL_MANAGED,
							                    &i_buffer[0][a][b],
                                  NULL);
      
      while(i_buffer[0][a][b]->Lock(0, 0, (void**)&pVoid, 0));
      memcpy(pVoid, ibuff, sizeof(short)*16*16*6);
      i_buffer[0][a][b]->Unlock();

	  }
  }


  TerrainTexture->Release();
  addDrawList();
  addHitTest();
  addStaticShotTestableList();
  TerrainTexture = Tools::LoadTexture("conc02.jpg");

  return;
  
  /*

  LPD3DXBUFFER materialBuffer;
  DWORD numMaterials;            
  Tools::LoadXFile(model, TerrainMesh, &materialBuffer,  &numMaterials);

  
  D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();

  TerrainTexture = Tools::LoadTexture(d3dxMaterials[0].pTextureFilename);
  
  int m_NumVertices = TerrainMesh->GetNumFaces();

  PBYTE pVertices = NULL;
  while(FAILED(TerrainMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID *)&pVertices)));

  ZeroMemory(terrain, 129*129*4);

  Vertex *m_pVertices = new Vertex[m_NumVertices];
  ZeroMemory(m_pVertices, sizeof(Vertex)*m_NumVertices);
  memcpy(m_pVertices, pVertices, sizeof(Vertex) * 129*129);
  

  TerrainMesh->UnlockVertexBuffer(); 
 
  int j = 129*64;

  double min = 20000, max = -200000;
  for(int i=0; i<129*129; i++){
    int x = (int)(((m_pVertices[i].X + 10000)*128)/20000);
    int y = (int)(((m_pVertices[i].Z + 10000)*128)/20000);
    if(x < 0 || y < 0 || x > 129 || y > 129){
    //  PFILE2("ERROR0 %f %f\n",m_pVertices[i].X, m_pVertices[i].Z );
    }
    if(terrain[x][y] != 0){
    //  PFILE2("ERROR1 %f %f\n",m_pVertices[i].X, m_pVertices[i].Z );
    }

    //PFILE3("%d %d %f\n", x, y, m_pVertices[i].Y);

    terrain[x][y] = m_pVertices[i].Y;


    if(min > terrain[x][y]) min = terrain[x][y];
    if(max < terrain[x][y]) max = terrain[x][y];


     // fprintf(f, "%f\n", m_pVertices[i].Y); 
  }



  
  IDirect3DTexture9* tt;
  Tools::LoadTexture("./terrain.jpg", tt);


  /*D3DLOCKED_RECT rect;
  if(tt->LockRect(0, &rect, NULL, D3DLOCK_READONLY) != D3D_OK) ALERT;

  unsigned char *arr = (unsigned char*)rect.pBits;
  for(int i=0; i<128; i++){
    for(int j=0; j<128; j++){
      arr[(i*128+j)*4 + 0] = (terrain[i][j]-min)*64.0f/(max-min);
      arr[(i*128+j)*4 + 1] = (terrain[i][j]-min)*64.0f/(max-min);
      arr[(i*128+j)*4 + 2] = (terrain[i][j]-min)*64.0f/(max-min);
      arr[(i*128+j)*4 + 3] = 127;
    }
  }

  Tools::f << "min:" << min << endl << "max:" << max << endl;

  tt->UnlockRect(0);

  if(D3DXSaveTextureToFile("./terrain2.bmp", D3DXIFF_BMP, tt, NULL) != D3D_OK) ALERT;

  delete[] m_pVertices;
  
  add();*/
}


float Terrain::getZ(float x, float y){
  x += 10000;
  y += 10000;

  float inc = 20000.0/128;

  int tx = (int)(x/inc);
  int ty = (int)(y/inc);

  float tx2 = x - inc*tx;
  float ty2 = y - inc*ty;

  float zhedef = 0;

  float a ,d;
  float b = terrain[tx+1][ty];
  float c = terrain[tx][ty+1];
  if (tx2 + ty2 > inc) {
    d = terrain[tx+1][ty+1];
    a = b - (d-c);
  } else {
    a = terrain[tx][ty];
    d = c + b-a;
  }
  float sol = a*(inc-ty2) + c*ty2;
  float sag = b*(inc-ty2) + d*ty2;
  sol /= inc;
  sag /= inc;
  zhedef = sol*(inc-tx2) + sag*tx2;
  zhedef /= inc;

  return zhedef;
}


void Terrain::draw(int m){
  D3DXMATRIX World;
  D3DXMatrixIdentity(&World);
  
  effect->SetMatrix( "World", &(World));
  effect->SetMatrix( "WorldViewProjection", &(World * (*View) * (*Proj)) );

  effect->SetFloatArray("ConstantColor", D3DXVECTOR4(0.7f,0.7f,0.7f,1.0f),4);
  effect->SetTexture( "Texture0",TerrainTexture );



  D3DXVECTOR2 paintMaxUV(20.0, 20.0);
  D3DXVECTOR2 paintMinUV(0, 0);

  Tools::D3DDevice->SetFVF(CUSTOMFVF);
  Tools::D3DDevice->SetStreamSource(0, v_buffer, 0, sizeof(customvertex));

  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      Tools::D3DDevice->SetIndices(i_buffer[0][i][j]);
      effect->SetTexture( "Paint0", paints[i][j] );
      
      effect->SetFloatArray("maxUV", D3DXVECTOR2((20.0f/8), 20.0f/8), 2);
      effect->SetFloatArray("minUV", D3DXVECTOR2(i*(20.0f/8), j*(20.0f/8)),2);


      effect->Begin(0, 0);
      effect->BeginPass(m);

      Tools::D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 128*128, 0, 16*16*2); 

      effect->EndPass();
      effect->End();
    }
  }
}

int Terrain::HitTest(D3DXVECTOR3 oldn, D3DXVECTOR3 *newn, float *speed, float DeltaTime){
  
  if(newn->z < -10000) newn->z = -10000;
  else if(newn->z > 10000) newn->z = 10000;
  if(newn->x < -10000) newn->x = -10000;
  else if(newn->x > 10000) newn->x = 10000;

  float minz = getZ(newn->x, newn->z) + 80;
  if(newn->y < minz){
    newn->y  = minz;
    speed[1] = 0.0f;
    speed[3] = 100.0f;
    return 1;
  }

  return 0;
}

inline bool IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
                        D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                        FLOAT* t, FLOAT* u, FLOAT* v )
{

    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.000001f ){
        return 0;
    }

    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return 0;

    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return 0;

    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return 1;
}


inline int Terrain::checkTriangle(int i, int j, D3DXVECTOR3 &pos, D3DXVECTOR3 &dir,
								   int *index, float *pU, float* pV, float *dist){

   D3DXVECTOR3 v0, v1, v2, v3;

   if(i < 0 || i > 128 || j < 0 || j > 128) return 0;
   float inc = 20000.0/128;

   v0.x = v1.x = i*inc - 10000;
   v0.z = v2.z = j*inc - 10000;

   v2.x = v3.x = (i+1)*inc - 10000;
   v1.z = v3.z = (j+1)*inc - 10000;

   v0.y = terrain[i][j];
   v1.y = terrain[i][j+1];
   v2.y = terrain[i+1][j];
   v3.y = terrain[i+1][j+1];

   
   
   if(IntersectTriangle(pos, dir, v0, v1, v2, dist, pU, pV)){
     *index = i*128+j;

     
    float pU2 = v0.x+ (v1.x-v0.x)**pU + (v2.x-v0.x)**pV;
    float pV2 = v0.z+ (v1.z-v0.z)**pU + (v2.z-v0.z)**pV;
    *pU = pU2;
    *pV = pV2;

	   return 1;
   }
   if(IntersectTriangle(pos, dir, v3, v2, v1, dist, pU, pV)){
     *index = i*128+j;
    float pU2 = v3.x+ (v2.x-v3.x)**pU + (v1.x-v3.x)**pV;
    float pV2 = v3.z+ (v2.z-v3.z)**pU + (v1.z-v3.z)**pV;
    *pU = pU2;
    *pV = pV2;
	   return 1;
	}
  return 0;
}

  

void Terrain::paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  
  float u, v;

  pU += 10000;
  pV += 10000;

  int pa = pU*8/20000;
  int pb = pV*8/20000;


  u = (pU-pa*2500)/2500;
  v = (pV-pb*2500)/2500;

  D3DLOCKED_RECT temp;


  float fx = circleSize*8/20000.0f;
  float fy = circleSize*8/20000.0f;
 

  int xmin = (u-fx)*paintSize;
  int xmax = (u+fx)*paintSize;
  int ymin = (v-fy)*paintSize;
  int ymax = (v+fy)*paintSize;

  unsigned int* a[3][3];

  if(xmin < 0){
    if(ymin < 0){
      if(paints[pa-1][pb-1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
      a[0][0] = (unsigned int*)temp.pBits;
    }
 
    if(paints[pa-1][pb]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
    a[0][1] = (unsigned int*)temp.pBits;
  
    if(ymax > paintSize){
      if(paints[pa-1][pb+1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
      a[0][2] = (unsigned int*)temp.pBits;
    }
  }

  if(xmax > paintSize){
    if(ymin < 0){
      if(paints[pa+1][pb-1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
      a[2][0] = (unsigned int*)temp.pBits;
    }
 
    if(paints[pa+1][pb]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
    a[2][1] = (unsigned int*)temp.pBits;
  
    if(ymax > paintSize){
      if(paints[pa+1][pb+1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
      a[2][2] = (unsigned int*)temp.pBits;
    }
  }

  if(ymin < 0){
   if(paints[pa][pb-1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
    a[1][0] = (unsigned int*)temp.pBits;
  }

  if(ymax > paintSize){
   if(paints[pa][pb+1]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
    a[1][2] = (unsigned int*)temp.pBits;
  }

  if(paints[pa][pb]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
  a[1][1] = (unsigned int*)temp.pBits;     

  float rx = fx*paintSize;
  float ry = fy*paintSize;
  float cx = u*paintSize;
  float cy = v*paintSize;

  unsigned int alp = c.w*255;
  unsigned int r = c.x*255;
  unsigned int g = c.y*255;
  unsigned int b = c.z*255;

  DWORD color = (alp<<24)&0xFF000000 | (r<<16)&0x00FF0000
          | (g<<8)&0x0000FF00
          | (b)&0x000000FF; 



  for(int i=xmin; i<xmax; i++){
    for(int j=ymin; j<ymax; j++){
      float xdiff = abs(i-cx)/rx;
      float ydiff = abs(j-cy)/ry;


      float rad = sqrt(xdiff*xdiff+ydiff*ydiff);

      if(rad <= 1){
        if(i<0 && j < 0){
          a[0][0][(i+paintSize)+(j+paintSize)*paintSize] = color;
        }else if(i < 0 && j < paintSize){
          a[0][1][(i+paintSize)+(j)*paintSize] = color;
        }else if(i < 0 && j >= paintSize){
          a[0][2][(i+paintSize)+(j-paintSize)*paintSize] = color;
        }else if(i < paintSize && j < 0){
          a[1][0][(i)+(j+paintSize)*paintSize] = color;
        }else if(i < paintSize && j < paintSize){
          a[1][1][i+j*paintSize] = color;
        }else if(i < paintSize && j >= paintSize){
          a[1][2][(i)+(j-paintSize)*paintSize] = color;
        }else if(i >= paintSize && j < 0){
          a[2][0][(i-paintSize)+(j+paintSize)*paintSize] = color;
        }else if(i >= paintSize && j < paintSize){
          a[2][1][(i-paintSize)+(j)*paintSize] = color;
        }else if(i >= paintSize && j >= paintSize){
          a[2][2][(i-paintSize)+(j-paintSize)*paintSize] = color;
        }else{
          //yuh
        }
      }
    }
  }
  if(paints[pa][pb]->UnlockRect(0) != D3D_OK) ALERT;


  if(xmin < 0){
    if(ymin < 0){
      if(paints[pa-1][pb-1]->UnlockRect(0) != D3D_OK) ALERT;
    }
 
    if(paints[pa-1][pb]->UnlockRect(0) != D3D_OK) ALERT;
  
    if(ymax > paintSize){
      if(paints[pa-1][pb+1]->UnlockRect(0) != D3D_OK) ALERT;
    }
  }

  if(xmax > paintSize){
    if(ymin < 0){
      if(paints[pa+1][pb-1]->UnlockRect(0) != D3D_OK) ALERT;
    }
 
    if(paints[pa+1][pb]->UnlockRect(0) != D3D_OK) ALERT;
  
    if(ymax > paintSize){
      if(paints[pa+1][pb+1]->UnlockRect(0) != D3D_OK) ALERT;
    }
  }

  if(ymin < 0){
    if(paints[pa][pb-1]->UnlockRect(0) != D3D_OK) ALERT;
  }

  if(ymax > paintSize){
    if(paints[pa][pb+1]->UnlockRect(0) != D3D_OK) ALERT;
  }

 
}


int Terrain::shotTest(D3DXVECTOR3 pos, D3DXVECTOR3 dir,
                          
  int *index, float *pU, float* pV, float *dist){

  float d1 = (-250-pos.y)/(dir.y);
  float d2 = (+200-pos.y)/(dir.y);

  D3DXVECTOR3 a, b;

  a = pos;
  b = pos+d1*dir;

  if(d1 < 0) return 0;
  float ang = atan2(b.z-a.z,b.x-a.x);
 
  float inc = 20000.0/128;

  if(ang > -PI/4 && ang <= PI/4){
    int begin = (a.x+10000)/inc;
    int end = (b.x+10000)/inc;

	  int other = (a.z+10000)/inc;

    for(int i=begin-1; i<=end; i++){
	    int j = other+(i-begin)*tan(ang);
	    if(checkTriangle(i,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i-1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i+1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j-1,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j+1,pos, dir, index, pU, pV, dist)) return 1;
    }
	
  }else if( ang > PI/4 && ang <= 3*PI/4){
    int begin = (a.z+10000)/inc;
    int end = (b.z+10000)/inc;

	  int other = (a.x+10000)/inc;

    for(int j=begin-1; j<=end; j++){
	    int i = other+(j-begin)/tan(ang);
	    if(checkTriangle(i,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i-1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i+1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j-1,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j+1,pos, dir, index, pU, pV, dist)) return 1;
    }
	

  }else if( ang > 3*PI/4 || ang <= -3*PI/4){
    int begin = (a.x+10000)/inc;
    int end = (b.x+10000)/inc;

	  int other = (a.z+10000)/inc;

    for(int i=begin+1; i>=end; i--){
	    int j = other+(i-begin)*tan(ang);
	    if(checkTriangle(i,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i-1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i+1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j-1,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j+1,pos, dir, index, pU, pV, dist)) return 1;
    }

	  
  }else if( ang > -3*PI/4 && ang <= -PI/4){
	  int begin = (a.z+10000)/inc;
    int end = (b.z+10000)/inc;

	  int other = (a.x+10000)/inc;

    for(int j=begin+1; j>=end; j--){
	  int i = other+(j-begin)/tan(ang);
	  if(checkTriangle(i,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i-1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i+1,j,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j-1,pos, dir, index, pU, pV, dist)) return 1;
	    if(checkTriangle(i,j+1,pos, dir, index, pU, pV, dist)) return 1;
    }
	
  }else{
	  
  }

  return 0;
}


void Terrain::bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){
  paint(index, pU, pV, c, circleSize);
}

int Terrain::getSide(int index, float pU, float pV, D3DXVECTOR3* coor){

  
  float u, v;

  pU += 10000;
  pV += 10000;

  int pa = pU*8/20000;
  int pb = pV*8/20000;

  u = (pU-pa*2500)/2500;
  v = (pV-pb*2500)/2500;


  int x = (u)*paintSize;
  int y = (v)*paintSize;

  D3DLOCKED_RECT temp;
  if(paints[pa][pb]->LockRect(0, &temp, NULL, 0) != D3D_OK) ALERT;
  unsigned int *a = (unsigned int*)temp.pBits;     
  
  unsigned int c = a[(int)x+(int)y*paintSize];

  paints[pa][pb]->UnlockRect(0);
  
  if(c>>24 < 10) return -1;
 
  if((c>>16)&255 > 10) return 0;
  if((c)&255 > 10) return 1;
  return -1;
}