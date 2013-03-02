#include "Bullet.h"

#include "Drawable.h"
#include "Game.h"

#include "Tools.h"
#include "DynamicShotTestable.h"


int Bullet::recievedEarlyNum = 0;
struct recievedBuff Bullet::recBuff[128];


int Bullet::nextId = 0;

ID3DXMesh* Bullet::ball = 0;
ID3DXEffect * Bullet::effectS = 0;
Bullet* Bullet::bullets[1024];
int Bullet::bulletNum = 0;

void Bullet::setup(){
  dir = Tools::Normalize(dir);


  bornTime = Tools::g->CurrentTime;

  cindex = -1;
  float cdist = 9999999999.0f;
  float pU, pV, dist;
  int index;
  n=-1;

  id = nextId;
  nextId++;

  for(int i=0; i<StaticShotTestable::StaticShotTestablenum; i++){
		if(StaticShotTestable::allStaticShotTestables[i]->shotTest(coors, dir, &index, &pU, &pV, &dist)){
		  if(dist < cdist){
		    cdist = dist;
		    cpU = pU;
			  cpV = pV;
			  cindex = index;
			  n = i;
		  }
	  }
	}

  if(n!=-1){
    dieTime = cdist/10000;
  }else{
    dieTime = 10;
  }

  addDrawList();
}

int Bullet::tick(float deltaTime){
  dieTime -= deltaTime;

  D3DXVECTOR3 oldCoors = coors;
  coors += deltaTime*dir*10000;

  if(owner == Networks::num){
    for(int i=0; i<DynamicShotTestable::DynamicShotTestablenum; i++){

      float pU, pV, dist;
      int index;

      if(int type = DynamicShotTestable::allDynamicShotTestables[i]->shotTest(oldCoors, coors, &index, &pU, &pV, &dist)){
  	
        DynamicShotTestable::allDynamicShotTestables[i]->bulletHit(index, pU, pV, color, 40);
        Networks::sendBulletHit(id, type, i, index, pU, pV, color);
        return 1;
      }
    }
    
    if(dieTime < 0){
      if(n != -1){
        StaticShotTestable::allStaticShotTestables[n]->bulletHit(cindex, cpU, cpV, color, 40);
        Networks::sendBulletHit(id, 0, n, cindex, cpU, cpV, color);
      }
      return 1;
    }

  }

  return 0;
}

void Bullet::draw(int m){

  if(owner == Networks::num && Tools::g->CurrentTime - bornTime < 5) return;
  
  D3DXMATRIX World, W1;

  D3DXMatrixTranslation(&World, coors.x, coors.y, coors.z);


  D3DXMatrixScaling(&W1,8, 8, 8);

  World = W1*World;

  effectS->SetMatrix( "World", &(World));
  effectS->SetMatrix( "WorldViewProjection", &(World * (*Drawable::View) * (*Drawable::Proj)) );
  effectS->SetFloatArray("ConstantColor", color,4);
	effectS->Begin(0, 0);

  effectS->BeginPass(0);
	ball->DrawSubset(m);
	effectS->EndPass();
  
	effectS->End();


}

Bullet* Bullet::add(D3DXVECTOR3 pos, D3DXVECTOR3 dir, int owner, DWORD currentTime, D3DXVECTOR4 c){
  
  bullets[bulletNum] = new Bullet();

  bullets[bulletNum]->coors = pos;
  bullets[bulletNum]->dir = dir;

  bullets[bulletNum]->owner = owner;
  bullets[bulletNum]->initTime = currentTime;

  bullets[bulletNum]->color = c;

  bullets[bulletNum]->side = Tools::g->side;

  bullets[bulletNum]->setup();



  bulletNum++;

  return bullets[bulletNum-1];

}

Bullet* Bullet::add(New_Bullet_struct *nbs){
  for(int i=0; i<recievedEarlyNum; i++){
    if(recBuff[i].id == nbs->id && recBuff[i].owner == nbs->num){
      if(recBuff[i].hitType == 0){
        StaticShotTestable::allStaticShotTestables[recBuff[i].paintableNum]->bulletHit(recBuff[i].index, recBuff[i].u, recBuff[i].v, recBuff[i].color, 40);
      }else if(recBuff[i].hitType == 1){
        DynamicShotTestable::allDynamicShotTestables[recBuff[i].paintableNum]->bulletHit(recBuff[i].index, recBuff[i].u, recBuff[i].v, recBuff[i].color, 40);
	  }else{
		if(recBuff[i].index == Networks::num && Tools::g->side != nbs->side){
          if(Tools::g->isDead == 0) Tools::g->life -= 0.5;
        }
	  }
      recievedEarlyNum--;
      recBuff[i] = recBuff[recievedEarlyNum];
      return 0;
    }
  }

  bullets[bulletNum] = new Bullet();

  bullets[bulletNum]->coors = nbs->coors;
  bullets[bulletNum]->dir = nbs->dir;

  bullets[bulletNum]->owner = nbs->num;
  bullets[bulletNum]->initTime = nbs->initTime;
  bullets[bulletNum]->dieTime = nbs->dieTime;

  bullets[bulletNum]->cpU = nbs->cpU;
  bullets[bulletNum]->cpV = nbs->cpV;
  bullets[bulletNum]->n = nbs->n;
  bullets[bulletNum]->cindex = nbs->cindex;

  bullets[bulletNum]->id = nbs->id;
  
  bullets[bulletNum]->side = nbs->side;

  bullets[bulletNum]->color = D3DXVECTOR4(nbs->color);
  bullets[bulletNum]->addDrawList();


  bulletNum++;

  return bullets[bulletNum-1];

}

void Bullet::tickAll(float deltaTime){
  for(int i=0; i<bulletNum; i++){
    if(bullets[i]->tick(deltaTime)){
      bullets[i]->removeDrawList();
      delete bullets[i];

      bullets[i] = bullets[bulletNum-1];
      bulletNum--;
      i--;
    }
  }
}

void Bullet::gotHit( int hitType, int owner, int id, int paintableNum, int index, float u, float v, D3DXVECTOR4 c){
  for(int i=0; i<bulletNum; i++){
    if(bullets[i]->owner == owner && bullets[i]->id == id){
	
      if(hitType == 0){

		  StaticShotTestable::allStaticShotTestables[paintableNum]->bulletHit(index, u, v, c, 40);
      }else if(hitType == 1){
          DynamicShotTestable::allDynamicShotTestables[paintableNum]->bulletHit(index, u, v, c, 40);
      }else if(hitType == 2){
		  if(index == Networks::num  && Tools::g->side != bullets[i]->side){
			if(Tools::g->isDead == 0) Tools::g->life -= 0.5;
		 }
      }

      bullets[i]->removeDrawList();
      delete bullets[i];

      bullets[i] = bullets[bulletNum-1];
	  
      bulletNum--;
      i--;
      return;
    }
  }
  int i = recievedEarlyNum;

  recBuff[i].hitType = hitType;
  recBuff[i].owner = owner;
  recBuff[i].id = id;
  recBuff[i].paintableNum = paintableNum;
  recBuff[i].u = u;
  recBuff[i].v = v;

  recievedEarlyNum++;
  //ALERTN("NO ID");
}