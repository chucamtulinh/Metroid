#include "TestScene.h"
#include "../GameComponents/GameGlobal.h"
#include "../GameControllers/SceneManager.h"
#include "../Transition/TransitionCircleScan.h"
#include "../GameComponents/GameLog.h"
#include "DemoScene.h"

TestScene::TestScene()
{
    LoadContent();
}

void TestScene::LoadContent()
{
    //set mau backcolor cho scene o day la mau xanh
    mBackColor = 0x590f0f;

    mMap = new GameMap("Resources/test.tmx");

    mCamera = new Camera(GameGlobal::GetWidth(), GameGlobal::GetHeight());
    mCamera->SetPosition(GameGlobal::GetWidth() / 2, 
                            mMap->GetHeight() - mCamera->GetHeight() / 2);

    mMap->SetCamera(mCamera);
}

void TestScene::Update(float dt)
{

}

void TestScene::Draw()
{
    mMap->Draw();
}

void TestScene::OnKeyDown(int keyCode)
{
}

void TestScene::OnKeyUp(int keyCode)
{
}

void TestScene::OnMouseDown(float x, float y)
{
    SceneManager::GetInstance()->ReplaceScene(new DemoScene(), new TransitionCircleScan());
}

