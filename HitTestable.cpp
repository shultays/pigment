#include "HitTestable.h"


int HitTestable::HitTestnum = 0;
HitTestable* HitTestable::allHitTestables[MAX_HITTESTABLE];


 
void HitTestable::addHitTest(){
  HitTestplace = HitTestnum;
  allHitTestables[HitTestnum] = this;
  HitTestnum++;
}


void HitTestable::removeHitTest(){
  HitTestnum--;
  allHitTestables[HitTestplace] = allHitTestables[HitTestnum];
  HitTestplace = -1;
}
