#ifndef SHOTTESTABLE_H
#define SHOTTESTABLE_H



class Paintable{
public:
  
  virtual void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize) = 0;

};

#endif