#pragma once
#include "GameObject.h"

class CModel;

class CPlanet : CGameObject
{
public:
	CPlanet(void);
	~CPlanet(void);
	
	void setSize(float planetSize);

	// Overridden methods from the parent class
	bool create(LPDIRECT3DDEVICE9 device);
	void render(LPDIRECT3DDEVICE9 device);

private:
	CModel *Model;

	D3DXVECTOR3 position;
	//LPDirect3DTexture9 texture;

	float size;
	float rotationRate;
};
