#pragma once
#include <vector>
#include <string>

#include "Emitter.h"
#include "Particle.h"

// forward class declarations
class Particle;
class Emitter;

class particleManager
{
public:
	particleManager(void);
	~particleManager(void);

	bool init(void);
	void shutdown(void);

	bool particleManager::createEmitter(LPDIRECT3DDEVICE9 pDevice, int numParticles, std::string textureName, D3DXVECTOR3 position, D3DCOLOR color);
	void removeEmitter(int emitterNum);
	void removeEmitter(Emitter *which);

	void update(void);
	void render(LPDIRECT3DDEVICE9 pDevice);

	std::vector <Emitter*> emitters;
};
