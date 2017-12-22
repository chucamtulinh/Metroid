#pragma once
#include "GameObject.h"

class CModel;

class CShip : CGameObject
{
public:
	CShip(void);
	~CShip(void);

	// enumeration controlling the direction of the ship
	enum { FORWARD=0, BACKWARD=1, LEFT=2, RIGHT=3 };

	// Overridden methods from the parent class
	bool create(LPDIRECT3DDEVICE9 device);
	void render(LPDIRECT3DDEVICE9 device);

	void setPosition(D3DXVECTOR3 newPosition);
	void move(int direction, float anim_rate);

private:
	CModel *Model;

	// The current position of the ship
	D3DXVECTOR3 position;

	// The ship's forward vector, what direction it is facing
	D3DXVECTOR3 forwardVector;

	// The movement rate of the ship
	float  moveRate;
};
