#include "Tools.h"
#include "Game.h"

IDirect3DDevice9* Tools::D3DDevice = NULL;
DirectInput* Tools::DInput = NULL;

fstream Tools::f;

Game* Tools::g;
int Tools::textureSize = 0;

int Tools::ScreenWidth = 1;
int Tools::ScreenHeight = 1;
int Tools::Windowed = 1;


IDirect3DTexture9* Tools::textureList[MAX_TEXTURE];
char* Tools::textureNames[MAX_TEXTURE];

HRESULT Tools::LoadTexture(char* textureFile, IDirect3DTexture9* &texture){
  return D3DXCreateTextureFromFile( Tools::D3DDevice, textureFile, &texture );
  
}

IDirect3DTexture9* Tools::LoadTexture(char* textureFile){
  if(textureFile == NULL){
    f << "NULL texture name.\n";

    return textureList[0];
  }
  for(int i=0; i<textureSize; i++){
    if(strcmp(textureNames[i], textureFile) == 0){
      f << textureFile << i << "\n";
      return textureList[i];
    }
  }

  if(LoadTexture(textureFile, textureList[textureSize]) != D3D_OK){

    f << textureFile << " not found.\n";
    
    return textureList[0];
  }


  textureNames[textureSize] = new char[strlen(textureFile)+1];
  strcpy(textureNames[textureSize], textureFile);
  textureSize++;

  return textureList[textureSize-1];
}

void Tools::LoadXFile( char* MeshFilename, ID3DXMesh* &Mesh){
  LoadXFile(MeshFilename, Mesh, NULL, NULL);
}

void Tools::LoadXFile( char* MeshFilename, ID3DXMesh* &Mesh, LPD3DXBUFFER *materialBuffer, DWORD *numMaterials){
	Mesh = NULL;
	ID3DXBuffer* MeshBuffer = NULL;

	if(D3DXLoadMeshFromX(MeshFilename, D3DXMESH_MANAGED, D3DDevice, &MeshBuffer, materialBuffer, 0, numMaterials, &Mesh) != D3D_OK) ALERTM(MeshFilename,123);

	if(Mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)MeshBuffer->GetBufferPointer(), 0, 0, 0) != D3D_OK) ALERT;

	MeshBuffer->Release();
  MeshBuffer = NULL;
}


float Tools::Dot(D3DXVECTOR3 a, D3DXVECTOR3 b){ 
  return (a.x*b.x + a.y*b.y + a.z*b.z);
}

D3DXVECTOR3 Tools::Normalize(D3DXVECTOR3 a){
  D3DXVECTOR3 r;
  float d = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
  r.x = a.x / d;
  r.y = a.y / d;
  r.z = a.z / d;

  return r;
}


float Tools::Distance(D3DXVECTOR3 a, D3DXVECTOR3 b){
  float x = a.x - b.x;
  float y = a.y - b.y;
  float z = a.z - b.z;

  return sqrt(x*x + y*y + z*z);
}

float Tools::Distance2(D3DXVECTOR3 a, D3DXVECTOR3 b){
  float x = a.x - b.x;
  float y = a.y - b.y;
  float z = a.z - b.z;

  return (x*x + y*y + z*z);
}

D3DXVECTOR3 Tools::Cross(D3DXVECTOR3 a, D3DXVECTOR3 b){
  D3DXVECTOR3 r;
	r.x = (a.y*b.z - a.z*b.y);
	r.y = (a.z*b.x - a.x*b.z);
	r.z = (a.x*b.y - a.y*b.x);
  return r;
}



float Tools::m3_det( float mat[9]){
  float det;
  det = mat[0] * ( mat[4]*mat[8] - mat[7]*mat[5] )
    - mat[1] * ( mat[3]*mat[8] - mat[6]*mat[5] )
    + mat[2] * ( mat[3]*mat[7] - mat[6]*mat[4] );
  return det ;
}

  
void Tools::m4_submat( float mr[16], float mb[9], int i, int j ) {
 int di, dj, si, sj;
 for( di = 0; di < 3; di ++ ) {
   for( dj = 0; dj < 3; dj ++ ) {
	   si = di + ( ( di >= i ) ? 1 : 0 );
	    sj = dj + ( ( dj >= j ) ? 1 : 0 );
	   
	    mb[di * 3 + dj] = mr[si * 4 + sj];
	  }
	}
}


float Tools::m4_det( float mr[16] ){
  float  det, result = 0, i = 1;
  float msub3[9];
  int n;
  for ( n = 0; n < 4; n++, i *= -1 ){

    m4_submat( mr, msub3, 0, n );
    det = m3_det( msub3 );
    result += mr[n] * det * i;
  }
  return result;
}

int Tools::m4_inverse( float mr[16], float ma[16]){

  float  mdet = m4_det( ma );
  float mtemp[9];
  int i, j, sign;
  if ( fabs( mdet ) < 0.0005 )  return 0;

  for ( i = 0; i < 4; i++ ){
    for ( j = 0; j < 4; j++ ){
      sign = 1 - ( (i +j) % 2 ) * 2;
      m4_submat( ma, mtemp, i, j );
      mr[i+j*4] = ( m3_det( mtemp ) * sign ) / mdet;
    }
  }

  return 1;
}



D3DXVECTOR3 Tools::ClosestPointOnLine(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vPoint){
    D3DXVECTOR3 vVector1 = vPoint - vA;
    D3DXVECTOR3 vVector2 = Tools::Normalize(vB - vA);

    float d = Tools::Distance(vA, vB);

    float t = Tools::Dot(vVector2, vVector1) ;

    if (t <= 0)
        return vA;

    if (t >= d)
        return vB;
 
    D3DXVECTOR3 vClosestPoint = vA + vVector2 * t;
    return vClosestPoint;
}
