#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9mesh.h>

HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;  // The untransformed, 3D position for the vertex
	DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// Direct3D variables
LPDIRECT3D9             pD3D;
LPDIRECT3DDEVICE9       pd3dDevice;

// camera variables
D3DXMATRIX matView;					// the view matrix
D3DXMATRIX matProj;					// the projection matrix
D3DXVECTOR3 cameraPosition;			// the position of the camera
D3DXVECTOR3 cameraLook;				// where the camera is pointing

LPD3DXMESH boxMesh;					// holds the newly created mesh
D3DXMATERIAL materials[2];			// holds the materials for the mesh

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// direct3D functions
bool initDirect3D(HWND hwnd);
void shutdown(void);

// mesh and buffer functions
HRESULT SetupVBIB();
void SetupMaterial(void);
void drawMesh(LPD3DXMESH mesh);

// camera functions
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

	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
	D3DXDeclaratorFromFVF(D3DFVF_CUSTOMVERTEX, Declaration);

	HRESULT hr=D3DXCreateMesh(12, 8, D3DXMESH_MANAGED, Declaration, pd3dDevice, &boxMesh);

	// Setup the material
	SetupMaterial();

	// setup the vertex and index buffer
	SetupVBIB();

	// Set the output filename to the current directory\\cube.X
	char buffer[1024];
	GetCurrentDirectory(sizeof(buffer), buffer);
	strcat(buffer, "\\cube.X");
	D3DXSaveMeshToX(buffer, boxMesh, NULL, materials, NULL, 2, D3DXF_FILEFORMAT_TEXT);
	
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
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

			pd3dDevice->BeginScene();

			// create the camera
			createCamera(0.1f, 50.0f);		// near clip plane, far clip plane
			moveCamera(D3DXVECTOR3(0.0f, 0.0f, -3.0f));
			pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

			// draws the mesh
			drawMesh(boxMesh);
			
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
* Setup the vertex buffer and index buffer
*************************************************************************/
HRESULT SetupVBIB()
{
	HRESULT hr;

	// vertices for the vertex buffer
	CUSTOMVERTEX g_Vertices[ ] = {	
		// X     Y     Z    ARGB COLOR
		{-1.0f,-1.0f,-1.0f, D3DCOLOR_ARGB(0,0,0,255)},	// 0
		{-1.0f, 1.0f,-1.0f, D3DCOLOR_ARGB(0,0,0,255)},	// 1
		{1.0f, 1.0f,-1.0f,  D3DCOLOR_ARGB(0,0,0,255)},	// 2
		{ 1.0f,-1.0f,-1.0f, D3DCOLOR_ARGB(0,0,0,255)},	// 3
		{-1.0f,-1.0f, 1.0f, D3DCOLOR_ARGB(0,0,0,255)},	// 4
		{1.0f,-1.0f, 1.0f,  D3DCOLOR_ARGB(0,0,0,255)},	// 5
		{ 1.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(0,0,0,255)},	// 6
		{-1.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(0,0,0,255)}	// 7
	};

	// index buffer data
	WORD IndexData[ ] = {
		0,1,2,
		2,3,0,
		4,5,6,
		6,7,4,
		0,3,5,
		5,4,0,
		3,2,6,
		6,5,3,
		2,1,7,
		7,6,2,
		1,0,4,
		4,7,1
	};
	
	// prepare to copy the vertices into the vertex buffer
	VOID* pVertices;
	// lock the vertex buffer
	hr = boxMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void**)&pVertices);

	// check to make sure the vertex buffer can be locked
	if FAILED (hr)
		return hr;

	// copy the vertices into the buffer
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );

	// unlock the vertex buffer
	boxMesh->UnlockVertexBuffer();

	// prepare to copy the indexes into the index buffer
	VOID* IndexPtr;
	// Lock the index buffer
	hr = boxMesh->LockIndexBuffer( 0, &IndexPtr );

	// check to make sure the index buffer can be locked
	if FAILED (hr)
		return hr;

	// Copy the indexes into the buffer
	memcpy( IndexPtr, IndexData, sizeof(IndexData)*sizeof(WORD) );

	// unlock the buffer
	boxMesh->UnlockIndexBuffer();

	boxMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, 0, NULL, NULL, NULL);
	
	DWORD numAttr;
	D3DXATTRIBUTERANGE *attribTable = new D3DXATTRIBUTERANGE[2];
	boxMesh->GetAttributeTable(NULL, &numAttr);
	boxMesh->GetAttributeTable(attribTable, &numAttr); 

	attribTable[0].AttribId = 0;
	attribTable[0].FaceStart = 0;
	attribTable[0].FaceCount = 6;
	attribTable[0].VertexStart = 0;
	attribTable[0].VertexCount = 8;

	attribTable[1].AttribId = 1;
	attribTable[1].FaceStart = 6;
	attribTable[1].FaceCount = 6;
	attribTable[1].VertexStart = 0;
	attribTable[1].VertexCount = 8;

	boxMesh->SetAttributeTable(attribTable, 2);
	
    return S_OK;
}

/*************************************************************************
* SetupMaterial
* Setup the material
*************************************************************************/
void SetupMaterial(void)
{
	// first material, just set the ambient property as this scene
	// only contains ambient lighting
    materials[0].MatD3D.Ambient.a = 0.0f;
    materials[0].MatD3D.Ambient.r = 1.0f;		// all red
    materials[0].MatD3D.Ambient.g = 0.0f;
    materials[0].MatD3D.Ambient.b = 0.0f;
    materials[0].MatD3D.Power = 38.4f;
	materials[0].pTextureFilename = NULL;

	// second material, just set the ambient property as this scene
	// only contains ambient lighting    
	materials[1].MatD3D.Ambient.a = 0.0f;
    materials[1].MatD3D.Ambient.r = 0.0f;
    materials[1].MatD3D.Ambient.g = 1.0f;		// all green
    materials[1].MatD3D.Ambient.b = 0.0f;
    materials[1].MatD3D.Power = 38.4f;
	materials[1].pTextureFilename = NULL;
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
        return false;

	// Set the default render states
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	return true;
}

/*************************************************************************
* void drawMesh(LPD3DXMESH mesh)
* draws the mesh
*************************************************************************/
void drawMesh(LPD3DXMESH mesh)
{
	// rotate the mesh
	D3DXMATRIX matRot;
	D3DXMatrixRotationY(&matRot, timeGetTime()/1000.0f);
	
	D3DXMATRIX matWorld;
	D3DXMatrixMultiply(&matWorld, &matRot, &matView);	
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// set the material
	pd3dDevice->SetMaterial(&materials[0].MatD3D);
	// draw the first subset of the mesh
	mesh->DrawSubset(0);

	// set the second material
	pd3dDevice->SetMaterial(&materials[1].MatD3D);
	// draw the second subset of the mesh
	mesh->DrawSubset(1);
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
