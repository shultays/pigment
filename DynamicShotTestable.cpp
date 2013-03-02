#include "DynamicShotTestable.h"


int DynamicShotTestable::DynamicShotTestablenum = 0;
DynamicShotTestable* DynamicShotTestable::allDynamicShotTestables[MAX_DynamicShotTestable];


void DynamicShotTestable::addDynamicShotTestableList(){
  allDynamicShotTestables[DynamicShotTestablenum] = this;
  DynamicShotTestablenum++;
}

