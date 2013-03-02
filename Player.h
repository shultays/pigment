#ifndef PLAYER_H
#define PLAYER_H

#include "PlayerModel.h"
#include "Tools.h"
#include "Camera.h"
#include "Drawable.h"
#include "DynamicShotTestable.h"


enum ANIMATIONS_LOW { WALKCR=0, WALK, RUN, BACK, SWIM, JUMP, LAND, JUMPB, LANDB, IDLE, IDLECR, TURN };
enum ANIMATIONS_HIGH { GESTURE=0, ATTACK, ATTACK2, DROP, RAISE, STAND, STAND2 };
//enum anim_bot { NOANIM=-1, DEATH1=0, DEAD1, DEATH2, DEAD2, DEATH3, DEAD3};

class Player : public Drawable, public DynamicShotTestable{
public:
  void draw(int m);
  
  
  float rotateHor, rotateVer;
  float rotateAll;

  int jumped;
  float framenumlow; 
  float framenumhigh;

  int isDead;

  float scale;

  int model;
  int weapon;

  ANIMATIONS_LOW anim_low;
  ANIMATIONS_HIGH anim_high;


  float pitch, yaw;

  void tick(float DeltaTime);



  ANIMATIONS_LOW low_state;
  ANIMATIONS_HIGH high_state;

  int anim_low_reversed;

  int sidewalk;

  float legangle;
  void changeLowState(ANIMATIONS_LOW anim);
  void playhighAnim(ANIMATIONS_HIGH anim);
  void playlowAnim(ANIMATIONS_LOW anim);
  Player();
  int one_shot_low;
  int one_shot_high;

  int high_looped;
  int low_looped;

  static md3Model *weapons;
  static PlayerModel *models;

  int side;

  float life;

  int num;

  int shotTest(D3DXVECTOR3 posO, D3DXVECTOR3 posN, int *index, float *pU, float* pV, float *dist);

  void bulletHit(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize);

  int hasFlag;

  float minY;
};


#endif