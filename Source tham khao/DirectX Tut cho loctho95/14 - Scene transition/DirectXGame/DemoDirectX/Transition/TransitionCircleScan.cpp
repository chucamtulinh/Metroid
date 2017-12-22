#include <iostream>
#include "TransitionCircleScan.h"
#include "../GameComponents/GameGlobal.h"
#include "../GameComponents/GameLog.h"

TransitionCircleScan::TransitionCircleScan(float speed)
{
    angle = 0.0;
    this->speed = speed;
    shader = new Shader("Resources/Shader/transitioncirclescan.fx");

    InitvertexData();
}


TransitionCircleScan::~TransitionCircleScan()
{
}


void TransitionCircleScan::Update(float dt)
{
    angle += speed * dt;

    GameGlobal::GetCurrentDevice()->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
    GameGlobal::GetCurrentDevice()->BeginScene();

    unsigned int Passes = 0;
    shader->getEffect()->Begin(&Passes, 0);

    D3DXMATRIX mtxViewProj;
    D3DXMatrixIdentity(&mtxViewProj);
    mtxViewProj._11 = 2.0;
    mtxViewProj._22 = -2.0; //toa do texel no nguoc voi toa do hlsl nen phai cover
    mtxViewProj._41 = -1.0;
    mtxViewProj._42 = 1.0;

    for (int i = 0; i < Passes; ++i)
    {
        shader->getEffect()->BeginPass(i);

        shader->getEffect()->SetMatrix("WorldViewProj", &(mtxViewProj));
        shader->getEffect()->SetTexture("texture0", texture0);
        shader->getEffect()->SetTexture("texture1", texture1);
        shader->getEffect()->SetFloat("angle", D3DXToRadian(angle));

        GameGlobal::GetCurrentDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexData));
        GameGlobal::GetCurrentDevice()->SetFVF(D3DFVF_CUSTOMVERTEX);
        GameGlobal::GetCurrentDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

        shader->getEffect()->EndPass();
    }

    shader->getEffect()->End();

    GameGlobal::GetCurrentDevice()->EndScene();
    GameGlobal::GetCurrentDevice()->Present(NULL, NULL, NULL, NULL);

    if (angle >= 180.0)
    {
        OnFinish();
    }
}

bool TransitionCircleScan::IsFinish()
{
    return angle >= 180.0;
}

void TransitionCircleScan::DoAfterSetScene()
{
    auto device = GameGlobal::GetCurrentDevice();

    D3DXCreateTexture(GameGlobal::GetCurrentDevice(), GameGlobal::GetWidth(), GameGlobal::GetHeight(), D3DX_DEFAULT,
        D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &texture0);

    D3DXCreateTexture(GameGlobal::GetCurrentDevice(), GameGlobal::GetWidth(), GameGlobal::GetHeight(), D3DX_DEFAULT,
        D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &texture1);

    IDirect3DSurface9 *surface, *backsurface;

    mSceneDest->Render();

    texture1->GetSurfaceLevel(0, &surface);
    GameGlobal::GetCurrentDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backsurface);

    D3DXLoadSurfaceFromSurface(surface, NULL, NULL, backsurface, NULL, NULL, D3DX_FILTER_NONE, 0);
    surface->Release();
    surface = nullptr;

    mSceneSource->Render();

    texture0->GetSurfaceLevel(0, &surface);
    D3DXLoadSurfaceFromSurface(surface, NULL, NULL, backsurface, NULL, NULL, D3DX_FILTER_NONE, 0);
    surface->Release();
}
