#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;  // The untransformed, 3D position for the vertex
	float tu, tv;	// the texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

LPDIRECT3D9             pD3D;
LPDIRECT3DDEVICE9       pd3dDevice;

// camera variables
D3DXMATRIX matView;					// the view matrix
D3DXMATRIX matProj;					// the projection matrix
D3DXVECTOR3 cameraPosition;			// the position of the camera
D3DXVECTOR3 cameraLook;				// where the camera is pointing

// vertex buffer
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices

// texture info
LPDIRECT3DTEXTURE9      g_pTexture   = NULL;
LPDIRECT3DTEXTURE9      g_pTexture2   = NULL;

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// direct3D functions
HRESULT SetupVB();
bool initDirect3D(HWND hwnd);
void shutdown(void);
LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(int size, DWORD usage);
void drawVB(LPDIRECT3DVERTEXBUFFER9 buffer);
void createCamera(float nearClip, float farClip);
void moveCamera(D3DXVECTOR3 vec);
void pointCamera(D3DXVECTOR3 vec);

/*************************************************************************
* WinMain
* entry point
*************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// init direct3d
	initDirect3D(wndHandle);

	D3DXCreateTextureFromFile( pd3dDevice, "test.bmp", &g_pTexture );
	if (g_pTexture == NULL)
	{
		MessageBox(NULL, "Please make sure the bitmap test.bmp is in the current directory", "ERROR", MB_OK);
		shutdown();
		return 0;
	}

	D3DXCreateTextureFromFile( pd3dDevice, "test2.bmp", &g_pTexture2 );
	if (g_pTexture2 == NULL)
	{
		MessageBox(NULL, "Please make sure the bitmap test2.bmp is in the current directory", "ERROR", MB_OK);
		shutdown();
		return 0;
	}

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// setup the vertex buffer and add the triangle to it.
	SetupVB();

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
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

			pd3dDevice->BeginScene();

			// draw the contents of the vertex buffer
			drawVB(g_pVB);
			
			pd3dDevice->EndScene();

			pd3dDevice->Present( NULL, NULL, NULL, NULL );	
		}
    }

	// shutdown the directx manager
	shutdown();

	return (int) msg.wParam;
}

/*************************************************************************
* SetupVB
* Setup the vertex buffer
*************************************************************************/
HRESULT SetupVB()
{
	HRESULT hr;

    // Initialize three vertices for rendering a triangle
	CUSTOMVERTEX g_Vertices[] =
	{
		{-1.0f, 1.0f,-1.0f,  0.0f,0.0f },
		{ 1.0f, 1.0f,-1.0f,  1.0f,0.0f },
		{-1.0f,-1.0f,-1.0f,  0.0f,1.0f },
		{ 1.0f,-1.0f,-1.0f,  1.0f,1.0f },
	
		{-1.0f, 1.0f, 1.0f,  1.0f,0.0f },
		{-1.0f,-1.0f, 1.0f,  1.0f,1.0f },
		{ 1.0f, 1.0f, 1.0f,  0.0f,0.0f },
		{ 1.0f,-1.0f, 1.0f,  0.0f,1.0f },
	
		{-1.0f, 1.0f, 1.0f,  0.0f,0.0f },
		{ 1.0f, 1.0f, 1.0f,  1.0f,0.0f },
		{-1.0f, 1.0f,-1.0f,  0.0f,1.0f },
		{ 1.0f, 1.0f,-1.0f,  1.0f,1.0f },
	
		{-1.0f,-1.0f, 1.0f,  0.0f,0.0f },
		{-1.0f,-1.0f,-1.0f,  0.0f,1.0f },
		{ 1.0f,-1.0f, 1.0f,  1.0f,0.0f },
		{ 1.0f,-1.0f,-1.0f,  1.0f,1.0f },
	
		{ 1.0f, 1.0f,-1.0f,  0.0f,0.0f },
		{ 1.0f, 1.0f, 1.0f,  1.0f,0.0f },
		{ 1.0f,-1.0f,-1.0f,  0.0f,1.0f },
		{ 1.0f,-1.0f, 1.0f,  1.0f,1.0f },
		
		{-1.0f, 1.0f,-1.0f,  1.0f,0.0f },
		{-1.0f,-1.0f,-1.0f,  1.0f,1.0f },
		{-1.0f, 1.0f, 1.0f,  0.0f,0.0f },
		{-1.0f,-1.0f, 1.0f,  0.0f,1.0f }
	};

   
	// Create the vertex buffer
	g_pVB = createVertexBuffer(sizeof(g_Vertices) * sizeof(CUSTOMVERTEX), D3DFVF_CUSTOMVERTEX);

    // Fill the vertex buffer.
    VOID* pVertices;
	
	hr = g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 );
	if FAILED (hr)
        return E_FAIL;

	// copy the vertices into the buffer
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );

	// unlock the vertex buffer
    g_pVB->Unlock();

    return S_OK;
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

	// create the direct3D device
    hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                             D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                             &d3dpp, &pd3dDevice );
	if FAILED (hr)
    {
        return false;
    }

	return true;
}

/*************************************************************************
* createVertexBuffer
* creates the vertexbuffer
*************************************************************************/
LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(int size, DWORD usage)
{
	HRESULT hr;
	LPDIRECT3DVERTEXBUFFER9 buffer;

    // Create the vertex buffer.
    hr = pd3dDevice->CreateVertexBuffer( size,
                                         0, 
										 usage,
                                         D3DPOOL_DEFAULT, 
										 &buffer, 
										 NULL );
	if FAILED ( hr)
        return NULL;
    
	return buffer;
}

/*************************************************************************
* drawVB
* draws the vertex buffer
*************************************************************************/
void drawVB(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	// create the camera
	createCamera(0.1f, 50.0f);		// near clip plane, far clip plane
	moveCamera(D3DXVECTOR3(0.0f, 0.0f, -3.0f));
	pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	D3DXMATRIX matRot;
	D3DXMatrixRotationY(&matRot, timeGetTime()/1000.0f);
	
	D3DXMATRIX matWorld;
	D3DXMatrixMultiply(&matWorld, &matRot, &matView);	
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetTexture( 0, g_pTexture );
	pd3dDevice->SetStreamSource( 0, buffer, 0, sizeof(CUSTOMVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  0, 2 );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  4, 2 );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,  8, 2 );
	pd3dDevice->SetTexture( 0, g_pTexture2 );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 12, 2 );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 16, 2 );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 20, 2 );
}

/*************************************************************************
* shutdown
* shuts down and releases direct3D
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
* inits and creates the application window
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
* the application window procedure
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
                                 &cameraLook,			//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}
