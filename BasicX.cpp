#include "BasicX.h"
#include "Game.h"

BasicX::BasicX(char *xFile){

  size = 1;
  coors = D3DXVECTOR3(0.0, 0.0, 0.0);
  rotation = 0;
  recalculateWorld();

  Tools::LoadXFile(xFile, mesh, &materialBuffer, &numMaterials );
  
  D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();

  textures = new IDirect3DTexture9*[numMaterials];
  hasTexture = new int[numMaterials];
  color = new D3DXVECTOR4[numMaterials];

  for(DWORD i=0; i<numMaterials; i++){
    if(d3dxMaterials[i].pTextureFilename == NULL){
      hasTexture[i] = 0;
      color[i] = D3DXVECTOR4(d3dxMaterials[i].MatD3D.Diffuse.r, d3dxMaterials[i].MatD3D.Diffuse.g, d3dxMaterials[i].MatD3D.Diffuse.b, 1.0);
      
    }else{
      hasTexture[i] = 1;
      textures[i] = Tools::LoadTexture(d3dxMaterials[i].pTextureFilename);
      
    }
  }

  mesh->GetAttributeTable(NULL,&subsetNum);
  addDrawList();
}

void BasicX::draw(int m){

  effect->SetMatrix( "World", &(World));
  effect->SetMatrix( "WorldViewProjection", &(World * (*View) * (*Proj)) );

  for(DWORD i=0; i<subsetNum; i++){
    if(hasTexture[i]){
      effect->SetTexture( "Texture0", textures[i]);
      effect->SetFloatArray("ConstantColor", D3DXVECTOR4(0.0,0.0,0.0,0.0),4);
    }else{  
      effect->SetFloatArray("ConstantColor", color[i],4);
    }
    
	effect->Begin(0, 0);
	effect->BeginPass(m);
	 mesh->DrawSubset(i);
	effect->EndPass();
    effect->End();
  }
}