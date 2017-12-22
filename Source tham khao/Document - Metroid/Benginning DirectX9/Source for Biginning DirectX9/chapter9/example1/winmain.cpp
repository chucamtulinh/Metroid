#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include "dxManager.h"
#include <dinput.h>

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

LPDIRECTINPUT8        g_lpDI;	// the direct input object
LPDIRECTINPUTDEVICE8  g_lpDIDevice; // the direct input device

char     buffer[256]; 

#define KEYDOWN(name, key) (name[key] & 0x80) 

enum { LEFT_ARROW = 0,
		UP_ARROW,
		DOWN_ARROW,
		RIGHT_ARROW };

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr; 
	RECT src;

	src.left = src.right = src.top = src.bottom = 0;
	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	dxManager *dxMgr = new dxManager();
	dxMgr->init(wndHandle);

	IDirect3DSurface9* arrows = dxMgr->getSurfaceFromBitmap("./arrows.bmp",192, 48);
	if (arrows == NULL)
	{
		MessageBox(wndHandle, "Make sure arrows.bmp is in the current directory", "ERROR", MB_OK);
		return 0;
	}

	// Create the DirectInput object. 
    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
                            IID_IDirectInput8, (void**)&g_lpDI, NULL); 

	if FAILED(hr) return FALSE; 

    // Retrieve a pointer to an IDirectInputDevice8 interface 
    hr = g_lpDI->CreateDevice(GUID_SysKeyboard, &g_lpDIDevice, NULL); 

	hr = g_lpDIDevice->SetDataFormat(&c_dfDIKeyboard); 

	if FAILED(hr) { 
		return FALSE; 
	} 

	// Set the cooperative level 
    hr = g_lpDIDevice->SetCooperativeLevel(wndHandle, 
                             DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
    if FAILED(hr) 
    { 
        return FALSE; 
    } 

    // Get access to the input device. 
    hr = g_lpDIDevice->Acquire(); 
    if FAILED(hr) 
    { 
        return FALSE; 
    } 

	// Main message loop:
	// Enter the message loop
    MSG msg; 
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
    {
		// check for messages
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		// this is called when no messages are pending
		else
		{
			hr = g_lpDIDevice->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 

			src.top = 0;
			src.bottom = 48;
				
			if (KEYDOWN(buffer, DIK_LEFT))
				src.left = (48 * LEFT_ARROW);
			else if (KEYDOWN(buffer, DIK_UP))
				src.left = (48 * UP_ARROW);
			else if (KEYDOWN(buffer, DIK_DOWN))
				src.left = (48 * DOWN_ARROW);
			else if (KEYDOWN(buffer, DIK_RIGHT))
				src.left = (48 * RIGHT_ARROW);
			
			src.right = src.left + 48;

			// call our render function
			dxMgr->beginRender();

			// blit this letter to the back buffer
			dxMgr->blitToSurface(arrows, &src, NULL);

			dxMgr->endRender();
		}
    }

	if (g_lpDI) 
    { 
        if (g_lpDIDevice) 
        { 
        // Always unacquire device before calling Release(). 
            g_lpDIDevice->Unacquire(); 
            g_lpDIDevice->Release();
            g_lpDIDevice = NULL; 
        } 
        g_lpDI->Release();
        g_lpDI = NULL; 
    } 

	// shutdown the directx manager
	dxMgr->shutdown();

	return (int) msg.wParam;
}

bool initWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "DirectXExample";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

	wndHandle = CreateWindow("DirectXExample", 
							 "DirectXExample", 
							 WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, 
							 CW_USEDEFAULT, 
							 640, 
							 480, 
							 NULL, 
							 NULL, 
							 hInstance, 
							 NULL);
   if (!wndHandle)
      return false;
   
   ShowWindow(wndHandle, SW_SHOW);
   UpdateWindow(wndHandle);

   return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}