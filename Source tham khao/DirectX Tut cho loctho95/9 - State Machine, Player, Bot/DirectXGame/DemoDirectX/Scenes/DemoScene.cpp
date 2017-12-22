#include "DemoScene.h"

DemoScene::DemoScene()
{
    LoadContent();
}

void DemoScene::LoadContent()
{
    //set mau backcolor cho scene o day la mau xanh
    mBackColor = 0x54acd2;

    mMap = new GameMap("Resources/marioworld1-1.tmx");

    mCamera = new Camera(GameGlobal::GetWidth(), GameGlobal::GetHeight());
    mCamera->SetPosition(0, 0);

    mMap->SetCamera(mCamera);

    mPlayer = new Player();
    mPlayer->SetPosition(GameGlobal::GetWidth() / 2, GameGlobal::GetHeight() / 2);
}

void DemoScene::Update(float dt)
{
    mPlayer->HandleKeyboard(keys);
    mPlayer->Update(dt);
}

void DemoScene::Draw()
{
    mMap->Draw();
    mPlayer->Draw();
}

void DemoScene::OnKeyDown(int keyCode)
{
    keys[keyCode] = true;

    mPlayer->OnKeyPressed(keyCode);
}

void DemoScene::OnKeyUp(int keyCode)
{
    keys[keyCode] = false;
}

void DemoScene::OnMouseDown(float x, float y)
{
}
