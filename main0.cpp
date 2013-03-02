#define STRICT
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "DirectInput.h"
#include "Game.h"

HWND hwnd = NULL;
IDirect3DDevice9* D3DDevice = NULL;

DirectInput *DInput;

void InitializeDirect3D(void);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

Game *g;

void Cleanup( void ){
	D3DDevice->Release();

	g->Cleanup();
}

int quit = 0;
LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg,  WPARAM wParam, LPARAM lParam ){
  switch( msg ){
  case WM_KEYDOWN:
	  if( wParam == VK_ESCAPE ){
      quit = 1;
			PostQuitMessage(0);
		}
    break;

		case WM_CLOSE:	
    case WM_DESTROY:	
	  PostQuitMessage(0);
    break;

		default:

	  return DefWindowProc( hWnd, msg, wParam, lParam );
		break;
	}
	return 0;
}


void InitializeDirect3D( void )
{
	IDirect3D9* D3D = NULL;

  D3D = Direct3DCreate9( D3D_SDK_VERSION );

	if( !D3D )
	{
		if( D3D != NULL )
			D3D->Release();

    ALERTN("Direct3DCreate9() - Failed");
		return;
	}

  D3DDISPLAYMODE d3ddm;

  if( FAILED( D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		if( D3D != NULL )
			D3D->Release();

    ALERTN("GetAdapterDisplayMode() - Failed");
		return;
	}

	HRESULT hr;

	if( FAILED( hr = D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
												d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
												D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
	{
		if( hr == D3DERR_NOTAVAILABLE )
		{
			if( D3D != NULL )
				D3D->Release();

      ALERTN("CheckDeviceFormat() - Failed");
			return;
		}
	}

	D3DCAPS9 d3dCaps;

	if( FAILED( D3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps ) ) )
	{
		if( D3D != NULL )
			D3D->Release();

		::MessageBox(0, "GetDeviceCaps() - Failed", 0, 0);
		return;
	}

	DWORD dwBehaviorFlags = 0;


	dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	dwBehaviorFlags |= D3DCREATE_MULTITHREADED;
  D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	
  d3dpp.BackBufferFormat       = d3ddm.Format;
  d3dpp.BackBufferCount        = 1;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_FLIP;
  d3dpp.Windowed               = TRUE;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
  d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
  //d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  d3dpp.BackBufferHeight       = Tools::ScreenHeight;
	d3dpp.BackBufferWidth        = Tools::ScreenWidth;

  if( FAILED( D3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                   dwBehaviorFlags, &d3dpp, &D3DDevice ) ) ){
	  if( D3D != NULL )
			D3D->Release();

		::MessageBox(0, "CreateDevice() - Failed", 0, 0);
		return;
	}

	 if( D3D != NULL )
        D3D->Release();
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	WNDCLASSEX winClass;
	MSG uMsg;

  Tools::Windowed = 1;

  Tools::ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
  Tools::ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
  
  if(Tools::Windowed){
    Tools::ScreenWidth /= 2;
    Tools::ScreenHeight /= 2;
  }

  memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MainWindow";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = ::LoadIcon(0, IDI_APPLICATION);
  winClass.hIconSm	     = ::LoadIcon(0, IDI_APPLICATION);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

  DWORD grfStyle;
  RECT rc;
  if(Tools::Windowed){
    grfStyle = WS_OVERLAPPEDWINDOW;
    SetRect (&rc, 200, 100, Tools::ScreenWidth+200, Tools::ScreenHeight+100);
  }else{
    grfStyle =WS_EX_TOPMOST | WS_POPUP;
    SetRect (&rc, 0, 0, Tools::ScreenWidth, Tools::ScreenHeight);
  }

  DWORD grfExStyle = NULL;

  if(!AdjustWindowRectEx (&rc, grfStyle, FALSE, grfExStyle)) ALERTN("AdjustWindowRectEx");


	hwnd = CreateWindowEx( grfExStyle, "MainWindow", 
                             "Hello World!",
						 grfStyle,
                          rc.left, rc.top,    

                          rc.right - rc.left, rc.bottom - rc.top, 
                          //640, 480,
                          NULL, NULL, hInstance, NULL );

	if( hwnd == NULL )
		return E_FAIL;

  ShowWindow( hwnd, nCmdShow );
  UpdateWindow( hwnd );

	InitializeDirect3D();
  
  Tools::D3DDevice = D3DDevice;
  g = new Game();

  g->D3DDevice = D3DDevice;
    

	DInput = new DirectInput();
	DInput->InitializeDirectInput(hInstance, hwnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND,
												   DISCL_EXCLUSIVE|DISCL_FOREGROUND);

	g->Setup();
  g->DInput = DInput;
  
  Tools::DInput = DInput;

	while( !quit){
    while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
    }
    g->Tick();
	}
    
	Cleanup();

  UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );

	return (int)uMsg.wParam;
}
