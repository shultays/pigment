#include "Flag.h"
#include "Game.h"

Flag::Flag():BasicX("./flag.x"){


  percent = 0;
  recalculateWorld();
}


void Flag::draw(int m){
//  recalculateWorld();

  D3DXMATRIX world2, temp;
  world2 = World;

  if(owner == -1){
    if(type == 0){
      D3DXMatrixTranslation(&world2, Tools::g->cps[1]->coors.x, Tools::g->cps[1]->coors.y+93, Tools::g->cps[1]->coors.z);
      D3DXMatrixScaling(&temp, 3, 3, 3);
      world2 = temp*world2;
      D3DXMatrixRotationY(&temp, PId2);
      world2 = temp*world2;
    }else if(type == 1){
      D3DXMatrixTranslation(&world2, Tools::g->cps[2]->coors.x, Tools::g->cps[2]->coors.y+93, Tools::g->cps[2]->coors.z);
      D3DXMatrixScaling(&temp, 3, 3, 3);
      world2 = temp*world2;
      D3DXMatrixRotationY(&temp, -PId2);
      world2 = temp*world2;
    }else if(type == 2) {
      D3DXMatrixTranslation(&world2, Tools::g->cps[0]->coors.x, Tools::g->cps[0]->coors.y+93, Tools::g->cps[0]->coors.z);
      D3DXMatrixScaling(&temp, 3, 3, 3);
      world2 = temp*world2;
      D3DXMatrixRotationY(&temp, PId2);
      world2 = temp*world2;
    }
  }


  effect->SetMatrix( "World", &(World));
	effect->SetMatrix( "WorldViewProjection", &(world2 * (*View) * (*Proj)) );
  effect->SetFloat( "Height", percent*44);


  if(side == 0){
    effect->SetVector( "ConstantColor", &D3DXVECTOR4(1.0, 0.0, 0.0, 1.0));
  }else if(side == 1){
    effect->SetVector( "ConstantColor", &D3DXVECTOR4(0.0, 0.0, 1.0, 1.0));
  }else{
    effect->SetVector( "ConstantColor", &D3DXVECTOR4(1.0, 1.0, 1.0, 1.0));
  }


	effect->Begin(0, 0);
	effect->BeginPass(m);
  for(DWORD i=0; i<subsetNum; i++){
		mesh->DrawSubset(i);
  }
	effect->EndPass();
  effect->End();
}

