#include <windows.h>
#include "dxManager.h"

#include ".\game.h"

/* Init the one and only ProjectManager */	
static dxManager& dxMgr = dxManager::getInstance();

Game::Game(void)
{
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

	return true;
}

/********************************************************************
* shutdown
* This function is used to release any memory that the game
* uses
********************************************************************/
void Game::shutdown(void)
{
}

/********************************************************************
* update
* This function is called once per frame. It's used to get the user
* input and move the objects in the game accordingly.
********************************************************************/
int Game::update(void)
{
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

	dxMgr.endRender();

	// get the updated time
	QueryPerformanceCounter(&timeEnd);

	// determine the animation rate
	anim_rate = ( (float)timeEnd.QuadPart - (float)timeStart.QuadPart ) / timerFreq.QuadPart;
}
