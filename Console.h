#ifndef CONSOLE_H
#define CONSOLE_H
 
#include "Tools.h"

#include <iostream>
#include <sstream>
#include <string>
#include <list>



using namespace std;
#define LINENUM 64

class Game;

const char chars[] = {DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J,
                    DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T,
                    DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z};

const char digits[] = { DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9 };


typedef struct boolvar{
  boolvar(char *c1, char *c2){
    name = c1;
    var = c2;
  }
  char *name;
  char *var;
}boolvar;


typedef struct intvar{
  intvar(char *c1, int *c2){
    name = c1;
    var = c2;
  }
  char *name;
  int *var;
}intvar;

class Console{
private:
  

  list<boolvar> bools;
  list<intvar> intvars;
  char strings[4][64];
  int ints[4];
  float floats[4];
  LPDIRECT3DTEXTURE9 sprite;
  LPD3DXSPRITE d3dspt;
  string lines[LINENUM];
  string inputline;
  int currentline;
  int length;

  D3DXMATRIX mat;
  LPD3DXFONT dxfont;
  void DispatchLine(string &str);


  void KeyCheck();
public:
  void setup();
  
  Console();
  void Print(char *str);
  void Print();
  stringstream buff;
  void Render();
  Game *g;

  void Tick();
};

#endif