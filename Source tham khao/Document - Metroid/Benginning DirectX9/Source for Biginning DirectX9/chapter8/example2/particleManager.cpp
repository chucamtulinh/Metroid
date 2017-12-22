#include ".\particlemanager.h"
#include <vector>

particleManager::particleManager(void)
{
	emitters.clear();
}

particleManager::~particleManager(void)
{
	emitters.clear();
}

bool particleManager::init(void)
{
	// clear the emitters
	emitters.clear();

	return true;
}

void particleManager::shutdown(void)
{
	// clear out the emitters vector
	emitters.clear();
}

bool particleManager::createEmitter(LPDIRECT3DDEVICE9 pDevice, int numParticles, std::string textureName, D3DXVECTOR3 position, D3DCOLOR color)
{
	// create a new emitter
	Emitter *tempEmitter = new Emitter(pDevice);

	// load in the texture
	if (!tempEmitter->addTexture(textureName))
	{
		return false;
	}

	// set the number of particles
	tempEmitter->setNumParticles(numParticles);

	tempEmitter->initParticles(position, color);

	// add this emitter to the vector
	emitters.push_back(tempEmitter);

	return true;
}

void particleManager::update(void)
{
	for (unsigned int i=0; i<emitters.size(); i++)
	{
		// check whether this emitter is active
		if (emitters[i]->getAlive())
			// if so, then update it
			emitters[i]->update();
	}
}

void particleManager::render(LPDIRECT3DDEVICE9 pDevice)
{
	for (unsigned int i=0; i<emitters.size(); i++)
	{
		// check whether this emitter is active
		if (emitters[i]->getAlive())
			// if so, render this emitter
			emitters[i]->render();
	}
}