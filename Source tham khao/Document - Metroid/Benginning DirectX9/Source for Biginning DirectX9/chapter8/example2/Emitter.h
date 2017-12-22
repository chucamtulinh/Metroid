#pragma once
#include <string>
#include <vector>

#include <d3d9.h>
#include <d3dx9tex.h>

#include "Particle.h"
class Particle;

class Emitter
{
	// setup the vertex structure for the pointsprites
	struct CUSTOMVERTEX
	{
		D3DXVECTOR3 psPosition;
		D3DCOLOR color;
	};

	#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

public:
	Emitter(void);
	Emitter(LPDIRECT3DDEVICE9 pDevice);
	~Emitter(void);

	// add a texture to this emitter
	bool addTexture(std::string textureName);

	// set the number of particles and size the vector
	void setNumParticles(int nParticles);

	// init the particles and set the position of the emitter
	void initParticles(D3DXVECTOR3 position, D3DCOLOR color);

	// update all the particles in this emitter
	void update(void);
	// render the particles in this emitter
	void render();

	// inline functions
	inline bool getAlive(void) { return m_bAlive; }
	// inline function that converts a float to a DWORD value
	inline DWORD FLOAT_TO_DWORD( FLOAT f ) { return *((DWORD*)&f); }

private:
	LPDIRECT3DDEVICE9 emitterDevice;

	// the current position of this particle
	D3DXVECTOR3 m_vCurPos;
	// the direction and velocity of this particle
    D3DXVECTOR3 m_vCurVel;

	// vertex buffer to hold the pointsprites
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	// the texture that will be applied to each pointsprite
	LPDIRECT3DTEXTURE9      pTexture;

	Particle *m_particles;

	int numParticles;

	bool m_bAlive;

	// private functions
	LPDIRECT3DVERTEXBUFFER9 createVertexBuffer(unsigned int size, DWORD usage, DWORD fvf);
};
