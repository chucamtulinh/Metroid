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