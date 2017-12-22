#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>
using namespace std;

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3D9             pD3D;
LPDIRECT3DDEVICE9       pd3dDevice;

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// direct3D functions
HRESULT SetupVB();
bool initDirect3D(HWND hwnd);
void shutdown(void);
LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(int size, DWORD usage);
void drawVB(LPDIRECT3DVERTEXBUFFER9 buffer);

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
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

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

HRESULT SetupVB()
{
	HRESULT hr;

    // Initialize three vertices for rendering a triangle
    CUSTOMVERTEX g_Vertices[] =
    {
        { 320.0f,  50.0f, 0.5f, 1.0f, D3DCOLOR_ARGB(0,255,0,0), }, // x, y, z, rhw, color
        { 250.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_ARGB(0,0,255,0), },
        {  50.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_ARGB(0,0,0,255), },
    };

	// Create the vertex buffer
	g_pVB = createVertexBuffer(3*sizeof(CUSTOMVERTEX), D3DFVF_CUSTOMVERTEX);

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

    hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                             D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                             &d3dpp, &pd3dDevice );
	if FAILED (hr)
    {
        return false;
    }

	return true;
}

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

void drawVB(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	pd3dDevice->SetStreamSource( 0, buffer, 0, sizeof(CUSTOMVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
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
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}