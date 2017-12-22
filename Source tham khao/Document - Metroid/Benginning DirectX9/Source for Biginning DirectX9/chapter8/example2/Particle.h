#pragma once

#include <d3d9.h>
#include <d3dx9tex.h>

class Particle
{
public:
	Particle(void);
	~Particle(void);

	// the current position of this particle
	D3DXVECTOR3 m_vCurPos;
	// the direction and velocity of this particle
    D3DXVECTOR3 m_vCurVel;
	// the color of this particle
	D3DCOLOR    m_vColor;
	// is this particle alive?
	bool m_bAlive;
};
