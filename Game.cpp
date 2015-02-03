#include "Game.h"
#include <stdio.h>
#include <mmsystem.h>

#include "DeadPlayer.h"

void Game::UpdateDeltaTime() {
  CurrentTime = timeGetTime();

  DeltaTime = ((float)CurrentTime - (float)LastTime) * 0.001f * slowfactor;
  if (DeltaTime == 0.0f){
	  DeltaTime = 0.001f;
  }
  fpsnow++;
  if(CurrentTime - LastSec > 1000){
    LastSec = CurrentTime;
    fps = fpsnow;
    fpsnow = 0;
  }

  LastTime = CurrentTime;
}

void Game::Setup( void ){
  
  side = 0;

  life = 100;
  isDead = 0;

  debug.open("debug.txt", fstream::out);
  
  srand(322221);
  for(int i=0; i<6*4; i++){
    debug << (rand()%1000/1000.0) << ", ";
  }

  playerNum = -1;

  Tools::g = this;

  Player::weapons = guns;
  Player::models = &p;

  strcpy_s(name, 32, "Nameless");
  Networks::g = this;

  flying = 0;
  collision = 1;
  
  if(D3DXCreateEffectFromFile( D3DDevice, "shadowCPU.fx", NULL, NULL, NULL, NULL, &ShadowVolumeCPU::shadowEffect, NULL) != D3D_OK) ALERTN("./shadow.fx");
  if(D3DXCreateEffectFromFile( D3DDevice, "shadowGPU.fx", NULL, NULL, NULL, NULL, &ShadowVolumeGPU::shadowEffect, NULL) != D3D_OK) ALERTN("./shadow2.fx");
  if(D3DXCreateEffectFromFile( D3DDevice, "Mesh.fx", NULL, NULL, NULL, NULL, &standartEffect, NULL ) != D3D_OK) ALERTN("./ps_vs.fx");
  if(D3DXCreateEffectFromFile( D3DDevice, "MD3.fx", NULL, NULL, NULL, NULL, &(PlayerModel::md3Effect), NULL ) != D3D_OK) ALERTN("./md3.fx");
  if(D3DXCreateEffectFromFile( D3DDevice, "Flag.fx", NULL, NULL, NULL, NULL, &(flag.effect ), NULL ) != D3D_OK) ALERTN("./md3.fx");

  t2 = Tools::LoadTexture("dama.jpg");

  ActiveCamera = new Camera();
  ActiveCamera->Position = D3DXVECTOR3(0, 4000 , 500);

  D3DXMatrixPerspectiveFovLH( &Proj, D3DX_PI /4, ((float)Tools::ScreenWidth) / Tools::ScreenHeight, 10.0f, 100000.0f );
  Drawable::Proj = &Proj;
  Drawable::View = &(ActiveCamera->View);

  D3DDevice->SetTransform(D3DTS_PROJECTION, &Proj); 

  D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
  D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE); 
  D3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE); 

  D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  D3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  //D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  speed[0] = speed[1] = speed[2] = 0;

  onWall = 0;
  D3DXCreateFont(D3DDevice, 24, 0, FW_NORMAL, 1, true, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                   "Arial",  &dxfont);

  map = new SolidX("Map.X");
  map->effect = standartEffect;

  //ShadowVolumeCPU *t = new ShadowVolumeCPU(map);
  ShadowVolumeGPU *t = new ShadowVolumeGPU(map);

  /*
  for(int i=0; i<10; i++){
	  BasicX *tea = new BasicX("tea.X");
	  tea->effect = standartEffect;
	  tea->size = 20+rand()%150;
	  tea->rotation = (rand()%1000)/300.0;

	  
	  tea->coors = D3DXVECTOR3(rand()%12000 - 6000, 1000.0+rand()%3000, rand()%12000 - 6000);
	  tea->recalculateWorld();
	  tea->addDrawList();
	 new ShadowVolumeGPU(tea);

  }*/

  SolidX *w1 = new SolidX("Sky.X");
  w1->effect = standartEffect;


  float arrYaw[] = {2.082, 2.934, 2.668, 3, 2.338, 3.448, 2.956, 2.716, 3.924, 2.928, 3.41, 2.29, 2.562, 3.654, 3.922, 2.982, 3.99, 3.884, 3.654, 2.872, 2.782, 3.208, 3.804, 2.306};
  float arrInit[] = {1.114, 0.656, 0.252, 0.1, 0.694, 0.692, 1.124, 1.898, 1.636, 1.116, 1.934, 0.27, 0.586, 0.458, 1.432, 0.478, 0.504, 0.166, 1.006, 0.052, 1.244, 0.192, 1.69, 0.008};
  float speeds[] = {0.7, 0.75, 0.943, 0.22, 0.402, 0.839, 0.032, 0.724, 0.264, 0.287, 0.818, 0.311, 0.205, 0.804, 0.507, 0.032, 0.689, 0.769, 0.669, 0.526, 0.135, 0.768, 0.324, 0.321};
  int k=0;
  for(int i=0; i<4; i++){
    OrbitX *w2 = new OrbitX("Cloud0.X");
    w2->effect = standartEffect;
    
    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;

    k++;

    w2 = new OrbitX("Cloud1.X");
    w2->effect = standartEffect;
    
    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;

    k++;

    w2 = new OrbitX("Cloud2.X");
    w2->effect = standartEffect;

    
    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;

    k++;

    w2 = new OrbitX("Cloud3.X");
    w2->effect = standartEffect;
    
    
    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;
    
    k++;

    w2 = new OrbitX("Cloud4.X");
    w2->effect = standartEffect;

    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;


    k++;

    w2 = new OrbitX("Cloud5.X");
    w2->effect = standartEffect;

    w2->initPos = PI*arrInit[k];
    w2->yaw = PI*arrYaw[k]/10;
    w2->speeds = 128 + 256*speeds[k];
    w2->speeds *= 0.0000005;
    k++;
  }

  sun = new OrbitX("Sun.X");
  sun->effect = standartEffect;
  sun->initPos = PI;
  sun->yaw = (2*PI)/(10);
  sun->speeds = 350;
  sun->speeds *= 0.0000005;
  sun->rotateSelf = 1;
  terrain = new Terrain("T4.X");
  terrain->effect = standartEffect;

  
  Bullet::effectS = standartEffect;

  
  cps[0] = new ControlPoint("ControlPoint.x", D3DXVECTOR4(153/255.0f, 228/255.0f, 214/255.0f, 1.0f));
  cps[0]->effect = standartEffect;
  cps[0]->coors.y = 2720;
  cps[0]->recalculateWorld();

  
  cps[2] = new ControlPoint("ControlPoint.x", D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
  cps[2]->effect = standartEffect;
  cps[2]->coors.x = -42;
  cps[2]->coors.y = 1125;
  cps[2]->coors.z = -5455;
  cps[2]->recalculateWorld();

  
  cps[1] = new ControlPoint("ControlPoint.x", D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
  cps[1]->effect = standartEffect;
  cps[1]->coors.x = 42;
  cps[1]->coors.y = 1125;
  cps[1]->coors.z = 5455;
  cps[1]->recalculateWorld();

  
  flag.coors = Tools::g->cps[0]->coors;
  flag.size = 1.5;
  flag.owner = -1;
  flag.type = 2;
  flag.recalculateWorld();

  con.g = this;
  con.setup();


  Drawable::setup();
  consoleOn = 0;
    
  con.buff << "Hello world!";
  con.Print();
  
  fps = fpsnow = 0;

  testplayer.model = 0;

  testplayer.weapon = 0;


  push[0] = push[1] = push[2] = 0;
  thirdperson = 0;
  //testplayer.addDrawList();
  weapon = 0;
  nextWeapon = -1;


  slowfactor = 1.0f;

  p.load("./players/player.txt" );

  //guns[0].load("./guns/machinegun/machinegun_barrel_1.md3", "./guns/machinegun/machinegun.jpg",0);
  guns[0].load("./guns/shotgun/shotgun_1.md3", "./guns/shotgun/shotgun.jpg",0);
  guns[1].load("./guns/rocketlauncher/rocketl_1.md3", "./guns/rocketlauncher/rocketl.jpg",0);
  
  //hands[0].load("./guns/rocketlauncher/rocketl_1.md3", "./guns/rocketlauncher/rocketl.jpg",0);

  guns[0].clear();
  guns[1].clear();



  Tools::LoadXFile("./ball.x", Bullet::ball);

  shotTimer = -1;

  for(int i=0; i<32; i++){
    players[i].addDynamicShotTestableList();
  }

  LastSec = LastTime = CurrentTime = timeGetTime();


  D3DXCreateSprite(Tools::D3DDevice, &sprite);

  
  D3DXCreateTextureFromFileEx(Tools::D3DDevice,    
                            "cross.bmp", 
                            D3DX_DEFAULT_NONPOW2, 
                            D3DX_DEFAULT_NONPOW2,

							D3DX_FROM_FILE,
                            NULL,    
                            D3DFMT_A8R8G8B8,  
                            D3DPOOL_MANAGED,
                            D3DX_DEFAULT,   
                            D3DX_DEFAULT,   
                            D3DCOLOR_ARGB(255,0,0,0),   
                            NULL,    
                            NULL,   
                            &cross);

}

void Game::render(void){ 
  
 
  standartEffect->SetFloatArray( "EyePos", ActiveCamera->Position, 3);
  PlayerModel::md3Effect->SetFloatArray( "EyePos", ActiveCamera->Position, 3);
  
  
	D3DXMATRIX View, World;

  D3DDevice->SetTransform(D3DTS_VIEW, &ActiveCamera->View);

  
  Drawable::render();

  D3DXVECTOR3 center(Tools::ScreenWidth/2-8, Tools::ScreenHeight/2-8, 0.0f);

  
  if(thirdperson == 0){
    sprite->Begin(D3DXSPRITE_ALPHABLEND); 
    sprite->Draw(cross, NULL, NULL, &center, D3DCOLOR_ARGB(255, 255, 255, 255)); 
    sprite->End();
  }

  static RECT textbox; SetRect(&textbox, 0, 0, 400, 100); 
  stringstream str;
  str << fps;


  
  dxfont->DrawTextA(NULL,

                      str.str().c_str(),
                      (int)str.str().length(),
                      &textbox,
                      DT_RIGHT | DT_RIGHT,
                      D3DCOLOR_ARGB(255, 255, 255, 255));


  if(consoleOn)  con.Render();
}



void Game::Cleanup(){
  terrain->Release();
  delete terrain;
 // RenderSurface->Release();
 // standartEffect->Release();
 // postProcessingEffect->Release();

  debug.close();

  Networks::Release();

  delete DInput;
  delete ActiveCamera;
}

Game::Game(){

}


void Game::Tick(){

  DInput->Poll();
  if(DInput->KeyPressed(DIK_F12) || DInput->KeyPressed(DIK_F11) ){
    consoleOn = 1 - consoleOn;
  }
  if(consoleOn){
    con.Tick();
    DInput->Clear();
  }

  GetLock();
  UpdateDeltaTime();

  
  Networks::getScene();
  if(nextWeapon != -1){
    weaponTimer -= DeltaTime*1000;
    if(weaponTimer < 0){
      weapon = nextWeapon;
      testplayer.weapon = weapon;
      nextWeapon = -1;
      testplayer.playhighAnim(RAISE);
      //ALERT;
    }
  }


  if( DInput->KeyPressed( DIK_C) ){
    thirdperson = 1-thirdperson;

    if(thirdperson){
      testplayer.addDrawList();
    }else{
      testplayer.removeDrawList();
    }
  }
  if( DInput->KeyPressed( DIK_F1 ) ) changeWeapon(0);
  if( DInput->KeyPressed( DIK_F2 ) ) changeWeapon(1);

  if( DInput->KeyPressed(DIK_F7)){
    ActiveCamera->Position = D3DXVECTOR3(0, 3000, 0);
  }
  if( DInput->KeyPressed(DIK_F5)){
    fstream file;
    file.open(L"save.txt", ios::out);

    file << ActiveCamera->Position[0] << " " 
         << ActiveCamera->Position[1] << " " 
         << ActiveCamera->Position[2] << "\n" ;
    
    file << ActiveCamera->Pitch<< " " 
         << ActiveCamera->Yaw << "\n" ;

    file.close();

    debug << "D3DXVECTOR3(" << ActiveCamera->Position[0] << "," 
         << ActiveCamera->Position[1] << "," 
         << ActiveCamera->Position[2] << ")," ;
    
  }
  if( DInput->KeyPressed( DIK_O ) ){
    side = 1-side; 
    testplayer.side = side;
  }

  if( DInput->KeyPressed(DIK_F6)){
    fstream file;
    file.open(L"save.txt", ios::in);
    file >> ActiveCamera->Position[0]
         >> ActiveCamera->Position[1]
         >> ActiveCamera->Position[2];

    
    file >> ActiveCamera->Pitch
         >> ActiveCamera->Yaw;


    file.close();

  }

  if(Tools::DInput->KeyDown(DIK_J)) life -= 1;
  if(Tools::DInput->KeyDown(DIK_K)) life += 1;

  testplayer.life = life;

  controlPlayer();
  for(int i=0; i<playerNum; i++){
    players[i].tick(DeltaTime);
  }

  for(int i=0; i<DeadPlayer::dpsNum; i++){
    DeadPlayer::dps[i]->tick(DeltaTime);
  }
  Bullet::tickAll(DeltaTime);

  Camera *moveCam = ActiveCamera;

  D3DXVECTOR3 oldN = moveCam->Position;

  moveCam->Pitch -= DInput->MouseState.lY  *0.005f;
  moveCam->Yaw -= DInput->MouseState.lX *0.005f;


  ActiveCamera->Position += push*DeltaTime*50;

  
  if(D3DXVec3Length(&push) < 100*DeltaTime){
     push.x = push.y = push.z = 0;

  }else{
    push -=Tools::Normalize(push)*100*DeltaTime;
  }

  moveCam->UpdateCamera(speed[0], speed[2], speed[1], DeltaTime, speedDir);


  D3DXVECTOR3 newN = moveCam->Position;
  if(collision){
    for(int i=0; i<HitTestable::HitTestnum; i++){
	    HitTestable::allHitTestables[i]->HitTest(oldN, &newN, speed, DeltaTime);
    }
  }
  if(speed[3] > 0) onWall = 1;
  moveCam->Position = newN;
  moveCam->UpdateView(thirdperson);

  
  testplayer.coors = ActiveCamera->Position;
  testplayer.tick(DeltaTime);
  //150 1675 6750

  //125 200 6200

  if(DInput->KeyPressed(DIK_R)){
    Player *p = new Player();
    *p = testplayer;
    //p->placeDrawable = -1;
    p->addDrawList();
  }


  if(isDead == 1){
    if(side == 0){
      if( ActiveCamera->Position.x > -150 && ActiveCamera->Position.y > 1605 && ActiveCamera->Position.z > 6150
        && ActiveCamera->Position.x < 200 && ActiveCamera->Position.y < 1965 && ActiveCamera->Position.z < 6750){
        life += DeltaTime*10;
      }
    }else{
    if( ActiveCamera->Position.x > -200 && ActiveCamera->Position.y > 1605 && ActiveCamera->Position.z > -6750
        && ActiveCamera->Position.x < 150 && ActiveCamera->Position.y < 1965 && ActiveCamera->Position.z < -6150){
        life += DeltaTime*10;
      }
    }

    if(life > 100){
      life = 100;
      isDead = 0;
      testplayer.isDead = 0;
      
      push.x = rand()%40 - 20;
      ActiveCamera->Position.y = 1800;
      ActiveCamera->Pitch = 0;
      if(side == 0){
        push.z = -100;
        ActiveCamera->Position.x = 25;
        ActiveCamera->Position.z = 6700;
        ActiveCamera->Yaw = -PId2;
      }else{

        push.z = 100;
        ActiveCamera->Position.x = -25;
        ActiveCamera->Position.z = -6700;
        ActiveCamera->Yaw = PId2;
      }
  
      onWall = 0;

      speed[3] = 0;
      collision = 1;
      flying = 0;
      thirdperson = 0;
      
      testplayer.removeDrawList();
    }
  }else{

    int index;
    float u, v, dist;
    D3DXVECTOR3 coor;
    int onCorrect = 0;

    if(flying){
      onCorrect = 1;
    }
    if(shotTimer >= 0) shotTimer -= DeltaTime;

    if( DInput->MouseButtonDown(0)  && shotTimer < 0){
      D3DXVECTOR4 c(1.0, 0.0, 0.0, 1.0);
      if(this->side == 1){
        c = D3DXVECTOR4(0.0, 0.0, 1.0, 1.0); 
      }
      Bullet *b = Bullet::add(ActiveCamera->Position + Tools::Normalize(ActiveCamera->Target - ActiveCamera->Position)*50, ActiveCamera->Target - ActiveCamera->Position,
                  Networks::num, CurrentTime, c);

      Networks::sendNewBulet(b);

      shotTimer = 0.01f;
    }

    dist = -100000;

    if(map->shotTest(ActiveCamera->Position, D3DXVECTOR3(0.0, -1.0, 0.0), &index, &u, &v, &dist)){
      //con.buff << dist;
      //con.Print();
      if(dist < 150){
        if(map->getSide(index, u, v, &coor) == side){
          onCorrect = 1;
        }
      }
    }

    float dist2;

    if(terrain->shotTest(ActiveCamera->Position, D3DXVECTOR3(0.0, -1.0, 0.0), &index, &u, &v, &dist2)){
      //con.buff << dist;
      //con.Print();
      if(dist2 < dist || dist2 < 150){
        if(terrain->getSide(index, u, v, &coor) == side){
          onCorrect = 1;
        }
      }
    }

    if(cps[0]->onTop(ActiveCamera->Position)){
      onCorrect = 1;
      if(flag.owner == -1 && flag.type == 2){
        Networks::sendFlagDemand();
      }
    }
    
    if(side == 0){
      if(cps[2]->onTop(ActiveCamera->Position)){
        onCorrect = 1;
        if(flag.owner == -1 && flag.type == 1){
          Networks::sendFlagDemand();
        }
      }
    }else{
      if(cps[1]->onTop(ActiveCamera->Position)){
        onCorrect = 1; 
        if(flag.owner == -1 && flag.type == 0){
          Networks::sendFlagDemand();
        }
      }
    }

    
    if(life <= 0){
      collision = 0;
      flying = 1;
      testplayer.addDrawList();
      thirdperson = 1;
      isDead = 1;
      testplayer.isDead = 1;
      DeadPlayer *dp = new DeadPlayer();
      dp->addDeadList(&testplayer);
	  if(flag.owner == Networks::num && Networks::connected) Networks::sendFlagRelease(-1, flag.coors);
    }
   

    if(Networks::connected && flag.owner == Networks::num){
      if(side == 0 && cps[1]->onTop(ActiveCamera->Position)){
        Networks::sendFlagRelease(0, flag.coors);
      }else if(side == 1 && cps[2]->onTop(ActiveCamera->Position)){
        Networks::sendFlagRelease(1, flag.coors);
      }else{

        if(onCorrect){
          flag.coors = coor;
        }else{

          Networks::sendFlagRelease(-1, flag.coors);
        }
      }
    }

    

    if(flag.owner == -1){
      if(onCorrect){

        if(Tools::Distance(D3DXVECTOR3(flag.coors.x, 0, flag.coors.z), 
          D3DXVECTOR3(ActiveCamera->Position.x, 0, ActiveCamera->Position.z)) < 30

          && ActiveCamera->Position.y - 120 < flag.coors.y && ActiveCamera->Position.y + 50 > flag.coors.y)
            
        {

          if(flag.type != side){
            Networks::sendFlagDemand();
          }
            
        }
      }
    }
  }

  Networks::sendScene();
  
  D3DDevice->BeginScene();
  render(); 
  D3DDevice->EndScene();
  ReleaseLock();
  D3DDevice->Present( NULL, NULL, NULL, NULL );

  

}

void Game::changeWeapon(int w){
  if(nextWeapon == -1){
    nextWeapon = w;
    weaponTimer = 260.0;
    
    testplayer.playhighAnim(DROP);
  }
}

void Game::controlPlayer(){
  if(DInput->KeyPressed(DIK_F)){
    flying = 1 - flying;
    collision = 1 - collision;
  }
  if(!flying) {
    if(onWall == 0 && speed[3] < 0) {
      speed[1]-= 1200.0f*DeltaTime;
	  if(speed[1] < -5000) speed[1] = -5000;
    }
    if( DInput->KeyPressed(DIK_SPACE) && speed[3] > 0.0f){
      speed[3] = 0.0f;
      speed[1] = 450.0f;
      testplayer.jumped = 1;
      testplayer.playlowAnim(JUMP);
      onWall = 0;
    }
  }else speed[1] = 0;

  speed[3] -= DeltaTime*1000;

  float s = 400.0f;
  if(isDead) s = 800;
  reverseleganim = 0;

  if(testplayer.jumped){
    walkstate = JUMP;
    if(speed[3] > 0){
      testplayer.jumped = 0;
      testplayer.playlowAnim(LAND);
      walkstate = IDLE;
    }
  }else if(DInput->KeyDown( DIK_W ) || DInput->KeyDown( DIK_S ) || DInput->KeyDown( DIK_A ) || DInput->KeyDown( DIK_D )){
    
    walkstate = RUN;
    if(DInput->KeyDown( DIK_S )) walkstate = BACK;
      if(DInput->KeyDown( DIK_S )) reverseleganim = 1;

    if(DInput->KeyDown( DIK_LSHIFT )){
      walkstate = WALK;
      s = 200.0f;
    }
    
    if(DInput->KeyDown( DIK_LCONTROL )){
      walkstate = WALKCR;
      s = 200.0f;
    }
  }else if(DInput->KeyDown(DIK_LCONTROL)){
    walkstate = IDLECR;
  }else{
    walkstate = IDLE;
  }

  testplayer.sidewalk = 0;
  if( DInput->KeyDown( DIK_A ) ) testplayer.sidewalk = -1;
  if( DInput->KeyDown( DIK_D ) ) testplayer.sidewalk = 1;

  testplayer.anim_low_reversed = reverseleganim;
  testplayer.changeLowState(walkstate);

  testplayer.pitch = ActiveCamera->Pitch;
  testplayer.yaw = -ActiveCamera->Yaw;
  
  if(DInput->KeyPressed(DIK_T)) testplayer.playhighAnim(ATTACK);

  speed[0] = 0;
  speed[2] = 0;
  if( DInput->KeyDown( DIK_W ) )
    speed[0] = s;
  if( DInput->KeyDown( DIK_S ) )
    speed[0] = -s;
  if( DInput->KeyDown( DIK_A ) )
    speed[2] = -s;
  if( DInput->KeyDown( DIK_D ) )
    speed[2] = s; 

  /*if( DInput->MouseButtonDown(0) )
    speed[0] = 1250.0f;*/

  if( DInput->MouseButtonDown(1) && flying)
    speed[0] = 10000.0f;

  if(speed[0] != 0 || speed[2] != 0) onWall = 0;

}
