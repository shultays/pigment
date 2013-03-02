#include "ShadowVolume.h"


ShadowVolume* ShadowVolume::allShadows[MAX_SHADOW];
int ShadowVolume::shadowSize = 0;


void ShadowVolume::addShadowList(){
  shadowNum = shadowSize;
  allShadows[shadowNum] = this;
  shadowSize++;
}

ShadowVolume::ShadowVolume(){
  addShadowList();

}