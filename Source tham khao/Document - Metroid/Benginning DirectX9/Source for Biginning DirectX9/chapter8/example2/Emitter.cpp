#include ".\emitter.h"

Emitter::Emitter(void)
{
	emitterDevice = NULL;
}

Emitter::Emitter(LPDIRECT3DDEVICE9 pDevice)
{
	emitterDevice = pDevice;

	// set the default position for this emitter
	m_vCurPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// set the default velocity and direction
     m_vCurVel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

Emitter::~Emitter(void)
{
	// remove the array of particles
	if (m_particles)
		delete m_particles;

	// release the vertex buffer that was used
	if (pVertexBuffer)
		pVertexBuffer->Release();
}

bool Emitter::addTexture(std::string textureName)
{
	HRESULT result = D3DXCreateTextureFromFile( emitterDevice, textureName.c_str(), &pTexture );
	if ((pTexture == NULL) || (result != D3D_OK))
	{
		return false;
	}

	emitterDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	emitterDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return true;
}

void Emitter::setNumParticles(int nParticles)
{ 
	numParticles = nParticles; 

	m_particles = new Particle[numParticles];
}

void Emitter::initParticles(D3DXVECTOR3 position, D3DCOLOR color)
{
	pVertexBuffer = createVertexBuffer(numParticles * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, D3DFVF_CUSTOMVERTEX);

	for (int i=0; i<numParticles; i++)
	{
		m_particles[i].m_bAlive = true;
		m_particles[i].m_vColor = color;
		m_particles[i].m_vCurPos = position;

		// generate a random value for each part of the direction/velocity vector
		float vecX = ((float)rand() / RAND_MAX);
		float vecY = ((float)rand() / RAND_MAX);
		float vecZ = ((float)rand() / RAND_MAX);
		m_particles[i].m_vCurVel = D3DXVECTOR3(vecX,vecY,vecZ);

	}
}

/**********************************************************************
* createVertexBuffer
**********************************************************************/
LPDIRECT3DVERTEXBUFFER9 Emitter::createVertexBuffer(unsigned int size, DWORD usage, DWORD fvf)
{
	LPDIRECT3DVERTEXBUFFER9 tempBuffer;

	emitterDevice->CreateVertexBuffer(size, usage, fvf, D3DPOOL_DEFAULT, &tempBuffer, NULL);

	if (tempBuffer)
		return tempBuffer;

	return NULL;
}

void Emitter::update(void)
{
	// loop through and update the position of the particles
	for (int i=0; i<numParticles; i++)
	{
		m_particles[i].m_vCurPos += m_particles[i].m_vCurVel;
	}
}

void Emitter::render()
{
	emitterDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    emitterDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    emitterDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// enable pointsprite render states

	// turn on pointsprites
    emitterDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	// enable scaling
    emitterDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	// the point size to use when the vertex does not include this information
    emitterDevice->SetRenderState( D3DRS_POINTSIZE,     FLOAT_TO_DWORD(1.0f) );
	// the minimum size of the points
    emitterDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FLOAT_TO_DWORD(1.0f) );
	// these three renderstates control the scaling of the pointsprite
    emitterDevice->SetRenderState( D3DRS_POINTSCALE_A,  FLOAT_TO_DWORD(0.0f) );
    emitterDevice->SetRenderState( D3DRS_POINTSCALE_B,  FLOAT_TO_DWORD(0.0f) );
    emitterDevice->SetRenderState( D3DRS_POINTSCALE_C,  FLOAT_TO_DWORD(1.0f) );

	// Lock the vertex buffer, and set up our point sprites in accordance with 
	// our particles that we're keeping track of in our application.
	CUSTOMVERTEX *pPointVertices;

	pVertexBuffer->Lock( 0, numParticles * sizeof(CUSTOMVERTEX),
		                   (void**)&pPointVertices, D3DLOCK_DISCARD );

	for( int i = 0; i < numParticles; ++i )
    {
        pPointVertices->psPosition = m_particles[i].m_vCurPos;
        pPointVertices->color = m_particles[i].m_vColor;
        pPointVertices++;
    }

    pVertexBuffer->Unlock();
	
	// draw the point sprites
	emitterDevice->SetTexture( 0, pTexture );
    emitterDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
    emitterDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	emitterDevice->DrawPrimitive( D3DPT_POINTLIST, 0, numParticles );

    // set the render states back
	emitterDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    emitterDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    emitterDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    emitterDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
}