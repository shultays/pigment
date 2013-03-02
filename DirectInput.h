
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <d3dx9.h>

class DirectInput
{
	IDirectInput8*       DInput;
	IDirectInputDevice8* Keyboard;
	char                 KeyboardState[256]; 
  char                 KeyPressedArr[256];
  char                 KeyReleasedArr[256];
  char                 pressedArr[256];

public:
  
	IDirectInputDevice8* Mouse;
	DIMOUSESTATE2        MouseState;

	DirectInput();
	~DirectInput();

	void InitializeDirectInput(HINSTANCE appInstance, HWND hwnd, 
		                       DWORD keyboardCoopFlags, DWORD mouseCoopFlags);

	void Poll();

	int KeyDown(char key);
	int MouseButtonDown(int button);
 
  int KeyPressed(char key);
  int KeyReleased(char key);

  int MouseXDiff();
  int MouseYDiff();

  void Clear();
};