#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>

HINSTANCE hInst;					// holds the instance for this app
HWND wndHandle;						// global window handle

LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
ID3DXMesh *teapotMesh;				// the teapot mesh
D3DMATERIAL9            mtrl;		// material for the teapot mesh

LPDIRECT3D9             pD3D;		// the direct3D objec
LPDIRECT3DDEVICE9       pd3dDevice;	// the direct3d device

// camera variables
D3DXMATRIX matView;					// the view matrix
D3DXMATRIX matProj;					// the projection matrix
D3DXVECTOR3 cameraPosition;			// the position of the camera
D3DXVECTOR3 cameraLook;				// where the camera is pointing

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// direct3D functions
HRESULT SetupVB();
bool initDirect3D(HWND hwnd);
void shutdown(void);
void createLighting(void);
void createCamera(float nearClip, float farClip);
void moveCamera(D3DXVECTOR3 vec);
void pointCamera(D3DXVECTOR3 vec);

/*************************************************************************
* WinMain
* entry point
*************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MessageBox(NULL, "Point Light Example", "MESSAGE", MB_OK);

	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// init direct3d
	if (!initDirect3D(wndHandle))
	{
		MessageBox(NULL, "Unable to init Direct3D", "ERROR", MB_OK);
		shutdown();
		return false;
	}

	// create the lighting for this scene
	createLighting();

	// Create and add the teapot to the scene
	D3DXCreateTeapot(pd3dDevice, &teapotMesh, NULL);
	D3DXComputeNormals(teapotMesh, NULL);

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
			// call our render function
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

			pd3dDevice->BeginScene();

			D3DXMATRIX meshMat, meshScale, meshRotate;

			// create the camera
			createCamera(1.0f, 750.0f);		// near clip plane, far clip plane
			moveCamera(D3DXVECTOR3(0.0f, 0.0f, -5.0f));
			pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

			// set the rotation
			D3DXMatrixRotationY(&meshRotate, timeGetTime()/1000.0f);
			// set the scaling
			D3DXMatrixScaling(&meshScale, 1.0f, 1.0f, 1.0f);
			// multiple the scaling and rotation matrices to create the meshMat matrix
			D3DXMatrixMultiply(&meshMat, &meshScale, &meshRotate);

			// transform the object in world space
			pd3dDevice->SetTransform(D3DTS_WORLD, &meshMat);

			// set the material to use
			pd3dDevice->SetMaterial(&mtrl);

			// draw the teapot mesh
			teapotMesh->DrawSubset(0);
					
			pd3dDevice->EndScene();

			pd3dDevice->Present( NULL, NULL, NULL, NULL );	
		}
	}

	// shutdown the directx manager
	shutdown();

	return (int) msg.wParam;
}

/*************************************************************************
* initDirect3D
* initialize direct3D
*************************************************************************/
bool initDirect3D(HWND hwnd)
{
	HRESULT hr;

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
	d3dpp.BackBufferHeight = 480;
	d3dpp.BackBufferWidth  = 640;
	d3dpp.hDeviceWindow    = hwnd;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                             D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                             &d3dpp, &pd3dDevice );
	if FAILED (hr)
    {
        return false;
    }

	// Set the default render states
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	return true;
}

/*************************************************************************
* createLighting
* sets the lights for this scene
*************************************************************************/
void createLighting(void)
{
	// set the ambient lighting to red
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(20, 20, 20));
	pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);	

	// Create and turn on a directional light
	D3DLIGHT9 light;
    light.Type         = D3DLIGHT_POINT;
    light.Diffuse.r    = 1.0f;
	light.Diffuse.g  = light.Diffuse.b  = 0.5f;
    light.Ambient.r    = 0.5f;
	light.Ambient.g  = light.Ambient.b  = 0.0f;
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
	light.Position     = D3DXVECTOR3( -2500.0f, 0.0f, 0.0f );
    light.Range        = sqrtf(FLT_MAX);
	pd3dDevice->SetLight(0, &light );
    pd3dDevice->LightEnable(0, TRUE );

	// Create and define the material to use for the teapot
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.5f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.5f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.9f;
	mtrl.Specular.r = 1.0f;
	mtrl.Specular.g = 0.3f;
	mtrl.Specular.b = 1.0f;
	mtrl.Power = 8.0f;	

}

/*************************************************************************
* shutdown
* releases the direct3d interfaces
*************************************************************************/
void shutdown(void)
{
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
}

/*************************************************************************
* initWindow
* creates and shows a 640x480 window
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

/*************************************************************************
* WndProc
* the windows procedure for this application
*************************************************************************/
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
                                 &cameraLook,		//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}