#include ".\dxmanager.h"

dxManager::dxManager(void)
{
	pD3D = NULL;
 	pd3dDevice = NULL;
}

dxManager::~dxManager(void)
{
}

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
	d3dpp.BackBufferHeight = 480;
	d3dpp.BackBufferWidth  = 640;
	d3dpp.hDeviceWindow    = hwnd;

    if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice ) ) )
    {
		lastResult = E_FAIL;
        return false;
    }

		// Turn off culling, so we see the front and back of the triangle
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting, since we are providing our own vertex colors
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return true;
}

LPDIRECT3DVERTEXBUFFER9 dxManager::createVertexBuffer(int size, DWORD usage)
{
	LPDIRECT3DVERTEXBUFFER9 buffer;

    // Create the vertex buffer.
    if( FAILED( pd3dDevice->CreateVertexBuffer( size,
                                                  0, usage,
                                                  D3DPOOL_DEFAULT, &buffer, NULL ) ) )
    {
        return NULL;
    }
	return buffer;
}

void dxManager::drawVB(LPDIRECT3DVERTEXBUFFER9 buffer, DWORD bufferFormat)
{
	pd3dDevice->SetStreamSource( 0, buffer, 0, sizeof(bufferFormat) );
    pd3dDevice->SetFVF( bufferFormat );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
}

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

void dxManager::beginRender()
{
	if( NULL == pd3dDevice )
        return;

    // Clear the backbuffer to a black color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
}

void dxManager::endRender(void)
{
	// Present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

IDirect3DSurface9* dxManager::getSurfaceFromBitmap(std::string filename, int width, int height)
{
	HRESULT hResult;
	IDirect3DSurface9* surface = NULL;

	hResult = pd3dDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
	if (FAILED(hResult))
		return NULL;

	hResult = D3DXLoadSurfaceFromFile(surface, NULL, NULL, filename.c_str(), NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(hResult))
		return NULL;

	return surface;
}

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

void dxManager::blitToSurface(IDirect3DSurface9* srcSurface, const RECT *srcRect, const RECT *destRect)
{
	pd3dDevice->StretchRect(srcSurface, srcRect, getBackBuffer(), destRect, D3DTEXF_NONE);
}