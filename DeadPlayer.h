#ifndef DEADPLAYER_H
#define DEADPLAYER_H

#include "Player.h"
#include "StaticShotTestable.h"

class DeadPlayer: public Player{

public:

  static DeadPlayer *dps[64];
  static int dpsNum;
  int thisdpsNum;



  int groundNum;
  int index;
  float pU, pV, dist;

  void addDeadList(Player *p){

    this->coors = p->coors;

    this->effect = p->effect;
    this->weapon = p->weapon;
    this->side = p->side;
    this->life = 0;
    this->rotateAll = p->rotateAll;
    this->rotateHor = p->rotateHor;
    this->rotateVer = p->rotateVer;
    this->rotation = p->rotation;
    this->anim_high = p->anim_high;
    this->anim_low = p->anim_low;
    this->anim_low_reversed = p->anim_low_reversed;
    this->yaw = p->yaw;
    this->pitch = p->pitch;

    this->framenumhigh = p->framenumhigh;
    this->framenumlow = p->framenumlow;
    this->hasFlag = 0;
    this->legangle = p->legangle;


    dist = 9000000000;
    int index2;
    float pU2, pV2, dist2;
    for(int i=0; i<StaticShotTestable::StaticShotTestablenum; i++){
      if(StaticShotTestable::allStaticShotTestables[i]->shotTest(coors, D3DXVECTOR3(0.0, -1.0, 0.0), &index2, &pU2, &pV2, &dist2)){
        if(dist2 < dist){
          dist = dist2;
          pU = pU2;
          pV = pV2;
          index  = index2;
          groundNum = i;
        }
      }
    }

    minY = coors.y - dist;

    addDrawList();

    thisdpsNum = dpsNum;
    dps[dpsNum] = this;
    dpsNum++;
  }


  void tick(float DeltaTime){
    coors.y -= DeltaTime*200;

    if(coors.y < minY+80){
      D3DXVECTOR4 c(1.0, 0.0, 0.0, 1.0);
      if(side == 1){
        c = D3DXVECTOR4(0.0, 0.0, 1.0, 1.0);
      }
      StaticShotTestable::allStaticShotTestables[groundNum]->bulletHit(index, pU, pV, c, (minY+80-coors.y));

    }
    if(coors.y < minY){
      
      dpsNum--;
      dps[thisdpsNum] = dps[dpsNum];
      dps[thisdpsNum]->thisdpsNum = thisdpsNum;

      removeDrawList();
      delete this;
    }
  }
};


#endif
