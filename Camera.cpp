#include "Camera.h"


#include "Game.h"
extern Game *g;

Camera::Camera(){

	Pitch = 0.0f;
	Yaw = 0.0f;
	Position = D3DXVECTOR3(0.0f, 6.0f, 0.0f);
	Target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVec3Normalize(&LookVector, &(Target - Position));
}

Camera::~Camera(){
}

void Camera::UpdateCamera(float ForwardUnits, float SidewardUnits, float UP,
                          float delta, float newSpeed[3]){

	if( Pitch < -1.56f )
		Pitch = -1.56f;
	if( Pitch > 1.56f )
		Pitch = 1.56f;

	if( Yaw >= 6.28f )
		Yaw = 0.0f;
	if( Yaw <= -6.28f)
		Yaw = 0.0f;

	D3DXVec3Normalize(&LookVector, &(Target - Position));

	D3DXVECTOR3 XZLookVector = LookVector;
	
  if(!g->flying)
    XZLookVector.y = 0;

	D3DXVec3Normalize(&XZLookVector, &XZLookVector);
	D3DXVECTOR3 SideVector( XZLookVector.z, 0.0f, -XZLookVector.x );

  D3DXVECTOR3 diff =  (XZLookVector * ForwardUnits) + (SideVector * SidewardUnits);

  diff.y += UP;

	Position += diff*delta;
	Target += diff*delta;

  newSpeed[0] = diff.x;
  newSpeed[1] = diff.y;
  newSpeed[2] = diff.z;

	Target = D3DXVECTOR3( cosf(Pitch) * cosf(Yaw) * 10.0f, sinf(Pitch) * 10.0f, sinf(Yaw) * cosf(Pitch) * 10.0f) + Position;

  D3DXMatrixLookAtLH(&View, &Position, &Target, &Up);
}


void Camera::UpdateView(int tp){

  if(tp == 0){
	  Target = D3DXVECTOR3( cosf(Pitch) * cosf(Yaw) * 10.0f, sinf(Pitch) * 10.0f, sinf(Yaw) * cosf(Pitch) * 10.0f) + Position;
    D3DXMatrixLookAtLH(&View, &Position, &Target, &Up);
  }else{
    D3DXVECTOR3 look = D3DXVECTOR3( cosf(Pitch) * cosf(Yaw) * 10.0f, sinf(Pitch) * 10.0f, sinf(Yaw) * cosf(Pitch) * 10.0f);
    D3DXMatrixLookAtLH(&View, &(Position-look*18+Up*4), &Position, &Up);
  }
}