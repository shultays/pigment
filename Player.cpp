#include "Player.h"


md3Model *Player::weapons = 0;
PlayerModel  *Player::models = 0;

Player::Player(){
  jumped = 0;
  legangle = 0;
  
  anim_low = IDLE;
  anim_high = STAND;
  low_state = IDLE;
  high_state = STAND;

  pitch = 0;
  yaw = 0;
  coors = D3DXVECTOR3(0, 0, 0);

  model = NULL;

  framenumlow = 0;
  framenumhigh = 0;

  side = 0;

  hasFlag = 0;
  life = 0;
  minY = -10000;

  isDead = 0;
}

void Player::draw(int m){

  PlayerModel *mod = &models[model];
  mod->bnum = anim_low;
  mod->nnum = anim_high;
  mod->side = side;
  mod->life = life;
  mod->hasFlag = hasFlag;

  mod->framenumlow = framenumlow; 
  mod->framenumhigh = framenumhigh;

  mod->coors[0] = coors.x;
  mod->coors[1] = coors.y;
  mod->coors[2] = coors.z;
  
  mod->rotateAll = yaw;
  mod->rotateHor = pitch;

  mod->weapon = &weapons[weapon];

  mod->minY = minY;
  mod->isDead = isDead;

  if(anim_low_reversed)
    mod->legAngle = -legangle*PI/180;
  else 
    mod->legAngle = legangle*PI/180;

  mod->low_reversed = anim_low_reversed;

  mod->draw(m);
}

void Player::playhighAnim(ANIMATIONS_HIGH anim){
  high_looped = 0;
  anim_high = anim;
  framenumhigh = 0;
}

void Player::playlowAnim(ANIMATIONS_LOW anim){
  low_looped = 0;
  anim_low = anim;
  framenumlow  = 0;
}

void Player::changeLowState(ANIMATIONS_LOW anim){
  low_state = anim;

  if(anim == IDLE){
    if(anim_low != LAND) anim_low = anim;
  }else if(anim == JUMP){
    anim_low = anim;
  }else if(jumped == 0){
    anim_low = anim;
  }
}

void Player::tick(float DeltaTime){ 
 
  PlayerModel *mod = &models[model];

  framenumlow += DeltaTime*0.8f*mod->b[(int)anim_low][3];
  framenumhigh += DeltaTime*0.8f*mod->n[(int)anim_high][3];

  if(!high_looped && (int)framenumhigh >= mod->n[(int)anim_high][1]-1){
    if(anim_high == DROP){

      framenumhigh = (float)mod->b[(int)DROP][1]-1;
    }else{
      anim_high = high_state;
      framenumhigh = 0;
      high_looped = 1;
    }
  }
  if(!low_looped && (int)framenumlow >= mod->b[(int)anim_low][1]-1){
   
    if(jumped){
      framenumlow = (float)mod->b[(int)JUMP][1]-1;
    }else{
      anim_low = low_state;
      framenumlow = 0;
      low_looped = 1;
    }
  }

  float step = 400*DeltaTime;

  if(sidewalk == 0){
    if(legangle > step){
      legangle -= step;
    }else if(legangle < -step){
      legangle += step;
    }else legangle = 0;
  }else if(sidewalk == -1){
    if(legangle > -45 + step){
      legangle -= step;
    }else legangle = -45;
  }else if(sidewalk == 1){
    if(legangle < 45 - step){
      legangle += step;
    }else legangle = 45;
  }
}

#include "Game.h"



int Player::shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist){
  
  if(isDead) return 0;

  D3DXVECTOR3 pos1, pos2, point;

  float ymin = coors[1]-80, ymax = coors[1]+10;

  if(posO.y < ymin && posN.y < ymin) return 0;
  if(posO.y > ymax && posN.y > ymax) return 0;
  
  D3DXVECTOR3 dir = posN-posO;
  if(posO.y < ymin){
    float ydiff = (ymin-posO.y)/dir.y;
    pos1 = posO + ydiff*dir;
  }else if(posO.y > ymax){
    float ydiff = (ymax-posO.y)/dir.y;
    pos1 = posO + ydiff*dir;
  }else{
    pos1 = posO;
  }

  dir = posO-posN;
  if(posN.y < ymin){
    float ydiff = (ymin-posN.y)/dir.y;
    pos2 = posN + ydiff*dir;
  }else if(posN.y > ymax){
    float ydiff = (ymax-posN.y)/dir.y;
    pos2 = posN + ydiff*dir;
  }else{
    pos2 = posN;
  }

  pos1.y = 0;
  pos2.y = 0;

  point.x = coors[0];
  point.y = 0;
  point.z = coors[2];

  pos1 = Tools::ClosestPointOnLine(pos1, pos2, point);

  Tools::g->con.buff << Tools::Distance(pos1, point);
  Tools::g->con.Print();
  *index = num;

  if( Tools::Distance2(pos1, point)< 14*14) return 2;

  return 0;
}

void Player::bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){

    
}

