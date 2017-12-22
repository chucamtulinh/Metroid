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
                            mMap->GetHeight() - GameGlobal::GetHeight() / 2);

    mMap->SetCamera(mCamera);
}

void DemoScene::Update(float dt)
{

}

void DemoScene::Draw()
{
    mMap->Draw();
}

void DemoScene::OnKeyDown(int keyCode)
{
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
}

void DemoScene::OnKeyUp(int keyCode)
{

}

void DemoScene::OnMouseDown(float x, float y)
{
}
