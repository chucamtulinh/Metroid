#pragma once

#include <math.h>
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>

#include "../GameComponents/Scene.h"
#include "../GameComponents/Sprite.h"

class DemoScene : public Scene
{
public:
    DemoScene();

    void Update(float dt);
    void LoadContent();
    void Draw();

protected:
    std::vector<Sprite*> mSprite;

    float mTimeCounter;
};

