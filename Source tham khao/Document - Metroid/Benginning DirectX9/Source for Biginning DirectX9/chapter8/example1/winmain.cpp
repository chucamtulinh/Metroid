#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9tex.h>

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Direct3D functions and variables
bool initDirect3D(HWND hwnd);
LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(unsigned int size, DWORD usage, DWORD fvf);
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

// vertex buffer to hold the pointsprites
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
// the texture that will be applied to each pointsprite
LPDIRECT3DTEXTURE9      g_pTexture      = NULL;

// setup the vertex structure for the pointsprites
struct CUSTOMVERTEX
{
    D3DXVECTOR3 psPosition;
    D3DCOLOR color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// inline function that converts a float to a DWORD value
inline DWORD FLOAT_TO_DWORD( FLOAT f ) { return *((DWORD*)&f); }

// Particle functions and variables
#define MAXNUM_PARTICLES 1
typedef struct
{
    D3DXVECTOR3 m_vCurPos;
    D3DXVECTOR3 m_vCurVel;
	D3DCOLOR    m_vColor;
} Particle;
Particle ParticleArray[MAXNUM_PARTICLES];

void initParticles(int numParticles);
void updateParticles(Particle *particleArray, int numParticles);
void renderParticles(const Particle *particleArray, int numParticles);


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

	
	// load in the texture
	D3DXCreateTextureFromFile( pd3dDevice, "particle.bmp", &g_pTexture );
	if (g_pTexture == NULL)
	{
		MessageBox(NULL, "Make sure the bitmap file particle.bmp resides in the current directory", "ERROR", MB_OK);
		return 0;
	}

	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// create the vertex buffer to hold the pointsprites
	g_pVertexBuffer = createVertexBuffer(2048 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, D3DFVF_CUSTOMVERTEX);

	// initialize the particle structures
	initParticles(MAXNUM_PARTICLES);

	// create and setup the camera
	createCamera(0.0f, 500.0f);
	moveCamera(D3DXVECTOR3(0.0f, 0.0f, -10.0f));
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
			updateParticles(ParticleArray, MAXNUM_PARTICLES);

			// Clear the backbuffer to a black color
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );

			pd3dDevice->BeginScene();

			// render the particles
			renderParticles(ParticleArray, MAXNUM_PARTICLES);

			pd3dDevice->EndScene();

			// Present the backbuffer contents to the display
			pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
    }

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

/**********************************************************************
* createVertexBuffer
**********************************************************************/
LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(unsigned int size, DWORD usage, DWORD fvf)
{
	LPDIRECT3DVERTEXBUFFER9 tempBuffer;

	pd3dDevice->CreateVertexBuffer(size, usage, fvf, D3DPOOL_DEFAULT, &tempBuffer, NULL);

	if (tempBuffer)
		return tempBuffer;

	return NULL;
}

/**********************************************************************
* initParticles
**********************************************************************/
void initParticles(int numParticles)
{
	// loop through the amount of particles that are to be created
	// and init their values
	for( int i = 0; i < numParticles; ++i )
    {
		// set the position of each particle at the origin
		ParticleArray[i].m_vCurPos = D3DXVECTOR3(0.0f,0.0f,0.0f);

		// generate a random value for each part of the direction/velocity vector
		float vecX = ((float)rand() / RAND_MAX);
		float vecY = ((float)rand() / RAND_MAX);
		float vecZ = ((float)rand() / RAND_MAX);
		ParticleArray[i].m_vCurVel = D3DXVECTOR3(vecX,vecY,vecZ);

		// each particle is white
		ParticleArray[i].m_vColor = D3DCOLOR_RGBA( 255, 255, 255, 255);
	}
}

/**********************************************************************
* updateParticles
**********************************************************************/
void updateParticles(Particle *particleArray, int numParticles)
{
	static double dStartAppTime = timeGetTime();
	float fElpasedAppTime = (float)((timeGetTime() - dStartAppTime) * 0.001);

	//
	// To move the particles via their velocity, keep track of how much time  
	// has elapsed since last frame update...
	//

	static double dLastFrameTime = timeGetTime();
	double dCurrenFrameTime = timeGetTime();
	double dElpasedFrameTime = (float)((dCurrenFrameTime - dLastFrameTime) * 0.001);
	dLastFrameTime = dCurrenFrameTime;

	//
	// After 5 seconds, repeat the sample by returning all the particles 
	// back to the origin.
	//

    /*if( fElpasedAppTime >= 5.0f )
	{
		for( int i = 0; i < numParticles; ++i )
			particleArray[i].m_vCurPos = D3DXVECTOR3(0.0f,0.0f,0.0f);

		dStartAppTime = timeGetTime();
	}*/
	
	//
	// Move each particle via its velocity and elapsed frame time.
	//
	
	for( int i = 0; i < numParticles; ++i )
		particleArray[i].m_vCurPos += particleArray[i].m_vCurVel * (float)dElpasedFrameTime;
	
}

/**********************************************************************
* renderParticles
**********************************************************************/
void renderParticles(const Particle *particleArray, int numParticles)
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// enable pointsprite render states

	// turn on pointsprites
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	// enable scaling
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	// the point size to use when the vertex does not include this information
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FLOAT_TO_DWORD(1.0f) );
	// the minimum size of the points
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FLOAT_TO_DWORD(1.0f) );
	// these three renderstates control the scaling of the pointsprite
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FLOAT_TO_DWORD(0.0f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FLOAT_TO_DWORD(0.0f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FLOAT_TO_DWORD(1.0f) );

	// Lock the vertex buffer, and set up our point sprites in accordance with 
	// our particles that we're keeping track of in our application.
	CUSTOMVERTEX *pPointVertices;

	g_pVertexBuffer->Lock( 0, numParticles * sizeof(CUSTOMVERTEX),
		                   (void**)&pPointVertices, D3DLOCK_DISCARD );

	for( int i = 0; i < numParticles; ++i )
    {
        pPointVertices->psPosition = particleArray[i].m_vCurPos;
        pPointVertices->color = particleArray[i].m_vColor;
        pPointVertices++;
    }

    g_pVertexBuffer->Unlock();
	
	//
	// Render point sprites...
	//

	pd3dDevice->SetTexture( 0, g_pTexture );
    pd3dDevice->SetStreamSource( 0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, numParticles );

    // Reset render states...
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
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