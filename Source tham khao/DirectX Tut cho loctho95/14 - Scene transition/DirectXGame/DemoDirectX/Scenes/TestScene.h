#pragma once

#include <math.h>
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>

#include "../GameComponents/Scene.h"
#include "../GameComponents/GameMap.h"
#include "../GameComponents/Camera.h"

class TestScene : public Scene
{
public:
    TestScene();

    void Update(float dt);
    void LoadContent();
    void Draw();

    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    void OnMouseDown(float x, float y);

protected:
    GameMap *mMap;
    Camera *mCamera;
};

