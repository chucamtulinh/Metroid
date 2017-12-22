#include <windows.h>
#include "dxManager.h"
#include "diManager.h"
#include "Game.h"

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/* Get a reference to the DirectX Manager */	
static dxManager& dxMgr = dxManager::getInstance();

/* Get a reference to the DirectInput Manager */	
static diManager& diMgr = diManager::getInstance();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MessageBox(NULL, "Control the spaceship with the mouse\nLeft click to go forward\nRight click to go in reverse", "MESSAGE", MB_OK);

	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// initialize DirectX
	if (!dxMgr.init(wndHandle))
	{
		MessageBox(NULL, "Unable to init DirectX", "ERROR", MB_OK);
		return false;
	}

	if (!diMgr.initDirectInput(hInst, wndHandle))
	{
		MessageBox(NULL, "Unable to init DirectInput", "ERROR", MB_OK);
		return false;
	}

	// Create the game object
	Game *pGame = new Game();

	// Initialize the game object
	if (!pGame->init(wndHandle))
		return 0;

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
			pGame->update();
			pGame->render();
		}
    }

	// Shutdown the game
	pGame->shutdown();

	// Delete the game object
	if (pGame)
		delete pGame;

	// shutdown the directx manager
	dxMgr.shutdown();

	return (int) msg.wParam;
}

/********************************************************************
* initWindow
* Creates the main application window
********************************************************************/
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
 
   // Set the global instance handle
   hInst = hInstance;

   ShowWindow(wndHandle, SW_SHOW);
   UpdateWindow(wndHandle);

   return true;
}

/********************************************************************
* WndProc
* The application's window procedure
********************************************************************/
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