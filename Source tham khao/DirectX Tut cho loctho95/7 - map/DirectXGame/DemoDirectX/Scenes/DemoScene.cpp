#include "DemoScene.h"

DemoScene::DemoScene()
{
    LoadContent();
}

void DemoScene::LoadContent()
{
    //set mau backcolor cho scene o day la mau xanh
    mBackColor = 0x54acd2;

    mMap = new GameMap("Resources/untitled.tmx");
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

}

void DemoScene::OnKeyUp(int keyCode)
{

}

void DemoScene::OnMouseDown(float x, float y)
{
}
