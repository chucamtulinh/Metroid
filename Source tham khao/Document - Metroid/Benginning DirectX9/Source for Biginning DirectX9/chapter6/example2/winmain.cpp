#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>
using namespace std;

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
ID3DXMesh *teapotMesh;

LPDIRECT3D9             pD3D;                                              
LPDIRECT3DDEVICE9       pd3dDevice;

D3DXMATRIX matView;
D3DXMATRIX matProj;
D3DXVECTOR3 position;
D3DXVECTOR3 lookPos;
D3DMATERIAL9            mtrl;
////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// direct3D functions

bool initDirect3D(HWND hwnd);
void shutdown(void);
void createCamera(float nearClip, float farClip);
void moveCamera(D3DXVECTOR3 vec);
void pointCamera(D3DXVECTOR3 vec);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MessageBox(NULL, "Light and Shading Example", "MESSAGE", MB_OK);

	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// put up a messagebox with the instructions
	MessageBox(wndHandle, "Press the W key to turn on/off wireframe\nPress the S key to change the shading\nPress the Z key to turn on/off the Z-buffer\nPress the L key to turn on/off lighting", "", MB_OK);

	// init direct3d
	initDirect3D(wndHandle);

	// create a teapot using the d3dx util library
	D3DXCreateTeapot(pd3dDevice, &teapotMesh, NULL);
	
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
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

			pd3dDevice->BeginScene();

			D3DXMATRIX matShip1, matScale2, matRotate;

			createCamera(1.0f, 750.0f);		// near clip plane, far clip plane
			moveCamera(D3DXVECTOR3(0.0f, 0.0f, -5.0f));
			pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

			// set the rotation
			D3DXMatrixRotationY(&matRotate, timeGetTime()/1000.0f);
			// set the scaling
			D3DXMatrixScaling(&matScale2, 1.0f, 1.0f, 1.0f);
			// multiple the scaling and rotation matrices to create the matShip1 matrix
			D3DXMatrixMultiply(&matShip1, &matScale2, &matRotate);

			// transform the object in world space
			pd3dDevice->SetTransform(D3DTS_WORLD, &matShip1);

			pd3dDevice->SetMaterial(&mtrl);
			teapotMesh->DrawSubset(0);
					
			pd3dDevice->EndScene();

			pd3dDevice->Present( NULL, NULL, NULL, NULL );	
		}
	}

	// shutdown the directx manager
	shutdown();

	return (int) msg.wParam;
}


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

	// set the default render states
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(55, 55, 55));
	pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// Create and turn on a directional light
	D3DLIGHT9 light;
    D3DXVECTOR3 vDir( 0.3f, 0.0f, 1.0f );
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 0.5f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.5f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, -50.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vDir );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
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

	return true;
}

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
	static bool bLight = true;			// lights on or off
	static bool bWireFrame = false;		// wireframe or solid mode
	static bool bFlatShade = false;		// flat or gouraud shading
	static bool bZBuffer = true;		// z-buffer on or off

	switch (message) 
	{
		// Check the keyboard keydown event
		case WM_KEYDOWN: 
			switch (wParam)
			{
				// check for the W key to be pressed
				// change from solid to wireframe rendering
				case (0x57):
					bWireFrame = !bWireFrame;
					if (bWireFrame)
						pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
					else
						pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				break;

				// Check for the L key, turn on/off lighting
				case (0x4C):
					bLight = !bLight;
					if (bLight)
						pd3dDevice->LightEnable(0, TRUE );
					else
						pd3dDevice->LightEnable(0, FALSE );
				break;

				// check for the S key to be pressed
				// change the shading from flat to gouraud
				case (0x53):
					bFlatShade = !bFlatShade;
					if (bFlatShade)
						pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
					else
						pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
				break;

				// Check for the Z key, turn on/off the z-buffer
				case (0x5A):
					bZBuffer = !bZBuffer;
					if (bZBuffer)
						pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
					else
						pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
				break;
			}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////
// CAMERA CODE

/****************************************************************
* createCamera
****************************************************************/
void createCamera(float nearClip, float farClip)
{
	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 640/480, nearClip, farClip);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

}

/****************************************************************
* moveCamera
****************************************************************/
void moveCamera(D3DXVECTOR3 vec)
{
	position = vec;
}

/****************************************************************
* pointCamera
****************************************************************/
void pointCamera(D3DXVECTOR3 vec)
{
	lookPos = vec;

	D3DXMatrixLookAtLH(&matView, &position,		//Camera Position
                                 &lookPos,		//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}