#pragma once
#ifndef __GAME_MAP__
#define __GAME_MAP__

#include <d3dx9.h>
#include <d3d9.h>
#include <vector>

#include "Sprite.h"
#include "../MapReader/Tmx.h.in"
#include "GameGlobal.h"
#include "../GameComponents/Camera.h"

class GameMap
{
public:
    GameMap(char* filePath);

    Tmx::Map* GetMap();

    int GetWidth();
    int GetHeight();
    int GetTileWidth();
    int GetTileHeight();

    void SetCamera(Camera *camera);

    void Draw();    

    ~GameMap();

private:
    void LoadMap(char* filePath);

    bool isContain(RECT rect1, RECT rect2);

    Tmx::Map                        *mMap;
    std::map<int, Sprite*>          mListTileset;
    Camera *mCamera;
};

#endif

