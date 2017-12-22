#include "GameMap.h"

GameMap::GameMap(char* filePath)
{
    LoadMap(filePath);
}

GameMap::~GameMap()
{
    delete mMap;
}

void GameMap::LoadMap(char* filePath)
{
    mMap = new Tmx::Map();
    mMap->ParseFile(filePath);

    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = this->GetWidth();
    r.bottom = this->GetHeight();

    for (size_t i = 0; i < mMap->GetNumTilesets(); i++)
    {
        const Tmx::Tileset *tileset = mMap->GetTileset(i);

        Sprite *sprite = new Sprite(tileset->GetImage()->GetSource().c_str());

        mListTileset.insert(std::pair<int, Sprite*>(i, sprite));
    }
}

bool GameMap::isContain(RECT rect1, RECT rect2)
{
    if (rect1.left > rect2.right || rect1.right < rect2.left || rect1.top > rect2.bottom || rect1.bottom < rect2.top)
    {
        return false;
    }

    return true;
}

Tmx::Map* GameMap::GetMap()
{
    return mMap;
}

int GameMap::GetWidth()
{
    return mMap->GetWidth() * mMap->GetTileWidth();
}

int GameMap::GetHeight()
{
    return mMap->GetHeight() * mMap->GetTileHeight();
}

int GameMap::GetTileWidth()
{
    return mMap->GetTileWidth();
}

int GameMap::GetTileHeight()
{
    return mMap->GetTileHeight();
}

void GameMap::Draw()
{
    D3DXVECTOR2 trans = D3DXVECTOR2(GameGlobal::GetWidth() / 2 - mCamera->GetPosition().x,
                                    GameGlobal::GetHeight() / 2 - mCamera->GetPosition().y);

    for (size_t i = 0; i < mMap->GetNumTileLayers(); i++)
    {
        const Tmx::TileLayer *layer = mMap->GetTileLayer(i);

        if (!layer->IsVisible())
        {
            continue;
        }

        RECT sourceRECT;

        int tileWidth = mMap->GetTileWidth();
        int tileHeight = mMap->GetTileHeight();

        for (size_t m = 0; m < layer->GetHeight(); m++)
        {
            for (size_t n = 0; n < layer->GetWidth(); n++)
            {
                int tilesetIndex = layer->GetTileTilesetIndex(n, m);

                if (tilesetIndex != -1)
                {
                    const Tmx::Tileset *tileSet = mMap->GetTileset(tilesetIndex);

                    int tileSetWidth = tileSet->GetImage()->GetWidth() / tileWidth;
                    int tileSetHeight = tileSet->GetImage()->GetHeight() / tileHeight;

                    Sprite* sprite = mListTileset[layer->GetTileTilesetIndex(n, m)];

                    //tile index
                    int tileID = layer->GetTileId(n, m);

                    int y = tileID / tileSetWidth;
                    int x = tileID - y * tileSetWidth;

                    sourceRECT.top = y * tileHeight;
                    sourceRECT.bottom = sourceRECT.top + tileHeight;
                    sourceRECT.left = x * tileWidth;
                    sourceRECT.right = sourceRECT.left + tileWidth;

                    //tru tilewidth/2 va tileheight/2 vi Sprite ve o vi tri giua hinh anh cho nen doi hinh de cho
                    //dung toa do (0,0) cua the gioi thuc la (0,0) neu khong thi se la (-tilewidth/2, -tileheigth/2);
                    D3DXVECTOR3 position(n * tileWidth + tileWidth / 2, m * tileHeight + tileHeight / 2, 0);

                    if (mCamera != NULL)
                    {
                        RECT objRECT;
                        objRECT.left = position.x - tileWidth / 2;
                        objRECT.top = position.y - tileHeight / 2;
                        objRECT.right = objRECT.left + tileWidth;
                        objRECT.bottom = objRECT.top + tileHeight;

                        //neu nam ngoai camera thi khong Draw
                        if (isContain(objRECT, mCamera->GetBound()) == false)
                            continue;
                    }

                    sprite->SetWidth(tileWidth);
                    sprite->SetHeight(tileHeight);

                    sprite->Draw(position, sourceRECT, D3DXVECTOR2(), trans);
                }
            }
        }
    }    
}

void GameMap::SetCamera(Camera * camera)
{
    this->mCamera = camera;
}
