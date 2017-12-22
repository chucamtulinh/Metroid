#include "DemoScene.h"

DemoScene::DemoScene()
{
    LoadContent();
}

void DemoScene::LoadContent()
{
    //set time counter = 0
    mTimeCounter = 0;

    //set mau backcolor cho scene o day la mau xanh
    mBackColor = 0x54acd2;

    for (size_t i = 0; i < 10; i++)
    {
        //tao nhung normalbrick tu hinh anh theo duong dan
        Sprite *sprite = new Sprite("Resources/normalbrick.png");

        //set random position cho cac sprite tu 0 -> 500 theo x, y
        sprite->SetPosition(rand() % 500, rand() % 500);

        //add sprite vao vector
        mSprite.push_back(sprite);
    }
}

void DemoScene::Update(float dt)
{
    //cong don thoi gian lai 60 FPS = 1 / 60 giay trong 1 lan goi update
    mTimeCounter += dt;

    //update sau 1 giay
    if (mTimeCounter >= 1.0f)
    {
        for (Sprite *child : mSprite)
        {
            //set lai position random tu 0 -> 500 theo x va y cho moi sprite
            child->SetPosition(rand() % 500, rand() % 500);
        }

        //set lai counter
        mTimeCounter = 0;
    }
}

void DemoScene::Draw()
{
    //goi va draw cac sprite trong vector

    for (Sprite *child : mSprite)
    {
        child->Draw();
    }
}