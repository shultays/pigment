#include "FPSWeapon.h"
#include "Game.h"

FPSWeapon::FPSWeapon():BasicX("./flag.x"){


  recalculateWorld();
}


void FPSWeapon::draw(int m){
//  recalculateWorld();

  D3DXMATRIX world2, temp, temp2, viewFPS;
  world2 = World;


  D3DXMatrixTranslation(&world2, 100, 0, -20);
  D3DXMatrixScaling(&temp, 3, 3, 3);
  D3DXMatrixRotationZ(&temp2, 90);


    D3DXMatrixLookAtLH(&viewFPS, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(1, 0, 0), &D3DXVECTOR3(0, 1, 0));



  World = temp2*temp*world2;

  Tools::g->D3DDevice->SetTransform(D3DTS_VIEW, &viewFPS);


  effect->SetMatrix( "World", &(World));
	effect->SetMatrix( "WorldViewProjection", &(World * (viewFPS) * (*Proj)) );


  effect->SetVector( "ConstantColor", &D3DXVECTOR4(1.0, 0.0, 0.0, 1.0));



	effect->Begin(0, 0);
	effect->BeginPass(m);
  for(DWORD i=0; i<subsetNum; i++){
		mesh->DrawSubset(i);
  }
	effect->EndPass();
  effect->End();
  Tools::g->D3DDevice->SetTransform(D3DTS_VIEW, &Tools::g->ActiveCamera->View);
}

