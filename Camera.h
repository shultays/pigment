
#pragma once
#include <d3dx9.h>


class Camera{
public:
	Camera();
	~Camera();

	float Pitch, Yaw;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Target;
	D3DXVECTOR3 Up;
	D3DXVECTOR3 LookVector;
	D3DXMATRIX View;

  void UpdateCamera(float ForwardUnits, float SidewardUnits, float UP, float delta, float newSpeed[3]);
  void UpdateView(int tp);

};