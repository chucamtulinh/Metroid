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
    mCamera->SetPosition(GameGlobal::GetWidth() / 2, 
                            mMap->GetHeight() - mCamera->GetHeight() / 2);

    mMap->SetCamera(mCamera);

    mPlayer = new Player();
    mPlayer->SetPosition(GameGlobal::GetWidth() / 2, GameGlobal::GetHeight() / 2);
}

void DemoScene::Update(float dt)
{
    //mPlayer->HandleKeyboard(keys);
    mMap->Update(dt);
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

    if (keyCode == VK_LEFT)
    {
        mCamera->SetPosition(mCamera->GetPosition() + D3DXVECTOR3(-10, 0, 0));
    }

    if (keyCode == VK_RIGHT)
    {
        mCamera->SetPosition(mCamera->GetPosition() + D3DXVECTOR3(10, 0, 0));
    }

    if (keyCode == VK_UP)
    {
        mCamera->SetPosition(mCamera->GetPosition() + D3DXVECTOR3(0, -10, 0));
    }

    if (keyCode == VK_DOWN)
    {
        mCamera->SetPosition(mCamera->GetPosition() + D3DXVECTOR3(0, 10, 0));
    }

    //mPlayer->OnKeyPressed(keyCode);
}

void DemoScene::OnKeyUp(int keyCode)
{
    keys[keyCode] = false;
}

void DemoScene::OnMouseDown(float x, float y)
{
}
