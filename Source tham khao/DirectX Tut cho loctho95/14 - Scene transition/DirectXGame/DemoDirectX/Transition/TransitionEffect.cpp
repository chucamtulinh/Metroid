#include "TransitionEffect.h"
#include "../GameComponents/GameGlobal.h"
#include "../GameControllers/SceneManager.h"

TransitionEffect::TransitionEffect()
{
    InitvertexData();
}


void TransitionEffect::Update(float dt)
{
}

bool TransitionEffect::IsFinish()
 { 
    return false; 
}

void TransitionEffect::SetScenes(Scene * source, Scene * dest)
{
    mSceneSource = source;
    mSceneDest = dest;

    DoAfterSetScene();
    ReplaceNullScene();
}

void TransitionEffect::DoAfterSetScene()
{
}

void TransitionEffect::ReplaceNullScene()
{
}


void TransitionEffect::InitvertexData()
{
    VertexData vertexData[] =
    {
        //  x     y     z       color
        { D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) },
        { D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) },
        { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) },
        { D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) }
    };

    LPD3DXBUFFER bufferErrors = NULL;
    HRESULT hr;

    unsigned int vertexDataSize = 4 * sizeof(VertexData);

    GameGlobal::GetCurrentDevice()->CreateVertexBuffer(vertexDataSize, D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &vertexBuffer, 0);

    void *pVertices = NULL;
    vertexBuffer->Lock(0, vertexDataSize, (void**)&pVertices, 0);
    memcpy(pVertices, vertexData, vertexDataSize);
    vertexBuffer->Unlock();
}

void TransitionEffect::OnFinish()
{
    SceneManager::GetInstance()->ReplaceScene(mSceneDest);
    delete this;
}
