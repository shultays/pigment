#include "StaticShotTestable.h"


int StaticShotTestable::StaticShotTestablenum = 0;
StaticShotTestable* StaticShotTestable::allStaticShotTestables[MAX_STATICShotTestable];


void StaticShotTestable::addStaticShotTestableList(){
  allStaticShotTestables[StaticShotTestablenum] = this;
  StaticShotTestablenum++;
}

