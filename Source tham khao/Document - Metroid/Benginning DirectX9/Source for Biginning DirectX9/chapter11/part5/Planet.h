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
	// The size of the planet
	float size;

	// The rate at which the planet rotates
	float rotationRate;
};
