#include "Game.h"
#include "GameGlobal.h"
#include "Sprite.h"

Sprite *sprite;

Game::Game(int fps)
{
	mFPS = fps;

	sprite = new Sprite("Resources/loctho.png");
	sprite->SetPosition(GameGlobal::GetWidth() / 2, GameGlobal::GetHeight() / 2);

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

	//0x4866ff set mau cho surface
	device->Clear(0, NULL, D3DCLEAR_TARGET, 0x4866ff, 0.0f, 0);

	{
		device->BeginScene();

		//bat dau ve
		GameGlobal::GetCurrentSpriteHandler()->Begin(D3DXSPRITE_ALPHABLEND);

		//draw here
		sprite->Draw();

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