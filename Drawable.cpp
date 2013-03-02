#include "Drawable.h"
#include "Game.h"
#include <vector>

#include "FPSWeapon.h"
int Drawable::numDrawable = 0;
Drawable* Drawable::allDrawables[MAX_DRAWABLE];

ID3DXEffect* Drawable::postProcessingEffect;

IDirect3DSurface9* Drawable::RenderSurface[8];
IDirect3DTexture9* Drawable::QuadTexture[8];
IDirect3DTexture9* Drawable::paintTexture;
IDirect3DSurface9* Drawable::BackBuffer;
ID3DXMesh* Drawable::PPQuadMesh;

D3DXMATRIX *Drawable::Proj;
D3DXMATRIX *Drawable::View;

Method Drawable::drawMethod = Shadow;

D3DXVECTOR3 lightPos[1024];
D3DXVECTOR3 lightColors[1024];

float lightRanges[1024];
float lightFallOffs[1024];

int hasShadow[1024];
int Drawable::lightNum = 0;
LPD3DXMESH cube;


char Drawable::lightTest = 0;
char Drawable::shadowOn = 1;



void Drawable::CreatePPQuad(){
  struct Vertex2{
    Vertex2(){}
    Vertex2(float x, float y, float z, float u, float v)    {
      _x = x;   _y = y;   _z = z;
      _u = u;   _v = v;
    }

    float _x, _y, _z, _u, _v;
  };

  DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;

  D3DXCreateMeshFVF(2, 4, D3DXMESH_MANAGED, FVF, Tools::D3DDevice, &PPQuadMesh);

  Vertex2* v = 0;
  while(FAILED(PPQuadMesh->LockVertexBuffer(0, (void**)&v)));

  v[0] = Vertex2( -1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
  v[1] = Vertex2( -1.0f,  1.0f, 1.0f, 0.0f, 0.0f);
  v[2] = Vertex2(  1.0f,  1.0f, 1.0f, 1.0f, 0.0f);
  v[3] = Vertex2(  1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

  PPQuadMesh->UnlockVertexBuffer();

  WORD* i = 0;
  while(FAILED(PPQuadMesh->LockIndexBuffer(0, (void**)&i)));

  i[0] = 0; i[1] = 1; i[2] = 2;
  i[3] = 0; i[4] = 2; i[5] = 3;

  PPQuadMesh->UnlockIndexBuffer();

  std::vector<DWORD> adjacencyBuffer(PPQuadMesh->GetNumFaces() * 3);
  PPQuadMesh->GenerateAdjacency(0, &adjacencyBuffer[0]);
}


void Drawable::setup(){
  
  if(D3DXCreateEffectFromFile(Tools::D3DDevice, "./PostProcess.fx", NULL, NULL, NULL, NULL, &postProcessingEffect, NULL) != D3D_OK) ALERTN("post process");
  CreatePPQuad();
  postProcessingEffect->SetFloat("dx",1.0f/Tools::ScreenWidth);
  postProcessingEffect->SetFloat("dy",1.0f/Tools::ScreenHeight);

 
  for(int i=0; i<7; i++){
    
    Tools::D3DDevice->CreateTexture(Tools::ScreenWidth, Tools::ScreenHeight, 1, 
      
      D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &QuadTexture[i], NULL);
    QuadTexture[i]->GetSurfaceLevel(0, &RenderSurface[i]);
  }  



  if(Tools::D3DDevice->CreateTexture(Tools::ScreenWidth, Tools::ScreenHeight, 1,
    D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &QuadTexture[7], NULL) != D3D_OK)ALERTN("no floating point texture format");

  QuadTexture[7]->GetSurfaceLevel(0, &RenderSurface[7]);

  Tools::LoadTexture("paint.jpg", paintTexture);

  D3DXCreateBox(Tools::D3DDevice,1,1,1, &cube, 0);

  lightColors[0] = D3DXVECTOR3(0.6, 0.0, 0.0);
  lightColors[1] = D3DXVECTOR3(0.0, 0.6, 0.0);
  lightColors[2] = D3DXVECTOR3(0.0, 0.0, 0.6);
  lightRanges[0] = 20000;
  lightFallOffs[0] = 10000;
  lightRanges[1] = 10000;
  lightFallOffs[1] = 5000;
  lightRanges[2] = 10000;
  lightFallOffs[2] = 5000;




  srand(120);

  for(int i=3; i<1024; i++){
    lightPos[i] = D3DXVECTOR3(rand()%20000-10000, rand()%6000+500, rand()%20000-10000);
    lightColors[i] = D3DXVECTOR3((rand()%1000)/1000.0+0.2, (rand()%1000)/1000.0+0.2, (rand()%1000)/1000.0+0.2);
    

    if(rand()%3 == 0){
      lightColors[i][0] = 0.8;
      lightColors[i][1] /= 4;
      lightColors[i][2] /= 4;
    }else if(rand()%2 == 0){
      lightColors[i][1] = 0.8;
      lightColors[i][0] /= 4;
      lightColors[i][2] /= 4;
    }else{
      lightColors[i][2] = 0.8;
      lightColors[i][1] /= 4;
      lightColors[i][0] /= 4;
    }
    lightRanges[i] = 1500;
    lightFallOffs[i] = 300;
    hasShadow[i] = 0;
  }

  hasShadow[0] = hasShadow[1] = hasShadow[2] = 1;
  lightNum = 3;
  
  lightPos[3] = D3DXVECTOR3(4175.1,2393.91,-320.939);
  lightPos[4] =D3DXVECTOR3(7441.54,426.063,-4404.17);
  lightPos[5] =D3DXVECTOR3(6564.2,1209.13,3917.75);
  lightPos[6] =D3DXVECTOR3(27.6844,1871.58,6185.81);
  lightPos[7] =D3DXVECTOR3(-816.547,1021.309,4477.18);
  lightPos[8] =D3DXVECTOR3(-1344.22,748.014,-4230.82);
  lightPos[9] =D3DXVECTOR3(-54.1311,2280.7,-6054.13);
  lightPos[10] =D3DXVECTOR3(1582.57,1947.53,-3314.64);
  lightPos[11] =D3DXVECTOR3(-3513.87,1734.41,-200.868);
  lightPos[14] =D3DXVECTOR3(-1522.67,1851.04,-2845.19);
  lightPos[15] =D3DXVECTOR3(4313.96,978.356,2063.29);
  lightPos[16] = D3DXVECTOR3(676.658,4167.81,119.471);
  lightPos[17] =  D3DXVECTOR3(-3870.55,2233.44,-22.5679);
  lightPos[18] = D3DXVECTOR3(1802.13,1726.82,2758.33);
  lightPos[19] = D3DXVECTOR3(292.709,2645.64,3446.9);
  lightPos[20] = D3DXVECTOR3(660.784,4138.74,80.6851);
  lightPos[21] = D3DXVECTOR3(-4212.47,2233.33,305.358);
  lightPos[22] = D3DXVECTOR3(-1774.61,2653.7,-113.955);

  lightPos[23] = D3DXVECTOR3(-108.428,2847.97,-1226.13);
  lightPos[24] = D3DXVECTOR3(4606.68,1838.43,7824.23);
  lightPos[25] = D3DXVECTOR3(4876.63,3349.55,-3792.57);
  lightPos[26] = D3DXVECTOR3(-1148.56,1443.46,-1603.52);
  lightPos[27] = D3DXVECTOR3(-9057.69,1550.46,334.515);
  lightPos[28] = D3DXVECTOR3(-4551.138,369.348,5286.69);
  lightPos[29] = D3DXVECTOR3(750.146,1679.88,3686.1);
  lightPos[30] = D3DXVECTOR3(178.791,3263.06,-6537.29);
  lightPos[31] = D3DXVECTOR3(94.5956,3313.04,6742.01);

  

FPSWeapon *testWeapon = new FPSWeapon();
testWeapon->effect = Tools::g->standartEffect;
  testWeapon->addDrawList();
  
}

void ClearTexture(IDirect3DTexture9* pd3dTexture,D3DCOLOR xColor){
	IDirect3DSurface9* pd3dSurface;
	pd3dTexture->GetSurfaceLevel(0,&pd3dSurface);
	
  Tools::D3DDevice->ColorFill(pd3dSurface,NULL,xColor);
	pd3dSurface->Release();
}

void Drawable::renderQuad(){
  
	postProcessingEffect->Begin(0, 0);
  postProcessingEffect->BeginPass(0);
  PPQuadMesh->DrawSubset(0);
  postProcessingEffect->EndPass();
	postProcessingEffect->End();
}


void Drawable::render(){

  Tools::D3DDevice->GetRenderTarget(0, &BackBuffer);
  if(BackBuffer == NULL) ALERT;


  Tools::D3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x00000000, 1.0f, 0 );
 
  

  /*
  ClearTexture(QuadTexture[0], 0x00000000);
  ClearTexture(QuadTexture[1], 0x00000000);
  ClearTexture(QuadTexture[2], 0x00000000);
  ClearTexture(QuadTexture[3], 0x00000000);
  ClearTexture(QuadTexture[4], 0x00000000);
  ClearTexture(QuadTexture[5], 0x00000000);
  ClearTexture(QuadTexture[6], 0x00000000);
  ClearTexture(QuadTexture[7], 0x00000000);*/
   
 
  if(lightTest){
    Tools::D3DDevice->SetRenderTarget(0, RenderSurface[4]);
    ClearTexture(QuadTexture[0], 0xFFFFFFFF);
  }else{
    if(Tools::g->isDead){
      Tools::D3DDevice->SetRenderTarget(0, RenderSurface[4]);
    }else{
      Tools::D3DDevice->SetRenderTarget(0, RenderSurface[0]);
    }
  }

  Tools::D3DDevice->SetRenderTarget(1, RenderSurface[1]);
  Tools::D3DDevice->SetRenderTarget(2, RenderSurface[7]);

  drawAll(0);
  
  Tools::D3DDevice->SetRenderTarget(0, NULL);
  Tools::D3DDevice->SetRenderTarget(1, NULL);
  Tools::D3DDevice->SetRenderTarget(2, NULL);


  if(Tools::g->isDead && !lightTest){
	  postProcessingEffect->SetTexture("ColorTex0", QuadTexture[4]);
    Tools::D3DDevice->SetRenderTarget(0, RenderSurface[0]);
    postProcessingEffect->SetTechnique( "Death");
    if(Tools::g->side == 0){
      postProcessingEffect->SetFloatArray("color", D3DXVECTOR3(1.0, 0.0, 0.0), 3);
    }else{
      postProcessingEffect->SetFloatArray("color", D3DXVECTOR3(0.0, 0.0, 1.0), 3);
    }
    renderQuad();
    Tools::D3DDevice->SetRenderTarget(0, NULL);
  }


	postProcessingEffect->SetTexture("ColorTex0", QuadTexture[0]);
	postProcessingEffect->SetTexture("ColorTex1", QuadTexture[1]);
	postProcessingEffect->SetTexture("ColorTex2", QuadTexture[7]);
  
  static float ang = 0.0;
  ang += Tools::g->DeltaTime;
     
  D3DXMATRIX sunMatr = Tools::g->sun->World, trans;
  D3DXMatrixTranslation(&trans,0,8500,0);
  trans =  trans*sunMatr;
  lightPos[0] =  D3DXVECTOR3 (trans._41, trans._42, trans._43);
  D3DXMATRIX temp;
  D3DXMatrixRotationY(&temp, PI*2/3);
  trans =  trans*temp;
  lightPos[1] =  D3DXVECTOR3 (trans._41, trans._42, trans._43);
  D3DXMatrixRotationY(&temp, PI*2/3);
  trans =  trans*temp;
  lightPos[2] =  D3DXVECTOR3 (trans._41, trans._42, trans._43);

  Tools::D3DDevice->SetRenderTarget(0, RenderSurface[3]);
  postProcessingEffect->SetTechnique( "Edge");
  renderQuad();

  int lightTargetNum = 0;
  
  D3DXVECTOR4 ambient = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

  if(lightTest){
    lightColors[0] = D3DXVECTOR3(0.6, 0.0, 0.0);
  }else{
    ambient = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f);
    lightColors[0] = D3DXVECTOR3(0.8, 0.8, 0.8);
  }

	postProcessingEffect->SetTexture("ColorTex4", QuadTexture[4]);
  ClearTexture(QuadTexture[4], 0xFF000000);
  Tools::D3DDevice->SetRenderTarget(0, RenderSurface[4]);
  for(int i=0; i<lightNum; i++){

    Tools::D3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0 );
	if(hasShadow[i] && shadowOn ){
      for(int j=0; j<ShadowVolume::shadowSize; j++){
        ShadowVolume::allShadows[j]->drawShadowVolume(lightPos[i]);
      }
    }
   
    

    D3DXMATRIX world, world2;
    D3DXMatrixScaling(&world, lightRanges[i]*2, lightRanges[i]*2, lightRanges[i]*2);
    D3DXMatrixTranslation(&world2, lightPos[i].x, lightPos[i].y, lightPos[i].z);
    world=world*world2;

    postProcessingEffect->SetMatrix("WorldViewProj", &(world*(*Drawable::View)*(*Drawable::Proj)));
    //postProcessingEffect->SetMatrix("WorldView", &(world*(*Drawable::View)));
    //postProcessingEffect->SetMatrix("Proj", Drawable::Proj);

    postProcessingEffect->SetTechnique("LightVolume");


	postProcessingEffect->Begin(0, 0);
  postProcessingEffect->BeginPass(0);
    cube->DrawSubset(0);
  postProcessingEffect->EndPass();
	postProcessingEffect->End();


  postProcessingEffect->SetFloatArray("lightCoor", lightPos[i], 3);
  postProcessingEffect->SetFloatArray("lightColor", lightColors[i], 3);
  postProcessingEffect->SetFloat("lightRange", lightRanges[i]);
  postProcessingEffect->SetFloat("lightFallOff", lightFallOffs[i]);
  postProcessingEffect->SetTechnique("Light");
   

	postProcessingEffect->Begin(0, 0);
  postProcessingEffect->BeginPass(0);
    cube->DrawSubset(0);
  postProcessingEffect->EndPass();
	postProcessingEffect->End();


  if(lightTest == 0) break;
    
  }

    
  Tools::D3DDevice->SetRenderTarget(0, BackBuffer);
  
  postProcessingEffect->SetTechnique( "Combine");
	postProcessingEffect->SetTexture("ColorTex1", QuadTexture[3]);
  postProcessingEffect->SetTexture("ColorTex2", QuadTexture[4]);
 

  postProcessingEffect->SetFloatArray("ambientColor", ambient, 4);

  renderQuad();
      
	Tools::D3DDevice->SetRenderTarget(0, BackBuffer);
	Tools::D3DDevice->SetRenderTarget(1,NULL);
	Tools::D3DDevice->SetRenderTarget(2,NULL);

  BackBuffer->Release();
}


void Drawable::drawAll(int n){
  for(int i=0; i<numDrawable; i++) allDrawables[i]->draw(n);
  
  if(Networks::connected){
    for(int i=0; i<Tools::g->playerNum; i++){
      Tools::g->players[i].draw(n);
    }
  }
}

 
void Drawable::addDrawList(){
  if(placeDrawable != -1) return;
  placeDrawable = numDrawable;
  allDrawables[numDrawable] = this;
  numDrawable++;
}


void Drawable::removeDrawList(){
  if(placeDrawable == -1) return;
  numDrawable--;
  allDrawables[numDrawable]->placeDrawable = placeDrawable;
  allDrawables[placeDrawable] = allDrawables[numDrawable];
  placeDrawable = -1;
}


void Drawable::renderToTexture(int i, int n){
  Tools::D3DDevice->SetRenderTarget(0, RenderSurface[i]);
  ALERT;

  Tools::D3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 0x00000000, 1.0f, 0 );

  drawAll(n);

  Tools::D3DDevice->SetRenderTarget(0, BackBuffer);
}

void Drawable::postProcess(IDirect3DTexture9* source, IDirect3DSurface9* dest, char* tech, int pass){

  Tools::D3DDevice->SetRenderTarget(0, dest);

  postProcessingEffect->SetTechnique(tech);
	postProcessingEffect->SetTexture("ColorTex0", source);

	postProcessingEffect->Begin(0, 0);

	postProcessingEffect->BeginPass(pass);
	PPQuadMesh->DrawSubset(0);
	postProcessingEffect->EndPass();
	
	postProcessingEffect->End();
}

void Drawable::recalculateWorld(){
    D3DXMATRIX  Size, Translate, Rotation;

	  D3DXMatrixIdentity(&World);
    D3DXMatrixScaling(&Size, size, size, size);
    D3DXMatrixTranslation(&Translate, coors.x, coors.y, coors.z);
    D3DXMatrixRotationY(&Rotation, rotation);

    World = World*Size*Rotation*Translate;

    Tools::m4_inverse(WorldInverse, World);
}

Drawable::Drawable(){
    placeDrawable = -1;
}
