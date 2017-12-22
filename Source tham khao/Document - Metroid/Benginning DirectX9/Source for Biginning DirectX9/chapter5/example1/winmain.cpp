#include <windows.h>
#include <d3d9.h>
#include <d3dx9tex.h>

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

LPDIRECT3D9             pD3D;
LPDIRECT3DDEVICE9       pd3dDevice;
LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

// camera variables
D3DXMATRIX matView;					// the view matrix
D3DXMATRIX matProj;					// the projection matrix
D3DXVECTOR3 cameraPosition;			// the position of the camera
D3DXVECTOR3 cameraLook;				// where the camera is pointing

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;  // The untransformed, 3D position for the vertex
	DWORD color;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool initDirect3D(HWND hwnd);
void shutdownDirect3D(void);
bool createCube(void);

void createCamera(float nearClip, float farClip);
void moveCamera(D3DXVECTOR3 vec);
void pointCamera(D3DXVECTOR3 vec);

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	if (!initDirect3D(wndHandle))
	{
		MessageBox(NULL, "Unable to init Direct3D", "ERROR", MB_OK);
		return false;
	}

	if (!createCube())
	{
		MessageBox(NULL, "Cube could not be created", "ERROR", MB_OK);
		return false;
	}

	createCamera(1.0f, 600.0f);		// near clip plane, far clip plane
	moveCamera(D3DXVECTOR3(0.0f, 0.0f, -450.0f));
	pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	D3DXMATRIX meshMat, meshScale, meshRotate;
	// set the rotation
	D3DXMatrixRotationY(&meshRotate, D3DXToRadian(45));
	// set the scaling
	D3DXMatrixScaling(&meshScale, 1.0f, 1.0f, 1.0f);
	// multiple the scaling and rotation matrices to create the meshMat matrix
	D3DXMatrixMultiply(&meshMat, &meshScale, &meshRotate);

	// transform the object in world space
	pd3dDevice->SetTransform(D3DTS_WORLD, &meshMat);

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
			// Clear the backbuffer to a black color
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

			pd3dDevice->BeginScene();

			pd3dDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof(CUSTOMVERTEX) );
		    pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  0, 2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  4, 2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  8, 2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 12, 2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 16, 2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 20, 2 );

			pd3dDevice->EndScene();

			// Present the backbuffer contents to the display
			pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
    }

	// release and shutdown Direct3D
	shutdownDirect3D();

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
							 SCREEN_WIDTH, 
							 SCREEN_HEIGHT, 
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

bool createCube(void)
{
	// Initialize three vertices for rendering a triangle	
	CUSTOMVERTEX g_Vertices[] =
	{
		// 1
		{ -64.0f,  64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f,  64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ -64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},

		// 2
		{ -64.0f,  64.0f, 64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ -64.0f, -64.0f, 64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f,  64.0f, 64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, -64.0f, 64.0f, D3DCOLOR_ARGB(0,0,0,255)},

		// 3
		{ -64.0f, 64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, 64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ -64.0f, 64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, 64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},

		// 4
		{ -64.0f, -64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ -64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, -64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{  64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},

		// 5
		{ 64.0f,  64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ 64.0f,  64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ 64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{ 64.0f, -64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},

		// 6
		{-64.0f,  64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{-64.0f, -64.0f, -64.0f, D3DCOLOR_ARGB(0,0,0,255)},
		{-64.0f,  64.0f,  64.0f, D3DCOLOR_ARGB(0,255,0,255)},
		{-64.0f, -64.0f,  64.0f, D3DCOLOR_ARGB(0,0,0,255)},
	};

	// Create the vertex buffer.
    HRESULT hr;
	
	hr = pd3dDevice->CreateVertexBuffer(sizeof(g_Vertices) * sizeof(CUSTOMVERTEX),
		  							    0, 
										D3DFVF_CUSTOMVERTEX,
										D3DPOOL_DEFAULT, 
										&vertexBuffer, 
										NULL );
	if FAILED (hr)
		return false;

	// prepare to copy the vertices into the vertex buffer
	VOID* pVertices;
	// lock the vertex buffer
	hr = vertexBuffer->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0);

	// check to make sure the vertex buffer can be locked
	if FAILED (hr)
		return false;

	// copy the vertices into the buffer
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );

	// unlock the vertex buffer
	vertexBuffer->Unlock();

	return true;
}
bool initDirect3D(HWND hwnd)
{
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;
	
	D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount  = 1;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferWidth  = SCREEN_WIDTH;
	d3dpp.hDeviceWindow    = hwnd;
	
    if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice ) ) )
    return false;
    
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	return true;
}

void shutdownDirect3D(void)
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