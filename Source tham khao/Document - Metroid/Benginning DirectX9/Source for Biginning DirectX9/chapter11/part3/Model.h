#pragma once
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

class CModel
{
public:
	CModel(void);
	virtual ~CModel(void);

	bool loadModel(LPDIRECT3DDEVICE9 device, std::string filename);

	void render(LPDIRECT3DDEVICE9 pDevice);

private:
	LPD3DXMESH mesh;
	DWORD      numMaterials;

	LPD3DXBUFFER matBuffer;
	LPDIRECT3DTEXTURE9* m_pMeshTextures;
	D3DMATERIAL9* m_pMeshMaterials;
};
