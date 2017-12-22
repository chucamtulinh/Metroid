#include "DemoScene.h"

DemoScene::DemoScene()
{
    LoadContent();
}

void DemoScene::LoadContent()
{
    //set mau backcolor cho scene o day la mau xanh
    mBackColor = 0x54acd2;

    mGoldBlock = new Animation("Resources/goldbrick.png", 4, 1, 4, 0.15);
    mGoldBlock->SetPosition(GameGlobal::GetWidth() / 2, GameGlobal::GetHeight() / 2);
    mGoldBlock->SetScale(D3DXVECTOR2(2, 2));
}

void DemoScene::Update(float dt)
{
    mGoldBlock->Update(dt);
}

void DemoScene::Draw()
{
    mGoldBlock->Draw();
}

void DemoScene::OnKeyDown(int keyCode)
{
    if (keyCode == VK_LEFT)
    {
        mGoldBlock->SetPosition(mGoldBlock->GetPosition() + D3DXVECTOR3(-10, 0, 0));
    }

    if (keyCode == VK_RIGHT)
    {
        mGoldBlock->SetPosition(mGoldBlock->GetPosition() + D3DXVECTOR3(10, 0, 0));
    }

    if (keyCode == VK_UP)
    {
        mGoldBlock->SetPosition(mGoldBlock->GetPosition() + D3DXVECTOR3(0, -10, 0));
    }

    if (keyCode == VK_DOWN)
    {
        mGoldBlock->SetPosition(mGoldBlock->GetPosition() + D3DXVECTOR3(0, 10, 0));
    }
}

void DemoScene::OnKeyUp(int keyCode)
{

}

void DemoScene::OnMouseDown(float x, float y)
{
}
