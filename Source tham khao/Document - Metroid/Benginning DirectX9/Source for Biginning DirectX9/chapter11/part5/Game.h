#pragma once

#include <vector>

// Forward class declarations
class dxManager;
class CShip;
class CPlanet;
class CGameObject;

class Game
{
public:
	Game(void);
	~Game(void);

	bool init(HWND wndHandle);
	void shutdown(void);

	int update(void);
	void render(void);

private:
	bool createGameObjects(void);
	
	// timer variables
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timerFreq;
	float         anim_rate;
	
	// Pointer to a CShip object
	CShip *pShip;						// the player's spaceship

	// Pointer to a CPlanet object
	CPlanet *pPlanet;					// The main planet

	// The game objects vector
	// This vector will store all the in-game objects to be updated and rendered
	std::vector <CGameObject*> objects;
};
