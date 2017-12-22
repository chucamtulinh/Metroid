#include ".\dxmanager.h"

dxManager::dxManager(void)
{
	pD3D = NULL;
 	pd3dDevice = NULL;

	screen_width = 640;
	screen_height = 480;
}

dxManager::~dxManager(void)
{
}

/********************************************************************
* init
* Initializes DirectX
********************************************************************/
bool dxManager::init(HWND hwnd)
{
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		lastResult = E_FAIL;
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
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice ) ) )
    {
		lastResult = E_FAIL;
        return false;
    }

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));

	D3DLIGHT9 light;
    D3DXVECTOR3 vDir( 0.3f, -1.0f, 1.0f );
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vDir );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
    light.Range        = sqrtf(FLT_MAX);
    pd3dDevice->SetLight(0, &light );
    pd3dDevice->LightEnable(0, TRUE );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
	return true;
}

/********************************************************************
* shutdown
* Releases the DirectX interfaces
********************************************************************/
void dxManager::shutdown(void)
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

/********************************************************************
* beginRender
* Prepares DirectX for rendering each frame
********************************************************************/
void dxManager::beginRender()
{
	if( NULL == pd3dDevice )
        return;

    // Clear the backbuffer to a black color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(55,55,55), 1.0f, 0 );

	pd3dDevice->BeginScene();
}

/********************************************************************
* endRender
* Called after rendering has completed. Presents the result to the
* screen.
********************************************************************/
void dxManager::endRender(void)
{	
	pd3dDevice->EndScene();
	// Present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

/********************************************************************
* getSurfaceFromBitmap
* Utility function to load in a bitmap
********************************************************************/
IDirect3DSurface9* dxManager::getSurfaceFromBitmap(std::string filename)
{
	HRESULT hResult;
	IDirect3DSurface9* surface = NULL;
	D3DXIMAGE_INFO imageInfo;

	// Get the width and height info from this bitmap
	hResult = D3DXGetImageInfoFromFile(filename.c_str(), &imageInfo);
	if FAILED (hResult)
		return NULL;

	hResult = pd3dDevice->CreateOffscreenPlainSurface(imageInfo.Width, imageInfo.Height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
	if (FAILED(hResult))
		return NULL;

	hResult = D3DXLoadSurfaceFromFile(surface, NULL, NULL, filename.c_str(), NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(hResult))
		return NULL;

	return surface;
}

/********************************************************************
* getBackBuffer
* Utility function used to return a pointer to the current back
* buffer
********************************************************************/
IDirect3DSurface9* dxManager::getBackBuffer(void)
{
	IDirect3DSurface9* backbuffer = NULL;

	if (!pd3dDevice)
		return NULL;

	HRESULT hResult = pd3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	if (FAILED(hResult))
		return NULL;
	else
		return backbuffer;
}

/********************************************************************
* blitToSurface
* Utility function used to copy an off screen surface to the back
* buffer
********************************************************************/
void dxManager::blitToSurface(IDirect3DSurface9* srcSurface, const RECT *srcRect, const RECT *destRect)
{
	pd3dDevice->StretchRect(srcSurface, srcRect, getBackBuffer(), destRect, D3DTEXF_NONE);
}

/********************************************************************
* loadMesh
* Utility function used to load a mesh
********************************************************************/
bool dxManager::loadMesh(std::string filename)
{
	HRESULT hr;

	LPD3DXBUFFER ppAdjacencyBuffer;
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;
	
	pMeshSysMem = NULL;

	    // Load the mesh from the specified file
    hr = D3DXLoadMeshFromX( filename.c_str(), D3DXMESH_SYSTEMMEM, pd3dDevice, 
                            &ppAdjacencyBuffer, &pD3DXMtrlBuffer, NULL,
                            &m_dwNumMaterials, &pMeshSysMem );
    if( FAILED(hr) )
		return false;
	
	D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    
    //Create two arrays. One to hold the materials and only to hold the textures
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    
    for(DWORD i = 0; i < m_dwNumMaterials; i++)
    {
        //Copy the material
        m_pMeshMaterials[i] = matMaterials[i].MatD3D;

        //Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;
    }

	return true;
}

/********************************************************************
* createVertexBuffer
* Utility function used to create a vertex buffer of a particular
* size and format.
********************************************************************/
LPDIRECT3DVERTEXBUFFER9 dxManager::createVertexBuffer(int size, DWORD usage)
{
	LPDIRECT3DVERTEXBUFFER9 buffer;

    // Create the vertex buffer.
    if( FAILED(pd3dDevice->CreateVertexBuffer(size,0, usage,D3DPOOL_DEFAULT, &buffer, NULL )))
		return NULL;
   
	return buffer;
}

/********************************************************************
* createCamera
* Utility function used to define a camera
********************************************************************/
void dxManager::createCamera(float nearClip, float farClip)
{
	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, screen_width/screen_height, nearClip, farClip);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void dxManager::moveCamera(D3DXVECTOR3 vec)
{
	position = vec;
}

/********************************************************************
* pointCamera
* Utility function used to point a camera at a particular point
* in space
********************************************************************/
void dxManager::pointCamera(D3DXVECTOR3 vec)
{
	lookPos = vec;

	D3DXMatrixLookAtLH(&matView, &position,		//Camera Position
                                 &lookPos,		//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction

	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}