#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <d3d9.h>
#include <d3dx9.h>

#define MAX_DRAWABLE 2048

enum Method { Regular, Shadow, CelShader};

class Drawable {
public:

  static D3DXMATRIX *Proj, *View;

  ID3DXEffect *effect;
  int placeDrawable;

  virtual void draw(int m) = 0;

  static int numDrawable;
  static Drawable* allDrawables[MAX_DRAWABLE];

  static void drawAll(int);
  void addDrawList();
  void removeDrawList();

  static Method drawMethod;

  static ID3DXEffect *postProcessingEffect;

  static IDirect3DSurface9* RenderSurface[8], *BackBuffer;
  static IDirect3DTexture9* QuadTexture[8], *paintTexture;
  static ID3DXMesh* PPQuadMesh;

  static void CreatePPQuad();

  static void setup();

  static void renderToTexture(int, int);
  static void render();

  static void postProcess(IDirect3DTexture9* source, IDirect3DSurface9* dest, char* tech, int pass);

  
  D3DXVECTOR3 coors;
  float size , rotation; 

  D3DXMATRIX World, WorldInverse;
  void recalculateWorld();

  Drawable();

  static void Drawable::renderQuad();

  static char lightTest;
  static char shadowOn;

  static int lightNum;
};


#endif