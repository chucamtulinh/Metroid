#include "Game.h"
#include "GameGlobal.h"

Game::Game(int fps)
{
	mFPS = fps;

	InitLoop();
}

Game::~Game()
{

}

void Game::Update(float dt)
{

	Render();
}

void Game::Render()
{
	auto device = GameGlobal::GetCurrentDevice();

	device->Clear(0, NULL, D3DCLEAR_TARGET, 0x4866ff, 0.0f, 0);

	if (device->BeginScene())
	{        
		//bat dau ve
		GameGlobal::GetCurrentSpriteHandler()->Begin(D3DXSPRITE_ALPHABLEND);

		//draw here

		//ket thuc ve
		GameGlobal::GetCurrentSpriteHandler()->End();


		device->EndScene();
	}

	device->Present(0, 0, 0, 0);
}

void Game::InitLoop()
{
	MSG msg;

	float tickPerFrame = 1.0f / mFPS, delta = 0;

	while (GameGlobal::isGameRunning)
	{
		GameTime::GetInstance()->StartCounter();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		delta += GameTime::GetInstance()->GetCouter();

		if (delta >= tickPerFrame)
		{
			Update((delta));
			delta = 0;
		}
		else
		{
			Sleep(tickPerFrame - delta);
			delta = tickPerFrame;
		}
	}
}