#pragma once
#include "TransitionEffect.h"

class TransitionCircleScan : public TransitionEffect
{
public:
    //degree/frame
    TransitionCircleScan(float speed = 120.0);
    ~TransitionCircleScan();

    void DoAfterSetScene();
    void Update(float dt);
    bool IsFinish();

private:
    IDirect3DTexture9 *texture0, *texture1;
    float angle, speed;
};

