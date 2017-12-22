#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>

#include "particleManager.h"

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Direct3D functions and variables
bool initDirect3D(HWND hwnd);
void createCamera(float nearClip, float farClip);
void moveCamera(D3DXVECTOR3 vec);
void pointCamera(D3DXVECTOR3 vec);

// camera variables
D3DXMATRIX matView;					// the view matrix
D3DXMATRIX matProj;					// the projection matrix
D3DXVECTOR3 cameraPosition;			// the position of the camera
D3DXVECTOR3 cameraLook;				// where the camera is pointing

int screen_height = 480;			// the width of the window
int screen_width = 640;				// the height of the window

// the Direct3D object and Direct3D device
LPDIRECT3D9             pD3D;
LPDIRECT3DDEVICE9       pd3dDevice;




/**********************************************************************
* WinMain - entry point
**********************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// init and create the window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// init Direct3D
	if (!initDirect3D(wndHandle))
	{
		MessageBox(NULL, "Unable to init Direct3D", "ERROR", MB_OK);
		return false;
	}

	// Create the particle manager
	particleManager *pMgr = new particleManager();

	// add an emitter
	if (!pMgr->createEmitter(pd3dDevice, 10, "particle.bmp", D3DXVECTOR3(0.0f,-3.0f,0.0f), D3DCOLOR_ARGB(0,255,0,0)))
	{
		MessageBox(wndHandle, "Emitter failed to create, particle.bmp not found", "ERROR", MB_OK);
		return 0;
	}

	// add another emitter
	if (!pMgr->createEmitter(pd3dDevice, 30, "spark.bmp", D3DXVECTOR3(-5.0f,-3.0f,0.0f), D3DCOLOR_ARGB(0,0,255,0)))
	{
		MessageBox(wndHandle, "Emitter failed to create, spark.bmp not found", "ERROR", MB_OK);
		return 0;
	}

	// create and setup the camera
	createCamera(0.0f, 500.0f);
	moveCamera(D3DXVECTOR3(0.0f, 0.0f, -15.0f));
	pointCamera(D3DXVECTOR3(0.0f, 0.0f, 1.0f));

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
			if( NULL == pd3dDevice )
		        break;

			// update the motion of the particles first
			pMgr->update();

			// Clear the backbuffer to a black color
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );

			pd3dDevice->BeginScene();

			// render the particles
			pMgr->render(pd3dDevice);

			pd3dDevice->EndScene();

			// Present the backbuffer contents to the display
			pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
    }
	
	// shutdown the particle manager
	pMgr->shutdown();

	// delete the particle manager
	if (pMgr)
		delete pMgr;

	// release Direct3D
	if( pd3dDevice != NULL) 
	{
        pd3dDevice->Release();
		pd3dDevice = NULL;
	}
    if( pD3D != NULL)
	{
        pD3D->Release();
		pD3D = NULL;
	}

	return (int) msg.wParam;
}

/*************************************************************************
* initWindow
* initalize the window

*************************************************************************/
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

/**********************************************************************
* WndProc - the window procedure
**********************************************************************/
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

/**********************************************************************
* initDirect3D
**********************************************************************/
bool initDirect3D(HWND hwnd)
{
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount  = 1;
	d3dpp.BackBufferHeight = screen_height;
	d3dpp.BackBufferWidth  = screen_width;
	d3dpp.hDeviceWindow    = hwnd;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.EnableAutoDepthStencil = TRUE;

    if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice ) ) )
    {
        return false;
    }

	// turn off lighting and enable the z-buffer
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	
	return true;
}

/*************************************************************************
* createCamera
* creates a virtual camera
*************************************************************************/
void createCamera(float nearClip, float farClip)
{
	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 640/480, nearClip, farClip);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

/*************************************************************************
* moveCamera
* moves the camera to a position specified by the vector passed as a 
* parameter
*************************************************************************/
void moveCamera(D3DXVECTOR3 vec)
{
	cameraPosition = vec;
}

/*************************************************************************
* pointCamera
* points the camera a location specified by the passed vector
*************************************************************************/
void pointCamera(D3DXVECTOR3 vec)
{
	cameraLook = vec;

	D3DXMatrixLookAtLH(&matView, &cameraPosition,		//Camera Position
                                 &cameraLook,			//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}