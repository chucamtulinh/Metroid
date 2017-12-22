#include "Player.h"
#include "PlayerStandingState.h"
#include "PlayerJumpingState.h"

Player::Player()
{
    mAnimationStanding = new Animation("Resources/mario/standingright.png", 1, 1, 1, 0);
    mAnimationJumping = new Animation("Resources/mario/jumpingright.png", 1, 1, 1, 0);
    mAnimationRunning = new Animation("Resources/mario/runningright.png", 2, 1, 2, 0.15f);

    this->mPlayerData = new PlayerData();
    this->mPlayerData->player = this;
    this->vx = 0;
    this->vy = 0;
    this->SetState(new PlayerStandingState(this->mPlayerData));

    allowJump = true;
}

Player::~Player()
{
}

void Player::Update(float dt)
{    
    mCurrentAnimation->Update(dt);

    if (this->mPlayerData->state)
    {
        this->mPlayerData->state->Update(dt);
    }

    Entity::Update(dt);
}

void Player::HandleKeyboard(std::map<int, bool> keys)
{
    if (this->mPlayerData->state)
    {
        this->mPlayerData->state->HandleKeyboard(keys);
    }
}

void Player::OnKeyPressed(int key)
{
    if (key == VK_SPACE)
    {
        if (allowJump)
        {
            if (mCurrentState == PlayerState::Running || mCurrentState == PlayerState::Standing)
            {
                this->SetState(new PlayerJumpingState(this->mPlayerData));
            }

            allowJump = false;
        }
    }
}

void Player::OnKeyUp(int key)
{
    if (key == VK_SPACE)
        allowJump = true;
}

void Player::SetReverse(bool flag)
{
    mCurrentReverse = flag;
}

void Player::Draw(D3DXVECTOR3 position, RECT sourceRect, D3DXVECTOR2 scale, D3DXVECTOR2 transform, float angle, D3DXVECTOR2 rotationCenter, D3DXCOLOR colorKey)
{
    mCurrentAnimation->FlipVertical(mCurrentReverse);
    mCurrentAnimation->SetPosition(this->GetPosition());

    mCurrentAnimation->Draw(D3DXVECTOR3(posX, posY, 0));
}

void Player::SetState(PlayerState *newState)
{
    delete this->mPlayerData->state;

    this->mPlayerData->state = newState;

    this->changeAnimation(newState->GetState());

    mCurrentState = newState->GetState();
}

RECT Player::GetBound()
{
    RECT rect;
    rect.left = this->posX - mCurrentAnimation->GetWidth() / 2;
    rect.right = rect.left + mCurrentAnimation->GetWidth();
    rect.top = this->posY - mCurrentAnimation->GetHeight() / 2;
    rect.bottom = rect.top + mCurrentAnimation->GetHeight();

    return rect;
}

void Player::changeAnimation(PlayerState::StateName state)
{
    switch (state)
    {
        case PlayerState::Running:
            mCurrentAnimation = mAnimationRunning;
            break;

        case PlayerState::Standing:
            mCurrentAnimation = mAnimationStanding;
            break;

        case PlayerState::Falling:
            mCurrentAnimation = mAnimationJumping;
            break;

        case PlayerState::Jumping:
            mCurrentAnimation = mAnimationJumping;
            break;
    }

    this->width = mCurrentAnimation->GetWidth();
    this->height = mCurrentAnimation->GetHeight();
}

Player::MoveDirection Player::getMoveDirection()
{
    if (this->vx > 0)
    {
        return MoveDirection::MoveToRight;
    }
    else if (this->vx < 0)
    {
        return MoveDirection::MoveToLeft;
    }

    return MoveDirection::None;
}

PlayerState::StateName Player::getState()
{
    return mCurrentState;
}