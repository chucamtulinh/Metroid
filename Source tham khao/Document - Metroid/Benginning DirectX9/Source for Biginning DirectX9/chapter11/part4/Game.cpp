#include <windows.h>
#include "dxManager.h"

#include ".\GameObject.h"
#include ".\Planet.h"

#include ".\game.h"

/* Init the one and only ProjectManager */	
static dxManager& dxMgr = dxManager::getInstance();

Game::Game(void)
{
	pPlanet = NULL;					// game objects
}

Game::~Game(void)
{
}

/********************************************************************
* init
* Initializes the game object. This involves setting the initial
* game state and preparing the timer
********************************************************************/
bool Game::init(HWND wndHandle)
{
	QueryPerformanceFrequency(&timerFreq);

	// init the DirectX Manager
	if (!dxMgr.init(wndHandle))
	{
		MessageBox(wndHandle, "Unable to init DirectX", "ERROR", MB_OK);
		return false;
	}

	// Initialize the in-game objects
	// The spaceship and the planets
	if (!createGameObjects())
	{
		MessageBox(wndHandle, "Unable to create GameObjects", "ERROR", MB_OK);
		return false;
	}

	// setup the camera for the game
	dxMgr.createCamera(1.0f, 1000.0f);		// near clip plane, far clip plane	
	dxMgr.moveCamera(D3DXVECTOR3(0.0f, 0.0f, -40.0f));
	dxMgr.pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	gameState = GAME_PLAY;

	return true;
}

/********************************************************************
* shutdown
* This function is used to release any memory that the game
* uses
********************************************************************/
void Game::shutdown(void)
{
	// Loop through all the objects in the vector and kill them off
	for (unsigned int i=0; i<objects.size(); i++)
	{
		if (objects[i])
			delete objects[i];
	}

	// Null out the game object pointers
	pPlanet = NULL;

	// shutdown the directx manager
	dxMgr.shutdown();
}

/********************************************************************
* update
* This function is called once per frame. It's used to get the user
* input and move the objects in the game accordingly.
********************************************************************/
int Game::update(void)
{
	// check the game state
	switch (gameState)
	{
		case GAME_START:
			gameState = GAME_PLAY;
		break;

		case GAME_PLAY:
		break;

		case GAME_OVER_WIN:
			gameState = GAME_START;
		break;

		case GAME_OVER_LOSE:
			gameState = MENU;
		break;
	}

	return 1;
}

/********************************************************************
* render
* Renders the in-game objects based on a timer
********************************************************************/
void Game::render(void)
{
	// get the time before rendering
	QueryPerformanceCounter(&timeStart);

	// call our render function
	dxMgr.beginRender();

	// Render by looping through all the objects in the list
	for (unsigned int i=0; i<objects.size(); i++)
		objects[i]->render(dxMgr.getD3DDevice());
	
	dxMgr.endRender();

	// get the updated time
	QueryPerformanceCounter(&timeEnd);

	// determine the animation rate
	anim_rate = ( (float)timeEnd.QuadPart - (float)timeStart.QuadPart ) / timerFreq.QuadPart;
}

/********************************************************************
* createGameObjects
* Private function used to create the in-game objects
********************************************************************/
bool Game::createGameObjects(void)
{
	// Clear out the objects vector
	objects.clear();

	// Create the planet object
	pPlanet = new CPlanet();

	// Scale the planet to make it larger
	pPlanet->setSize(7);

	// Create the planet
	if (!pPlanet->create(dxMgr.getD3DDevice()))
	{
		MessageBox(NULL, "sphere.x or earth.bmp missing", "ERROR", MB_OK);
		return false;
	}

	// Add the planet to the in-game objects list
	objects.push_back((CGameObject*)pPlanet);

	return true;
}