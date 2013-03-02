
#include "DirectInput.h"
#include "Tools.h"

DirectInput::DirectInput(){

}

DirectInput::~DirectInput(){
	DInput->Release(); DInput = NULL;
	Keyboard->Release(); Keyboard = NULL;
	Mouse->Release(); Mouse = NULL;
}


void DirectInput::InitializeDirectInput(HINSTANCE appInstance, HWND hwnd, 
								   DWORD keyboardCoopFlags = DISCL_NONEXCLUSIVE|DISCL_FOREGROUND,
		                           DWORD mouseCoopFlags    = DISCL_NONEXCLUSIVE|DISCL_FOREGROUND)
{


	ZeroMemory(KeyboardState, sizeof(KeyboardState));
	ZeroMemory(&MouseState, sizeof(MouseState));

	DirectInput8Create( appInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DInput, 0);

	DInput->CreateDevice(GUID_SysKeyboard, &Keyboard, 0);
	Keyboard->SetDataFormat(&c_dfDIKeyboard);
	Keyboard->SetCooperativeLevel(hwnd, keyboardCoopFlags);
	Keyboard->Acquire();

	DInput->CreateDevice(GUID_SysMouse, &Mouse, 0);
	Mouse->SetDataFormat(&c_dfDIMouse2);
	Mouse->SetCooperativeLevel(hwnd, mouseCoopFlags);
	Mouse->Acquire();

  for(int i=0; i<256; i++){
    pressedArr[i] = 0;
    KeyPressedArr[i] = 0;
    KeyReleasedArr[i] = 0;
  }
}

void DirectInput::Clear(){
  for(int i=0; i<256; i++){
    //pressedArr[i] = 0;
    KeyPressedArr[i] = 0;
    KeyReleasedArr[i] = 0;
    KeyboardState[i] = 0;
  }
}

void DirectInput::Poll(){

	HRESULT hr = Keyboard->GetDeviceState(sizeof(KeyboardState), 
		(void**)&KeyboardState); 

	if( FAILED(hr) )	{
		ZeroMemory(KeyboardState, sizeof(KeyboardState));

		hr = Keyboard->Acquire();
	}

	hr = Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), 
		(void**)&MouseState); 

	if( FAILED(hr) )	{
		ZeroMemory(&MouseState, sizeof(MouseState));

		hr = Mouse->Acquire(); 
	}
  int i;
  for(i=0; i<256; i++){
    KeyPressedArr[i] = KeyReleasedArr[i] = 0;
     
    if(KeyboardState[i] & 0x80){
      if(pressedArr[i] == 0){
        pressedArr[i] = 1;
        KeyPressedArr[i] = 1;
      }
    }else{
      if(pressedArr[i] == 1){
        pressedArr[i] = 0;
        KeyReleasedArr[i] = 1;
      }
    }
  }
}



int DirectInput::KeyDown(char key){
	return (KeyboardState[key] & 0x80);
}


int DirectInput::KeyPressed(char key){
	return (KeyPressedArr[key]);
}

int DirectInput::KeyReleased(char key){
	return (KeyReleasedArr[key]);
}


int DirectInput::MouseButtonDown(int button){
	return (MouseState.rgbButtons[button] & 0x80);
}

int DirectInput::MouseXDiff(){
  return MouseState.lX;
}

int DirectInput::MouseYDiff(){
  return MouseState.lY;
}