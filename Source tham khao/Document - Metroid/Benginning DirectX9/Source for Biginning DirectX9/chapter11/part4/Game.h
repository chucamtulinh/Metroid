#pragma once

#include <vector>

// Forward class declarations
class dxManager;
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
	enum { MENU = 0, GAME_START, GAME_PLAY, GAME_OVER_LOSE, GAME_OVER_WIN, GAME_STOP };
	bool createGameObjects(void);
	
	int gameState;

	// timer variables
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timerFreq;
	float         anim_rate;
	
	// Pointer to a CPlanet object
	CPlanet *pPlanet;

	// The game objects vector
	// This vector will store all the in-game objects to be updated and rendered
	std::vector <CGameObject*> objects;
};
