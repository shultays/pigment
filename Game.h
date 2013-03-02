#ifndef GAME_H
#define GAME_H

#include "Networks.h"

#include "Camera.h"
#include "DirectInput.h"
#include "String.h"
#include "Tools.h"
#include "SolidX.h"
#include "PlayerModel.h"
#include "md3Model.h"
#include "Player.h"
#include "Console.h"
#include "Terrain.h"
#include "ShadowVolumeCPU.h"
#include "ShadowVolumeGPU.h"
#include "BasicX.h"
#include "Bullet.h"
#include "OrbitX.h"
#include "Flag.h"
#include "ControlPoint.h"

class Game {
public:
  float slowfactor;
  OrbitX *sun;
  char name[32];

  LPD3DXFONT dxfont;
  int fps, fpsnow;
  DWORD LastSec;

  SolidX *map;

  IDirect3DDevice9* D3DDevice;
  DirectInput *DInput;
  void Setup(void);
  void Cleanup(void);
  
  Game();
  
  LPD3DXSPRITE sprite; 

  int onWall;
  float speed[4];
  float speedDir[3];
  D3DXVECTOR3 push;
  
  ANIMATIONS_LOW walkstate;
  int reverseleganim;

  void Tick();
  int currentShader;

  Camera *ActiveCamera;
  ID3DXMesh *m2;
  IDirect3DTexture9 *t2, *cross;

  ID3DXEffect *standartEffect, *shadowEffect;

  D3DXMATRIX Proj;

  DWORD CurrentTime;
  DWORD LastTime;
  float DeltaTime;

  Terrain *terrain;

  void UpdateDeltaTime(void);
  void renderToTexture();
  void renderWorld();

/*
  IDirect3DTexture9* TempTexture;
  IDirect3DSurface9* TempSurface;
  IDirect3DTexture9* TempTexture2;
  IDirect3DSurface9* TempSurface2;*/


  
  //void glowDraw(ID3DXMesh* mesh, D3DXVECTOR3 color);
  void render(void);

  Console con;
  char consoleOn;

  void CheckConsole(void);

  void controlPlayer();

public:
  int playerNum;
  Player players[32];

  Player testplayer;

  PlayerModel p;
  md3Model guns[3];

  Flag flag;

  int side;

  int weapon;
  int nextWeapon;
  float weaponTimer;
  void changeWeapon(int w);

  //bools
  char thirdperson;
  char collision;
  char flying;
  
  float shotTimer;

  float life;
  
  int isDead;

  ControlPoint *cps[3];

  fstream debug;
  
};


const static char *shaders[] = { "Normal", "Greyscale","BlackAndWhite2","Negative", "Emboss", "Fish", "Waves" , "Red", "Blue", "Green"};

#endif
