#ifndef TOOLS_H
#define TOOLS_H

#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include "DirectInput.h"

#include <fstream>
using namespace std;

#define PI 3.14159265358979f
#define PId2 1.570796326794895f
#define testvar 123131313121

#define PFILE0(str) fprintf(Tools::f, str)
#define PFILE1(str,x) fprintf(Tools::f, str,x)
#define PFILE2(str,x,y) fprintf(Tools::f, str,x,y)
#define PFILE3(str,x,y,z) fprintf(Tools::f, str,x,y, z)

struct Vertex {FLOAT X, Y, Z; D3DVECTOR NORMAL; FLOAT U, V; };


#define MAX_TEXTURE 128

#define ALERT MessageBox(0,"ALERT","THERE IS TOO MUCH CONFUSION",0)

#define ALERT1(x) { int i=x; char str[40]; itoa(i, str, 10); MessageBox(0,str,str,0); }
#define ALERT2(x,y) { int i=x, j=y;char str[40], str2[40]; itoa(i, str, 10); itoa(j, str2, 10);MessageBox(0,str,str2,0); }
#define ALERTM(s,x) { int i=x; char str[40]; sprintf_s(str, "%d", i, 40); MessageBox(0,s,str,0); }
#define ALERTN(s){ MessageBox(0,s,s,0); }

class Game;

class Tools{
  
  static IDirect3DTexture9* textureList[MAX_TEXTURE];
  static char* textureNames[MAX_TEXTURE];
  static int textureSize;

public:
  static IDirect3DDevice9* D3DDevice;
  static void LoadXFile( char* MeshFilename, ID3DXMesh* &Mesh );
  static void LoadXFile( char* MeshFilename, ID3DXMesh* &Mesh, LPD3DXBUFFER *materialBuffer, DWORD *numMaterials);
  static HRESULT LoadTexture(char* textureFile, IDirect3DTexture9* &texture);
  static fstream f;

  static float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b);
  static float Distance2(D3DXVECTOR3 a, D3DXVECTOR3 b);
  static float Dot(D3DXVECTOR3 a, D3DXVECTOR3 b);
  static D3DXVECTOR3 Normalize(D3DXVECTOR3 a);
  static D3DXVECTOR3 Cross(D3DXVECTOR3 a, D3DXVECTOR3 b);

  static DirectInput *DInput;

  static Game *g;

  static IDirect3DTexture9* LoadTexture(char* textureFile);

  static int ScreenWidth, ScreenHeight, Windowed;

  

static float m3_det( float mat[9]);
static void m4_submat( float mr[16], float mb[9], int i, int j );
static float m4_det( float mr[16] );
static int m4_inverse( float mr[16], float ma[16]);

static D3DXVECTOR3 ClosestPointOnLine(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vPoint);

};

#endif