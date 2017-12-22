#include <windows.h>
#include "dxManager.h"

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

// this holds the DirectX Manager
dxManager *dxMgr;
// this is the surface that will hold the sprites
IDirect3DSurface9* spriteSurface;

#define MAX_SPRITES 10			// this is the number of sprites we want
#define SPRITE_WIDTH 48			// the width of all sprites
#define SPRITE_HEIGHT 48		// the height of all sprites
// this is the sprite structure
struct {
	RECT srcRect;				// holds the location of this sprite
								// in the source bitmap
	int posX;					// the sprites X position
	int posY;					// the sprites Y position
} spriteStruct[MAX_SPRITES];

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// sprite functions
bool initSprites(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// create the directx manager
	dxMgr = new dxManager();

	// init the directx manager
	if (!dxMgr->init(wndHandle))
	{
		MessageBox(NULL, "Unable to Init the DirectX Manager", "ERROR", MB_OK);
		return false;
	}

	// initialise the sprites
	if (!initSprites())
	{
		MessageBox(NULL, "Unable to Init the Sprites", "ERROR", MB_OK);
		return false;
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
			dxMgr->beginRender();
			for (int i=0; i < MAX_SPRITES; i++)
			{				
				RECT destRect;
				destRect.left = spriteStruct[i].posX;
				destRect.top = spriteStruct[i].posY;
				destRect.bottom = destRect.top + SPRITE_HEIGHT;
				destRect.right = destRect.left + SPRITE_WIDTH;
				dxMgr->blitToSurface(spriteSurface, &spriteStruct[i].srcRect, &destRect);
			}
			dxMgr->endRender();
		}
    }
	
	// shutdown the directX Manager
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


bool initSprites(void)
{
	spriteSurface = dxMgr->getSurfaceFromBitmap("sprites.bmp");
	if (spriteSurface == NULL)
		return false;

	for (int i=0; i < MAX_SPRITES; i++)
	{
		spriteStruct[i].srcRect.top = 0;
		spriteStruct[i].srcRect.left = i * SPRITE_WIDTH;
		spriteStruct[i].srcRect.right = spriteStruct[i].srcRect.left + SPRITE_WIDTH;
		spriteStruct[i].srcRect.bottom = SPRITE_HEIGHT;
		spriteStruct[i].posX = rand()%600;
		spriteStruct[i].posY = rand()%430;
	}

	return true;
}

