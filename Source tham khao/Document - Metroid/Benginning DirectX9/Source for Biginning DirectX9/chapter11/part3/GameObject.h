#pragma once
#include <d3d9.h>
#include <d3dx9tex.h>

class CModel;

class CGameObject
{
public:
	CGameObject(void);
	virtual ~CGameObject(void);

	// Abstract methods, must be overridden
	virtual bool create(LPDIRECT3DDEVICE9 device) = 0;
	virtual void render(LPDIRECT3DDEVICE9 device) = 0;

protected:
	CModel *Model;				// A pointer to a model object

	D3DXVECTOR3 position;		// The position of this object in world space
};
