 #ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "Tools.h"
#include "SolidX.h"


class ControlPoint: public SolidX{
public:

  ControlPoint(char *c, D3DXVECTOR4 cpcolor):SolidX(c){
    for(int i=0; i<subsetNum; i++){
      color[i] = cpcolor;
      
    }

  }

  
  void paint(int index, float pU, float pV, D3DXVECTOR4 c, float circleSize){

  }

  int onTop(D3DXVECTOR3 pos){
    if(pos.y > coors.y - 50&& pos.y < coors.y + 120){
      pos.y = 0;
      D3DXVECTOR3 pos2 = coors;
      pos2.y = 0;
      if(Tools::Distance(pos, pos2) < 120){
        return 1;
      }
    }
    return 0;
  }
};


#endif