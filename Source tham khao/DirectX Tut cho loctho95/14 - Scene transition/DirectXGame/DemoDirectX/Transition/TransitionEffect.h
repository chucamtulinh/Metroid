#pragma once
#include "../GameComponents/Scene.h"
#include "Shader.h"

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

class TransitionEffect
{
public:

    struct VertexData
    {
        D3DXVECTOR3 Position;
        D3DCOLOR Color;
    };

    virtual void Update(float dt);

    virtual bool IsFinish();

    virtual void SetScenes(Scene *source, Scene *dest);

    virtual void DoAfterSetScene();

    virtual void ReplaceNullScene();

    virtual void InitvertexData();    

    virtual void OnFinish();

protected:
    TransitionEffect();

    Shader *shader;
    LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
    Scene *mSceneSource, *mSceneDest;
};