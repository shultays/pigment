#include "PlayerModel.h"

#include <math.h>
#include "Game.h"

ID3DXEffect *PlayerModel::md3Effect;

void PlayerModel::load(char *pfile){

  int i;                 
  int f;

  effect = md3Effect;
  

  fstream playerfile;
  char headfile[256], lowfile[256], highfile[256] ,
      headskinfile[256], lowskinfile[256], highskinfile[256],
        animationfile[256];

  playerfile.open(pfile, fstream::in);
  playerfile >> headfile;
  playerfile >> lowfile;
  playerfile >> highfile;
  playerfile >> headskinfile;
  playerfile >> highskinfile;
  playerfile >> lowskinfile;
  playerfile >> animationfile;
  playerfile.close();

fstream fanim;
  fanim.open(animationfile, fstream::in);
  char str[256];
  int position;
  int tempi[25][4];
  int j=0;
  for(i=0; i<100; i++){
     position = fanim.tellg();
     fanim.getline(str, 256);
     
     if(fanim.eof()) break;
     if(str[0] >= '0' && str[0] <= '9'){
         fanim.seekg(position);
         fanim >> tempi[j][0] >> tempi[j][1] >> tempi[j][2] >> tempi[j][3];
         fanim.getline(str, 256);
         j++;
     }
  }
  fanim.close();

  for(i=0; i<6; i++){
    v[i][0] = tempi[i][0];
    v[i][1] = tempi[i][1];
    v[i][2] = tempi[i][2];
    v[i][3] = tempi[i][3];
    
    //PFILE2("%d %d\n", v[i][0], v[i][1]);
  }

 for(i=0; i<7; i++){
    n[i][0] = tempi[i+6][0];
    n[i][1] = tempi[i+6][1];
    n[i][2] = tempi[i+6][2];
    n[i][3] = tempi[i+6][3];
    
    //PFILE2("%d %d\n", n[i][0], n[i][1]);
  }
  

  j = n[6][0] + n[6][1] - n[0][0];

  for(i=0; i<12; i++){
    b[i][0] = tempi[i+13][0] -j;
    b[i][1] = tempi[i+13][1];
    b[i][2] = tempi[i+13][2];
    b[i][3] = tempi[i+13][3];
    
    //PFILE2("%d %d\n", b[i][0], b[i][1]);
  }
    
  low.load(lowfile, lowskinfile, b[9][0]);
  f = -1;
  for(i=0; i<low.h.tagnum; i++){
    if(strcmp(low.tags[i].name, "tag_torso") == 0){
      f = i;
      break;
    }
  }
  
  bodytolegs = new translator[low.h.framenum];
  for(i=0; i<low.h.framenum; i++){
    int t = f+low.h.tagnum*i;
    bodytolegs[i].move[0] = low.tags[t].org.x;
    bodytolegs[i].move[1] = low.tags[t].org.z;
    bodytolegs[i].move[2] = low.tags[t].org.y;

    bodytolegs[i].matr = D3DXMATRIX( 
      low.tags[t].ori[0].x, low.tags[t].ori[0].z, low.tags[t].ori[0].y, 0.0,
      low.tags[t].ori[2].x, low.tags[t].ori[2].z, low.tags[t].ori[2].y, 0.0, 
      low.tags[t].ori[1].x, low.tags[t].ori[1].z, low.tags[t].ori[1].y, 0.0,
      0.0,0.0,0.0,1.0);

  }


  high.load(highfile, highskinfile, n[5][0]);
  f = -1;
  for(i=0; i<high.h.tagnum; i++){
    if(strcmp(high.tags[i].name, "tag_head") == 0){
      f = i;
      break;
    }
  }

  headtobody = new translator[high.h.framenum];
  for(i=0; i<high.h.framenum; i++){
    int t = f+high.h.tagnum*i;
    headtobody[i].move[0] = high.tags[t].org.x;
    headtobody[i].move[1] = high.tags[t].org.z;
    headtobody[i].move[2] = high.tags[t].org.y;

    headtobody[i].matr = D3DXMATRIX( 
      high.tags[t].ori[0].x, high.tags[t].ori[0].z, high.tags[t].ori[0].y, 0.0,
      high.tags[t].ori[2].x, high.tags[t].ori[2].z, high.tags[t].ori[2].y, 0.0, 
      high.tags[t].ori[1].x, high.tags[t].ori[1].z, high.tags[t].ori[1].y, 0.0,
      0.0,0.0,0.0,1.0);

  }

  f = -1;
  for(i=0; i<high.h.tagnum; i++){
    if(strcmp(high.tags[i].name, "tag_weapon") == 0){
      f = i;
      break;
    }
  }

  weapontobody = new translator[high.h.framenum];
  for(i=0; i<high.h.framenum; i++){
    int t = f+high.h.tagnum*i;
    weapontobody[i].move[0] = high.tags[t].org.x;
    weapontobody[i].move[1] = high.tags[t].org.z;
    weapontobody[i].move[2] = high.tags[t].org.y;

    weapontobody[i].matr = D3DXMATRIX( 
      high.tags[t].ori[0].x, high.tags[t].ori[0].z, high.tags[t].ori[0].y, 0.0,
      high.tags[t].ori[2].x, high.tags[t].ori[2].z, high.tags[t].ori[2].y, 0.0, 
      high.tags[t].ori[1].x, high.tags[t].ori[1].z, high.tags[t].ori[1].y, 0.0,
      0,0,0,1.0);

  }


  head.load(headfile, headskinfile, 0);

  coors[0] = 400;
  coors[1] = 150;
  coors[2] = 0;

  
  high.clear();
  head.clear();
  low.clear();

  vnum = -1;
  bnum = 9;
  nnum = 5;
  framenumlow = 0.0;
  framenumhigh = 0.0;

  scale = 1.5f;

  yMove = -45;

  //head.max += 5;

  D3DXCreateCylinder(Tools::D3DDevice, 14, 14,
  1,
  20,
  20,
  &cyl,NULL);
    
  rotateHor = 0;
  rotateVer = 0;
  rotateAll = 0;
  legAngle = 0;

  showCyl = 0;

  life = 50;

  hasFlag = 0;
}


void PlayerModel::draw(int m){


  if(isDead && !Tools::g->isDead) return;

  if(Tools::DInput->KeyDown(DIK_H))showCyl = 1;
  else showCyl = 0;

  int framehigh = (int)framenumhigh;
  int framelow = (int)framenumlow;

  int lowbegin, highbegin, lowint, highint;

  if(vnum == -1){
    lowbegin = b[bnum][0];
    lowint = b[bnum][1];

    highbegin = n[nnum][0] ;
    highint = n[nnum][1];
  }else{
    lowbegin = v[vnum][0];
    lowint = v[vnum][1];

    highbegin = v[vnum][0] ;
    highint = v[vnum][1];
  }


  int lowframe, highframe;
  
  lowframe = lowbegin + framelow % lowint;
  highframe = highbegin + framehigh % highint;

  int l1 = lowframe;
  int l2;
  float lowinterpol = framenumlow - framelow;


  if(low_reversed){
   if(lowint == 1)
      l2 = l1;
    else if(l1 == lowbegin)
      l2 = lowbegin + lowint -1;
    else 
      l2 = l1-1;
    
    lowinterpol = 1- lowinterpol;
  }else{
    if(lowint == 1)
      l2 = l1;
    else if(l1 == lowbegin + lowint -1)
      l2 = lowbegin;
    else 
      l2 = l1+1;
  }
 


  int h1 = highframe;
  int h2;
  if(highint == 1)
    h2 = h1;
  else if(h1 == highbegin + highint -1)
    h2 = highbegin;
  else 
    h2 = h1+1;

  float highinterpol = framenumhigh - framehigh;
  /*
  l1 = l2 = lowbegin;
  h1 = h2 = highbegin;*/

	D3DXMATRIX World, World2, World3;
  UINT  Passes;

  //low
  D3DXMatrixScaling(&World2, scale, scale, scale);  
  D3DXMatrixTranslation(&World, coors[0], coors[1]+yMove, coors[2]);
  D3DXMatrixRotationY(&World3, rotateAll);
  World = World3*World2*World;

  D3DXMATRIX legrot;

  D3DXMatrixRotationY(&legrot, legAngle);

  effect->SetFloat("minY", minY);
  if(isDead){
    effect->SetTechnique("PointLight2");
  }else{
    effect->SetTechnique("PointLight");
  }

  if(side == 0){
    effect->SetFloatArray("Color", D3DXVECTOR3(1.0, 0.0, 0.0), 3);
  }else{
    effect->SetFloatArray("Color", D3DXVECTOR3(0.0, 0.0, 1.0), 3);
  }

  
  effect->SetMatrix( "World", &(World*legrot));
	effect->SetMatrix( "WorldViewProjection", &( legrot * World * *View * *Proj) );
  effect->SetTexture( "Texture0", low.texture );
  effect->SetFloat("inter",lowinterpol);

  effect->SetFloat("lifeBar", ((100-life)/50)*(low.max-low.min)+low.min);

	effect->Begin(0, 0);
	effect->BeginPass(m);
  low.draw(l1, l2, lowinterpol);
	effect->EndPass();
	effect->End();

  //high
  D3DXMatrixTranslation(&World2, 
    (bodytolegs[l1].move[0]*(1-lowinterpol) + bodytolegs[l2].move[0]*(lowinterpol)),
    (bodytolegs[l1].move[1]*(1-lowinterpol) + bodytolegs[l2].move[1]*(lowinterpol)), 
    (bodytolegs[l1].move[2]*(1-lowinterpol) + bodytolegs[l2].move[2]*(lowinterpol)));

  World3 = bodytolegs[l1].matr*(1-lowinterpol) +bodytolegs[l2].matr*(lowinterpol);

  World = World3*World2*World;

  D3DXVECTOR3 rot = D3DXVECTOR3(World3._13, World3._23, World3._33);
  D3DXVECTOR3 rot2 = D3DXVECTOR3(World3._12, World3._22, World3._32);
  D3DXMatrixRotationX(&World3, rotateHor*rot.x);
  World = World3*World;
  D3DXMatrixRotationY(&World3, rotateHor*rot.y);
  World = World3*World;
  D3DXMatrixRotationZ(&World3, rotateHor*rot.z);
  World = World3*World;

  D3DXMatrixRotationX(&World3, rotateVer*rot2.x);
  World = World3*World;
  D3DXMatrixRotationY(&World3, rotateVer*rot2.y);
  World = World3*World;
  D3DXMatrixRotationZ(&World3, rotateVer*rot2.z);
  World = World3*World;

  effect->SetMatrix( "World", &(World));
	effect->SetMatrix( "WorldViewProjection", &(World * *View * *Proj) );
  effect->SetFloat("inter",highinterpol);
  effect->SetTexture( "Texture0", high.texture );

  effect->SetFloat("lifeBar", ((55-life)/40)*(high.max-high.min)+high.min);

	effect->Begin(0, 0);
	effect->BeginPass(m);
  high.draw(h1,h2,highinterpol);
	effect->EndPass();
	effect->End(); 

  //head
    D3DXMatrixTranslation(&World2, 
    (headtobody[h1].move[0]*(1-highinterpol) + headtobody[h2].move[0]*(highinterpol)),
    (headtobody[h1].move[1]*(1-highinterpol) + headtobody[h2].move[1]*(highinterpol)), 
    (headtobody[h1].move[2]*(1-highinterpol) + headtobody[h2].move[2]*(highinterpol)));
 
  World3 = headtobody[h1].matr*(1-highinterpol) +headtobody[h2].matr*(highinterpol);


  D3DXMATRIX tempWorld = World;


  World = World3*World2*World;

  effect->SetMatrix( "World", &(World));
	effect->SetMatrix( "WorldViewProjection", &(World * *View * *Proj) );
  effect->SetTexture( "Texture0", head.texture );
  effect->SetFloat("inter",0.0f);
  effect->SetFloat("lifeBar", ((10-life)/10)*((isDead?0:5)+head.max-head.min)+head.min);
  
	effect->Begin(0, 0);
	effect->BeginPass(m);
  head.draw(0,0,0.0);
	effect->EndPass();
	effect->End();

  if(hasFlag){
    D3DXMatrixTranslation(&World2, -3, -3, 0);
    Tools::g->flag.World = World2*World;
    Tools::g->flag.draw(0);
  }
  
  
  //weapon
  
    D3DXMatrixTranslation(&World2, 
    (weapontobody[h1].move[0]*(1-highinterpol) + weapontobody[h2].move[0]*(highinterpol)),
    (weapontobody[h1].move[1]*(1-highinterpol) + weapontobody[h2].move[1]*(highinterpol)), 
    (weapontobody[h1].move[2]*(1-highinterpol) + weapontobody[h2].move[2]*(highinterpol)));
 
  World3 = weapontobody[h1].matr*(1-highinterpol) +weapontobody[h2].matr*(highinterpol);



  World = World3*World2*tempWorld;

  effect->SetMatrix( "World", &(World));
	effect->SetMatrix( "WorldViewProjection", &(World * *View * *Proj) );
  effect->SetTexture( "Texture0", weapon->texture );
  effect->SetFloat("inter",0.0f);
  
	effect->Begin(0, 0);
	effect->BeginPass(m);
  weapon->draw(0,0,0.0);
	effect->EndPass();
	effect->End();

  if(showCyl){
    D3DXMatrixTranslation(&World, coors[0], coors[1]+10, coors[2]);
    D3DXMatrixRotationX(&World3, PId2);

    World = World3*World;

    effect->SetMatrix( "World", &(World));
	  effect->SetMatrix( "WorldViewProjection", &(World * *View * *Proj) );

	  effect->Begin(&Passes, 0);
	  effect->BeginPass(m);
    cyl->DrawSubset(0);
	  effect->EndPass();
	  effect->End();
  }
}

void PlayerModel::release(void){
  head.release();
  low.release();
  high.release();
  delete[] bodytolegs;
  delete[] headtobody;
}
