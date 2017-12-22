#include <windows.h>
#include "dxManager.h"
#include "diManager.h"

#include ".\GameObject.h"
#include ".\Ship.h"
#include ".\Planet.h"

#include ".\game.h"

/* Init the one and only ProjectManager */	
static dxManager& dxMgr = dxManager::getInstance();

/* Get a reference to the DirectInput Manager */	
static diManager& diMgr = diManager::getInstance();

Game::Game(void)
{
	pShip   = NULL;					// Clear out the pointers to the 
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

	// Initialize the in-game objects
	// The spaceship and the planets
	if (!createGameObjects())
	{
		MessageBox(wndHandle, "Unable to create GameObjects\nbigship1.x, earth.bmp and sphere.x must be in the current directory", "ERROR", MB_OK);
		return false;
	}

	// setup the camera for the game
	dxMgr.createCamera(1.0f, 1000.0f);		// near clip plane, far clip plane	
	dxMgr.moveCamera(D3DXVECTOR3(0.0f, 0.0f, -40.0f));
	dxMgr.pointCamera(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

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
	pShip = NULL;
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
	// Get the current user input
	diMgr.getInput();

	// If the left mouse button is pressed, move forward
	if (diMgr.isButtonDown(0))
		pShip->move(CShip::FORWARD, anim_rate);
			
	// If the right mouse button is pressed, move backwards
	if (diMgr.isButtonDown(1))
		pShip->move(CShip::BACKWARD, anim_rate);
			
	// If the mouse is moved to the left, turn left
	if (diMgr.getCurMouseX() < 0)			
		pShip->move(CShip::LEFT, anim_rate);
			
	// If the mouse is moved to the right, turn right
	if (diMgr.getCurMouseX() > 0)
		pShip->move(CShip::RIGHT, anim_rate);

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

	// Render all the objects in the scene
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

	// Create the spaceship
	pShip = new CShip();
	
	// Init the spaceship
	if (!pShip->create(dxMgr.getD3DDevice()))
		return false;

	// Move the ship to the origin
	pShip->setPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// Add the spaceship to the objects vector
	objects.push_back((CGameObject*)pShip);

	// Create the planet object
	pPlanet = new CPlanet();

	// Scale the planet to make it larger
	pPlanet->setSize(7);

	// Create the planet
	if (!pPlanet->create(dxMgr.getD3DDevice()))
		return false;

	// Add the planet to the in-game objects list
	objects.push_back((CGameObject*)pPlanet);
	
	return true;
}