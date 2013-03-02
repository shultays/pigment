

#include "Console.h"

#include "Game.h"



void Console::setup(){
  bools.push_back(boolvar("CLS", &(g->collision)));
  bools.push_back(boolvar("FLY", &(g->flying)));
  bools.push_back(boolvar("LIGHTTEST", &(Drawable::lightTest)));
  bools.push_back(boolvar("SHADOW", &(Drawable::shadowOn)));

  intvars.push_back(intvar("LIGHTNUM", &(Drawable::lightNum)));
}


void Console::Tick(){
  KeyCheck();
}

void Console::DispatchLine(string &str){
  stringstream stream;
  stream << str.c_str();
  stream >> strings[0];

  if(strcmp(strings[0], "CONNECT") == 0){
    stream >> strings[0];
    ints[0] = 15001;
    Networks::Setup(strings[0], ints[0]);
  }else if(strcmp(strings[0], "CONNECTP") == 0){
    stream >> strings[0];
    stream >> ints[0];
    Networks::Setup(strings[0], ints[0]);
  }else if(strcmp(strings[0], "LOCAL") == 0){
    

    ints[0] = 15001;
    Networks::Setup(NULL, ints[0]);
  }else if(strcmp(strings[0], "MSG") == 0){
    stream >> strings[0];
    Networks::SendStr(strings[0]);
  }else if(strcmp(strings[0], "TP") == 0){

    g->thirdperson = 1-g->thirdperson;

    if(g->thirdperson){
      Print("Third Person is on");
      g->testplayer.addDrawList();
    }else{
      Print("Third Person is off");
      g->testplayer.removeDrawList();
    }
  }
     
  {
  list<boolvar>::iterator i;

  for(i=bools.begin(); i != bools.end(); ++i){
    if(strcmp(strings[0], i->name) == 0){
      *i->var = 1-*i->var;
      if(*i->var) buff << i->name << " is on";
      else buff << i->name << " is off";
      Print();

    }
  }
  }
  {

  list<intvar>::iterator i;

  for(i=intvars.begin(); i != intvars.end(); ++i){
    if(strcmp(strings[0], i->name) == 0){
      stream >> ints[0];
      *i->var = ints[0];
      buff << i->name << " is " << *(i->var);

      Print();

    }
  }
  }
}


void Console::Render(){
    
  D3DXMATRIX World;
  D3DXMatrixIdentity(&World);
  Tools::D3DDevice->SetTransform(D3DTS_WORLD, &World);

  d3dspt->Begin(D3DXSPRITE_ALPHABLEND); 

  D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
  d3dspt->SetTransform(&mat);
  
  d3dspt->Draw(sprite, NULL, NULL, &center, D3DCOLOR_ARGB(200, 255, 255, 255)); 

  d3dspt->End();
  RECT textbox; 
  SetRect(&textbox, 0, Tools::ScreenHeight-16, Tools::ScreenWidth, Tools::ScreenHeight); 

  dxfont->DrawTextA(NULL,(inputline+'|').c_str(),
                            (int)inputline.length()+1,
                          &textbox,
                    DT_LEFT,
                    D3DCOLOR_ARGB(255, 255, 255, 255));

  for(int i=0; i<LINENUM; i++){
    SetRect(&textbox, 0, Tools::ScreenHeight - 16*(i+1), Tools::ScreenWidth, Tools::ScreenHeight - 16*i); 
 
    dxfont->DrawTextA(NULL,

                    lines[ (currentline + i) % LINENUM].c_str(),
                    (int)lines[ (currentline + i) % LINENUM].length(),
                    &textbox,
                    DT_LEFT,
                    D3DCOLOR_ARGB(255, 255, 255, 255));
  }
}

void Console::Print(){
  lines[currentline]=buff.str();
  currentline--;
  if(currentline == -1) currentline = LINENUM-1;
  buff.str("");
  lines[currentline] = "";
}

void Console::Print(char *str){
  lines[currentline]=str;
  currentline--;
  if(currentline == -1) currentline = LINENUM-1;
  lines[currentline] = "";
}

void Console::KeyCheck(){  
  if(length < 127){
    for(int i=0; i<26; i++){
      if(Tools::DInput->KeyPressed(chars[i])){
        inputline += 'A' + i;
      }
    }
    for(int i=0; i<10; i++){
      if(Tools::DInput->KeyPressed(digits[i])){
        inputline += '0' + i;
      }
    }

    if(Tools::DInput->KeyPressed(DIK_SPACE)){
      inputline += ' ';
    }
    if(Tools::DInput->KeyPressed(DIK_PERIOD)){
      inputline += '.';
    }
  }

  if(Tools::DInput->KeyPressed(DIK_BACKSPACE) && inputline.length() > 0){
    inputline.resize(inputline.size()-1);
  }

  if(Tools::DInput->KeyPressed(DIK_RETURN) && inputline.length() > 0){
    lines[currentline] = inputline;
    currentline--;
    if(currentline == -1) currentline = LINENUM-1;

    DispatchLine(inputline);
    inputline="";

  }
}

Console::Console(){

    Tools::LoadTexture("coffee2.jpg", sprite);

    float scaleY = Tools::ScreenHeight/1024.0f;
    float scaleX = Tools::ScreenWidth/1024.0f;
    D3DXCreateSprite(Tools::D3DDevice, &d3dspt);    
    D3DXVECTOR2 scaling(scaleX, scaleY);
    D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,NULL,0,NULL);

    for(int i=0; i<LINENUM; i++){
      lines[i] = "";
    }
    inputline = "";
    length = 0;
    currentline = 0;

    D3DXCreateFont(Tools::D3DDevice,    // the D3D Device
                   10,    // font height of 30
                   0,    // default font width
                   FW_BOLD,    // font weight
                   1,    // not using MipLevels
                   false,    // italic font
                   DEFAULT_CHARSET,    // default character set
                   OUT_DEFAULT_PRECIS,    // default OutputPrecision,
                   DEFAULT_QUALITY,    // default Quality
                   FF_SCRIPT,    // default pitch and family
                   "Courier",    // use Facename Arial

                   &dxfont);    // the font object
  }