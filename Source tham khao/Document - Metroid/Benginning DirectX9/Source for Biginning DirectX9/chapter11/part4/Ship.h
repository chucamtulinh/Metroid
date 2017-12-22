#pragma once
#include "GameObject.h"

class CModel;

class CShip : CGameObject
{
public:
	CShip(void);
	~CShip(void);
	bool create(LPDIRECT3DDEVICE9 device);
	void render(LPDIRECT3DDEVICE9 device);

	void setPosition(D3DXVECTOR3 newPosition);
	void move(int direction);

private:
	CModel *Model;

	D3DXVECTOR3 position;

	D3DXVECTOR3 forwardVector;

	float  moveRate;
};
