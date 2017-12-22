#include <d3d9.h>
#include <d3dx9tex.h>

#include ".\particle.h"

Particle::Particle(void)
{
	m_vCurPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// set the default color
	m_vColor = D3DCOLOR_ARGB(0, 0, 0, 0);
	// this particle is created alive
	m_bAlive = true;
}

Particle::~Particle(void)
{
}
