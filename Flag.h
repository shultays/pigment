
#ifndef FLAG_H
#define FLAG_H

#include "Tools.h"
#include "BasicX.h"
#include "Camera.h"
#include "Drawable.h"

class Flag : public BasicX{
  
public:
  Flag();

  void draw(int m);


  int owner;

  int type;
  float percent;

  int side;
};


#endif